// Módosítva. "vol_step"
#include "Arduino.h"
#include "options.h"
#include "config.h"
#include "controls.h"

#include "player.h"
#include "display.h"
#include "network.h"
#include "netserver.h"
#include "driver/rtc_io.h"
#include "../pluginsManager/pluginsManager.h"

long encOldPosition = 0;
long enc2OldPosition = 0;
int  lpId = -1;

uint32_t wakeCheckUntil = 0;
bool     waitingForWakeIr = false;

#if DSP_MODEL == DSP_DUMMY
#    define DUMMYDISPLAY
#endif

#define ISPUSHBUTTONS BTN_LEFT != 255 || BTN_CENTER != 255 || BTN_RIGHT != 255 || ENC_BTNB != 255 || BTN_UP != 255 || BTN_DOWN != 255 || ENC2_BTNB != 255 || BTN_MODE != 255
#if ISPUSHBUTTONS
#    include "../OneButton/OneButton.h"
OneButton         button[]{{BTN_LEFT, true, BTN_INTERNALPULLUP}, {BTN_CENTER, true, BTN_INTERNALPULLUP}, {BTN_RIGHT, true, BTN_INTERNALPULLUP},  {ENC_BTNB, true, ENC_INTERNALPULLUP},
                           {BTN_UP, true, BTN_INTERNALPULLUP},   {BTN_DOWN, true, BTN_INTERNALPULLUP},   {ENC2_BTNB, true, ENC2_INTERNALPULLUP}, {BTN_MODE, true, BTN_INTERNALPULLUP}};
constexpr uint8_t nrOfButtons = sizeof(button) / sizeof(button[0]);
#endif

#if ENC_HALFQUARD == false
#    define ENCODER_STEPS 4
#elif ENC_HALFQUARD == true
#    define ENCODER_STEPS 2
#elif ENC_HALFQUARD == 255
#    define ENCODER_STEPS 1
#endif
#if ENC2_HALFQUARD == false
#    define ENCODER2_STEPS 4
#elif ENC2_HALFQUARD == true
#    define ENCODER2_STEPS 2
#elif ENC2_HALFQUARD == 255
#    define ENCODER2_STEPS 1
#endif

#if (ENC_BTNL != 255 && ENC_BTNR != 255) || (ENC2_BTNL != 255 && ENC2_BTNR != 255)
#    include "../yoEncoder/yoEncoder.h"
#    if (ENC_BTNL != 255 && ENC_BTNR != 255)
yoEncoder encoder = yoEncoder(ENC_BTNL, ENC_BTNR, ENCODER_STEPS, ENC_INTERNALPULLUP);
#    endif
#    if (ENC2_BTNL != 255 && ENC2_BTNR != 255)
yoEncoder encoder2 = yoEncoder(ENC2_BTNL, ENC2_BTNR, ENCODER2_STEPS, ENC2_INTERNALPULLUP);
#    endif
#endif

#if (TS_MODEL != TS_MODEL_UNDEFINED) && (DSP_MODEL != DSP_DUMMY)
#    include "touchscreen.h"
TouchScreen touchscreen;
#endif

#if IR_PIN != 255
#    include <assert.h>

#    include "../IRremoteESP8266/IRrecv.h"
#    include "../IRremoteESP8266/IRremoteESP8266.h"
#    include "../IRremoteESP8266/IRac.h"
#    include "../IRremoteESP8266/IRtext.h"
#    include "../IRremoteESP8266/IRutils.h"
uint8_t irVolRepeat = 0;
// const uint16_t kCaptureBufferSize = 1024;
const uint16_t kMinUnknownSize = 12;
#    define LEGACY_TIMING_INFO false

IRrecv         irrecv(IR_PIN, IR_BUFSIZE, IR_TIMEOUT, true);
decode_results irResults;
#endif

