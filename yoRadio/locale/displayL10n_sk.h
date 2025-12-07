// v0.9.555
#ifndef dsp_full_loc
#define dsp_full_loc
#include <pgmspace.h>
#include "../myoptions.h"
// clang-format off
/*************************************************************************************
    HOWTO:
    Copy this file to yoRadio/locale/displayL10n_custom.h
    and modify it
*************************************************************************************/
const char mon[] PROGMEM = "po";
const char tue[] PROGMEM = "ut";
const char wed[] PROGMEM = "st";
const char thu[] PROGMEM = "štt";
const char fri[] PROGMEM = "pi";
const char sat[] PROGMEM = "so";
const char sun[] PROGMEM = "ne";

const char monf[] PROGMEM = "pondelok";
const char tuef[] PROGMEM = "utorok";
const char wedf[] PROGMEM = "streda";
const char thuf[] PROGMEM = "štvrtok";
const char frif[] PROGMEM = "piatok";
const char satf[] PROGMEM = "sobota";
const char sunf[] PROGMEM = "nedeľa";

const char jan[] PROGMEM = "január";
const char feb[] PROGMEM = "február";
const char mar[] PROGMEM = "marec";
const char apr[] PROGMEM = "apríl";
const char may[] PROGMEM = "máj";
const char jun[] PROGMEM = "jún";
const char jul[] PROGMEM = "júl";
const char aug[] PROGMEM = "august";
const char sep[] PROGMEM = "september";
const char octt[] PROGMEM = "október";
const char nov[] PROGMEM = "november";
const char decc[] PROGMEM = "december";

const char wn_N[]      PROGMEM = "SEV";
const char wn_NNE[]    PROGMEM = "SSV";
const char wn_NE[]     PROGMEM = "SV";
const char wn_ENE[]    PROGMEM = "VSV";
const char wn_E[]      PROGMEM = "V";
const char wn_ESE[]    PROGMEM = "VVJ";
const char wn_SE[]     PROGMEM = "JV";
const char wn_SSE[]    PROGMEM = "JJV";
const char wn_S[]      PROGMEM = "JUH";
const char wn_SSW[]    PROGMEM = "JJZ";
const char wn_SW[]     PROGMEM = "JZ";
const char wn_WSW[]    PROGMEM = "ZJZ";
const char wn_W[]      PROGMEM = "ZAP";
const char wn_WNW[]    PROGMEM = "ZSZ";
const char wn_NW[]     PROGMEM = "SZ";
const char wn_NNW[]    PROGMEM = "SSZ";

const char* const dow[]     PROGMEM = { sun, mon, tue, wed, thu, fri, sat };
const char* const dowf[]    PROGMEM = { sunf, monf, tuef, wedf, thuf, frif, satf };
const char* const mnths[]   PROGMEM = { jan, feb, mar, apr, may, jun, jul, aug, sep, octt, nov, decc };
const char* const wind[]    PROGMEM = { wn_N, wn_NNE, wn_NE, wn_ENE, wn_E, wn_ESE, wn_SE, wn_SSE, wn_S, wn_SSW, wn_SW, wn_WSW, wn_W, wn_WNW, wn_NW, wn_NNW, wn_N };

const char    const_PlReady[]    PROGMEM = "[pripravené]";
const char  const_PlStopped[]    PROGMEM = "[zastavené]";
const char  const_PlConnect[]    PROGMEM = "--> pripája sa";
const char  const_DlgVolume[]    PROGMEM = "hlasitosť";
const char    const_DlgLost[]    PROGMEM = "* stratený signál *";
const char  const_DlgUpdate[]    PROGMEM = "* obnova *";
const char const_DlgNextion[]    PROGMEM = "* NEXTION *";
const char const_getWeather[]    PROGMEM = "";
const char  const_waitForSD[]    PROGMEM = "INDEX SD";

const char        apNameTxt[]    PROGMEM = "Nazov AP";
const char        apPassTxt[]    PROGMEM = "HESLO";
const char       bootstrFmt[]    PROGMEM = "Pripája sa %s";
const char        apSettFmt[]    PROGMEM = "Stránka s nastaveniami: HTTP://%s/";
// clang-format on
#ifdef WEATHER_FMT_SHORT
const char weatherFmt[] PROGMEM = "%.1f\011C  \007  %d hPa  \007  %d%% RH";
#else
  #if EXT_WEATHER
    #ifdef WIND_SPEED_IN_KMH
      #define WIND_UNIT "km/h"
    #else
      #define WIND_UNIT "m/s"
    #endif
const char weatherFmt[] PROGMEM = "%s, %.1f\011C \007 pocitová: %.1f\011C \007 Tlak: %d hPa \007 Vlhkosť: %d%% \007 Vietor: %.1f " WIND_UNIT " [%s]";
  #else
const char weatherFmt[] PROGMEM = "%s, %.1f\011C \007 Tlak: %d hPa \007 vlhkosť: %d%%";
  #endif
#endif
const char weatherUnits[] PROGMEM = "metric"; /* standard, metric, imperial */
const char weatherLang[] PROGMEM = "sk";      /* https://openweathermap.org/current#multi */

#endif
