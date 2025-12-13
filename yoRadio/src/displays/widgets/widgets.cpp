// Módosítva! v0.9.710 "bitrate"
#include "../../core/options.h"
#if DSP_MODEL != DSP_DUMMY
  #ifdef NAMEDAYS_FILE
    #include "../../core/namedays.h"
  #endif

  #include "../../core/config.h"
  #include "../../core/network.h"  //  for Clock widget
  #include "../../core/player.h"   //  for VU widget
  #include "../dspcore.h"
  #include "../tools/l10n.h"
  #include "../tools/psframebuffer.h"
  #include "Arduino.h"
  #include "widgets.h"

/************************
      FILL WIDGET
 ************************/
void FillWidget::init(FillConfig conf, uint16_t bgcolor) {
  Widget::init(conf.widget, bgcolor, bgcolor);
  _width = conf.width;
  _height = conf.height;
}

void FillWidget::_draw() {
  if (!_active) {
    return;
  }
  dsp.fillRect(_config.left, _config.top, _width, _height, _bgcolor);
}

void FillWidget::setHeight(uint16_t newHeight) {
  _height = newHeight;
  //_draw();
}
/************************
      TEXT WIDGET
 ************************/
TextWidget::~TextWidget() {
  free(_text);
  free(_oldtext);
}

void TextWidget::_charSize(uint8_t textsize, uint8_t &width, uint16_t &height) {
  #ifndef DSP_LCD
  width = textsize * CHARWIDTH;
  height = textsize * CHARHEIGHT;
  #else
  width = 1;
  height = 1;
  #endif
}

void TextWidget::init(WidgetConfig wconf, uint16_t buffsize, bool uppercase, uint16_t fgcolor, uint16_t bgcolor) {
  Widget::init(wconf, fgcolor, bgcolor);
  _buffsize = buffsize;
  _text = (char *)malloc(sizeof(char) * _buffsize);
  memset(_text, 0, _buffsize);
  _oldtext = (char *)malloc(sizeof(char) * _buffsize);
  memset(_oldtext, 0, _buffsize);
  _charSize(_config.textsize, _charWidth, _textheight);
  _textwidth = _oldtextwidth = _oldleft = 0;
  _uppercase = uppercase;
}

void TextWidget::setText(const char *txt) {
  strlcpy(_text, utf8To(txt, _uppercase), _buffsize);
  _textwidth = strlen(_text) * _charWidth;
  if (strcmp(_oldtext, _text) == 0) {
    return;
  }
  if (_active) {
    dsp.fillRect(_oldleft == 0 ? _realLeft() : min(_oldleft, _realLeft()), _config.top, max(_oldtextwidth, _textwidth), _textheight, _bgcolor);
  }
  _oldtextwidth = _textwidth;
  _oldleft = _realLeft();
  if (_active) {
    _draw();
  }
}

void TextWidget::setText(int val, const char *format) {
  char buf[_buffsize];
  snprintf(buf, _buffsize, format, val);
  setText(buf);
}

void TextWidget::setText(const char *txt, const char *format) {
  char buf[_buffsize];
  snprintf(buf, _buffsize, format, txt);
  setText(buf);
}

uint16_t TextWidget::_realLeft(bool w_fb) {
  uint16_t realwidth = (_width > 0 && w_fb) ? _width : dsp.width();
  switch (_config.align) {
    case WA_CENTER: return (realwidth - _textwidth) / 2; break;
    case WA_RIGHT:  return (realwidth - _textwidth - (!w_fb ? _config.left : 0)); break;
    default:        return !w_fb ? _config.left : 0; break;
  }
}

void TextWidget::_draw() {
  if (!_active) {
    return;
  }
  dsp.setTextColor(_fgcolor, _bgcolor);
  dsp.setCursor(_realLeft(), _config.top);
  dsp.setFont();
  dsp.setTextSize(_config.textsize);
  dsp.print(_text);
  strlcpy(_oldtext, _text, _buffsize);
}

/************************
      SCROLL WIDGET
 ************************/
ScrollWidget::ScrollWidget(const char *separator, ScrollConfig conf, uint16_t fgcolor, uint16_t bgcolor) {
  init(separator, conf, fgcolor, bgcolor);
}

ScrollWidget::~ScrollWidget() {
  free(_fb);
  free(_sep);
  free(_window);
}

void ScrollWidget::init(const char *separator, ScrollConfig conf, uint16_t fgcolor, uint16_t bgcolor) {
  TextWidget::init(conf.widget, conf.buffsize, conf.uppercase, fgcolor, bgcolor);
  _sep = (char *)malloc(sizeof(char) * 4);
  memset(_sep, 0, 4);
  snprintf(_sep, 4, " %.*s ", 1, separator);
  _x = conf.widget.left;
  _startscrolldelay = conf.startscrolldelay;
  _scrolldelta = conf.scrolldelta;
  _scrolltime = conf.scrolltime;
  _charSize(_config.textsize, _charWidth, _textheight);
  _sepwidth = strlen(_sep) * _charWidth;
  _width = conf.width;
  _backMove.width = _width;
  _window = (char *)malloc(sizeof(char) * (MAX_WIDTH / _charWidth + 1));
  memset(_window, 0, (MAX_WIDTH / _charWidth + 1));  // +1?
  _doscroll = false;
  #ifdef PSFBUFFER
  _fb = new psFrameBuffer(dsp.width(), dsp.height());
  uint16_t _rl = (_config.align == WA_CENTER) ? (dsp.width() - _width) / 2 : _config.left;
  _fb->begin(&dsp, _rl, _config.top, _width, _textheight, _bgcolor);
  #endif
}

void ScrollWidget::_setTextParams() {
  if (_config.textsize == 0) {
    return;
  }
  if (_fb->ready()) {
  #ifdef PSFBUFFER
    _fb->setTextSize(_config.textsize);
    _fb->setTextColor(_fgcolor, _bgcolor);
  #endif
  } else {
    dsp.setTextSize(_config.textsize);
    dsp.setTextColor(_fgcolor, _bgcolor);
  }
}

bool ScrollWidget::_checkIsScrollNeeded() {
  return _textwidth > _width;
}

void ScrollWidget::setText(const char *txt) {
  //Serial.printf("widget.cpp -> setText() -> txt %s\r\n", txt);
  strlcpy(_text, utf8To(txt, _uppercase), _buffsize - 1);
  if (strcmp(_oldtext, _text) == 0) {
    return;
  }
  _textwidth = strlen(_text) * _charWidth;
  _x = _fb->ready() ? 0 : _config.left;
  _doscroll = _checkIsScrollNeeded();
  if (dsp.getScrollId() == this) {
    dsp.setScrollId(NULL);
  }
  _scrolldelay = millis();
  if (_active) {
    _setTextParams();
    if (_doscroll) {
      if (_fb->ready()) {
  #ifdef PSFBUFFER
        _fb->fillRect(0, 0, _width, _textheight, _bgcolor);
        _fb->setCursor(0, 0);
        snprintf(_window, _width / _charWidth + 1, "%s", _text);  //TODO
        _fb->print(_window);
        _fb->display();
  #endif
      } else {
        dsp.fillRect(_config.left, _config.top, _width, _textheight, _bgcolor);
        dsp.setCursor(_config.left, _config.top);
        snprintf(_window, _width / _charWidth + 1, "%s", _text);  //TODO
        dsp.setClipping({_config.left, _config.top, _width, _textheight});
        dsp.print(_window);
        dsp.clearClipping();
      }
    } else {
      if (_fb->ready()) {
  #ifdef PSFBUFFER
        _fb->fillRect(0, 0, _width, _textheight, _bgcolor);
        _fb->setCursor(_realLeft(true), 0);
        _fb->print(_text);
        _fb->display();
  #endif
      } else {
        dsp.fillRect(_config.left, _config.top, _width, _textheight, _bgcolor);
        dsp.setCursor(_realLeft(), _config.top);
        //dsp.setClipping({_config.left, _config.top, _width, _textheight});
        dsp.print(_text);
        //dsp.clearClipping();
      }
    }
    strlcpy(_oldtext, _text, _buffsize);
  }
}