#if ENC_BTNL != 255
void IRAM_ATTR readEncoderISR() {
    if ((SDC_CS == 255 && display.mode() == LOST) || display.mode() == UPDATING) return;
    encoder.readEncoder_ISR();
}
#endif
#if ENC2_BTNL != 255
void IRAM_ATTR readEncoder2ISR() {
    if ((SDC_CS == 255 && display.mode() == LOST) || display.mode() == UPDATING) return;
    encoder2.readEncoder_ISR();
}
#endif

void initControls() {
    Serial.printf("Wake cause: %d\n", esp_sleep_get_wakeup_cause());
    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {

        Serial.println("Wake from IR");

        waitingForWakeIr = true;
        wakeCheckUntil = millis() + 600;
    }
#if ENC_BTNL != 255
    encoder.begin();
    encoder.setup(readEncoderISR);
    encoder.setBoundaries(0, 254, true);
    encoder.setAcceleration(config.store.encacc);
#endif
#if ENC2_BTNL != 255
    encoder2.begin();
    encoder2.setup(readEncoder2ISR);
    encoder2.setBoundaries(0, 254, true);
    encoder2.setAcceleration(config.store.encacc);
#endif

#if ISPUSHBUTTONS
    for (int i = 0; i < nrOfButtons; i++) {
        if ((i == 0 && BTN_LEFT == 255) || (i == 1 && BTN_CENTER == 255) || (i == 2 && BTN_RIGHT == 255) || (i == 3 && ENC_BTNB == 255) || (i == 4 && BTN_UP == 255) || (i == 5 && BTN_DOWN == 255) ||
            (i == 6 && ENC2_BTNB == 255) || (i == 7 && BTN_MODE == 255))
            continue;
        button[i].attachClick([](void* p) { onBtnClick((int)p); }, (void*)i);
        button[i].attachDoubleClick([](void* p) { onBtnDoubleClick((int)p); }, (void*)i);
        button[i].attachLongPressStart([](void* p) { onBtnLongPressStart((int)p); }, (void*)i);
        button[i].attachLongPressStop([](void* p) { onBtnLongPressStop((int)p); }, (void*)i);
        button[i].setClickTicks(BTN_CLICK_TICKS);
        button[i].setPressTicks(BTN_PRESS_TICKS);
    }
#endif
#if (TS_MODEL != TS_MODEL_UNDEFINED) && (DSP_MODEL != DSP_DUMMY)
    touchscreen.init(display.width(), display.height());
#endif
#if IR_PIN != 255
    pinMode(IR_PIN, INPUT);
    assert(irutils::lowLevelSanityCheck() == 0);
#    if DECODE_HASH
    irrecv.setUnknownThreshold(kMinUnknownSize);
#    endif // DECODE_HASH
    irrecv.setTolerance(config.store.irtlp);
    irrecv.enableIRIn();
#endif // IR_PIN!=255
}

void loopControls() {
    if (display.mode() == UPDATING || display.mode() == SDCHANGE) return;
    if (SDC_CS == 255 && display.mode() == LOST) return;
    if (ctrls_on_loop) ctrls_on_loop();
#if ENC_BTNL != 255
    encoder1Loop();
#endif
#if ENC2_BTNL != 255
    encoder2Loop();
#endif
#if ISPUSHBUTTONS
    for (unsigned i = 0; i < nrOfButtons; i++) {
        if ((i == 0 && BTN_LEFT == 255) || (i == 1 && BTN_CENTER == 255) || (i == 2 && BTN_RIGHT == 255) || (i == 3 && ENC_BTNB == 255) || (i == 4 && BTN_UP == 255) || (i == 5 && BTN_DOWN == 255) ||
            (i == 6 && ENC2_BTNB == 255))
            continue;
        button[i].tick();
        if (lpId >= 0) {
            if (DSP_MODEL == DSP_DUMMY && (lpId == 4 || lpId == 5)) continue;
            onBtnDuringLongPress(lpId);
        }
    }
#endif
#if IR_PIN != 255
    irLoop();
#endif
#if (TS_MODEL != TS_MODEL_UNDEFINED) && (DSP_MODEL != DSP_DUMMY)
    if (network.status == CONNECTED || network.status == SDREADY) touchscreen.loop();
#endif
}

