/*************************************************************************************
    SSD1322 265x64 displays configuration file.
    Copy this file to yoRadio/src/displays/conf/displaySSD1322conf_custom.h
    and modify it
    More info on https://github.com/e2002/yoradio/wiki/Widgets#widgets-description
*************************************************************************************/

#ifndef displaySSD1322conf_h
#define displaySSD1322conf_h

#define DSP_WIDTH    256
#define TFT_FRAMEWDT 1
#define MAX_WIDTH    DSP_WIDTH - TFT_FRAMEWDT * 2

#define HIDE_HEAPBAR
//#define HIDE_VOL
#define HIDE_WEATHER
//#define HIDE_TITLE2
#define TITLE_FIX 0
#define bootLogoTop 68
// clang-format off
/* SROLLS  */                            /* {{ left, top, fontsize, align }, buffsize, uppercase, width, scrolldelay, scrolldelta, scrolltime } */
const ScrollConfig metaConf       PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 2, WA_LEFT }, 140, true, DSP_WIDTH/2+45, 5000, 2, 25 };
const ScrollConfig title1Conf     PROGMEM = {{ 1, 18, 1, WA_LEFT }, 140, true, DSP_WIDTH/2+45, 5000, 2, 25 };
const ScrollConfig title2Conf     PROGMEM = {{ 1, 28, 1, WA_LEFT }, 140, true, DSP_WIDTH/2+45, 5000, 2, 25 };
const ScrollConfig playlistConf   PROGMEM = {{ TFT_FRAMEWDT, 30, 1, WA_LEFT }, 140, true, MAX_WIDTH, 500, 2, 25 };
const ScrollConfig apTitleConf    PROGMEM = {{ TFT_FRAMEWDT+1, TFT_FRAMEWDT+1, 1, WA_CENTER }, 140, false, MAX_WIDTH-2, 0, 2, 25 };
const ScrollConfig apSettConf     PROGMEM = {{ TFT_FRAMEWDT, 64-7, 1, WA_LEFT }, 140, false, MAX_WIDTH, 0, 2, 25 };
const ScrollConfig weatherConf    PROGMEM = {{ 20, 56, 1, WA_LEFT }, 140, true, DSP_WIDTH/2+40, 0, 2, 25 };

/* BACKGROUNGC9106DS  */                       /* {{ left, top, fontsize, align }, width, height, outlined } */
const FillConfig   metaBGConf     PROGMEM = {{ 0, 0, 0, WA_LEFT }, 0, 0, false };       
const FillConfig   volbarConf     PROGMEM = {{ 0, 0, 0, WA_LEFT }, DSP_WIDTH, 0, false };   // → a hangerő csík (SliderWidget)
const FillConfig  playlBGConf     PROGMEM = {{ 0, 66, 0, WA_LEFT }, DSP_WIDTH, 12, false };
//const FillConfig  heapbarConf     PROGMEM = {{ 0, 63, 0, WA_LEFT }, DSP_WIDTH, 1, false };

/* WIDGETS  */                           /* { left, top, fontsize, align } */
const WidgetConfig bootstrConf    PROGMEM = { 0, 64-8, 1, WA_CENTER };

/* FOOTER */
const WidgetConfig iptxtConf      PROGMEM = { TFT_FRAMEWDT, 56, 1, WA_LEFT };
const WidgetConfig voltxtConf     PROGMEM = { 120, 57, 1, WA_CENTER }; //→ az alsó, kis hangerő szöveg (PLAYER footer)
const WidgetConfig chtxtConf      PROGMEM = { 160, 57, 1, WA_LEFT };   //→ az aktuális csatorna CH:szöveg (PLAYER footer)
const WidgetConfig rssiConf       PROGMEM = { 214, 57, 1, WA_LEFT };   // WiFi
const WidgetConfig bitrateConf    PROGMEM = { 200, 45, 1, WA_RIGHT };  //audio kodek egysoros, nem fullbitrate

const WidgetConfig numConf        PROGMEM = { 0, 45, 1, WA_CENTER };   // → a nagy hangerő szám (VOL képernyő)
const WidgetConfig apNameConf     PROGMEM = { 0, 18, 1, WA_CENTER };
const WidgetConfig apName2Conf    PROGMEM = { 0, 26, 1, WA_CENTER };
const WidgetConfig apPassConf     PROGMEM = { 0, 37, 1, WA_CENTER };
const WidgetConfig apPass2Conf    PROGMEM = { 0, 45, 1, WA_CENTER };
const WidgetConfig clockConf      PROGMEM = { 0, 22, 14, WA_RIGHT };  
const WidgetConfig namedayConf    PROGMEM = { TFT_FRAMEWDT, 175, 2, WA_LEFT };  // Módosítás új sor "nameday"
const WidgetConfig dateConf       PROGMEM = { 1, 28, 1, WA_RIGHT }; // Módosítás új sor "date"

const WidgetConfig   bootWdtConf  PROGMEM = { 0, 64-8*2-5, 1, WA_CENTER };
const ProgressConfig bootPrgConf  PROGMEM = { 90, 10, 4 };
                                           //{{ left, top, fontsize, align }dimension}
const BitrateConfig fullbitrateConf PROGMEM = {{230, 42, 1, WA_RIGHT}, 24 };

/* BANDS  */  /* { onebandwidth, onebandheight, bandsHspace, bandsVspace, numofbands, fadespeed } 
                    width height space vspace perheight fadespeed */
const VUBandsConfig bandsConf     PROGMEM = { 150, 4, 2, 1, 50, 4 };//{ 12, 48, 2, 1, 8, 3 };
/* WIDGETS  */                           /* { left, top, fontsize, align } */
const WidgetConfig vuConf         PROGMEM = { 8, 40, 1, WA_CENTER };

/* STRINGS  */
const char         numtxtFmt[]    PROGMEM = "%d";
const char           rssiFmt[]    PROGMEM = "WiFi %d";
const char          iptxtFmt[]    PROGMEM = "%s";//\010 znaczek ip
const char         voltxtFmt[]    PROGMEM = "\023\025%d";
const char        bitrateFmt[]    PROGMEM = "%dkBs";

/* MOVES  */                             /* { left, top, width } */
const MoveConfig    clockMove     PROGMEM = { 0, 0, -1 };
const MoveConfig   weatherMove    PROGMEM = { 0, 0, -1 };
const MoveConfig   weatherMoveVU  PROGMEM = { 0, 0, -1 };

#endif
