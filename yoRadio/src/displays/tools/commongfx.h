#ifndef common_gfx_h
#define common_gfx_h
#include "../widgets/widgetsconfig.h" // displayXXXDDDDconf.h
#include "utf8To.h"
#define ADAFRUIT_CLIPPING  DSP_MODEL != DSP_ILI9225

typedef struct clipArea {
    uint16_t left;
    uint16_t top;
    uint16_t width;
    uint16_t height;
} clipArea;

class psFrameBuffer;

class DspCore : public yoDisplay {
  public:
    DspCore();
    void initDisplay();
    void clearDsp(bool black = false);
    void printClock() {}
#ifdef DSP_OLED
    inline void loop(bool force = false) {
        display();
        vTaskDelay(DSP_MODEL == DSP_ST7920 ? 10 : 0);
    }
    inline void drawLogo(uint16_t top) {
        drawBitmap((width() - LOGO_WIDTH) / 2, top, logo, LOGO_WIDTH, LOGO_HEIGHT, 1);
        display();
    }
#else
    inline void loop(bool force = false) {}
    inline void drawLogo(uint16_t top) { drawRGBBitmap((width() - LOGO_WIDTH) / 2, top, logo, LOGO_WIDTH, LOGO_HEIGHT); }
#endif
    void     flip();
    void     invert();
    void     sleep();
    void     wake();
    void     setScrollId(void* scrollid) { _scrollid = scrollid; }
    void*    getScrollId() { return _scrollid; }
    uint16_t textWidth(const char* txt);
#if DSP_MODEL == DSP_ILI9225
    uint16_t        width(void) { return (int16_t)maxX(); }
    uint16_t        height(void) { return (int16_t)maxY(); }
    inline void     drawRGBBitmap(int16_t x, int16_t y, const uint16_t* bitmap, int16_t w, int16_t h) { drawBitmap(x, y, bitmap, w, h); }
    uint16_t        print(const char* s);
    void            fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void            drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void            setFont(const GFXfont* f = NULL);
    void            setFont(uint8_t* font, bool monoSp = false);
    void            setTextColor(uint16_t fg, uint16_t bg);
    void            setCursor(int16_t x, int16_t y);
    void            drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void            drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    inline uint16_t drawChar(uint16_t x, uint16_t y, uint16_t ch, uint16_t color = COLOR_WHITE) {
        if (_clipping) {
            if ((x < _cliparea.left) || (x >= _cliparea.left + _cliparea.width) || (y < _cliparea.top) || (y > _cliparea.top + _cliparea.height)) { return cfont.width; }
        }
        uint16_t ret = TFT_22_ILI9225::drawChar(x, y, ch, color);
        return ret;
    }
    void setTextSize(uint8_t s);
#endif
#ifdef ADAFRUIT_CLIPPING
    inline void writePixel(int16_t x, int16_t y, uint16_t color) {
        if (_clipping) {
            if ((x < _cliparea.left) || (x > _cliparea.left + _cliparea.width) || (y < _cliparea.top) || (y > _cliparea.top + _cliparea.height)) return;
        }
        yoDisplay::writePixel(x, y, color);
    }
    inline void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        if (_clipping) {
            if ((x < _cliparea.left) || (x >= _cliparea.left + _cliparea.width) || (y < _cliparea.top) || (y > _cliparea.top + _cliparea.height)) return;
        }
        yoDisplay::writeFillRect(x, y, w, h, color);
    }
#else
    inline void writePixel(int16_t x, int16_t y, uint16_t color) {}
    inline void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {}
#endif
    inline void setClipping(clipArea ca) {
        _cliparea = ca;
        _clipping = true;
    }
    inline void clearClipping() {
        _clipping = false;
    }

  private:
    bool     _clipping;
    clipArea _cliparea;
    void*    _scrollid;
#ifdef PSFBUFFER
    psFrameBuffer* _fb = nullptr;
#endif
#if DSP_MODEL == DSP_ILI9225
    uint16_t _bgcolor, _fgcolor;
    int16_t  _cursorx, _cursory;
    bool     _gFont /*, _started*/;
#endif
};

extern DspCore dsp;
#endif
