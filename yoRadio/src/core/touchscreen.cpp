#include "options.h"
#if (TS_MODEL != TS_MODEL_UNDEFINED) && (DSP_MODEL != DSP_DUMMY)
  #include "Arduino.h"
  #include "touchscreen.h"
  #include "config.h"
  #include "controls.h"
  #include "display.h"
  #include "player.h"
  #include "presets.h"
  #include "../displays/dspcore.h"  // DspCore + extern dsp

  #ifndef TS_X_MIN
    #define TS_X_MIN 400
  #endif
  #ifndef TS_X_MAX
    #define TS_X_MAX 3800
  #endif
  #ifndef TS_Y_MIN
    #define TS_Y_MIN 260
  #endif
  #ifndef TS_Y_MAX
    #define TS_Y_MAX 3800
  #endif
  #ifndef TS_STEPS
    #define TS_STEPS 40
  #endif

  #if TS_MODEL == TS_MODEL_XPT2046
    #ifdef TS_SPIPINS
SPIClass TSSPI(HSPI);
    #endif
    #include <XPT2046_Touchscreen.h>
XPT2046_Touchscreen ts(TS_CS);
typedef TS_Point TSPoint;
  #elif TS_MODEL == TS_MODEL_GT911
    #include "../GT911_Touchscreen/TAMC_GT911.h"
TAMC_GT911 ts = TAMC_GT911(TS_SDA, TS_SCL, TS_INT, TS_RST, 0, 0);
typedef TP_Point TSPoint;
  #endif

void TouchScreen::init(uint16_t w, uint16_t h) {

  #if TS_MODEL == TS_MODEL_XPT2046
    #ifdef TS_SPIPINS
  TSSPI.begin(TS_SPIPINS);
  ts.begin(TSSPI);
    #else
      #if TS_HSPI
  ts.begin(SPI2);
      #else
  ts.begin();
      #endif
    #endif
  ts.setRotation(config.store.fliptouch ? 3 : 1);
  #endif
  #if TS_MODEL == TS_MODEL_GT911
  ts.begin();
  ts.setRotation(config.store.fliptouch ? 0 : 2);
  #endif
  _width = w;
  _height = h;
  #if TS_MODEL == TS_MODEL_GT911
  ts.setResolution(_width, _height);
  #endif
}

tsDirection_e TouchScreen::_tsDirection(uint16_t x, uint16_t y) {
  int16_t dX = x - _oldTouchX;
  int16_t dY = y - _oldTouchY;
  if (abs(dX) > 20 || abs(dY) > 20) {
    if (abs(dX) > abs(dY)) {
      if (dX > 0) {
        return TSD_RIGHT;
      } else {
        return TSD_LEFT;
      }
    } else {
      if (dY > 0) {
        return TSD_DOWN;
      } else {
        return TSD_UP;
      }
    }
  } else {
    return TDS_REQUEST;
  }
}

void TouchScreen::flip() {
  #if TS_MODEL == TS_MODEL_XPT2046
  ts.setRotation(config.store.fliptouch ? 3 : 1);
  #endif
  #if TS_MODEL == TS_MODEL_GT911
  ts.setRotation(config.store.fliptouch ? 0 : 2);
  #endif
}