#if ENC_BTNL != 255 || ENC2_BTNL != 255
void encodersLoop(yoEncoder* enc, bool first) {
    if (network.status != CONNECTED && network.status != SDREADY) { return; }
    if (display.mode() == LOST) { return; }
    int8_t delta = enc->encoderChanged();
    if (delta == 0) { return; }
#    if defined(DUMMYDISPLAY) && !defined(USE_NEXTION)
    // ===== Dummy display =====
    uint8_t btnState = digitalRead(first ? ENC_BTNB : ENC2_BTNB);
    bool    volumeMode = first ? btnState : !btnState;
    if (volumeMode) {
        int nv = config.store.volume + delta;
        if (nv < 0) { nv = 0; }
        if (nv > 254) { nv = 254; }
        player.setVol((uint8_t)nv);
    } else {
        if (delta > 0) {
            player.next();
        } else {
            player.prev();
        }
    }
#    else
#        if defined(ENCODERS_INDEPENDENT)
    // ==========================================================
    //                TWO INDEPENDENT ENCODERS
    // ==========================================================
    if (first) {
        // ----- Encoder 1 → Volume -----
        if (display.mode() != VOL) { display.putRequest(NEWMODE, VOL); }
        int nv = config.store.volume + delta * config.store.volsteps;
        if (nv < 0) { nv = 0; }
        if (nv > 100) { nv = 100; }
        player.setVol((uint8_t)nv);
        config.screensaverTicks = 0;
    } else {
        // ----- Encoder 2 → Stations -----
        // If not already in STATIONS → switch and exit (no blocking!)
        if (display.mode() != STATIONS) {
            display.putRequest(NEWMODE, STATIONS);
            return;
        }
        // Ignore acceleration — use direction only
        int      step = (delta > 0) ? 1 : -1;
        int      p = display.currentPlItem + step;
        uint16_t cs = config.playlistLength();
        if (p < 1)
            p = cs;
        else if (p > cs)
            p = 1;
        display.currentPlItem = p;
        display.resetQueue();
        display.putRequest(DRAWPLAYLIST, p);
        config.screensaverTicks = 0;
    }
#        else
    // ==========================================================
    //                  ORIGINAL STANDARD LOGIC
    // ==========================================================
    if (first) {
        controlsEvent(delta > 0, delta);
    } else {
        uint8_t btnState = digitalRead(first ? ENC_BTNB : ENC2_BTNB);
        if (btnState == HIGH && display.mode() == PLAYER) {
            if (config.store.skipPlaylistUpDown) {
                if (delta > 0) {
                    player.next();
                } else {
                    player.prev();
                }
                return;
            }
            display.putRequest(NEWMODE, STATIONS);
            return; // removed blocking wait
        }
        controlsEvent(delta > 0, delta);
    }
#        endif
#    endif
}
#endif // ENC_BTNL!=255 || ENC2_BTNL!=255

#if ENC_BTNL != 255
void encoder1Loop() {
    encodersLoop(&encoder, true);
}
#endif

#if ENC2_BTNL != 255
void encoder2Loop() {
    encodersLoop(&encoder2, false);
}
#endif

#if IR_PIN != 255
void irBlink() {
    if (REAL_LEDBUILTIN == 255) return;
    if (player.status() == STOPPED) {
        for (uint8_t i = 0; i < 7; i++) {
            digitalWrite(REAL_LEDBUILTIN, !digitalRead(REAL_LEDBUILTIN));
            delay(100);
        }
    }
}

void irNumber(uint8_t num) {
    uint16_t s;
    if (display.numOfNextStation == 0 && num == 0) return;
    display.putRequest(NEWMODE, NUMBERS);
    if (display.numOfNextStation > UINT16_MAX / 10) return;
    s = display.numOfNextStation * 10 + num;
    if (s > config.playlistLength()) return;
    display.numOfNextStation = s;
    display.putRequest(NEXTSTATION, s);
}