void ScrollWidget::setText(const char *txt, const char *format) {
  char buf[_buffsize];
  snprintf(buf, _buffsize, format, txt);
  setText(buf);
}

void ScrollWidget::loop() {
  if (_locked) {
    return;
  }
  if (!_doscroll || _config.textsize == 0 || (dsp.getScrollId() != NULL && dsp.getScrollId() != this)) {
    return;
  }
  uint16_t fbl = _fb->ready() ? 0 : _config.left;
  if (_checkDelay(_x == fbl ? _startscrolldelay : _scrolltime, _scrolldelay)) {
    _calcX();
    if (_active) {
      _draw();
    }
  }
}

void ScrollWidget::_clear() {
  if (_fb->ready()) {
  #ifdef PSFBUFFER
    _fb->fillRect(0, 0, _width, _textheight, _bgcolor);
    _fb->display();
  #endif
  } else {
    dsp.fillRect(_config.left, _config.top, _width, _textheight, _bgcolor);
  }
}

void ScrollWidget::_draw() {
  if (!_active || _locked) {
    return;
  }
  _setTextParams();
  if (_doscroll) {
    uint16_t fbl = _fb->ready() ? 0 : _config.left;
    uint16_t _newx = fbl - _x;
    const char *_cursor = _text + _newx / _charWidth;
    uint16_t hiddenChars = _cursor - _text;
    uint8_t addChars = _fb->ready() ? 2 : 1;
    if (hiddenChars < strlen(_text)) {
  //TODO
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wformat-truncation="
      snprintf(_window, _width / _charWidth + addChars, "%s%s%s", _cursor, _sep, _text);
  #pragma GCC diagnostic pop
    } else {
      const char *_scursor = _sep + (_cursor - (_text + strlen(_text)));
      snprintf(_window, _width / _charWidth + addChars, "%s%s", _scursor, _text);
    }
    if (_fb->ready()) {
  #ifdef PSFBUFFER
      _fb->fillRect(0, 0, _width, _textheight, _bgcolor);
      _fb->setCursor(_x + hiddenChars * _charWidth, 0);
      _fb->print(_window);
      _fb->display();
  #endif
    } else {
      dsp.setCursor(_x + hiddenChars * _charWidth, _config.top);
      dsp.setClipping({_config.left, _config.top, _width, _textheight});
      dsp.print(_window);
  #ifndef DSP_LCD
      dsp.print(" ");
  #endif
      dsp.clearClipping();
    }
  } else {
    if (_fb->ready()) {
  #ifdef PSFBUFFER
      _fb->fillRect(0, 0, _width, _textheight, _bgcolor);
      _fb->setCursor(_realLeft(true), 0);
      _fb->print(_text);
      _fb->display();
  #endif
    } else {
      dsp.fillRect(_config.left, _config.top, _width, _textheight, _bgcolor);
      dsp.setCursor(_realLeft(), _config.top);
      dsp.setClipping({_realLeft(), _config.top, _width, _textheight});
      dsp.print(_text);
      dsp.clearClipping();
    }
  }
}

void ScrollWidget::_calcX() {
  if (!_doscroll || _config.textsize == 0) {
    return;
  }
  _x -= _scrolldelta;
  uint16_t fbl = _fb->ready() ? 0 : _config.left;
  if (-_x > _textwidth + _sepwidth - fbl) {
    _x = fbl;
    dsp.setScrollId(NULL);
  } else {
    dsp.setScrollId(this);
  }
}

bool ScrollWidget::_checkDelay(int m, uint32_t &tstamp) {
  if (millis() - tstamp > m) {
    tstamp = millis();
    return true;
  } else {
    return false;
  }
}

void ScrollWidget::_reset() {
  dsp.setScrollId(NULL);
  _x = _fb->ready() ? 0 : _config.left;
  _scrolldelay = millis();
  _doscroll = _checkIsScrollNeeded();
  #ifdef PSFBUFFER
  _fb->freeBuffer();
  uint16_t _rl = (_config.align == WA_CENTER) ? (dsp.width() - _width) / 2 : _config.left;
  _fb->begin(&dsp, _rl, _config.top, _width, _textheight, _bgcolor);
  #endif
}

/************************
      SLIDER WIDGET
 ************************/
void SliderWidget::init(FillConfig conf, uint16_t fgcolor, uint16_t bgcolor, uint32_t maxval, uint16_t oucolor) {
  Widget::init(conf.widget, fgcolor, bgcolor);
  _width = conf.width;
  _height = conf.height;
  _outlined = conf.outlined;
  _oucolor = oucolor, _max = maxval;
  _oldvalwidth = _value = 0;
}

void SliderWidget::setValue(uint32_t val) {
  _value = val;
  if (_active && !_locked) {
    _drawslider();
  }
}

void SliderWidget::_drawslider() {
  uint16_t valwidth = map(_value, 0, _max, 0, _width - _outlined * 2);
  if (_oldvalwidth == valwidth) {
    return;
  }
  dsp.fillRect(
    _config.left + _outlined + min(valwidth, _oldvalwidth), _config.top + _outlined, abs(_oldvalwidth - valwidth), _height - _outlined * 2,
    _oldvalwidth > valwidth ? _bgcolor : _fgcolor
  );
  _oldvalwidth = valwidth;
}

void SliderWidget::_draw() {
  if (_locked) {
    return;
  }
  _clear();
  if (!_active) {
    return;
  }
  if (_outlined) {
    dsp.drawRect(_config.left, _config.top, _width, _height, _oucolor);
  }
  uint16_t valwidth = map(_value, 0, _max, 0, _width - _outlined * 2);
  dsp.fillRect(_config.left + _outlined, _config.top + _outlined, valwidth, _height - _outlined * 2, _fgcolor);
}

void SliderWidget::_clear() {
  //  _oldvalwidth = 0;
  dsp.fillRect(_config.left, _config.top, _width, _height, _bgcolor);
}
void SliderWidget::_reset() {
  _oldvalwidth = 0;
}

  /************************
      VU WIDGET
 ************************/
  #if !defined(DSP_LCD) && !defined(DSP_OLED)
VuWidget::~VuWidget() {
  if (_canvas) {
    free(_canvas);
  }
}

void VuWidget::init(WidgetConfig wconf, VUBandsConfig bands, uint16_t vumaxcolor, uint16_t vumidcolor, uint16_t vumincolor, uint16_t bgcolor) {
  Widget::init(wconf, bgcolor, bgcolor);
  _vumaxcolor = vumaxcolor;
  _vumidcolor = vumidcolor;  // Módosítás új sor.
  _vumincolor = vumincolor;
  _bands = bands;

    //  _canvas = new Canvas(_bands.width * 2 + _bands.space, _bands.height);
    // _canvas = new Canvas(_bands.width, _bands.height * 2 + _bands.space);
    #ifndef BOOMBOX_STYLE
  // két VU egymás alatt
  _canvas = new Canvas(_bands.width, _bands.height * 2 + _bands.space);
    #else
  // két VU egymás mellett
  _canvas = new Canvas(_bands.width * 2 + _bands.space, _bands.height);
    #endif
}

/* A _labelsDrawn változó true értéke esetén rajzolja meg a VU méter előtti L R feliratot.
A  BitrateWidget::_clear() -ben kap false értéket.*/
bool VuWidget::_labelsDrawn = false;  // Módosítás

