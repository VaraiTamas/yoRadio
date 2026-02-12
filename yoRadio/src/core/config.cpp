// "nameday"
#include "options.h"
#include <Wire.h>
#include "config.h"
#include "display.h"
#include "player.h"
#include "network.h"
#include "netserver.h"
#include "controls.h"
#include "timekeeper.h"
#include "telnet.h"
#include "rtcsupport.h"
#include "../displays/tools/l10n.h"
#ifdef USE_SD
    #include "sdmanager.h"
#endif
#ifdef USE_NEXTION
    #include "../displays/nextion.h"
#endif
#include <cstddef>

#if DSP_MODEL == DSP_DUMMY
    #define DUMMYDISPLAY
#endif

Config config;

void u8fix(char* src) { // Ha az utolsó tőbbájtos karakter (ékezetes) utolsó bájtja hiányzik akkor az elejét levágja.
    char last = src[strlen(src) - 1];
    if ((uint8_t)last >= 0xC2) { src[strlen(src) - 1] = '\0'; }
}

bool Config::_isFSempty() {
    const char*   reqiredFiles[] = {"dragpl.js.gz",   "ir.css.gz",    "irrecord.html.gz", "ir.js.gz",        "logo.svg.gz", "options.html.gz",
                                    "player.html.gz", "script.js.gz", "style.css.gz",     "updform.html.gz", "theme.css"};
    const uint8_t reqiredFilesSize = 11;
    char          fullpath[28];
    if (SPIFFS.exists("/www/settings.html")) { SPIFFS.remove("/www/settings.html"); }
    if (SPIFFS.exists("/www/update.html")) { SPIFFS.remove("/www/update.html"); }
    if (SPIFFS.exists("/www/index.html")) { SPIFFS.remove("/www/index.html"); }
    if (SPIFFS.exists("/www/ir.html")) { SPIFFS.remove("/www/ir.html"); }
    if (SPIFFS.exists("/www/elogo.png")) { SPIFFS.remove("/www/elogo.png"); }
    if (SPIFFS.exists("/www/elogo84.png")) { SPIFFS.remove("/www/elogo84.png"); }
    for (uint8_t i = 0; i < reqiredFilesSize; i++) {
        sprintf(fullpath, "/www/%s", reqiredFiles[i]);
        if (!SPIFFS.exists(fullpath)) {
            Serial.println(fullpath);
            return true;
        }
    }
    return false;
}

void Config::init() {
    EEPROM.begin(EEPROM_SIZE);
    sdResumePos = 0;
    /*----- I2C init -----*/
#if (RTC_MODULE == DS3231) || (TS_MODEL == TS_MODEL_FT6X36) || (TS_MODEL == TS_MODEL_GT911)
    Wire.begin(TS_SDA, TS_SCL);
    Wire.setClock(400000);
    Serial.println("Scanning I2S...");
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) { Serial.printf("Found: 0x%02X\n", addr); }
    }
#endif
    // DLNA modplus
#ifdef USE_DLNA
    isBooting = true;
    resumeAfterModeChange = false;
#endif
    // DLNA modplus
    screensaverTicks = 0;
    screensaverPlayingTicks = 0;
    newConfigMode = 0;
    isScreensaver = false;
    memset(tmpBuf, 0, BUFLEN);
    // bootInfo();
#if RTCSUPPORTED
    _rtcFound = false;
    BOOTLOG("RTC begin(SDA=%d,SCL=%d)", RTC_SDA, RTC_SCL);
    if (rtc.init()) {
        BOOTLOG("done");
        _rtcFound = true;
    } else {
        BOOTLOG("[ERROR] - Couldn't find RTC");
    }
#endif
    emptyFS = true;
#if IR_PIN != 255
    irindex = -1;
#endif
#if defined(SD_SPIPINS) || SD_HSPI
    #if !defined(SD_SPIPINS)
    SDSPI.begin();
    #else
    SDSPI.begin(SD_SPIPINS); // SCK, MISO, MOSI
    #endif
#endif
    eepromRead(EEPROM_START, store);
#ifdef USE_DLNA
    if (store.lastPlayedSource == PL_SRC_DLNA) {
        store.playlistSource = PL_SRC_DLNA;
    } else {
        store.playlistSource = PL_SRC_WEB;
    }
#endif
    bootInfo(); // https://github.com/e2002/yoradio/pull/149
    if (store.config_set != 4262) { setDefaults(); }
    if (store.version > CONFIG_VERSION) {
        saveValue(&store.version, (uint16_t)CONFIG_VERSION, true, true);
    } else {
        while (store.version != CONFIG_VERSION) { _setupVersion(); }
    }
    BOOTLOG("CONFIG_VERSION\t%d", store.version);

    store.play_mode = store.play_mode & 0b11;
    // DLNA modplus
#ifdef USE_DLNA
#else
    if (store.play_mode > 1) { store.play_mode = PM_WEB; }
#endif
    // DLNA modplus
    _initHW();
    if (!SPIFFS.begin(true)) {
        Serial.println("##[ERROR]#\tSPIFFS Mount Failed");
        return;
    }
    BOOTLOG("SPIFFS mounted");
    emptyFS = _isFSempty();
    if (emptyFS) { BOOTLOG("SPIFFS is empty!"); }
    ssidsCount = 0;
#ifdef USE_SD
    _SDplaylistFS = getMode() == PM_SDCARD ? &sdman : (true ? &SPIFFS : _SDplaylistFS);
#else
    _SDplaylistFS = &SPIFFS;
#endif
    _bootDone = false;
    setTimeConf();
// DLNA modplus
#ifdef USE_DLNA
    isBooting = false;
#endif
// DLNA modplus
#if PWR_AMP != 255 // "PWR_AMP"
    pinMode(PWR_AMP, OUTPUT);
    digitalWrite(PWR_AMP, HIGH);
#endif
}