void TouchScreen::loop() {
  uint16_t touchX, touchY;
  static uint32_t touchLongPress;
  static tsDirection_e direct;
  static uint16_t touchVol, touchStation;
  static uint32_t presetsLastActivity = 0;
  static int presetActionDone = 0;  // 0=play,1=save,2=del
  static int presetHoldSlot = -1;
  static int favHold = -1;
  static bool favLongTriggered = false;

  static bool lastStTouched = false;

  if (!_checklpdelay(20, _touchdelay)) {
    return;
  }

  // ---- touch debounce ----
  bool istouched = _istouched();
  static uint32_t lastTouchMs = 0;
  if (istouched) {
    lastTouchMs = millis();
  }
  // tekintsük érintettnek még 120ms-ig a legutóbbi touch után
  bool stTouched = istouched || ((uint32_t)(millis() - lastTouchMs) < 120);

  bool newTouch = (stTouched && !lastStTouched);
  bool endTouch = (!stTouched && lastStTouched);

  #if TS_MODEL == TS_MODEL_GT911
  ts.read();
  #endif
  // Auto-exit presets screen after 15s of no touch activity
  if (display.mode() == PRESETS) {
    if (presets_toastExpired()) {
      //presets_drawScreen();  // ⬅ KÉNYSZER REDRAW
      return;
    }
    if (presetsLastActivity == 0) {
      presetsLastActivity = millis();
    }
    if (stTouched) {
      presetsLastActivity = millis();
    } else if ((uint32_t)(millis() - presetsLastActivity) > 15000UL) {
      display.putRequest(NEWMODE, PLAYER);
      presetsLastActivity = 0;
      direct = TSD_STAY;
      return;
    }
  } else {
    presetsLastActivity = 0;
  }
  if (stTouched) {
  #if TS_MODEL == TS_MODEL_XPT2046
    TSPoint p = ts.getPoint();
    touchX = map(p.x, TS_X_MIN, TS_X_MAX, 0, _width);
    touchY = map(p.y, TS_Y_MIN, TS_Y_MAX, 0, _height);
  #elif TS_MODEL == TS_MODEL_GT911
    TSPoint p = ts.points[0];
    touchX = p.x;
    touchY = p.y;
  #endif
    if (newTouch) { /************************** START TOUCH *************************************/
      _oldTouchX = touchX;
      _oldTouchY = touchY;
      touchVol = touchX;
      touchStation = touchY;
      direct = TDS_REQUEST;
      touchLongPress = millis();
      if (display.mode() == PRESETS && !presets_keyboardActive()) {
        presetHoldSlot = presets_hitTest(touchX, touchY);
        presetActionDone = 0;
        presets_setPressedSlot(presetHoldSlot);
        presets_drawPressed();
        favHold = presets_hitTestFav(touchX, touchY);
        favLongTriggered = false;
      }
    } else { /*     HOLD / SWIPE TOUCH     */
      if (display.mode() == PRESETS) {
        direct = TDS_REQUEST;  // csak tartjuk, nincs swipe
      } else {
        direct = _tsDirection(touchX, touchY);
        switch (direct) {
          case TSD_LEFT:
          case TSD_RIGHT:
          {
            touchLongPress = millis();
            if (display.mode() == PLAYER || display.mode() == VOL) {
              int16_t xDelta = map(abs(touchVol - touchX), 0, _width, 0, TS_STEPS);
              display.putRequest(NEWMODE, VOL);
              if (xDelta > 1) {
                controlsEvent((touchVol - touchX) < 0);
                touchVol = touchX;
              }
            }
            break;
          }
          case TSD_UP:
          case TSD_DOWN:
          {
            touchLongPress = millis();
            if (display.mode() == PLAYER || display.mode() == STATIONS) {
              int16_t yDelta = map(abs(touchStation - touchY), 0, _height, 0, TS_STEPS);
              display.putRequest(NEWMODE, STATIONS);
              if (yDelta > 1) {
                controlsEvent((touchStation - touchY) < 0);
                touchStation = touchY;
              }
            }
            break;
          }
          default: break;
        }
      }
    }

    // ********************************** PRESETS HOLD (real-time) ************************************
    // ---- FAV HOLD (real-time) ----FAV buttons bar (bottom): long press = rename
    if (display.mode() == PRESETS && !presets_keyboardActive() && favHold >= 0 && !favLongTriggered) {
      uint32_t held = millis() - touchLongPress;
      if (held >= BTN_PRESS_TICKS * 2) {
        presets_keyboardOpen((uint8_t)favHold);
        presets_drawScreen();
        favLongTriggered = true;
        direct = TSD_STAY;
      }
    }

    if (display.mode() == PRESETS && !presets_keyboardActive() && presetHoldSlot >= 0) {
      uint32_t held = millis() - touchLongPress;
      presets_drawHoldBar(held);  // fejléc csík
      presets_setPressedSlot(presetHoldSlot);
      presets_drawPressed();

      if (held >= BTN_PRESS_TICKS * 4) {   // 2000
        presetActionDone = 2;  // DEL
      } else if (held >= BTN_PRESS_TICKS * 2) {  // 1000
        presetActionDone = 1;  // SAVE
      } else {
        presetActionDone = 0;  // PLAY
      }
    }

    if (config.store.dbgtouch) {
      Serial.print(", x = ");
      Serial.print(p.x);
      Serial.print(", y = ");
      Serial.println(p.y);
    }
  } else {
    if (endTouch) { /**************************** END TOUCH *********************************/
      if (direct == TDS_REQUEST) {
        uint32_t pressTicks = millis() - touchLongPress;
        Serial.printf(
          "END TOUCH: x=%u y=%u press=%lu holdSlot=%d action=%d display.mode=%d\n", _oldTouchX, _oldTouchY, pressTicks, presetHoldSlot, presetActionDone,
          display.mode()
        );
        presets_clearPressed();
        presets_setPressedSlot(-1);

        // 1) Open presets from PLAYER by tapping top bar
        if (dsp.width() == 480 && dsp.height() == 320) {
          if (display.mode() == PLAYER && _oldTouchY < 50 && pressTicks > 50 && pressTicks < BTN_PRESS_TICKS * 2) {
            display.putRequest(NEWMODE, PRESETS);
            presetsLastActivity = millis();
            presetHoldSlot = -1;
            presetActionDone = 0;
            touchLongPress = 0;
            direct = TSD_STAY;
            return;
          }
        }

        // 2) Close presets (back) by tapping top bar
        if (display.mode() == PRESETS && _oldTouchY < 50 && pressTicks > 50) {
          display.putRequest(NEWMODE, PLAYER);
          presetsLastActivity = 0;
          direct = TSD_STAY;
          return;
        }

        // 3) Interact with presets UI (keyboard / FAV bar / preset grid)
        if (display.mode() == PRESETS) {
          // 3a) On-screen keyboard (edit FAV label)
          if (presets_keyboardActive()) {
            if (pressTicks > 50) {
              // presets_keyboardTap() handles redraw (keyboard stays or closes)
              presets_keyboardTap(_oldTouchX, _oldTouchY);
              presetsLastActivity = millis();
            }
            direct = TSD_STAY;
            return;
          } else {

            // 3b) FAV buttons bar (bottom): tap = select bank.
            int fav = presets_hitTestFav(_oldTouchX, _oldTouchY);
            if (fav >= 0) {
              if (!favLongTriggered && pressTicks > 50) {
                presets_selectBank((uint8_t)fav);
              }
              presets_drawScreen();
              presetsLastActivity = millis();
              direct = TSD_STAY;
              return;
            }

            // 3c) Preset grid (buttons)
            int hit = presets_hitTest(_oldTouchX, _oldTouchY);
            if (hit >= 0) {
              if (presetActionDone == 2) {
                presets_clear((uint8_t)hit);
                presetsLastActivity = millis();
              } else if (presetActionDone == 1) {
                presets_save((uint8_t)hit);
                presetsLastActivity = millis();
              } else {
                bool stay = presets_play((uint8_t)hit);
                if (!stay) {
                  display.putRequest(NEWMODE, PLAYER);
                  presetsLastActivity = 0;
                } else {
                  presetsLastActivity = millis();
                }
              }

              presetHoldSlot = -1;
              presetActionDone = 0;
              direct = TSD_STAY;
              return;
            }
          }
        }
      }

      if (direct == TDS_REQUEST) {
        uint32_t pressTicks = millis() - touchLongPress;
        if (pressTicks < BTN_PRESS_TICKS * 2) {
          if (pressTicks > 200) {  // Érintési zajok kiszűrése 200ms alatt nem lesz STOP
            onBtnClick(EVT_BTNCENTER);
          }
        } else {
          display.putRequest(NEWMODE, display.mode() == PLAYER ? STATIONS : PLAYER);
        }
      }
      direct = TSD_STAY;
    }
  }
  lastStTouched = stTouched;
}

bool TouchScreen::_checklpdelay(int m, uint32_t &tstamp) {
  if (millis() - tstamp > m) {
    tstamp = millis();
    return true;
  } else {
    return false;
  }
}

bool TouchScreen::_istouched() {
  #if TS_MODEL == TS_MODEL_XPT2046
  return ts.touched();
  #elif TS_MODEL == TS_MODEL_GT911
  return ts.isTouched;
  #endif
}

#endif  // TS_MODEL!=TS_MODEL_UNDEFINED