void VuWidget::setLabelsDrawn(bool value) {  // Saját
  _labelsDrawn = value;
}

bool VuWidget::isLabelsDrawn() {  // Saját
  return _labelsDrawn;
}
/**************************** VU widget DRAW ********************************/
void VuWidget::_draw() {
  if (!_active || _locked) {
    return;
  }
  if (!config.store.vumeter) {
    return;
  }
  uint16_t bandColor;
  static uint16_t measLpx = 0;
  static uint16_t measRpx = 0;
  const uint8_t vu_decay_step = _bands.fadespeed;
  const uint16_t dimension = _config.align ? _bands.width : _bands.height;  //vízszintes vagy függőleges
  static uint32_t last_draw_time;
  uint8_t refresh_time = 33;  // A VU rajzolás frissítési ideje (millis)
    #ifdef VU_PEAK
  static uint16_t peakL = 0, peakR = 0;            // Csúcsértékek
  static uint32_t peakL_time = 0, peakR_time = 0;  // Csúcs időbélyeg
  const uint8_t peak_decay_step = 1;               // A csúcs bomlása pixelben
  const uint16_t peak_hold_ms = 200;               // Csúcs tartási idő
    #endif
  uint32_t now = millis();
  if (last_draw_time + refresh_time > now) {
    return;
  } else {
    last_draw_time = now;
  }
  uint16_t vulevel = player.getVUlevel();
  uint8_t vuLeft = (vulevel >> 8) & 0xFF;
  uint8_t vuRight = vulevel & 0xFF;
  // A maximális VU érték begyűjtése. Fájlonként nullázódik, és 2 másodpercenként -10 -el csökken.
  uint16_t maxVU = max(vuLeft, vuRight);
  if (maxVU > config.vuRefLevel) {
    config.vuRefLevel = maxVU;
  }
  // Minimális érték a kezdeti számításokhoz.
  if (config.vuRefLevel < 50) {
    config.vuRefLevel = 50;
  }
  // VU értéket pixel pozícióra skalázás
  uint16_t vuLpx = map(vuLeft, 0, config.vuRefLevel, 0, _bands.width);
  uint16_t vuRpx = map(vuRight, 0, config.vuRefLevel, 0, _bands.width);
  bool played = player.isRunning();
  if (played) {
    // BAL csatorna
    if (vuLpx > measLpx) {
      measLpx = vuLpx;  // gyors felfutás L
    } else {
      measLpx = (measLpx > vu_decay_step) ? measLpx - vu_decay_step : 0;  // lassított lebomlás L
    }
    // JOBB csatorna
    if (vuRpx > measRpx) {
      measRpx = vuRpx;  // gyors felfutás R
    } else {
      measRpx = (measRpx > vu_decay_step) ? measRpx - vu_decay_step : 0;  // lassított lebomlás R
    }
      // --- Csúcs logika ---
    #ifdef VU_PEAK
    // BAL csatorna
    if (measLpx > peakL) {
      peakL = measLpx;  // L - csúcs meghatározás
      peakL_time = now;
    } else if (now - peakL_time > peak_hold_ms && peakL > 0) {
      peakL = (peakL > peak_decay_step) ? peakL - peak_decay_step : 0;  // L - csúcs lebomlás
    }
    // JOBB csatorna
    if (measRpx > peakR) {
      peakR = measRpx;  // R - csúcs meghatározás
      peakR_time = now;
    } else if (now - peakR_time > peak_hold_ms && peakR > 0) {
      peakR = (peakR > peak_decay_step) ? peakR - peak_decay_step : 0;  // R - csúcs lebomlás
    }
    #endif  // VU_PEAK
    /*************************************  A VU sávok rajzolása  ***************************************/
    #ifdef BOOMBOX_STYLE
    // ===================== BOOMBOX_STYLE =====================
    // Két VU egymás mellett – TELJES TÖRLÉS kell a ghosting ellen!
    _canvas->fillRect(0, 0, _bands.width * 2 + _bands.space, _bands.height, _bgcolor);
    // --- LED színek határai ---
    int green_end = (_bands.width * 65) / 100;
    int yellow_end = (_bands.width * 85) / 100;
    // --- VU méretezés ---
    uint8_t ledWidth = (dimension / _bands.perheight) - _bands.vspace;
    uint16_t step = dimension / _bands.perheight;
    uint16_t litCountL = measLpx / step;
    uint16_t litCountR = measRpx / step;
    const int MID_GAP = 6;             // teljes hézag a két kijelző között
    const int MID_HALF = MID_GAP / 2;  // 3 pixel balra + 3 pixel jobbra
    // === BAL csatorna (balra indul a középtől) ===
    for (int led = 0; led < litCountL; led++) {
      int x = _bands.width - MID_HALF - (led * step) - ledWidth;
      if (x < 0) {
        break;
      }
      if (led * step < green_end) {
        bandColor = _vumincolor;
      } else if (led * step < yellow_end) {
        bandColor = _vumidcolor;
      } else {
        bandColor = _vumaxcolor;
      }
      _canvas->fillRect(x, 0, ledWidth, _bands.height, bandColor);
    }
    // === JOBB csatorna (jobbra indul a középtől) ===
    for (int led = 0; led < litCountR; led++) {
      int x = _bands.width + MID_HALF + (led * step);
      if (x > (int)_bands.width * 2) {
        break;
      }
      if (led * step < green_end) {
        bandColor = _vumincolor;
      } else if (led * step < yellow_end) {
        bandColor = _vumidcolor;
      } else {
        bandColor = _vumaxcolor;
      }
      _canvas->fillRect(x, 0, ledWidth, _bands.height, bandColor);
    }
      #ifdef VU_PEAK
    const uint16_t peak_color = 0xFFFF;
    const uint16_t peak_bright = 0xF7FF;
    const int peak_width = 1;
    // --- BAL peak ---
    {
      int pxL = _bands.width - MID_HALF - peakL;
      // clamp hogy sose fusson ki balra
      if (pxL < 0) {
        pxL = 0;
      }
      _canvas->fillRect((pxL - 1 < 0 ? 0 : pxL - 1), 0, peak_width + 2, _bands.height, peak_bright);
      _canvas->fillRect(pxL, 0, peak_width, _bands.height, peak_color);
    }
    // --- JOBB peak ---
    {
      int pxR = _bands.width + MID_HALF + peakR;
      // clamp védelem, de jobbra
      int maxX = (_bands.width * 2 + MID_GAP) - peak_width - 1;
      if (pxR > maxX) {
        pxR = maxX;
      }
      _canvas->fillRect(pxR - 1, 0, peak_width + 2, _bands.height, peak_bright);
      _canvas->fillRect(pxR, 0, peak_width, _bands.height, peak_color);
    }
      #endif  // VU_PEAK
    // --- KIJELZŐRE KÜLDÉS ---
    dsp.startWrite();
    dsp.setAddrWindow(_config.left + 4, _config.top + 10, _bands.width * 2 + _bands.space, _bands.height);
    dsp.writePixels((uint16_t *)_canvas->getBuffer(), (_bands.width * 2 + _bands.space) * _bands.height);
    dsp.endWrite();
        // ===================== BOOMBOX_STYLE vége =====================
    #else
    // ===================== NEM BOOMBOX_STYLE ======================
    // Háttér törlése – két VU egymás alatt
    _canvas->fillRect(0, 0, _bands.width, _bands.height * 2 + _bands.space, _bgcolor);
    // --- LED-sáv rajzolása színátmenettel ---
    int green_end = (_bands.width * 65) / 100;
    int yellow_end = (_bands.width * 85) / 100;
    // --- VU méretezés ---
    uint8_t ledWidth = (dimension / _bands.perheight) - _bands.vspace;
    uint16_t step = dimension / _bands.perheight;  // step pixelben
    uint16_t litCountL = measLpx / step;
    uint16_t litCountR = measRpx / step;
    // Bal
    for (int led = 0; led < litCountL; led++) {
      int x = led * step;
      if (x < green_end) {
        bandColor = _vumincolor;
      } else if (x < yellow_end) {
        bandColor = _vumidcolor;
      } else {
        bandColor = _vumaxcolor;
      }
      _canvas->fillRect(x, 0, ledWidth, _bands.height, bandColor);
    }
    // Jobb
    for (int led = 0; led < litCountR; led++) {
      int x = led * step;
      if (x < green_end) {
        bandColor = _vumincolor;
      } else if (x < yellow_end) {
        bandColor = _vumidcolor;
      } else {
        bandColor = _vumaxcolor;
      }
      _canvas->fillRect(x, _bands.height + _bands.space, ledWidth, _bands.height, bandColor);
    }
      #ifdef VU_PEAK
    const uint16_t peak_color = 0xFFFF;
    const uint16_t peak_bright = 0xF7FF;
    const int peak_width = 1;
    // Peak bal
    if (peakL > _bands.width - 2) {
      peakL -= 2;
    }
    if (peakL > 1 && peakL <= (int)_bands.width) {
      _canvas->fillRect(peakL - 1, 0, peak_width + 2, _bands.height, peak_bright);
      _canvas->fillRect(peakL, 0, peak_width, _bands.height, peak_color);
    }
    // Peak jobb
    if (peakR > _bands.width - 2) {
      peakR -= 2;
    }
    if (peakR > 1 && peakR <= (int)_bands.width) {
      _canvas->fillRect(peakR - 1, _bands.height + _bands.space, peak_width + 2, _bands.height, peak_bright);
      _canvas->fillRect(peakR, _bands.height + _bands.space, peak_width, _bands.height, peak_color);
    }
      #endif  // VU_PEAK
    // Kirajzolás
    int drawWidth = _bands.width;
    int drawHeight = _bands.height * 2 + _bands.space;
    dsp.startWrite();
    dsp.setAddrWindow(_config.left, _config.top, drawWidth, drawHeight);
    dsp.writePixels((uint16_t *)_canvas->getBuffer(), drawWidth * drawHeight);
    dsp.endWrite();
    #endif    // BOOMBOX_STYLE
    /********************************** --- L/R címkék rajzolása --- **********************************/
    #ifdef BOOMBOX_STYLE
    if (played && !_labelsDrawn) {
      // Serial.println("L/R rajzolás");
      int label_width = _bands.height + 15;
      int label_height = _bands.height + 4;
      // teljes szélesség (két címke + 6px hézag)
      int total_width = 2 * label_width + 6;
      // bal széle a középre igazított blokk
      int center_left = (dsp.width() - total_width) / 2;
      // bal és jobb címke pozíció
      int label_left_L = center_left;
      int label_left_R = center_left + label_width + 6;
      // Bal (L) téglalap
      dsp.fillRect(label_left_L, _config.top - 4, label_width, label_height, 0x7BEF);
      dsp.setTextSize(1);
      dsp.setFont();
      dsp.setTextColor(0xFFFF);
      int text_x_L = label_left_L + (label_width - 6) / 2;
      #if DSP_MODEL == DSP_ILI9341
      int text_y = ((_config.top - 2) + (label_height - 10) / 2) - 1;
      #else
      int text_y = (_config.top - 2) + (label_height - 10) / 2;
      #endif
      dsp.setCursor(text_x_L, text_y);
      dsp.print("L");
      // Jobb (R) téglalap
      dsp.fillRect(label_left_R, _config.top - 4, label_width, label_height, 0x7BEF);
      int text_x_R = label_left_R + (label_width - 6) / 2;
      dsp.setCursor(text_x_R, text_y);
      dsp.print("R");
      _labelsDrawn = true;
    }
    #else   //  NEM BOOMBOX_STYLE
    if (played && !_labelsDrawn) {
      // Serial.println("L/R rajzolás");
      int label_width = _bands.height + 15;
      int label_height = _bands.height + 4;
      int label_offset = label_width + 4;
      int label_left = _config.left - label_offset;
      if (label_left >= 0) {
        dsp.fillRect(label_left, _config.top - 4, label_width, label_height, 0x7BEF);
        dsp.fillRect(label_left, _config.top + _bands.height + _bands.space, label_width, label_height - 1, 0x7BEF);
        dsp.setTextSize(1);
        dsp.setFont();
        dsp.setTextColor(0xFFFF);
        int text_x = label_left + (label_width - 6) / 2;
        int text_y_L = (_config.top - 2) + (label_height - 10) / 2;
        int text_y_R = _config.top + _bands.height + _bands.space + (label_height - 8) / 2;
        dsp.setCursor(text_x, text_y_L);
        dsp.print("L");
        dsp.setCursor(text_x, text_y_R);
        dsp.print("R");
        _labelsDrawn = true;
      }
    }
    #endif  // BOOMBOX_STYLE
  }
}

