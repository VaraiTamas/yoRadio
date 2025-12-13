/*************************************************************************************
    ILI9341 320x240 displays configuration file.
    Copy this file to yoRadio/src/displays/conf/displayILI9341conf_custom.h
    and modify it
    More info on https://github.com/e2002/yoradio/wiki/Widgets#widgets-description
*************************************************************************************/
// clang-format off
#ifndef displayILI9341conf_h
#define displayILI9341conf_h

#define DSP_WIDTH       320
#define TFT_FRAMEWDT    8
#define MAX_WIDTH       DSP_WIDTH-TFT_FRAMEWDT*2

#define bootLogoTop     68

/* SROLLS  */                            /* {{ left, top, fontsize, align }, buffsize, uppercase, width, scrolldelay, scrolldelta, scrolltime } */
const ScrollConfig metaConf       PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 3, WA_LEFT }, 140, true, MAX_WIDTH, 5000, 5, 30 };
const ScrollConfig title1Conf     PROGMEM = {{ TFT_FRAMEWDT, 48, 2, WA_LEFT }, 140, true, MAX_WIDTH, 5000, 4, 30 };
const ScrollConfig title2Conf     PROGMEM = {{ TFT_FRAMEWDT, 70, 2, WA_LEFT }, 140, true, MAX_WIDTH, 5000, 4, 30 };
const ScrollConfig playlistConf   PROGMEM = {{ TFT_FRAMEWDT, 112, 2, WA_LEFT }, 140, true, MAX_WIDTH, 1000, 4, 30 };
const ScrollConfig apTitleConf    PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 3, WA_CENTER }, 140, false, MAX_WIDTH, 0, 4, 20 };
const ScrollConfig apSettConf     PROGMEM = {{ TFT_FRAMEWDT, 240-TFT_FRAMEWDT-16, 2, WA_LEFT }, 140, false, MAX_WIDTH, 0, 4, 30 };
const ScrollConfig weatherConf    PROGMEM = {{ TFT_FRAMEWDT, 95, 1, WA_CENTER }, 140, false, MAX_WIDTH, 0, 2, 30 };

/* BACKGROUNDS  */                       /* {{ left, top, fontsize, align }, width, height, outlined } */
const FillConfig   metaBGConf     PROGMEM = {{ 3, 36, 0, WA_CENTER }, DSP_WIDTH - 6, 1, true };
const FillConfig   metaBGConfInv  PROGMEM = {{ 0, 38, 0, WA_LEFT }, DSP_WIDTH, 1, false };
const FillConfig   volbarConf     PROGMEM = {{ TFT_FRAMEWDT, 240-TFT_FRAMEWDT-6, 0, WA_LEFT }, MAX_WIDTH, 4, true };
const FillConfig  playlBGConf     PROGMEM = {{ 0, 107, 0, WA_LEFT }, DSP_WIDTH, 24, false };
const FillConfig  heapbarConf     PROGMEM = {{ 0, 239, 0, WA_LEFT }, DSP_WIDTH, 1, false };

/* WIDGETS  */                           /* { left, top, fontsize, align } */
const WidgetConfig bootstrConf    PROGMEM = { 0, 182, 1, WA_CENTER };
const WidgetConfig bitrateConf    PROGMEM = { TFT_FRAMEWDT, 148, 1, WA_RIGHT };
const WidgetConfig voltxtConf     PROGMEM = { 0, 214, 1, WA_CENTER };  // Hangerő
const WidgetConfig  iptxtConf     PROGMEM = { TFT_FRAMEWDT, 214, 1, WA_LEFT };
const WidgetConfig   rssiConf     PROGMEM = { TFT_FRAMEWDT, 214-6, 2, WA_RIGHT };
const WidgetConfig numConf        PROGMEM = { 0, 120+30, 0, WA_CENTER };
const WidgetConfig apNameConf     PROGMEM = { TFT_FRAMEWDT, 66, 2, WA_CENTER };
const WidgetConfig apName2Conf    PROGMEM = { TFT_FRAMEWDT, 90, 2, WA_CENTER };
const WidgetConfig apPassConf     PROGMEM = { TFT_FRAMEWDT, 130, 2, WA_CENTER };
const WidgetConfig apPass2Conf    PROGMEM = { TFT_FRAMEWDT, 154, 2, WA_CENTER };
const WidgetConfig clockConf      PROGMEM = { TFT_FRAMEWDT*2, 160, 1, WA_RIGHT };
const WidgetConfig bootWdtConf    PROGMEM = { 0, 162, 1, WA_CENTER };
const WidgetConfig namedayConf    PROGMEM = { TFT_FRAMEWDT, 139, 1, WA_LEFT }; // Módosítás új sor "nameday"
const WidgetConfig dateConf       PROGMEM = { TFT_FRAMEWDT *2, 169, 1, WA_LEFT }; // Módosítás új sor "date"

const ProgressConfig bootPrgConf    PROGMEM = { 90, 14, 4 };
const BitrateConfig fullbitrateConf PROGMEM = {{8, 114, 1, WA_LEFT}, 41 }; // left, top, fontsize, align, border size

/* BANDS  */                             
#ifdef BOOMBOX_STYLE
const WidgetConfig  vuConf        PROGMEM = { 24, 190, 1, WA_CENTER }; // center fektetett, "align" nincs használva
const VUBandsConfig bandsConf     PROGMEM = { 130, 5, 4, 2, 20, 5 }; /* { onebandwidth, onebandheight, bandsHspace, bandsVspace, numofbands, fadespeed } */
#else
const WidgetConfig  vuConf        PROGMEM = { 33, 190, 1, WA_CENTER }; // center fektetett, "align" nincs használva
const VUBandsConfig bandsConf     PROGMEM = { 200, 6, 2, 2, 30, 4 }; /* { onebandwidth, onebandheight, bandsHspace, bandsVspace, numofbands, fadespeed } */
#endif
/* STRINGS  */
const char         numtxtFmt[]    PROGMEM = "%d";
const char           rssiFmt[]    PROGMEM = "WiFi %ddBm";
const char          iptxtFmt[]    PROGMEM = "\010 %s";
const char         voltxtFmt[]    PROGMEM = "\023\025%d%%";
const char        bitrateFmt[]    PROGMEM = "%d kBs";

/* MOVES  */                             /* { left, top, width } */
const MoveConfig    clockMove     PROGMEM = { 0, 176, -1 };
const MoveConfig   weatherMove    PROGMEM = { 10, 95, MAX_WIDTH };
const MoveConfig   weatherMoveVU  PROGMEM = { TFT_FRAMEWDT, 95, MAX_WIDTH};

#endif
// clang-format on