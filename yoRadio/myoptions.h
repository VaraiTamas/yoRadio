/* https://trip5.github.io/ehRadio_myoptions/generator.html?b=ESP32-S3-DevKitC-1_44Pin&r=72,1,2,4,5,6,76,7,8,31,42,43,54,55,58,60,63,65,77&i=1,2,3,4,15,16,17,28,29,30,31,32,33,34,39,48,49&v=9,10,-1,14,4,5,6,40,41,39,48,47,21,3,18,8,7
   https://github.com/e2002/yoradio
   Használat előtt olvasd el!!! - Read the before use !!!
   https://github.com/VaraiTamas/yoRadio/blob/main/README.md !!! 
*/

#ifndef myoptions_h
#define myoptions_h

#ifndef ARDUINO_ESP32S3_DEV
  #define ARDUINO_ESP32S3_DEV
#endif

//#define HEAP_DBG

/* Itt tudod beállítani a program nyelvét 
   You can set the program language here. 
   Supported languages: HU NL PL RU EN GR SK DE UA ES. */
#define L10N_LANGUAGE HU

/* -- Névnapok megjelenítése -- Display name days -- 
Supported languages: HU, PL, NL, GR, DE (UA Local/namedays/namedays_UA.h is not filled in.) */
#define NAMEDAYS_FILE HU

#define USE_BUILTIN_LED false /* The RGB LED does not turn on.. */

/* Arduino OTA Support */
// #define USE_OTA true                    /* Enable OTA updates from Arduino IDE */
// #define OTA_PASS "myotapassword12345"   /* OTA password for secure updates */

/* HTTP Authentication */
// #define HTTP_USER ""               /* HTTP basic authentication username */
// #define HTTP_PASS ""               /* HTTP basic authentication password */

/* LCD */
#define DSP_MODEL DSP_ILI9488
//#define DSP_MODEL DSP_ILI9341
//#define DSP_MODEL DSP_ST7796
#define TFT_DC         9
#define TFT_CS         10
#define TFT_RST        -1
#define BRIGHTNESS_PIN 14
/* 
   GPIO 11 - MOSI
   GPIO 12 - CLK
   GPIO 13 - MISO  // Ne csatlakoztasd a kijelzőhöz!!! - Do not connect to the LCD display!!! 
*/

/* Touch */
#define TS_MODEL TS_MODEL_XPT2046
#define TS_CS    3

// #define NEXTION_RX			15
// #define NEXTION_TX			16

/* PCM5102A  DAC */
#define I2S_DOUT 4
#define I2S_BCLK 5
#define I2S_LRC  6

/* ENCODER 1 */
#define ENC_BTNR 41  // S2
#define ENC_BTNL 40  // S1
#define ENC_BTNB 39  // KEY
// #define ENC_INTERNALPULLUP		true

/* ENCODER 2 */
#define ENC2_BTNR 47  // S2
#define ENC2_BTNL 48  // S1
#define ENC2_BTNB 21  // KEY
// #define ENC2_INTERNALPULLUP	true

/* CLOCK MODUL RTC DS3132 */
 #define RTC_SCL			     7
 #define RTC_SDA			     8
 #define RTC_MODULE DS3231

/* REMOTE CONTROL INFRARED RECEIVER */
#define IR_PIN 38

// #define WAKE_PIN	42

/* DS CARD */
#define SDC_CS 18
#define SDSPISPEED      4000000     /* 4MHz - Slower speed to prevent display flicker on shared SPI bus */

/* Az inaktív szegmens megjelenítése az óra számaiban true -> engedélyez, false -> nem engedélyez. 
   Inactive segments of the clock, true or false. */
#define CLOCKFONT_MONO true

/* 12 órás időformátum definiálása. 
   Define 12-hour time format.*/
//#define AM_PM_STYLE

/* Bekapcsolja az eredeti hétszegmenses óra betűtípust.
   Turn on the original seven-segment font. */
//#define CLOCKFONT VT_DIGI_OLD

/* Google TTS hanggal mondja be az ídőt megadott nyelven és megadott percenként.
   Speaks the time using Google TTS voice in the specified language and every specified minute.
*/
#define CLOCK_TTS_ENABLED          false // Enabled (true) or disabled (false)
#define CLOCK_TTS_LANGUAGE         "HU"  // Language ( EN, HU, PL, NL, DE, RU, RO ,FR, GR)
#define CLOCK_TTS_INTERVAL_MINUTES 5    // Hány percenként mondja be. - How many times a minute does it say.

/* Ezzel a beállítással nincs görgetés az időjárás sávon.
   With this setting there is no scrolling on the weather bar.
*/
#define WEATHER_FMT_SHORT

/* Ezzel a beállítással a teljes időjárás jelentés megjelenik.
   With this setting, the full weather report is displayed.
*/
//#define EXT_WEATHER  true


/* Ezzel a beállítással a szél sebessége km/h lesz.
   With this setting, the wind speed will be km/h.
*/
//#define WIND_SPEED_IN_KMH


/* A VU méter két fajta kijelzési módot támogat.
BOOMBOX_STYLE stílusa, amikor középről két oldalra leng ki a kijelző. Azt itt tudod beállítani.
Ha a sor elején ott van // jel akkor az alap VU méter működik ami balról jobbra leng ki.
 The VU meter supports two types of display modes.
BOOMBOX_STYLE is the style when the display swings out from the center to two sides. You can set it here.
If there is a // sign at the beginning of the line, the basic VU meter is working, swinging out from left to right. 
*/
//#define BOOMBOX_STYLE

/* A VU méter végén megjelenik egy fehér csík a csúcsértékeknél, ha ezt bekapcsolod. A sor elején a // jel letíltja.
 A white bar will appear at the end of the VU meter at the peak values ​​if you enable this. The // at the beginning of the line will disable it.
*/
#define VU_PEAK

/* Az állomások listájából való választásnál nem kell megnyomni a rotary encoder gombját, kilépéskor autómatikusan
átvált a csatorna. (Zsigmond Becskeházi által)
 When selecting from the station list, you do not need to press the rotary encoder button, the channel will automatically
change when you exit. (By Zsigmond Becskehazi) */
#define DIRECT_CHANNEL_CHANGE

/* Mennyi idő múlva lépjen vissza a főképernyőre az állomások listájából. (másodperc)
 How long to return to the main screen from the station list. (seconds) */
#define STATIONS_LIST_RETURN_TIME 2

/* Az itt beállított pin vezérelheti egy audio erősítő tápellátását. Zenelejátszás közben a pin HIGH (magas) állapotban van ami meghúzza az
erősítő tápellátását kapcsoló relét. Amikor nincs zenelejátszás (STOP vagy a hangerő 0), a pin LOW (alacsony) állapotban van. 
Ez a változás akkor történik, amikor a képernyővédő "while not playing" üzemmódban bekapcsol. 
This pin controls the amplifier's power supply. When music is playing, the pin is set to HIGH to control the relay. 
When music is not playing (stopped or volume is 0), the pin is set to LOW. This change occurs when the screensaver is running. */
//#define PWR_AMP 2

#endif  //myoptions_h