void VuWidget::loop() {
  if (_active || !_locked) {
    _draw();
  }
}

void VuWidget::_clear() {
  // dsp.fillRect(_config.left, _config.top, _bands.width * 2 + _bands.space, _bands.height, _bgcolor);
  dsp.fillRect(0, _config.top - 4, 479, 24, _bgcolor);
  _labelsDrawn = false;  // L és R meg keljen rajzolni. Módosítás.
  // Serial.println("widget.cpp -> VuWidget::_clear()");
}
  #else  // DSP_LCD
VuWidget::~VuWidget() {}
void VuWidget::init(WidgetConfig wconf, VUBandsConfig bands, uint16_t vumaxcolor, uint16_t vumincolor, uint16_t bgcolor) {
  Widget::init(wconf, bgcolor, bgcolor);
}
void VuWidget::_draw() {}
void VuWidget::loop() {}
void VuWidget::_clear() {}
  #endif

  /************************
      NUM & CLOCK
 ************************/
  #if !defined(DSP_LCD)
    #if TIME_SIZE < 19  //19->NOKIA
const GFXfont *Clock_GFXfontPtr = nullptr;
      #define CLOCKFONT5x7
    #else
const GFXfont *Clock_GFXfontPtr = &Clock_GFXfont;
const GFXfont *Clock_GFXfontPtr_Sec = &Clock_GFXfont_sec;  // Módosítás saját betű másodperchez. "font"
    #endif
  #endif  //!defined(DSP_LCD)

  #if !defined(CLOCKFONT5x7) && !defined(DSP_LCD)
inline GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c) {
  return gfxFont->glyph + c;
}
uint8_t _charWidth(unsigned char c) {
  GFXglyph *glyph = pgm_read_glyph_ptr(&Clock_GFXfont, c - 0x20);
  return pgm_read_byte(&glyph->xAdvance);
}
uint16_t _textHeight() {
  GFXglyph *glyph = pgm_read_glyph_ptr(&Clock_GFXfont, '8' - 0x20);
  return pgm_read_byte(&glyph->height);
}
  #else  //!defined(CLOCKFONT5x7) && !defined(DSP_LCD)
uint8_t _charWidth(unsigned char c) {
    #ifndef DSP_LCD
  return CHARWIDTH * TIME_SIZE;
    #else
  return 1;
    #endif
}
uint16_t _textHeight() {
  return CHARHEIGHT * TIME_SIZE;
}
  #endif
uint16_t _textWidth(const char *txt) {
  uint16_t w = 0, l = strlen(txt);
  for (uint16_t c = 0; c < l; c++) {
    w += _charWidth(txt[c]);
  }
  //  #if DSP_MODEL==DSP_ILI9225
  //  return w+l;
  //  #else
  return w;
  //  #endif
}