void Config::_setupVersion() {
    uint16_t currentVersion = store.version;
    switch (currentVersion) {
        case 1:
            saveValue(&store.screensaverEnabled, false);
            saveValue(&store.screensaverTimeout, (uint16_t)20);
            break;
        case 2:
            snprintf(tmpBuf, MDNS_LENGTH, "yoradio-%x", (unsigned int)getChipId());
            saveValue(store.mdnsname, tmpBuf, MDNS_LENGTH);
            saveValue(&store.skipPlaylistUpDown, false);
            break;
        case 3:
            saveValue(&store.screensaverBlank, false);
            saveValue(&store.screensaverPlayingEnabled, false);
            saveValue(&store.screensaverPlayingTimeout, (uint16_t)5);
            saveValue(&store.screensaverPlayingBlank, false);
            break;
        case 4:
            saveValue(&store.abuff, (uint16_t)(VS1053_CS == 255 ? 7 : 10));
            saveValue(&store.telnet, true);
            saveValue(&store.watchdog, true);
            saveValue(&store.nameday, true);                     // Módosítás új sor "nameday"
            saveValue(&store.timeSyncInterval, (uint16_t)60);    // min
            saveValue(&store.timeSyncIntervalRTC, (uint16_t)24); // hours
            saveValue(&store.weatherSyncInterval, (uint16_t)30); // min
        default: break;
    }
    currentVersion++;
    saveValue(&store.version, currentVersion);
}

void Config::changeMode(int newmode) { // DLNA mod
    // Serial.printf("Config.cpp-->changeMode() newmode: %d", newmode);
#ifdef USE_SD
    // Encoder dupla klikk (paraméter nélküli hívás)
    if (newmode == -1) {
        // DLNA nem választható encoderről
        newmode = (getMode() == PM_SDCARD) ? PM_WEB : PM_SDCARD;
    }

    // 🔒 biztonsági ellenőrzés
    if (newmode < 0 || newmode >= 2) { // 0 --> radio; 1 --> SD; 2 --> DLNA
        Serial.printf("##[ERROR]# changeMode invalid newmode: %d\n", newmode);
        return;
    }

    bool pir = player.isRunning();

    if (SDC_CS == 255 && newmode == PM_SDCARD) { return; }

    if (network.status == SOFT_AP || display.mode() == LOST) {
        saveValue(&store.play_mode, (uint8_t)PM_SDCARD);
        delay(50);
        ESP.restart();
    }

    /* === SD only when explicitly requested === */
    if (newmode == PM_SDCARD) {
        if (!sdman.ready) {
            if (!sdman.start()) {
                Serial.println("##[ERROR]# SD Not Found");
                netserver.requestOnChange(GETPLAYERMODE, 0);
                return;
            }
        }
    }

    /* === set mode === */
    store.play_mode = (playMode_e)newmode;
    saveValue(&store.play_mode, (uint8_t)store.play_mode, true, true);

    /* === filesystem binding === */
    if (getMode() == PM_SDCARD) {
        _SDplaylistFS = &sdman;
    } else {
        _SDplaylistFS = &SPIFFS; // WEB + DLNA
    }

    /* === SD specific actions === */
    if (getMode() == PM_SDCARD) {
        if (pir) { player.sendCommand({PR_STOP, 0}); }
        display.putRequest(NEWMODE, SDCHANGE);
        delay(50);
    } else {
        sdman.stop(); // WEB + DLNA → SD off
    }

    if (!_bootDone) { return; }

    initPlaylistMode();

    if (pir) {
    #ifdef USE_DLNA
        uint16_t st = (getMode() == PM_SDCARD) ? store.lastSdStation : (store.playlistSource == PL_SRC_DLNA ? store.lastDlnaStation : store.lastStation);
    #else
        uint16_t st = (getMode() == PM_SDCARD) ? store.lastSdStation : store.lastStation;
        player.sendCommand({PR_PLAY, st});
    #endif
    }

    netserver.resetQueue();
    netserver.requestOnChange(GETINDEX, 0);

    display.resetQueue();
    display.putRequest(NEWMODE, PLAYER);
    display.putRequest(NEWSTATION);
#endif
}

void Config::initSDPlaylist() {
#ifdef USE_SD
    // bool doIndex = !sdman.exists(INDEX_SD_PATH); // "módosítás"
    // if(doIndex) sdman.indexSDPlaylist();
    //  Mindig legyen indexelés az SD kártyán.
    sdman.indexSDPlaylist();
    if (SDPLFS()->exists(INDEX_SD_PATH)) {
        File index = SDPLFS()->open(INDEX_SD_PATH, "r");
        // if(doIndex){
        lastStation(_randomStation());
        sdResumePos = 0;
        // }
        index.close();
    }
#endif // #ifdef USE_SD
}

bool Config::spiffsCleanup() {
    bool ret = (SPIFFS.exists(PLAYLIST_SD_PATH)) || (SPIFFS.exists(INDEX_SD_PATH)) || (SPIFFS.exists(INDEX_PATH));
    if (SPIFFS.exists(PLAYLIST_SD_PATH)) { SPIFFS.remove(PLAYLIST_SD_PATH); }
    if (SPIFFS.exists(INDEX_SD_PATH)) { SPIFFS.remove(INDEX_SD_PATH); }
    if (SPIFFS.exists(INDEX_PATH)) { SPIFFS.remove(INDEX_PATH); }
    return ret;
}

void Config::waitConnection() {
#if I2S_DOUT == 255
    return;
#endif
    while (!player.connproc) { vTaskDelay(50); }
    vTaskDelay(500);
}