void irBackspace() {
    uint16_t s = display.numOfNextStation;
    if (s == 0) {
        display.putRequest(NEWMODE, PLAYER);
        return;
    }
    s = s / 10;
    display.numOfNextStation = s;
    display.putRequest(NEWMODE, NUMBERS);
    if (s > 0) {
        display.putRequest(NEXTSTATION, s);
    } else {
        display.putRequest(NEXTSTATION, 0);
    }
}
/*----- IR wake-up, SETUP() hívja, ha nem POWER kód, akkor visszaaltat. -----*/
void irWakeup() {
    auto wake = esp_sleep_get_wakeup_cause();
    if (wake == ESP_SLEEP_WAKEUP_EXT1) {
        uint64_t mask = esp_sleep_get_ext1_wakeup_status();
        if (mask & (1ULL << IR_PIN)) {
            Serial.println("IR wake → waiting for POWER code");
            rtc_gpio_deinit((gpio_num_t)IR_PIN); // RTC to GPIO
            pinMode(IR_PIN, INPUT);
            irrecv.enableIRIn();
            bool          valid = false;
            unsigned long start = millis();
            unsigned long timeout = 800; // alap ablak
            while (millis() - start < timeout) {
                if (irrecv.decode(&irResults)) {
                    uint32_t code = irResults.value;
                    // repeat → csak idő hosszabbítás
                    if (code == 0xFFFFFFFF) {
                        timeout = 800;
                        irrecv.resume();
                        continue;
                    }
                    // csak valódi kódokat vizsgálunk
                    if (isIrPowerCode(code)) {
                        Serial.printf("POWER: 0x%lX\n", code);
                        valid = true;
                        break;
                    } else {
                        Serial.printf("Not POWER: 0x%lX\n", code);
                    }
                }
            }
            if (!valid) {
                Serial.println("No POWER → sleep again");
                Serial.flush();
                delay(20);
                config.doSleepW();
            }
            Serial.println("POWER → starting up");
        }
    }
}

/*----- Összehasonlít a 3 tárolt POWER kóddal -----*/
bool isIrPowerCode(uint32_t code) {
    for (int i = 0; i < 3; i++) {
        Serial.printf("Stored[%d]: 0x%08lX\n", i, (uint32_t)config.ircodes.irVals[IR_POWER][i]);
        if (config.ircodes.irVals[IR_POWER][i] == code) { return true; }
    }
    return false;
}