/************************
      NUM WIDGET
 ************************/
void NumWidget::init(WidgetConfig wconf, uint16_t buffsize, bool uppercase, uint16_t fgcolor, uint16_t bgcolor) {
  Widget::init(wconf, fgcolor, bgcolor);
  _buffsize = buffsize;
  _text = (char *)malloc(sizeof(char) * _buffsize);
  memset(_text, 0, _buffsize);
  _oldtext = (char *)malloc(sizeof(char) * _buffsize);
  memset(_oldtext, 0, _buffsize);
  _textwidth = _oldtextwidth = _oldleft = 0;
  _uppercase = uppercase;
  _textheight = TIME_SIZE /*wconf.textsize*/;
}

void NumWidget::setText(const char *txt) {
  strlcpy(_text, txt, _buffsize);
  _getBounds();
  if (strcmp(_oldtext, _text) == 0) {
    return;
  }
  uint16_t realth = _textheight;
  #if defined(DSP_OLED) && DSP_MODEL != DSP_SSD1322
  if (Clock_GFXfontPtr == nullptr) {
    realth = _textheight * 8;  //CHARHEIGHT
  }
  #endif
  if (_active)
  #ifndef CLOCKFONT5x7
    dsp.fillRect(_oldleft == 0 ? _realLeft() : min(_oldleft, _realLeft()), _config.top - _textheight + 1, max(_oldtextwidth, _textwidth), realth, _bgcolor);
  #else
    dsp.fillRect(_oldleft == 0 ? _realLeft() : min(_oldleft, _realLeft()), _config.top, max(_oldtextwidth, _textwidth), realth, _bgcolor);
  #endif

  _oldtextwidth = _textwidth;
  _oldleft = _realLeft();
  if (_active) {
    _draw();
  }
}

void NumWidget::setText(int val, const char *format) {
  char buf[_buffsize];
  snprintf(buf, _buffsize, format, val);
  setText(buf);
}

void NumWidget::_getBounds() {
  _textwidth = _textWidth(_text);
}

void NumWidget::_draw() {
  #ifndef DSP_LCD
  if (!_active || TIME_SIZE < 2) {
    return;
  }
  dsp.setTextSize(Clock_GFXfontPtr == nullptr ? TIME_SIZE : 1);
  dsp.setFont(Clock_GFXfontPtr);
  dsp.setTextColor(_fgcolor, _bgcolor);
  #endif
  if (!_active) {
    return;
  }
  dsp.setCursor(_realLeft(), _config.top);
  dsp.print(_text);
  strlcpy(_oldtext, _text, _buffsize);
  dsp.setFont();
}

/**************************
      PROGRESS WIDGET
 **************************/
void ProgressWidget::_progress() {
  char buf[_width + 1];
  snprintf(buf, _width, "%*s%.*s%*s", _pg <= _barwidth ? 0 : _pg - _barwidth, "", _pg <= _barwidth ? _pg : 5, ".....", _width - _pg, "");
  _pg++;
  if (_pg >= _width + _barwidth) {
    _pg = 0;
  }
  setText(buf);
}

bool ProgressWidget::_checkDelay(int m, uint32_t &tstamp) {
  if (millis() - tstamp > m) {
    tstamp = millis();
    return true;
  } else {
    return false;
  }
}

void ProgressWidget::loop() {
  if (_checkDelay(_speed, _scrolldelay)) {
    _progress();
  }
}

/**************************
      CLOCK WIDGET
 **************************/
void ClockWidget::init(WidgetConfig wconf, uint16_t fgcolor, uint16_t bgcolor) {
  Widget::init(wconf, fgcolor, bgcolor);
  _timeheight = _textHeight();
  _fullclock = TIME_SIZE > 35 || DSP_MODEL == DSP_ILI9225;
  if (_fullclock) {
    _superfont = TIME_SIZE / 17;  //magick
  } else if (TIME_SIZE == 19 || TIME_SIZE == 2) {
    _superfont = 1;
  } else {
    _superfont = 0;
  }
  _space = (5 * _superfont) / 2;  //magick
  #ifndef HIDE_DATE
  if (_fullclock) {
    _dateheight = _superfont < 4 ? 1 : 2;
    // _clockheight = _timeheight + _space + CHARHEIGHT * _dateheight; //Original
    _clockheight = _timeheight;
  } else {
    _clockheight = _timeheight;
  }
  #else
  _clockheight = _timeheight;
  #endif
  _getTimeBounds();
  #ifdef PSFBUFFER
  _fb = new psFrameBuffer(dsp.width(), dsp.height());
  _begin();
  #endif
}

void ClockWidget::_begin() {
  #ifdef PSFBUFFER
  _fb->begin(&dsp, _clockleft, _config.top - _timeheight, _clockwidth, _clockheight + 1, config.theme.background);
  #endif
}

bool ClockWidget::_getTime() {
  #if defined AM_PM_STYLE
  strftime(_timebuffer, sizeof(_timebuffer), "%I:%M", &network.timeinfo);
  if (_timebuffer[0] == '0') {
    _timebuffer[0] = ' ';  // Ha az eslő számjegy 0 kicseréli szóközre (azonos karakterszélesség szükséges)
  }
  #else
  strftime(_timebuffer, sizeof(_timebuffer), "%H:%M", &network.timeinfo);
  #endif
  bool ret = network.timeinfo.tm_sec == 0 || _forceflag != network.timeinfo.tm_year;
  _forceflag = network.timeinfo.tm_year;
  return ret;
}

uint16_t ClockWidget::_left() {
  if (_fb->ready()) {
    return 0;
  } else {
    return _clockleft;
  }
}
uint16_t ClockWidget::_top() {
  if (_fb->ready()) {
    return _timeheight;
  } else {
    return _config.top;
  }
}

void ClockWidget::_getTimeBounds() {
  _timewidth = _textWidth(_timebuffer);
  uint8_t fs = _superfont > 0 ? _superfont : TIME_SIZE;
  uint16_t rightside = CHARWIDTH * fs * 2;  // seconds
  if (_fullclock) {
    rightside += _space * 2 + 1;  //2space+vline
    _clockwidth = _timewidth + rightside;
  } else {
    if (_superfont == 0) {
      _clockwidth = _timewidth;
    } else {
      _clockwidth = _timewidth + rightside;
    }
  }
  switch (_config.align) {
    case WA_LEFT:  _clockleft = _config.left; break;
    case WA_RIGHT: _clockleft = dsp.width() - _clockwidth - _config.left; break;
    default:       _clockleft = (dsp.width() / 2 - _clockwidth / 2) + _config.left; break;
  }
  char buf[4];
  strftime(buf, 4, "%H", &network.timeinfo);
  _dotsleft = _textWidth(buf);
}

  #ifndef DSP_LCD

Adafruit_GFX &ClockWidget::getRealDsp() {
    #ifdef PSFBUFFER
  if (_fb && _fb->ready()) {
    return *_fb;
  }
    #endif
  return dsp;
}

