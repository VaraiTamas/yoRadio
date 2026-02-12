#include "../core/options.h"
#if DSP_MODEL == DSP_SSD1322
  #include "dspcore.h"
  #include "../core/config.h"

  #if DSP_HSPI
DspCore::DspCore() : Jamis_SSD1322(256, 64, &SPI2, TFT_DC, TFT_RST, TFT_CS) {}
  #else
DspCore::DspCore() : Jamis_SSD1322(256, 64, &SPI, TFT_DC, TFT_RST, TFT_CS) {}
  #endif

void DspCore::initDisplay() {
  // clang-format off
 
  /*----- SCREEN -----*/
   config.theme.background    = TFT_BG;

  /*----- META -----*/
    config.theme.meta          = WHITE;
    config.theme.metabg        = BLACK;
    config.theme.metafill      = GRAY_9;
    config.theme.title1        = GRAY_7;
    config.theme.title2        = GRAY_3;
    config.theme.bitrate       = TFT_FG;

  /*----- WEATHER -----*/
    config.theme.weather       = GRAY_2;

  /*---- VOLUME SCREEN -----*/
    config.theme.digit         = TFT_FG;

  /*----- CLOCK, DATE -----*/
    config.theme.clock         = TFT_FG;
    config.theme.clockbg       = GRAY_1;
    config.theme.date          = GRAY_1;

  /*----- VU WIDGET ----*/
    config.theme.vumax         = 0xF;
    config.theme.vumid         = GRAY_5;
    config.theme.vumin         = GRAY_2;

 /*----- FOOTER -----*/
    config.theme.volbarout     = GRAY_9;
    config.theme.volbarin      = GRAY_9;
    config.theme.vol           = GRAY_3;
    config.theme.ip            = GRAY_1;
    config.theme.ch            = GRAY_1;   // Current channel number in the footer
    config.theme.rssi          = GRAY_5;
    config.theme.buffer        = TFT_FG;

 /*----- PLAYLIST -----*/
 #ifdef PLAYLIST_SCROLL_MOVING_CURSOR
    config.theme.plcurrent     = WHITE;
    config.theme.plcurrentbg   = BLACK;
    config.theme.plcurrentfill = BLACK;
    for(byte i=0;i<5;i++) config.theme.playlist[i] = GRAY_1;
#else
    config.theme.plcurrent     = TFT_BG;
    config.theme.plcurrentbg   = GRAY_7;
    config.theme.plcurrentfill = GRAY_7;
    for(byte i=0;i<5;i++) config.theme.playlist[i] = GRAY_1;
#endif

  begin();
  cp437(true);
  flip();
  invert();
  setTextWrap(false);
}

void DspCore::clearDsp(bool black){ clearDisplay(); }
void DspCore::flip(){ setRotation(config.store.flipscreen?2:0); }
void DspCore::invert(){ invertDisplay(config.store.invertdisplay); }
void DspCore::sleep(void){ oled_command(SSD1322_DISPLAYOFF); }
void DspCore::wake(void){ oled_command(SSD1322_DISPLAYON); }

#endif