void irLoop() {
    if (irrecv.decode(&irResults)) {
        if (irResults.value < 256) { return; }
        if (netserver.irRecordEnable) {
            IRCommand ircmd;
            while (xQueueReceive(irQueue, &ircmd, 0)) {
                if (ircmd.hasBtnId) {
                    config.irBtnId = ircmd.irBtnId;
                    Serial.printf("controls.cpp--> irLoop--> xQueueReceive: update config.irBtnId: %d \n", config.irBtnId);
                }
                if (ircmd.hasBank) {
                    config.irBankId = ircmd.irBankId;
                    Serial.printf("controls.cpp--> irLoop--> xQueueReceive: update config.irBankId: %d \n", config.irBankId);
                }
            }
            Serial.print(resultToHumanReadableBasic(&irResults));
            Serial.println("-------------------------------");
            config.ircodes.irVals[config.irBtnId][config.irBankId] = irResults.value;
            // Serial.printf("irLoop run on core: %d\n", xPortGetCoreID());
            Serial.printf("controls.cpp-->irLoop--> irResults.value: %08llX, config.irBtnId: %d, config.irBankId: %d\n\n", irResults.value, config.irBtnId, config.irBankId);
            netserver.irToWs(typeToString(irResults.decode_type, irResults.repeat).c_str(), irResults.value);
            return;
        }
        // if (!irResults.repeat /* && irResults.command!=0*/) { irVolRepeat = 0; }
        if (irResults.repeat) {
            switch (irVolRepeat) {
                case 1: controlsEvent(display.mode() == STATIONS ? false : true); break;

                case 2: controlsEvent(display.mode() == STATIONS ? true : false); break;
            }
            irrecv.resume(); // ← add (MB)
            return;          // ← add (MB)
        } else {
            irVolRepeat = 0;
        }
        for (int target = 0; target < 19; target++) {
            for (int j = 0; j < 3; j++) {
                // Serial.printf("Comparing with target %d, slot %d: 0x%08lX == 0x%08lX\n", target, j, (uint32_t)config.ircodes.irVals[target][j], (uint32_t)irResults.value);
                if (config.ircodes.irVals[target][j] == irResults.value) {
                    if (network.status != CONNECTED && network.status != SDREADY && target != IR_MODE) return;
                    if (target != IR_MODE && display.mode() == LOST) return;
                    if (display.mode() == SCREENSAVER || display.mode() == SCREENBLANK) {
                        display.putRequest(NEWMODE, PLAYER);
                        return;
                    }
                    switch (target) {
                        case IR_POWER: {
                            Serial.println("IR POWER -> sleep");
                            display.putRequest(NEWMODE, SLEEPING);
                            break;
                        }
                        case IR_PLAY_STOP: {
                            irBlink();
                            if (display.mode() == NUMBERS) {
                                display.putRequest(NEWMODE, PLAYER);
                                player.sendCommand({PR_PLAY, display.numOfNextStation});
                                display.numOfNextStation = 0;
                                break;
                            }
                            onBtnClick(1);
                            break;
                        }
                        case IR_BACK: {
                            if (display.mode() == STATIONS) display.putRequest(NEWMODE, PLAYER);
                            if (display.mode() == NUMBERS) irBackspace();
                            break;
                        }
                        case IR_PREV: {
                            if (display.mode() == STATIONS) {
                                controlsEvent(true);
                                break;
                            } else {
                                player.prev();
                            }
                            break;
                        }
                        case IR_NEXT: {
                            if (display.mode() == STATIONS) {
                                controlsEvent(false);
                                break;
                            } else {
                                player.next();
                            }
                            break;
                        }
                        case IR_LIST: {
                            if (display.mode() == NUMBERS) {
                                display.putRequest(NEWMODE, PLAYER);
                                display.numOfNextStation = 0;
                                break;
                            }
                            display.putRequest(NEWMODE, display.mode() == PLAYER ? STATIONS : PLAYER);
                            break;
                        }
                        case IR_VOLUME_DOWN: {
                            controlsEvent(display.mode() == STATIONS ? true : false);
                            irVolRepeat = 2;
                            break;
                        }
                        case IR_VOLUME_UP: {
                            controlsEvent(display.mode() == STATIONS ? false : true);
                            irVolRepeat = 1;
                            break;
                        }
                        case IR_MODE: {
                            onBtnClick(EVT_BTNMODE);
                            break;
                        }
                        case IR_0: {
                            irNumber(0);
                            break;
                        }
                        case IR_1: {
                            irNumber(1);
                            break;
                        }
                        case IR_2: {
                            irNumber(2);
                            break;
                        }
                        case IR_3: {
                            irNumber(3);
                            break;
                        }
                        case IR_4: {
                            irNumber(4);
                            break;
                        }
                        case IR_5: {
                            irNumber(5);
                            break;
                        }
                        case IR_6: {
                            irNumber(6);
                            break;
                        }
                        case IR_7: {
                            irNumber(7);
                            break;
                        }
                        case IR_8: {
                            irNumber(8);
                            break;
                        }
                        case IR_9: {
                            irNumber(9);
                            break;
                        }

                    } /* switch (target) */
                    target = 19;
                    break;
                } /* if(config.ircodes.irVals[target][j]==irResults.value) */
            } /* for(int j=0; j<3; j++) */
        } /* for(int target=0; target<19; target++) */
    } /* if (irrecv.decode(&irResults)) */
}
#endif // if IR_PIN!=255