void ClockWidget::_printClock(bool force) {
  auto &gfx = getRealDsp();
  gfx.setTextSize(Clock_GFXfontPtr == nullptr ? TIME_SIZE : 1);
  gfx.setFont(Clock_GFXfontPtr);
  bool clockInTitle = !config.isScreensaver && _config.top < _timeheight;  //DSP_SSD1306x32
  if (force) {
    _clearClock();
    _getTimeBounds();
    #ifndef DSP_OLED
    if (CLOCKFONT_MONO) {
      gfx.setTextColor(config.theme.clockbg, config.theme.background);
      gfx.setCursor(_left(), _top());
      gfx.print("88:88");
    }
    #endif
    if (clockInTitle) {
      gfx.setTextColor(config.theme.meta, config.theme.metabg);
    } else {
      gfx.setTextColor(config.theme.clock, config.theme.background);
    }
    gfx.setCursor(_left(), _top());
    gfx.print(_timebuffer);  // Az óra, perc kiírása.
    if (_fullclock) {
      bool fullClockOnScreensaver = (!config.isScreensaver || (_fb->ready() && FULL_SCR_CLOCK));
      _linesleft = _left() + _timewidth + _space;
      if (fullClockOnScreensaver) {
        gfx.drawFastVLine(_linesleft, _top() - _timeheight, _timeheight, config.theme.div);  // A másodperc vertikális vonala.
    #ifdef AM_PM_STYLE
        // A másodperc horizontális vonala.
        gfx.drawFastHLine(_linesleft, _top() - (_timeheight / 2), CHARWIDTH * _superfont * 2 + _space, config.theme.div);
        gfx.setTextSize(0);
        gfx.setFont(Clock_GFXfontPtr_Sec);
        char buf[3];
        strftime(buf, sizeof(buf), "%p", &network.timeinfo);
        gfx.setCursor(_linesleft + 8, _top());
        gfx.print(buf);  // AM vagy PM kiírása
    #else
      #if DSP_MODEL == DSP_ILI9341
        constexpr int lineOffset = 17;  // 320x240
      #else
        constexpr int lineOffset = 25;  // 480x320
      #endif
        // A másodperc horizontális vonala.
        gfx.drawFastHLine(_linesleft, _top() - (_timeheight / 2) + lineOffset, CHARWIDTH * _superfont * 2 + _space, config.theme.div);
    #endif
        if (!config.isScreensaver) {
          _formatDate();
    #ifndef HIDE_DATE
          memcpy_P(&_dateConf, &dateConf, sizeof(WidgetConfig));
          // Sor törlése teljes szélességben
          int lineHeight = _dateheight * 8;                                                  // kb. 8 pixel per TextSize
          dsp.fillRect(0, _dateConf.top, dsp.width(), lineHeight, config.theme.background);  //szürke 0x8410
          strlcpy(_datebuf, utf8To(_tmp, false), sizeof(_datebuf));
          uint16_t _datewidth = strlen(_datebuf) * CHARWIDTH * _dateheight;
          dsp.setFont();
          dsp.setTextSize(_dateheight);
          uint16_t _dateleft = dsp.width() - _datewidth - _dateConf.left;
          dsp.setCursor(_dateleft, _dateConf.top);  // Módosítás saját beállítás változó "_dateConf"
          dsp.setTextColor(config.theme.date, config.theme.background);
          dsp.print(_datebuf);
    #endif  // HIDE_DATE
        }
      }
    }
  }
  if (_fullclock || _superfont > 0) {
    // *** Másodperc kiírása ***
    gfx.setTextSize(0);
    gfx.setFont(Clock_GFXfontPtr_Sec);
    if (CLOCKFONT_MONO) {
      gfx.setTextColor(config.theme.clockbg, config.theme.background);
    } else {
      gfx.setTextColor(config.theme.background, config.theme.background);
    }
    uint16_t topSec;
    uint16_t leftSec;
    #if DSP_MODEL == DSP_ILI9341  // 320x240
      #ifdef AM_PM_STYLE
    topSec = _top() - _timeheight + 20;
    leftSec = _linesleft + 8;
      #else
    topSec = _top() - _timeheight + 38;
    leftSec = _linesleft + 3;
      #endif
    #else  // DSP_MODEL DSP_ILI9341  480x320
      #ifdef AM_PM_STYLE
    topSec = _top() - _timeheight + 30;
    leftSec = _linesleft + 8;
      #else
    topSec = _top() - _timeheight + 50;
    leftSec = _linesleft + 3;
      #endif
    #endif
    gfx.setCursor(leftSec, topSec);
    gfx.print("88");
    gfx.setTextColor(config.theme.seconds, config.theme.background);
    gfx.setCursor(leftSec, topSec);
    sprintf(_tmp, "%02d", network.timeinfo.tm_sec);
    gfx.print(_tmp);  // Másodperc kiírása
  }
  gfx.setTextSize(Clock_GFXfontPtr == nullptr ? TIME_SIZE : 1);
  gfx.setFont(Clock_GFXfontPtr);
    #ifndef DSP_OLED
  gfx.setTextColor(dots ? config.theme.clock : (CLOCKFONT_MONO ? config.theme.clockbg : config.theme.background), config.theme.background);
    #else
  if (clockInTitle) {
    gfx.setTextColor(dots ? config.theme.meta : config.theme.metabg, config.theme.metabg);
  } else {
    gfx.setTextColor(dots ? config.theme.clock : config.theme.background, config.theme.background);
  }
    #endif
  dots = !dots;
  gfx.setCursor(_left() + _dotsleft, _top());
  gfx.print(":");
  gfx.setFont();
  if (_fb->ready()) {
    _fb->display();
  }
    // Mai névnap letöltése - csak ha engedélyezve van.
    #ifdef NAMEDAYS_FILE
  if (config.store.nameday) {
    static uint32_t lastRotation = 0;
    if (millis() - lastRotation >= 4000) {
      getNamedayUpper(_namedayBuf, sizeof(_namedayBuf));
      if (!config.isScreensaver && strcmp(_oldNamedayBuf, _namedayBuf) != 0) {
        strlcpy(_oldNamedayBuf, _namedayBuf, sizeof(_oldNamedayBuf));
        _namedaywidth = strlen(_namedayBuf) * CHARWIDTH * namedayConf.textsize;  // csak változáskor számoljuk újra
        _printNameday();
      }
      lastRotation = millis();
    }
  }
    #endif  // NAMEDAYS_FILE
}

void ClockWidget::_formatDate() {
    // "multi_language"
    #if L10N_LANGUAGE == RU
  sprintf(_tmp, "%2d %s %d", network.timeinfo.tm_mday, LANG::mnths[network.timeinfo.tm_mon], network.timeinfo.tm_year + 1900);
    #elif L10N_LANGUAGE == EN
  sprintf(_tmp, "%2d %s %d", network.timeinfo.tm_mday, LANG::mnths[network.timeinfo.tm_mon], network.timeinfo.tm_year + 1900);
    #elif L10N_LANGUAGE == NL
  sprintf(
    _tmp, "%s %2d %s %d", LANG::dowf[network.timeinfo.tm_wday], network.timeinfo.tm_mday, LANG::mnths[network.timeinfo.tm_mon], network.timeinfo.tm_year + 1900
  );
    #elif L10N_LANGUAGE == HU
  sprintf(
    _tmp, "%d. %s %2d. %s", network.timeinfo.tm_year + 1900, LANG::mnths[network.timeinfo.tm_mon], network.timeinfo.tm_mday,
    LANG::dowf[network.timeinfo.tm_wday]
  );
    #elif L10N_LANGUAGE == PL
  sprintf(
    _tmp, "%s - %02d %s %04d", LANG::dowf[network.timeinfo.tm_wday], network.timeinfo.tm_mday, LANG::mnths[network.timeinfo.tm_mon],
    network.timeinfo.tm_year + 1900
  );
    #elif L10N_LANGUAGE == GR
  sprintf(_tmp, "%2d %s %d", network.timeinfo.tm_mday, LANG::mnths[network.timeinfo.tm_mon], network.timeinfo.tm_year + 1900);
    #elif L10N_LANGUAGE == SK
  sprintf(
    _tmp, "%s %d. %s %2d", LANG::dowf[network.timeinfo.tm_wday], network.timeinfo.tm_mday, LANG::mnths[network.timeinfo.tm_mon], network.timeinfo.tm_year + 1900
  );
    #elif L10N_LANGUAGE == UA
  sprintf(
    _tmp, "%s, %d %s %2d року", LANG::dowf[network.timeinfo.tm_wday], network.timeinfo.tm_mday, LANG::mnths[network.timeinfo.tm_mon],
    network.timeinfo.tm_year + 1900
  );
    #elif L10N_LANGUAGE == DE
  sprintf(
    _tmp, "%s, %02d. %s %d", LANG::dowf[network.timeinfo.tm_wday], network.timeinfo.tm_mday, LANG::mnths[network.timeinfo.tm_mon],
    network.timeinfo.tm_year + 1900
  );

    #endif
}

    /*********************  A névnapok kiírása. *****************************/
    #ifdef NAMEDAYS_FILE
