/*************************************************************************************
    ST7796 480X320 displays configuration file.
    Copy this file to yoRadio/src/displays/conf/displayST7789conf_custom.h
    and modify it
    More info on https://github.com/e2002/yoradio/wiki/Widgets#widgets-description
*************************************************************************************/

#ifndef displayST7789conf_h
#define displayST7789conf_h

#define DSP_WIDTH       480
#define DSP_HEIGHT      320
#define TFT_FRAMEWDT    10
#define MAX_WIDTH       DSP_WIDTH-TFT_FRAMEWDT*2

#if BITRATE_FULL
  #define TITLE_FIX 44
#else
  #define TITLE_FIX 0
#endif
#define bootLogoTop     110

/* SROLLS  */                            /* {{ left, top, fontsize, align }, buffsize, uppercase, width, scrolldelay, scrolldelta, scrolltime } */
const ScrollConfig metaConf       PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 4, WA_LEFT }, 140, true, MAX_WIDTH, 5000, 7, 40 };
const ScrollConfig title1Conf     PROGMEM = {{TFT_FRAMEWDT, 62, 2, WA_LEFT}, 140, true, MAX_WIDTH, 5000, 7, 40};
const ScrollConfig title2Conf     PROGMEM = {{TFT_FRAMEWDT, 86, 2, WA_LEFT}, 140, true, MAX_WIDTH, 5000, 7, 40};
const ScrollConfig playlistConf   PROGMEM = {{ TFT_FRAMEWDT, 146, 3, WA_LEFT }, 140, true, MAX_WIDTH, 1000, 7, 40 };
const ScrollConfig apTitleConf    PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 4, WA_CENTER }, 140, false, MAX_WIDTH, 0, 7, 40 };
const ScrollConfig apSettConf     PROGMEM = {{ TFT_FRAMEWDT, 320-TFT_FRAMEWDT-16, 2, WA_LEFT }, 140, false, MAX_WIDTH, 0, 7, 40 };
const ScrollConfig weatherConf    PROGMEM = {{ TFT_FRAMEWDT, 116, 2, WA_CENTER }, 140, false, MAX_WIDTH, 5000, 4, 40 };

/* BACKGROUNDS  */                       /* {{ left, top, fontsize, align }, width, height, outlined } */
const FillConfig metaBGConf       PROGMEM = {{3, 45, 0, WA_CENTER}, DSP_WIDTH - 6, 1, true}; // Csík rajzolása a rádióadó neve alá.
const FillConfig   metaBGConfInv  PROGMEM = {{ 0, 50, 0, WA_LEFT }, DSP_WIDTH, 2, false };
const FillConfig volbarConf       PROGMEM = {{TFT_FRAMEWDT, DSP_HEIGHT - TFT_FRAMEWDT - 8, 0, WA_LEFT}, MAX_WIDTH, 5, true};
const FillConfig  playlBGConf     PROGMEM = {{ 0, 138, 0, WA_LEFT }, DSP_WIDTH, 36, false };
const FillConfig  heapbarConf     PROGMEM = {{ 0, DSP_HEIGHT-2, 0, WA_LEFT }, DSP_WIDTH, 2, false };

/* WIDGETS  */ /* { left, top, fontsize, align } */
const WidgetConfig bootstrConf   PROGMEM = {0, 243, 2, WA_CENTER};
const WidgetConfig bitrateConf   PROGMEM = {TFT_FRAMEWDT, 145, 2, WA_RIGHT};
const WidgetConfig voltxtConf    PROGMEM = {0, DSP_HEIGHT - 38, 2, WA_CENTER}; // Hangerő
const WidgetConfig iptxtConf     PROGMEM = {TFT_FRAMEWDT, DSP_HEIGHT - 38, 2, WA_LEFT};
const WidgetConfig rssiConf      PROGMEM = {TFT_FRAMEWDT, DSP_HEIGHT - 34, 2, WA_RIGHT};
const WidgetConfig numConf       PROGMEM = {0, 200, 70, WA_CENTER};
const WidgetConfig apNameConf    PROGMEM = {TFT_FRAMEWDT, 88, 3, WA_CENTER};
const WidgetConfig apName2Conf   PROGMEM = {TFT_FRAMEWDT, 120, 3, WA_CENTER};
const WidgetConfig apPassConf    PROGMEM = {TFT_FRAMEWDT, 173, 3, WA_CENTER};
const WidgetConfig apPass2Conf   PROGMEM = {TFT_FRAMEWDT, 205, 3, WA_CENTER};
const WidgetConfig clockConf     PROGMEM = {10, 211, 2, WA_RIGHT}; // {jobb oldali távolság, top, fontsize}
const WidgetConfig vuConf        PROGMEM = {35, 258, 1, WA_CENTER}; // center fektetett, "align" nincs használva
const WidgetConfig bootWdtConf   PROGMEM = {0, 216, 1, WA_CENTER};
const WidgetConfig namedayConf   PROGMEM = { TFT_FRAMEWDT, 175, 2, WA_LEFT };  // Módosítás új sor "nameday"
const WidgetConfig dateConf      PROGMEM = { TFT_FRAMEWDT *2, 226, 1, WA_LEFT }; // Módosítás új sor "date"
const ProgressConfig bootPrgConf PROGMEM = {90, 14, 4};

//{{ left, top, fontsize, align }dimension}
														  
const BitrateConfig fullbitrateConf PROGMEM = {{10, 142, 2, WA_RIGHT}, 60};

/* BANDS { onebandwidth (width), onebandheight (height), bandsHspace (space), bandsVspace (vspace), numofbands (perheight), fadespeed (fadespeed)} */
#ifdef BOOMBOX_STYLE
const VUBandsConfig bandsConf PROGMEM = {200, 7, 4, 2, 20, 12}; // 29
#else
const VUBandsConfig bandsConf PROGMEM = {300, 7, 3, 2, 30, 6}; // saját  {400, 7, 3, 2, 8, 29}; 
#endif

/* STRINGS  */
const char numtxtFmt[]  PROGMEM = "%d";
const char rssiFmt[]    PROGMEM = "WiFi %ddBm";
// const char           rssiFmt[]    PROGMEM = "WiFi %d"; // Original
const char iptxtFmt[]   PROGMEM = "%s";
const char voltxtFmt[]  PROGMEM = "\023\025%d%%"; //Original "\023\025%d" Módosítás "vol_step"
const char bitrateFmt[] PROGMEM = "%d kBs";

/* MOVES  */ /* { left, top, width } */
const MoveConfig clockMove     PROGMEM = {0, 176, -1};
const MoveConfig weatherMove   PROGMEM = {10, 116, MAX_WIDTH}; // Ha a VU ki van kapcsolva (szélesített pozíció)
const MoveConfig weatherMoveVU PROGMEM = {10, 116, MAX_WIDTH}; // Az időjárás widget pozíciója.

#endif