void onBtnLongPressStart(int id) {
    switch ((controlEvt_e)id) {
        case EVT_BTNLEFT:
        case EVT_BTNRIGHT:
        case EVT_BTNUP:
        case EVT_BTNDOWN: {
            lpId = id;
            break;
        }
        case EVT_BTNCENTER:
        case EVT_ENCBTNB: {
#if defined(DUMMYDISPLAY) && !defined(USE_NEXTION)
            break;
#endif
#if !defined(ENCODERS_INDEPENDENT)
            // legacy: toggle PLAYER <-> STATIONS
            display.putRequest(NEWMODE, display.mode() == PLAYER ? STATIONS : PLAYER);
#endif
            break;
        }
        case EVT_ENC2BTNB: {
#if defined(DUMMYDISPLAY) && !defined(USE_NEXTION)
            break;
#endif
#if !defined(ENCODERS_INDEPENDENT)
            // legacy: toggle PLAYER <-> VOLUME
            display.putRequest(NEWMODE, display.mode() == PLAYER ? VOL : PLAYER);
#endif
            break;
        }
        case EVT_BTNMODE: {
            display.putRequest(NEWMODE, SLEEPING);
            break;
        }
        default: break;
    }
}

void onBtnLongPressStop(int id) {
    switch ((controlEvt_e)id) {
        case EVT_BTNLEFT:
        case EVT_BTNRIGHT:
        case EVT_BTNUP:
        case EVT_BTNDOWN: {
            lpId = -1;
            break;
        }
        case EVT_BTNMODE:
        default: break;
    }
}

unsigned long lpdelay;
boolean       checklpdelay(int m, unsigned long& tstamp) {
    if (millis() - tstamp > m) {
        tstamp = millis();
        return true;
    } else {
        return false;
    }
}

void onBtnDuringLongPress(int id) {
    if (network.status != CONNECTED && network.status != SDREADY) return;
    if (checklpdelay(BTN_LONGPRESS_LOOP_DELAY, lpdelay)) {
        switch ((controlEvt_e)id) {
            case EVT_BTNLEFT: {
                controlsEvent(false);
                break;
            }
            case EVT_BTNRIGHT: {
                controlsEvent(true);
                break;
            }
            case EVT_BTNUP:
            case EVT_BTNDOWN: {
                if (display.mode() == PLAYER) { display.putRequest(NEWMODE, STATIONS); }
                if (display.mode() == STATIONS) { controlsEvent(id == EVT_BTNDOWN); }
                break;
            }
            default: break;
        }
    }
}

void controlsEvent(bool toRight, int8_t volDelta) {
    if (display.mode() == NUMBERS) {
        display.numOfNextStation = 0;
        display.putRequest(NEWMODE, PLAYER);
    }
    if (display.mode() != STATIONS) {
#if !defined(DUMMYDISPLAY) || defined(USE_NEXTION)
        display.putRequest(NEWMODE, VOL); // Hangerő képernyőre vált.
#endif
        if (volDelta != 0) {
            int nv = config.store.volume + volDelta * config.store.volsteps;
            if (nv < 0) nv = 0;
            if (nv > 100) // Módosítva. "vol_step"
                nv = 100;
            player.setVol((uint8_t)nv);
        } else {
            player.stepVol(toRight);
        }
    }
    if (display.mode() == STATIONS) {
        display.resetQueue();
        int      p = toRight ? display.currentPlItem + 1 : display.currentPlItem - 1;
        uint16_t cs = config.playlistLength();
        if (p < 1) p = cs;
        if (p > cs) p = 1;
        display.currentPlItem = p;
        display.putRequest(DRAWPLAYLIST, p);
    }
}