void ClockWidget::getNamedayUpper(char *dest, size_t len) {  // commongfx.h - ban van deklarálva.
  const char *nameday = getNameDay(network.timeinfo.tm_mon + 1, network.timeinfo.tm_mday);
  char tmp[32];
  strlcpy(tmp, nameday, sizeof(tmp));
  for (int i = 0; tmp[i]; i++) {
    tmp[i] = toupper((unsigned char)tmp[i]);
  }
  strlcpy(dest, utf8To(tmp, true), len);
}

void ClockWidget::_printNameday() {
  uint16_t nameday_top;
  memcpy_P(&_namedayConf, &namedayConf, sizeof(WidgetConfig));
      #if DSP_MODEL == DSP_ILI9341
  nameday_top = _namedayConf.top + 14;
      #else
  nameday_top = _namedayConf.top + 22;
      #endif
  if (config.store.nameday) {
    // Rajzold le a nyelvfüggő "Névnap:" szót fehér színnel.
    dsp.setTextColor(config.theme.date, config.theme.background);
    dsp.setCursor(_namedayConf.left, _namedayConf.top);  // egy sorral feljebb
      #if NAMEDAYS_FILE == GR                            // Görög nyevnél túl hosszú, ezért 1- es méret.
    dsp.setTextSize(1);
      #else
    dsp.setTextSize(_namedayConf.textsize);
      #endif
    if (!config.isScreensaver) {
      //Serial.printf("Widget.cpp->nameday_label: %s \n", nameday_label);
      //Serial.printf("Widget.cpp->utf8To(nameday_label, false): %s \n", utf8To(nameday_label, false));
      dsp.print(utf8To(nameday_label, false));  // <<< Itt már a headerből jön "nameday"
      // Csak a neveket rajzolja arany színnel
      dsp.setTextColor(config.theme.nameday, config.theme.background);  // szürke 0x8410
      // Névnap nevének területének törlése a kijelzőről.
      int clearWidth = max(_oldnamedaywidth, _namedaywidth);  // A régi és az új név közül a szélesebb szélessége.
      dsp.fillRect(_namedayConf.left, nameday_top, clearWidth, CHARHEIGHT * _namedayConf.textsize, config.theme.background);
      dsp.setCursor(_namedayConf.left, nameday_top);
      dsp.setTextSize(_namedayConf.textsize);
      dsp.print(_namedayBuf);
      strlcpy(_oldNamedayBuf, _namedayBuf, sizeof(_namedayBuf));
      _oldnamedaywidth = _namedaywidth;
    }
  }
}

void ClockWidget ::clearNameday() {
  int clearWidth = max(_oldnamedaywidth, _namedaywidth);  // A régi és az új név közül a szélesebb szélessége.
  dsp.fillRect(namedayConf.left, namedayConf.top, clearWidth, (CHARHEIGHT * namedayConf.textsize * 2) + 5, config.theme.background);
}
    #endif  //NAMEDAYS_FILE

void ClockWidget::_clearClock() {
    #ifdef PSFBUFFER
  if (_fb->ready()) {
    _fb->clear();
  } else
    #endif
    #ifndef CLOCKFONT5x7
    // dsp.fillRect(_left(), _top()-_timeheight, _clockwidth, _clockheight+1, 0x8410);
    dsp.fillRect(_left(), _top() - (_timeheight + 2), _clockwidth, _clockheight + 3, config.theme.background);
    // Serial.println("Törlés");
    #else
  dsp.fillRect(_left(), _top(), _clockwidth + 1, _clockheight + 1, config.theme.background);
    #endif
}

void ClockWidget::draw(bool force) {
  if (!_active) {
    return;
  }
  _printClock(_getTime() || force);
}

void ClockWidget::_draw() {
  if (!_active) {
    return;
  }
  _printClock(true);
}

void ClockWidget::_reset() {
    #ifdef PSFBUFFER
  if (_fb->ready()) {
    _fb->freeBuffer();
    _getTimeBounds();
    _begin();
  }
    #endif
}

void ClockWidget::_clear() {
  _clearClock();
}
  #else   //#ifndef DSP_LCD

void ClockWidget::_printClock(bool force) {
  strftime(_timebuffer, sizeof(_timebuffer), "%H:%M", &network.timeinfo);
  if (force) {
    dsp.setCursor(dsp.width() - 5, 0);
    dsp.print(_timebuffer);
  }
  dsp.setCursor(dsp.width() - 5 + 2, 0);
  dsp.print((network.timeinfo.tm_sec % 2 == 0) ? ":" : " ");
}

void ClockWidget::_clearClock() {}

void ClockWidget::draw() {
  if (!_active) {
    return;
  }
  _printClock(true);
}
void ClockWidget::_draw() {
  if (!_active) {
    return;
  }
  _printClock(true);
}
void ClockWidget::_reset() {}
void ClockWidget::_clear() {}
  #endif  //#ifndef DSP_LCD

/**************************
      BITRATE WIDGET
 **************************/
void BitrateWidget::init(BitrateConfig bconf, uint16_t fgcolor, uint16_t bgcolor) {
  Widget::init(bconf.widget, fgcolor, bgcolor);
  _dimension = bconf.dimension;
  _bitrate = 0;
  _format = BF_UNKNOWN;
  _charSize(bconf.widget.textsize, _charWidth, _textheight);
  memset(_buf, 0, 6);
  // Serial.printf("widgets.cpp->BitrateWidget _init() _dimension %d\n", _dimension) ;
}

void BitrateWidget::setBitrate(uint16_t bitrate) {
  //  Serial.printf("widgets.cpp->BitrateWidget setBitrate() bitrate: %d \n", bitrate) ;
  _bitrate = bitrate;  // Módosítás
                       //  if(_bitrate>999) _bitrate = 999;
  if (_bitrate > 20000) {
    _bitrate = _bitrate / 1000;
  }
  _draw();
}

void BitrateWidget::setFormat(BitrateFormat format) {
  //  Serial.printf("widgets.cpp->BitrateWidget setFormat() format: %d \n", format) ;
  _format = format;
  _draw();
}

//TODO move to parent
void BitrateWidget::_charSize(uint8_t textsize, uint8_t &width, uint16_t &height) {
  #ifndef DSP_LCD
  width = textsize * CHARWIDTH;
  height = textsize * CHARHEIGHT;
  #else
  width = 1;
  height = 1;
  #endif
}