char* Config::ipToStr(IPAddress ip) {
    snprintf(ipBuf, 16, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
    return ipBuf;
}

bool Config::prepareForPlaying(uint16_t stationId) {
    setDspOn(1);
    vuRefLevel = 0;
    screensaverTicks = SCREENSAVERSTARTUPDELAY;
    screensaverPlayingTicks = SCREENSAVERSTARTUPDELAY;
    if (getMode() != PM_SDCARD) { display.putRequest(PSTOP); }
    if (!loadStation(stationId)) { return false; }
    setTitle(LANG::const_PlConnect); // inen van a connect felirat a kijelzőn
    station.bitrate = 0;
    setBitrateFormat(BF_UNKNOWN);
    display.putRequest(DBITRATE);
    display.putRequest(NEWSTATION);
    display.putRequest(NEWMODE, PLAYER);
    netserver.requestOnChange(STATION, 0);
    netserver.requestOnChange(MODE, 0);
    netserver.loop();
    netserver.loop();
    if (store.smartstart != 2) { setSmartStart(0); }
    return true;
}

void Config::configPostPlaying(uint16_t stationId) { // DLNA mod
    if (getMode() == PM_SDCARD) {
        saveValue(&store.lastSdStation, stationId);
    }
#ifdef USE_DLNA
    else if (store.playlistSource == PL_SRC_DLNA) {
        saveValue(&store.lastDlnaStation, stationId);
    }
#endif
    else {
        saveValue(&store.lastStation, stationId);
    }

    if (store.smartstart != 2) { setSmartStart(1); }
    netserver.requestOnChange(MODE, 0);
    display.putRequest(PSTART);
}

void Config::setSDpos(uint32_t val) {
    if (getMode() == PM_SDCARD) {
        sdResumePos = 0; // ha kézzel állítasz pozíciót, ne legyen régi resume
        if (!player.isRunning()) {
            config.sdResumePos = val - player.sd_min;
        } else {
            player.setAudioFilePosition(val - player.sd_min); // futó lejátszásnál seek webről
        }
    }
}

void Config::initPlaylistMode() {
    uint16_t _lastStation = 0;

#ifdef USE_SD
    if (getMode() == PM_SDCARD) {
        if (!sdman.start()) {
            changeMode(PM_WEB);
            return;
        }
        initSDPlaylist();
        uint16_t cs = playlistLength();
        _lastStation = store.lastSdStation;
        if (_lastStation == 0 && cs > 0) { _lastStation = _randomStation(); }
    } else
#endif
    {

#ifdef USE_DLNA
        if (store.playlistSource == PL_SRC_DLNA) {

            if (SPIFFS.exists(PLAYLIST_DLNA_PATH)) { initDLNAPlaylist(); }

            uint16_t cs = playlistLength();

            // ⬇️ DLNA indexet CSAK innen vesszük
            _lastStation = store.lastDlnaStation;
            if (_lastStation == 0 && cs > 0) { _lastStation = 1; }

        } else
#endif
        {
            initPlaylist();
            uint16_t cs = playlistLength();
            _lastStation = store.lastStation;
            if (_lastStation == 0 && cs > 0) { _lastStation = 1; }
        }
    }

    // ⬇️ EGYSZER
    lastStation(_lastStation);
    loadStation(_lastStation);

    _bootDone = true;
}

void Config::_initHW() {
    loadTheme();
#if IR_PIN != 255
    eepromRead(EEPROM_START_IR, ircodes);
    if (ircodes.ir_set != 4224) {
        ircodes.ir_set = 4224;
        memset(ircodes.irVals, 0, sizeof(ircodes.irVals));
    }
#endif
#if BRIGHTNESS_PIN != 255
    pinMode(BRIGHTNESS_PIN, OUTPUT);
    setBrightness(false);
#endif
}

uint16_t Config::color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void Config::loadTheme() {
    theme.background = color565(COLOR_BACKGROUND);
    theme.meta = color565(COLOR_STATION_NAME);
    theme.metabg = color565(COLOR_STATION_BG);
    theme.metafill = color565(COLOR_STATION_FILL);
    theme.title1 = color565(COLOR_SNG_TITLE_1);
    theme.title2 = color565(COLOR_SNG_TITLE_2);
    theme.digit = color565(COLOR_DIGITS);
    theme.div = color565(COLOR_DIVIDER);
    theme.weather = color565(COLOR_WEATHER);
    theme.vumax = color565(COLOR_VU_MAX);
    theme.vumid = color565(COLOR_VU_MID); // Módosítás: plussz sor.
    theme.vumin = color565(COLOR_VU_MIN);
    theme.nameday = color565(COLOR_NAMEDAY); // Módosítás: plussz sor.
    theme.clock = color565(COLOR_CLOCK);
    theme.clockbg = color565(COLOR_CLOCK_BG);
    theme.seconds = color565(COLOR_SECONDS);
    theme.dow = color565(COLOR_DAY_OF_W);
    theme.date = color565(COLOR_DATE);
    theme.heap = color565(COLOR_HEAP);
    theme.buffer = color565(COLOR_BUFFER);
    theme.ip = color565(COLOR_IP);
    theme.ch = color565(COLOR_CH);
    theme.vol = color565(COLOR_VOLUME_VALUE);
    theme.rssi = color565(COLOR_RSSI);
    theme.bitrate = color565(COLOR_BITRATE);
    theme.volbarout = color565(COLOR_VOLBAR_OUT);
    theme.volbarin = color565(COLOR_VOLBAR_IN);
    theme.plcurrent = color565(COLOR_PL_CURRENT);
    theme.plcurrentbg = color565(COLOR_PL_CURRENT_BG);
    theme.plcurrentfill = color565(COLOR_PL_CURRENT_FILL);
    theme.playlist[0] = color565(COLOR_PLAYLIST_0);
    theme.playlist[1] = color565(COLOR_PLAYLIST_1);
    theme.playlist[2] = color565(COLOR_PLAYLIST_2);
    theme.playlist[3] = color565(COLOR_PLAYLIST_3);
    theme.playlist[4] = color565(COLOR_PLAYLIST_4);
    theme.prst_button = color565(COLOR_PRST_BUTTON);  // Módosítás: plussz gombszín. "presets"
    theme.prst_card = color565(COLOR_PRST_CARD);      // Módosítás: plussz kártya szín. "presets"
    theme.prst_accent = color565(COLOR_PRST_ACCENT);  // Módosítás: plussz kiemelés szín. "presets"
    theme.prst_fav = color565(COLOR_PRST_FAV);        // Módosítás: plussz FAV gomb text szín. "presets"
    theme.prst_title1 = color565(COLOR_PRST_TITLE_1); // Módosítás: plussz kártya text1 szín. "presets"
    theme.prst_title2 = color565(COLOR_PRST_TITLE_2); // Módosítás: plussz kártya text2 szín. "presets"
    theme.prst_title3 = color565(COLOR_PRST_TITLE_3); // Módosítás: plussz kártya text3 inaktív szín. "presets"
    theme.prst_line = color565(COLOR_PRST_LINE);      // Módosítás: plussz vonal szín. "presets"
#include "../displays/tools/tftinverttitle.h"
}

void Config::reset() {
    setDefaults();
    delay(500);
    ESP.restart();
}
void Config::enableScreensaver(bool val) {
    saveValue(&store.screensaverEnabled, val);
#ifndef DSP_LCD
    display.putRequest(NEWMODE, PLAYER);
#endif
}
void Config::setScreensaverTimeout(uint16_t val) {
    val = constrain(val, 5, 65520);
    saveValue(&store.screensaverTimeout, val);
#ifndef DSP_LCD
    display.putRequest(NEWMODE, PLAYER);
#endif
}
void Config::setScreensaverBlank(bool val) {
    saveValue(&store.screensaverBlank, val);
#ifndef DSP_LCD
    display.putRequest(NEWMODE, PLAYER);
#endif
}
void Config::setScreensaverPlayingEnabled(bool val) {
    saveValue(&store.screensaverPlayingEnabled, val);
#ifndef DSP_LCD
    display.putRequest(NEWMODE, PLAYER);
#endif
}
void Config::setScreensaverPlayingTimeout(uint16_t val) {
    val = constrain(val, 1, 1080);
    config.saveValue(&config.store.screensaverPlayingTimeout, val);
#ifndef DSP_LCD
    display.putRequest(NEWMODE, PLAYER);
#endif
}
void Config::setScreensaverPlayingBlank(bool val) {
    saveValue(&store.screensaverPlayingBlank, val);
#ifndef DSP_LCD
    display.putRequest(NEWMODE, PLAYER);
#endif
}
void Config::setSntpOne(const char* val) {
    bool tzdone = false;
    if (strlen(val) > 0 && strlen(store.sntp2) > 0) {
        configTime(store.tzHour * 3600 + store.tzMin * 60, getTimezoneOffset(), val, store.sntp2);
        tzdone = true;
    } else if (strlen(val) > 0) {
        configTime(store.tzHour * 3600 + store.tzMin * 60, getTimezoneOffset(), val);
        tzdone = true;
    }
    if (tzdone) {
        timekeeper.forceTimeSync = true;
        saveValue(config.store.sntp1, val, 35);
    }
}
void Config::setShowweather(bool val) {
    config.saveValue(&config.store.showweather, val);
    timekeeper.forceWeather = true;
    display.putRequest(SHOWWEATHER);
}
void Config::setWeatherKey(const char* val) {
    saveValue(store.weatherkey, val, WEATHERKEY_LENGTH);
    display.putRequest(NEWMODE, CLEAR);
    display.putRequest(NEWMODE, PLAYER);
}

#if IR_PIN != 255
void Config::setIrBtn(int val) {
    irindex = val;
    netserver.irRecordEnable = (irindex >= 0);
    irchck = 0;
    netserver.irValsToWs();
    if (irindex < 0) { saveIR(); }
}
#endif
void Config::resetSystem(const char* val, uint8_t clientId) {
    if (strcmp(val, "system") == 0) {
        saveValue(&store.smartstart, (uint8_t)2, false);
        saveValue(&store.audioinfo, false, false);
        saveValue(&store.vumeter, false, false);
        saveValue(&store.softapdelay, (uint8_t)0, false);
        saveValue(&store.abuff, (uint16_t)(VS1053_CS == 255 ? 7 : 10), false);
        saveValue(&store.telnet, true);
        saveValue(&store.watchdog, true);
        saveValue(&store.nameday, true); // Módosítás "nameday"
        snprintf(store.mdnsname, MDNS_LENGTH, "yoradio-%x", (unsigned int)getChipId());
        saveValue(store.mdnsname, store.mdnsname, MDNS_LENGTH, true, true);
        display.putRequest(NEWMODE, CLEAR);
        display.putRequest(NEWMODE, PLAYER);
        netserver.requestOnChange(GETSYSTEM, clientId);
        return;
    }
    if (strcmp(val, "screen") == 0) {
        saveValue(&store.flipscreen, false, false);
        display.flip();
        saveValue(&store.invertdisplay, false, false);
        display.invert();
        saveValue(&store.dspon, true, false);
        store.brightness = 100;
        setBrightness(false);
        saveValue(&store.contrast, (uint8_t)55, false);
        display.setContrast();
        saveValue(&store.numplaylist, false);
        saveValue(&store.screensaverEnabled, false);
        saveValue(&store.screensaverTimeout, (uint16_t)20);
        saveValue(&store.screensaverBlank, false);
        saveValue(&store.screensaverPlayingEnabled, false);
        saveValue(&store.screensaverPlayingTimeout, (uint16_t)5);
        saveValue(&store.screensaverPlayingBlank, false);
        display.putRequest(NEWMODE, CLEAR);
        display.putRequest(NEWMODE, PLAYER);
        netserver.requestOnChange(GETSCREEN, clientId);
        return;
    }
    if (strcmp(val, "timezone") == 0) {
        saveValue(&store.tzHour, (int8_t)3, false);
        saveValue(&store.tzMin, (int8_t)0, false);
        saveValue(store.sntp1, "hu.pool.ntp.org", 35, false);
        saveValue(store.sntp2, "time.google.com", 35);
        saveValue(&store.timeSyncInterval, (uint16_t)60);
        saveValue(&store.timeSyncIntervalRTC, (uint16_t)24);
        configTime(store.tzHour * 3600 + store.tzMin * 60, getTimezoneOffset(), store.sntp1, store.sntp2);
        timekeeper.forceTimeSync = true;
        netserver.requestOnChange(GETTIMEZONE, clientId);
        return;
    }
    if (strcmp(val, "weather") == 0) {
        saveValue(&store.showweather, false, false);
        saveValue(store.weatherlat, "46.3873", 10, false);
        saveValue(store.weatherlon, "18.1513", 10, false);
        saveValue(store.weatherkey, "", WEATHERKEY_LENGTH);
        saveValue(&store.weatherSyncInterval, (uint16_t)30);
        // network.trueWeather=false;
        display.putRequest(NEWMODE, CLEAR);
        display.putRequest(NEWMODE, PLAYER);
        netserver.requestOnChange(GETWEATHER, clientId);
        return;
    }
    if (strcmp(val, "controls") == 0) {
        saveValue(&store.volsteps, (uint8_t)1, false);
        saveValue(&store.fliptouch, false, false);
        saveValue(&store.dbgtouch, false, false);
        saveValue(&store.skipPlaylistUpDown, false);
        setEncAcceleration(200);
        setIRTolerance(40);
        netserver.requestOnChange(GETCONTROLS, clientId);
        return;
    }
    if (strcmp(val, "1") == 0) {
        config.reset();
        return;
    }
}

void Config::setDefaults() {
    store.config_set = 4262;
    store.version = CONFIG_VERSION;
    store.volume = 12;
    store.balance = 0;
    store.trebble = 0;
    store.middle = 0;
    store.bass = 0;
    store.lastStation = 0;
    store.countStation = 0;
    store.lastSSID = 0;
    store.audioinfo = false;
    store.smartstart = 2;
    store.tzHour = 3;
    store.tzMin = 0;
    store.timezoneOffset = 0;

    store.vumeter = false;
    store.softapdelay = 0;
    store.flipscreen = false;
    store.invertdisplay = false;
    store.numplaylist = false;
    store.fliptouch = false;
    store.dbgtouch = false;
    store.dspon = true;
    store.brightness = 100;
    store.contrast = 55;
    strlcpy(store.sntp1, "pool.ntp.org", 35);
    strlcpy(store.sntp2, "1.ru.pool.ntp.org", 35);
    store.showweather = false;
    strlcpy(store.weatherlat, "46.3873", 10);
    strlcpy(store.weatherlon, "18.1513", 10);
    strlcpy(store.weatherkey, "", WEATHERKEY_LENGTH);
    store._reserved = 0;
    store.lastSdStation = 0;
    store.lastDlnaStation = 0; // DLNA mod
    store.sdsnuffle = false;
    store.volsteps = 1;
    store.encacc = 200;
    store.play_mode = 0;
    store.irtlp = 35;
    store.btnpullup = true;
    store.btnlongpress = 200;
    store.btnclickticks = 300;
    store.btnpressticks = 500;
    store.encpullup = false;
    store.enchalf = false;
    store.enc2pullup = false;
    store.enc2half = false;
    store.forcemono = false;
    store.i2sinternal = false;
    store.rotate90 = false;
    store.screensaverEnabled = false;
    store.screensaverTimeout = 20;
    store.screensaverBlank = false;
    snprintf(store.mdnsname, MDNS_LENGTH, "yoradio-%x", (unsigned int)getChipId());
    store.skipPlaylistUpDown = false;
    store.screensaverPlayingEnabled = false;
    store.screensaverPlayingTimeout = 5;
    store.screensaverPlayingBlank = false;
    store.abuff = VS1053_CS == 255 ? 7 : 10;
    store.telnet = true;
    store.watchdog = true;
    store.nameday = true;           // Módosítás "nameday" kezdő érték.
    store.timeSyncInterval = 60;    // min
    store.timeSyncIntervalRTC = 24; // hour
    store.weatherSyncInterval = 30; // min
#ifdef USE_DLNA                     // DLNA mod
    store.playlistSource = PL_SRC_WEB;
#endif
    eepromWrite(EEPROM_START, store);
}

void Config::setTimezone(int8_t tzh, int8_t tzm) {
    saveValue(&store.tzHour, tzh, false);
    saveValue(&store.tzMin, tzm);
}

void Config::setTimezoneOffset(uint16_t tzo) {
    saveValue(&store.timezoneOffset, tzo);
}

uint16_t Config::getTimezoneOffset() {
    return 0; // TODO
}
// Véletlen lejátszás beállítása.
void Config::setSnuffle(bool sn) {
    saveValue(&store.sdsnuffle, sn);
    // if(store.sdsnuffle) player.next(); //Továbbléptette egy másik fájlra, ezért kivettem.
}

#if IR_PIN != 255
void Config::saveIR() {
    eepromWrite(EEPROM_START_IR, ircodes);
}
#endif

void Config::saveVolume() {
    saveValue(&store.volume, store.volume, true, true);
}

uint8_t Config::setVolume(uint8_t val) {
    store.volume = val;
    display.putRequest(DRAWVOL);
    netserver.requestOnChange(VOLUME, 0);
    return store.volume;
}

void Config::setTone(int8_t bass, int8_t middle, int8_t trebble) {
    saveValue(&store.bass, bass, false);
    saveValue(&store.middle, middle, false);
    saveValue(&store.trebble, trebble);
    player.setTone(store.bass, store.middle, store.trebble);
    netserver.requestOnChange(EQUALIZER, 0);
}

void Config::setSmartStart(uint8_t ss) {
    saveValue(&store.smartstart, ss);
}

void Config::setBalance(int8_t balance) {
    saveValue(&store.balance, balance);
    player.setBalance(-store.balance); // "audio_change"  -16 to 16 fordítás 16 to -16
    netserver.requestOnChange(BALANCE, 0);
}

uint8_t Config::setLastStation(uint16_t val) {
    // Make "current item" persistent per mode
    if (getMode() == PM_SDCARD) {
        saveValue(&store.lastSdStation, val);
        return store.lastSdStation;
    }
#ifdef USE_DLNA
    if (store.playlistSource == PL_SRC_DLNA) {
        saveValue(&store.lastDlnaStation, val);
        return store.lastDlnaStation;
    }
#endif
    saveValue(&store.lastStation, val);
    return store.lastStation;
}

uint8_t Config::setCountStation(uint16_t val) {
    saveValue(&store.countStation, val);
    return store.countStation;
}

uint8_t Config::setLastSSID(uint8_t val) {
    saveValue(&store.lastSSID, val);
    return store.lastSSID;
}

void Config::setTitle(const char* title) {
    vuRefLevel = 0;
    memset(config.station.title, 0, BUFLEN);
    strlcpy(config.station.title, title, BUFLEN);
    u8fix(config.station.title);
    netserver.requestOnChange(TITLE, 0);
    netserver.loop();
    display.putRequest(NEWTITLE);
}

void Config::setStation(const char* station) {
    memset(config.station.name, 0, BUFLEN);
    strlcpy(config.station.name, station, BUFLEN);
    u8fix(config.station.title);
}

void Config::indexPlaylist() {
    File playlist = SPIFFS.open(PLAYLIST_PATH, "r");
    if (!playlist) { return; }
    int  sOvol;
    File index = SPIFFS.open(INDEX_PATH, "w");
    while (playlist.available()) {
        uint32_t pos = playlist.position();
        if (parseCSV(playlist.readStringUntil('\n').c_str(), tmpBuf, tmpBuf2, sOvol)) { index.write((uint8_t*)&pos, 4); }
    }
    index.close();
    playlist.close();
}

// DLNA mod
#ifdef USE_DLNA
void Config::indexDLNAPlaylist() {
    File playlist = SPIFFS.open(PLAYLIST_DLNA_PATH, "r");
    if (!playlist) {
        Serial.println("[DLNA][IDX] Cannot open DLNA playlist");
        return;
    }

    File index = SPIFFS.open(INDEX_DLNA_PATH, "w");
    if (!index) {
        Serial.println("[DLNA][IDX] Cannot create DLNA index");
        playlist.close();
        return;
    }

    static char lineBuf[512];
    int         sOvol = 0;

    uint32_t lines = 0;
    uint32_t ok = 0;

    while (playlist.available()) {
        uint32_t pos = playlist.position();

        // readBytesUntil nem allokál, stabil
        size_t n = playlist.readBytesUntil('\n', lineBuf, sizeof(lineBuf) - 1);
        lineBuf[n] = 0;

        // CRLF kezelés
        if (n > 0 && lineBuf[n - 1] == '\r') { lineBuf[n - 1] = 0; }

        // üres sor skip
        if (lineBuf[0] == 0) {
            lines++;
            continue;
        }

        // FONTOS: parseCSV kapjon ÍRHATÓ buffert (lineBuf), ne String.c_str()-t
        if (parseCSV(lineBuf, tmpBuf, tmpBuf2, sOvol)) {
            index.write((uint8_t*)&pos, 4);
            ok++;
        }

        lines++;

        // WDT/Task starvation ellen (DLNA/WiFi közben kellhet)
        if ((lines % 50) == 0) {
            delay(0); // vagy yield();
        }
    }

    index.close();
    playlist.close();

    Serial.printf("[DLNA][IDX] DLNA playlist indexed: %lu/%lu\n", (unsigned long)ok, (unsigned long)lines);
}
#endif

void Config::initPlaylist() {
    // store.countStation = 0;
    if (!SPIFFS.exists(INDEX_PATH)) { indexPlaylist(); }

    /*if (SPIFFS.exists(INDEX_PATH)) {
      File index = SPIFFS.open(INDEX_PATH, "r");
      store.countStation = index.size() / 4;
      index.close();
      saveValue(&store.countStation, store.countStation, true, true);
    }*/
}

#ifdef USE_DLNA // DLNA mod
void Config::initDLNAPlaylist() {
    indexDLNAPlaylist();

    if (SPIFFS.exists(INDEX_DLNA_PATH)) {
        File index = SPIFFS.open(INDEX_DLNA_PATH, "r");
        if (index) {
            // lastStation(_randomStation());
            index.close();
        }
    }
}
#endif

uint16_t Config::playlistLength() {
    uint16_t out = 0;
    if (SDPLFS()->exists(REAL_INDEX)) {
        File index = SDPLFS()->open(REAL_INDEX, "r");
        out = index.size() / 4;
        index.close();
    }
    return out;
}
bool Config::loadStation(uint16_t ls) {
    int      sOvol;
    uint16_t cs = playlistLength();
    if (cs == 0) {
        memset(station.url, 0, BUFLEN);
        memset(station.name, 0, BUFLEN);
        strncpy(station.name, "yoRadio", BUFLEN);
        station.ovol = 0;
        return false;
    }
    if (ls > playlistLength()) { ls = 1; }
    File playlist = SDPLFS()->open(REAL_PLAYL, "r");
    File index = SDPLFS()->open(REAL_INDEX, "r");
    index.seek((ls - 1) * 4, SeekSet);
    uint32_t pos;
    index.readBytes((char*)&pos, 4);
    index.close();
    playlist.seek(pos, SeekSet);
    if (parseCSV(playlist.readStringUntil('\n').c_str(), tmpBuf, tmpBuf2, sOvol)) {
        memset(station.url, 0, BUFLEN);
        memset(station.name, 0, BUFLEN);
        strncpy(station.name, tmpBuf, BUFLEN);
        strncpy(station.url, tmpBuf2, BUFLEN);
        station.ovol = sOvol;
        setLastStation(ls);
    }
    playlist.close();
    return true;
}

char* Config::stationByNum(uint16_t num) {
    File playlist = SDPLFS()->open(REAL_PLAYL, "r");
    File index = SDPLFS()->open(REAL_INDEX, "r");
    index.seek((num - 1) * 4, SeekSet);
    uint32_t pos;
    memset(_stationBuf, 0, sizeof(_stationBuf));
    index.readBytes((char*)&pos, 4);
    index.close();
    playlist.seek(pos, SeekSet);
    strncpy(_stationBuf, playlist.readStringUntil('\t').c_str(), sizeof(_stationBuf));
    playlist.close();
    return _stationBuf;
}

void Config::escapeQuotes(const char* input, char* output, size_t maxLen) {
    size_t j = 0;
    for (size_t i = 0; input[i] != '\0' && j < maxLen - 1; ++i) {
        if (input[i] == '"' && j < maxLen - 2) {
            output[j++] = '\\';
            output[j++] = '"';
        } else {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
}

bool Config::parseCSV(const char* line, char* name, char* url, int& ovol) {
    char*       tmpe;
    const char* cursor = line;
    char        buf[5];
    tmpe = strstr(cursor, "\t");
    if (tmpe == NULL) { return false; }
    strlcpy(name, cursor, tmpe - cursor + 1);
    if (strlen(name) == 0) { return false; }
    cursor = tmpe + 1;
    tmpe = strstr(cursor, "\t");
    if (tmpe == NULL) { return false; }
    strlcpy(url, cursor, tmpe - cursor + 1);
    if (strlen(url) == 0) { return false; }
    cursor = tmpe + 1;
    if (strlen(cursor) == 0) { return false; }
    strlcpy(buf, cursor, 4);
    ovol = atoi(buf);
    return true;
}

bool Config::parseJSON(const char* line, char* name, char* url, int& ovol) {
    char *      tmps, *tmpe;
    const char* cursor = line;
    char        port[8], host[246], file[254];
    tmps = strstr(cursor, "\":\"");
    if (tmps == NULL) { return false; }
    tmpe = strstr(tmps, "\",\"");
    if (tmpe == NULL) { return false; }
    strlcpy(name, tmps + 3, tmpe - tmps - 3 + 1);
    if (strlen(name) == 0) { return false; }
    cursor = tmpe + 3;
    tmps = strstr(cursor, "\":\"");
    if (tmps == NULL) { return false; }
    tmpe = strstr(tmps, "\",\"");
    if (tmpe == NULL) { return false; }
    strlcpy(host, tmps + 3, tmpe - tmps - 3 + 1);
    if (strlen(host) == 0) { return false; }
    if (strstr(host, "http://") == NULL && strstr(host, "https://") == NULL) {
        sprintf(file, "http://%s", host);
        strlcpy(host, file, strlen(file) + 1);
    }
    cursor = tmpe + 3;
    tmps = strstr(cursor, "\":\"");
    if (tmps == NULL) { return false; }
    tmpe = strstr(tmps, "\",\"");
    if (tmpe == NULL) { return false; }
    strlcpy(file, tmps + 3, tmpe - tmps - 3 + 1);
    cursor = tmpe + 3;
    tmps = strstr(cursor, "\":\"");
    if (tmps == NULL) { return false; }
    tmpe = strstr(tmps, "\",\"");
    if (tmpe == NULL) { return false; }
    strlcpy(port, tmps + 3, tmpe - tmps - 3 + 1);
    int p = atoi(port);
    if (p > 0) {
        sprintf(url, "%s:%d%s", host, p, file);
    } else {
        sprintf(url, "%s%s", host, file);
    }
    cursor = tmpe + 3;
    tmps = strstr(cursor, "\":\"");
    if (tmps == NULL) { return false; }
    tmpe = strstr(tmps, "\"}");
    if (tmpe == NULL) { return false; }
    strlcpy(port, tmps + 3, tmpe - tmps - 3 + 1);
    ovol = atoi(port);
    return true;
}

bool Config::parseWsCommand(const char* line, char* cmd, char* val, uint8_t cSize) {
    char* tmpe;
    tmpe = strstr(line, "=");
    if (tmpe == NULL) { return false; }
    memset(cmd, 0, cSize);
    strlcpy(cmd, line, tmpe - line + 1);
    // if (strlen(tmpe + 1) == 0) return false;
    memset(val, 0, cSize);
    strlcpy(val, tmpe + 1, strlen(line) - strlen(cmd) + 1);
    return true;
}

bool Config::parseSsid(const char* line, char* ssid, char* pass) {
    char* tmpe;
    tmpe = strstr(line, "\t");
    if (tmpe == NULL) { return false; }
    uint16_t pos = tmpe - line;
    if (pos > 29 || strlen(line) > 71) { return false; }
    memset(ssid, 0, 30);
    strlcpy(ssid, line, pos + 1);
    memset(pass, 0, 40);
    strlcpy(pass, line + pos + 1, strlen(line) - pos);
    return true;
}

bool Config::saveWifiFromNextion(const char* post) {
    File file = SPIFFS.open(SSIDS_PATH, "w");
    if (!file) {
        return false;
    } else {
        file.print(post);
        file.close();
        ESP.restart();
        return true;
    }
}

bool Config::saveWifi() {
    if (!SPIFFS.exists(TMP_PATH)) { return false; }
    SPIFFS.remove(SSIDS_PATH);
    SPIFFS.rename(TMP_PATH, SSIDS_PATH);
    ESP.restart();
    return true;
}

void Config::setTimeConf() {
    if (strlen(store.sntp1) > 0 && strlen(store.sntp2) > 0) {
        configTime(store.tzHour * 3600 + store.tzMin * 60, getTimezoneOffset(), store.sntp1, store.sntp2);
    } else if (strlen(store.sntp1) > 0) {
        configTime(store.tzHour * 3600 + store.tzMin * 60, getTimezoneOffset(), store.sntp1);
    }
}

bool Config::initNetwork() {
    File file = SPIFFS.open(SSIDS_PATH, "r");
    if (!file || file.isDirectory()) { return false; }
    char    ssidval[30], passval[40];
    uint8_t c = 0;
    while (file.available()) {
        if (parseSsid(file.readStringUntil('\n').c_str(), ssidval, passval)) {
            strlcpy(ssids[c].ssid, ssidval, 30);
            strlcpy(ssids[c].password, passval, 40);
            ssidsCount++;
            c++;
        }
    }
    file.close();
    return true;
}

void Config::setBrightness(bool dosave) {
#if BRIGHTNESS_PIN != 255
    if (!store.dspon && dosave) { display.wakeup(); }
    analogWrite(BRIGHTNESS_PIN, map(store.brightness, 0, 100, 0, 255));
    if (!store.dspon) { store.dspon = true; }
    if (dosave) {
        saveValue(&store.brightness, store.brightness, false, true);
        saveValue(&store.dspon, store.dspon, true, true);
    }
#endif
#ifdef USE_NEXTION
    nextion.wake();
    char cmd[15];
    snprintf(cmd, 15, "dims=%d", store.brightness);
    nextion.putcmd(cmd);
    if (!store.dspon) { store.dspon = true; }
    if (dosave) {
        saveValue(&store.brightness, store.brightness, false, true);
        saveValue(&store.dspon, store.dspon, true, true);
    }
#endif
}

void Config::setDspOn(bool dspon, bool saveval) {
    if (saveval) {
        store.dspon = dspon;
        saveValue(&store.dspon, store.dspon, true, true);
    }
#ifdef USE_NEXTION
    if (!dspon) {
        nextion.sleep();
    } else {
        nextion.wake();
    }
#endif
    if (!dspon) {
#if BRIGHTNESS_PIN != 255
        analogWrite(BRIGHTNESS_PIN, 0);
#endif
        display.deepsleep();
    } else {
        display.wakeup();
#if BRIGHTNESS_PIN != 255
        analogWrite(BRIGHTNESS_PIN, map(store.brightness, 0, 100, 0, 255));
#endif
    }
}

void Config::doSleep() {
    if (BRIGHTNESS_PIN != 255) { analogWrite(BRIGHTNESS_PIN, 0); }
    display.deepsleep();
#ifdef USE_NEXTION
    nextion.sleep();
#endif
#if !defined(ARDUINO_ESP32C3_DEV)
    if (WAKE_PIN != 255) { esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKE_PIN, LOW); }
    esp_sleep_enable_timer_wakeup(config.sleepfor * 60 * 1000000ULL);
    esp_deep_sleep_start();
#endif
}

void Config::doSleepW() {
    if (BRIGHTNESS_PIN != 255) { analogWrite(BRIGHTNESS_PIN, 0); }
    display.deepsleep();
#ifdef USE_NEXTION
    nextion.sleep();
#endif
#if !defined(ARDUINO_ESP32C3_DEV)
    if (WAKE_PIN != 255) { esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKE_PIN, LOW); }
    esp_deep_sleep_start();
#endif
}

void Config::sleepForAfter(uint16_t sf, uint16_t sa) {
    sleepfor = sf;
    if (sa > 0) {
        timekeeper.waitAndDo(sa * 60, doSleep);
    } else {
        doSleep();
    }
}

void Config::bootInfo() {
    BOOTLOG("************************************************");
    BOOTLOG("*               ёPadio v%s                *", YOVERSION);
    BOOTLOG("************************************************");
    BOOTLOG("------------------------------------------------");
    BOOTLOG("arduino:\t%d", ARDUINO);
    BOOTLOG("compiler:\t%s", __VERSION__);
    BOOTLOG("esp32core:\t%d.%d.%d", ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
    uint32_t chipId = 0;
    for (int i = 0; i < 17; i = i + 8) { chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i; }
    BOOTLOG("chip:\t\tmodel: %s | rev: %d | id: %lu | cores: %d | psram: %lu", ESP.getChipModel(), ESP.getChipRevision(), chipId, ESP.getChipCores(), ESP.getPsramSize());
    BOOTLOG("display:\t%d", DSP_MODEL);
    if (VS1053_CS == 255) {
        BOOTLOG("audio:\t\t%s (%d, %d, %d)", "I2S", I2S_DOUT, I2S_BCLK, I2S_LRC);
    } else {
        BOOTLOG("audio:\t\t%s (%d, %d, %d, %d, %s)", "VS1053", VS1053_CS, VS1053_DCS, VS1053_DREQ, VS1053_RST, VS_HSPI ? "true" : "false");
    }
    BOOTLOG("audioinfo:\t%s", store.audioinfo ? "true" : "false");
    BOOTLOG("smartstart:\t%d", store.smartstart);
    BOOTLOG("vumeter:\t%s", store.vumeter ? "true" : "false");
    BOOTLOG("softapdelay:\t%d", store.softapdelay);
    BOOTLOG("flipscreen:\t%s", store.flipscreen ? "true" : "false");
    BOOTLOG("invertdisplay:\t%s", store.invertdisplay ? "true" : "false");
    BOOTLOG("showweather:\t%s", store.showweather ? "true" : "false");
    BOOTLOG("buttons:\tleft=%d, center=%d, right=%d, up=%d, down=%d, mode=%d, pullup=%s", BTN_LEFT, BTN_CENTER, BTN_RIGHT, BTN_UP, BTN_DOWN, BTN_MODE, BTN_INTERNALPULLUP ? "true" : "false");
    BOOTLOG("encoders:\tl1=%d, b1=%d, r1=%d, pullup=%s, l2=%d, b2=%d, r2=%d, pullup=%s", ENC_BTNL, ENC_BTNB, ENC_BTNR, ENC_INTERNALPULLUP ? "true" : "false", ENC2_BTNL, ENC2_BTNB, ENC2_BTNR,
            ENC2_INTERNALPULLUP ? "true" : "false");
    BOOTLOG("ir:\t\t%d", IR_PIN);
    if (SDC_CS != 255) { BOOTLOG("SD:\t\t%d", SDC_CS); }
    BOOTLOG("------------------------------------------------");
    BOOTLOG("------------------------------------------------");
    BOOTLOG("CONFIG:\tsizeof(store)=%u B | EEPROM_START=%u | EEPROM_END=%u | EEPROM_SIZE=%u", (unsigned)sizeof(config.store), (unsigned)EEPROM_START, (unsigned)(EEPROM_START + sizeof(config.store)),
            (unsigned)EEPROM_SIZE);
    BOOTLOG("------------------------------------------------");
}