void onBtnClick(int id) {
    bool         passBnCenter = (controlEvt_e)id == EVT_BTNCENTER || (controlEvt_e)id == EVT_ENCBTNB || (controlEvt_e)id == EVT_ENC2BTNB;
    controlEvt_e btnid = static_cast<controlEvt_e>(id);
    pm.on_btn_click(btnid);
    if (network.status != CONNECTED && network.status != SDREADY && (controlEvt_e)id != EVT_BTNMODE && !passBnCenter) return;
    switch (btnid) {
        case EVT_BTNLEFT: {
            controlsEvent(false);
            break;
        }
        case EVT_BTNCENTER:
        case EVT_ENCBTNB:
        case EVT_ENC2BTNB: {
            if (display.mode() == NUMBERS) {
                display.numOfNextStation = 0;
                display.putRequest(NEWMODE, PLAYER);
            }
            if (display.mode() == PLAYER) { player.toggle(); }
            if (display.mode() == SCREENSAVER || display.mode() == SCREENBLANK) { display.putRequest(NEWMODE, PLAYER); }
            if (display.mode() == STATIONS) {
                display.putRequest(NEWMODE, PLAYER);
                if (config.lastStation() != display.currentPlItem) { display.putRequest(CLOSEPLAYLIST, display.currentPlItem); }
            }
            if (network.status == SOFT_AP || display.mode() == LOST) {
#ifdef USE_SD
                config.changeMode();
#endif
            }
            break;
        }
        case EVT_BTNRIGHT: {
            controlsEvent(true);
            break;
        }
        case EVT_BTNUP:
        case EVT_BTNDOWN: {
            if (DSP_MODEL == DSP_DUMMY) {
                if (id == EVT_BTNUP) {
                    player.next();
                } else {
                    player.prev();
                }
            } else {
                if (display.mode() == PLAYER) {
                    if (config.store.skipPlaylistUpDown || ENC2_BTNL != 255) {
                        if (id == EVT_BTNUP) {
                            player.prev();
                        } else {
                            player.next();
                        }
                    } else {
                        display.putRequest(NEWMODE, STATIONS);
                    }
                }
                if (display.mode() == STATIONS) { controlsEvent(id == EVT_BTNDOWN); }
            }
            break;
        }
#ifdef USE_SD
        case EVT_BTNMODE: {
            config.changeMode();
            break;
        }
#endif
        default: break;
    }
}

void onBtnDoubleClick(int id) {
    if (display.mode() == SCREENSAVER || display.mode() == SCREENBLANK) {
        display.putRequest(NEWMODE, PLAYER);
        return;
    }
    switch ((controlEvt_e)id) {
        case EVT_BTNLEFT: {
            if (display.mode() != PLAYER) return;
            if (network.status != CONNECTED && network.status != SDREADY) return;
            player.prev();
            break;
        }
        case EVT_BTNCENTER:
        case EVT_ENCBTNB:
        case EVT_ENC2BTNB: {
            onBtnClick(EVT_BTNMODE);
            break;
        }
        case EVT_BTNRIGHT: {
            if (display.mode() != PLAYER) return;
            if (network.status != CONNECTED && network.status != SDREADY) return;
            player.next();
            break;
        }
        default: break;
    }
}

void setIRTolerance(uint8_t tl) {
    config.saveValue(&config.store.irtlp, tl);
#if IR_PIN != 255
    irrecv.setTolerance(config.store.irtlp);
#endif
}
void setEncAcceleration(uint16_t acc) {
    config.saveValue(&config.store.encacc, acc);
#if ENC_BTNL != 255
    encoder.setAcceleration(config.store.encacc);
#endif
#if ENC2_BTNL != 255
    encoder2.setAcceleration(config.store.encacc);
#endif
}
void flipTS() {
#if (TS_MODEL != TS_MODEL_UNDEFINED) && (DSP_MODEL != DSP_DUMMY)
    touchscreen.flip();
#endif
}