void BitrateWidget::_draw() {  //Módosítás
  _clear();
  // Serial.printf("widgets.cpp->BitrateWidget _draw() _active: %d _format: %d _bitrate %d \n", _active, _format, _bitrate) ;
  if (!_active || _format == BF_UNKNOWN || _bitrate == 0) {
    // Serial.printf("widgets.cpp->BitrateWidget _draw() nem fut le. \n") ;
    return;
  }
  if (config.store.nameday) {  //  Ha be van kapcsolva a nameday Módosítás "nameday"
    dsp.drawRect(_config.left, _config.top, _dimension * 2, (_dimension / 2) - 6, _fgcolor);
    dsp.fillRect(_config.left + _dimension, _config.top, _dimension, (_dimension / 2) - 6, _fgcolor);
    // Serial.printf("widgets.cpp->BitrateWidget _draw() config.store.nameday: %d \n", config.store.nameday) ;
  } else {
    dsp.drawRect(_config.left, _config.top, _dimension, _dimension, _fgcolor);                               // Eredeti.
    dsp.fillRect(_config.left, _config.top + _dimension / 2 + 1, _dimension, _dimension / 2 - 1, _fgcolor);  // Eredeti
    // Serial.printf("widgets.cpp->BitrateWidget _draw() config.store.nameday: %d \n", config.store.nameday) ;
  }
  dsp.setFont();
  dsp.setTextSize(_config.textsize);
  dsp.setTextColor(_fgcolor, _bgcolor);
  if (_bitrate < 999) {
    snprintf(_buf, 6, "%d", _bitrate);  // Módisítás "bitrate"
  } else {
    float _br = (float)_bitrate / 1000;
    snprintf(_buf, 6, "%.1f", _br);
  }
  if (config.store.nameday) {  //  Ha be van kapcsolva a nameday
    dsp.setCursor(_config.left + _dimension / 2 - _charWidth * strlen(_buf) / 2, _config.top + _dimension / 4 - _textheight / 2 - 2);
  } else {
    dsp.setCursor(_config.left + _dimension / 2 - _charWidth * 3 / 2 + 1, _config.top + (_dimension / 2) - 3 - _textheight);
  }
  dsp.print(_buf);
  dsp.setTextColor(_bgcolor, _fgcolor);
  if (config.store.nameday) {  //  Ha be van kapcsolva a nameday
    dsp.setCursor(_config.left + _dimension + _dimension / 2 - _charWidth * 3 / 2, _config.top + _dimension / 4 - _textheight / 2 - 2);
  } else {
    dsp.setCursor(_config.left + _dimension / 2 - _charWidth * 3 / 2, _config.top + _dimension / 2 + _dimension / 4 - _textheight / 2 + 2);
  }
  switch (_format) {
    case BF_MP3:  dsp.print("MP3"); break;
    case BF_AAC:  dsp.print("AAC"); break;
    case BF_FLAC: dsp.print("FLC"); break;
    case BF_OGG:  dsp.print("OGG"); break;
    case BF_WAV:  dsp.print("WAV"); break;
    case BF_VOR:  dsp.print("VOR"); break;  // Módisítás "bitrate"
    case BF_OPU:  dsp.print("OPU"); break;  // Módisítás "bitrate"
    default:      break;
  }
}

void BitrateWidget::_clear() {
  if (config.store.nameday) {                                                           //  Ha be van kapcsolva a nameday
    dsp.fillRect(_config.left, _config.top, _dimension * 2, _dimension / 2, _bgcolor);  // lapos forma törlése
    //Serial.printf("widgets.cpp->BitrateWidget _clear() (lapos törlés) config.store.nameday: %d \n", config.store.nameday) ;
  } else {
    dsp.fillRect(_config.left, _config.top, _dimension, _dimension, _bgcolor);  // négyzetes forma törlése
    //Serial.printf("widgets.cpp->BitrateWidget _clear() (négyzetes törlés) config.store.nameday: %d \n", config.store.nameday) ;
  }
  VuWidget::setLabelsDrawn(false);  // Módosítás! (false) esetén újrarajzolja az L R címkét. "wumeter"
}

/* Törli mindkét bitratewidget területét és a "nameday" területet is. */
void BitrateWidget::clearAll() {
  dsp.fillRect(_config.left, _config.top, _dimension * 2, _dimension + 11, _bgcolor);
  // Serial.printf("widgets.cpp->BitrateWidget clearAll() \n") ;
}

/**************************
      PLAYLIST WIDGET
 **************************/
void PlayListWidget::init(ScrollWidget *current) {
  Widget::init({0, 0, 0, WA_LEFT}, 0, 0);
  _current = current;
  #ifndef DSP_LCD
  _plItemHeight = playlistConf.widget.textsize * (CHARHEIGHT - 1) + playlistConf.widget.textsize * 4;
  _plTtemsCount = round((float)dsp.height() / _plItemHeight);
  if (_plTtemsCount % 2 == 0) {
    _plTtemsCount++;
  }
  _plCurrentPos = _plTtemsCount / 2;
  _plYStart = (dsp.height() / 2 - _plItemHeight / 2) - _plItemHeight * (_plTtemsCount - 1) / 2 + playlistConf.widget.textsize * 2;
  #else
  _plTtemsCount = PLMITEMS;
  _plCurrentPos = 1;
  #endif
}

uint8_t PlayListWidget::_fillPlMenu(int from, uint8_t count) {
  int ls = from;
  uint8_t c = 0;
  bool finded = false;
  if (config.playlistLength() == 0) {
    return 0;
  }
  File playlist = config.SDPLFS()->open(REAL_PLAYL, "r");
  File index = config.SDPLFS()->open(REAL_INDEX, "r");
  while (true) {
    if (ls < 1) {
      ls++;
      _printPLitem(c, "");
      c++;
      continue;
    }
    if (!finded) {
      index.seek((ls - 1) * 4, SeekSet);
      uint32_t pos;
      index.readBytes((char *)&pos, 4);
      finded = true;
      index.close();
      playlist.seek(pos, SeekSet);
    }
    bool pla = true;
    while (pla) {
      pla = playlist.available();
      String stationName = playlist.readStringUntil('\n');
      stationName = stationName.substring(0, stationName.indexOf('\t'));
      if (config.store.numplaylist && stationName.length() > 0) {
        stationName = String(from + c) + " " + stationName;
      }
      _printPLitem(c, stationName.c_str());
      c++;
      if (c >= count) {
        break;
      }
    }
    break;
  }
  playlist.close();
  return c;
}
  #ifndef DSP_LCD
void PlayListWidget::drawPlaylist(uint16_t currentItem) {
  uint8_t lastPos = _fillPlMenu(currentItem - _plCurrentPos, _plTtemsCount);
  if (lastPos < _plTtemsCount) {
    dsp.fillRect(0, lastPos * _plItemHeight + _plYStart, dsp.width(), dsp.height() / 2, config.theme.background);
  }
}

void PlayListWidget::_printPLitem(uint8_t pos, const char *item) {
  dsp.setTextSize(playlistConf.widget.textsize);
  if (pos == _plCurrentPos) {
    _current->setText(item);
  } else {
    uint8_t plColor = (abs(pos - _plCurrentPos) - 1) > 4 ? 4 : abs(pos - _plCurrentPos) - 1;
    dsp.setTextColor(config.theme.playlist[plColor], config.theme.background);
    dsp.setCursor(TFT_FRAMEWDT, _plYStart + pos * _plItemHeight);
    dsp.fillRect(0, _plYStart + pos * _plItemHeight - 1, dsp.width(), _plItemHeight - 2, config.theme.background);
    dsp.print(utf8To(item, true));
  }
}
  #else
void PlayListWidget::_printPLitem(uint8_t pos, const char *item) {
  if (pos == _plCurrentPos) {
    _current->setText(item);
  } else {
    dsp.setCursor(1, pos);
    char tmp[dsp.width()] = {0};
    strlcpy(tmp, utf8To(item, true), dsp.width());
    dsp.print(tmp);
  }
}

void PlayListWidget::drawPlaylist(uint16_t currentItem) {
  dsp.clear();
  _fillPlMenu(currentItem - _plCurrentPos, _plTtemsCount);
  dsp.setCursor(0, 1);
  dsp.write(uint8_t(126));
}
  #endif  //DSP_LCD

#endif  // #if DSP_MODEL!=DSP_DUMMY
