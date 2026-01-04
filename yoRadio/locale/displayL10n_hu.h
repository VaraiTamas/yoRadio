#ifndef dsp_full_loc
#define dsp_full_loc
#include <pgmspace.h>
#include "../myoptions.h"
// clang-format off
const char mon[] PROGMEM = "hé";
const char tue[] PROGMEM = "ke";
const char wed[] PROGMEM = "sz";
const char thu[] PROGMEM = "cs";
const char fri[] PROGMEM = "pé";
const char sat[] PROGMEM = "SZ";
const char sun[] PROGMEM = "VA";

const char monf[] PROGMEM = "hétfő";
const char tuef[] PROGMEM = "kedd";
const char wedf[] PROGMEM = "szerda";
const char thuf[] PROGMEM = "csütörtök";
const char frif[] PROGMEM = "péntek";
const char satf[] PROGMEM = "szombat";
const char sunf[] PROGMEM = "vasárnap";

const char jan[] PROGMEM = "január";
const char feb[] PROGMEM = "február";
const char mar[] PROGMEM = "március";
const char apr[] PROGMEM = "április";
const char may[] PROGMEM = "május";
const char jun[] PROGMEM = "június";
const char jul[] PROGMEM = "július";
const char aug[] PROGMEM = "augusztus";
const char sep[] PROGMEM = "szeptember";
const char octc[] PROGMEM = "október";
const char nov[] PROGMEM = "november";
const char decc[] PROGMEM = "december";

const char wn_N[] PROGMEM = "észak";
const char wn_NNE[] PROGMEM = "észak-északkelet";
const char wn_NE[] PROGMEM = "északkelet";
const char wn_ENE[] PROGMEM = "kelet-északkelet";
const char wn_E[] PROGMEM = "kelet";
const char wn_ESE[] PROGMEM = "kelet-délkelet";
const char wn_SE[] PROGMEM = "délkelet";
const char wn_SSE[] PROGMEM = "délkelet-dél";
const char wn_S[] PROGMEM = "dél";
const char wn_SSW[] PROGMEM = "dél-délnyugat";
const char wn_SW[] PROGMEM = "délnyugat";
const char wn_WSW[] PROGMEM = "nyugat-délnyugat";
const char wn_W[] PROGMEM = "nyugat";
const char wn_WNW[] PROGMEM = "nyugat-északnyugat";
const char wn_NW[] PROGMEM = "északnyugat";
const char wn_NNW[] PROGMEM = "északnyugat-nyugat";

const char *const dow[] PROGMEM = {sun, mon, tue, wed, thu, fri, sat};
const char *const dowf[] PROGMEM = {sunf, monf, tuef, wedf, thuf, frif, satf};
const char *const mnths[] PROGMEM = {jan, feb, mar, apr, may, jun, jul, aug, sep, octc, nov, decc};
const char *const wind[] PROGMEM = {wn_N, wn_NNE, wn_NE, wn_ENE, wn_E, wn_ESE, wn_SE, wn_SSE, wn_S, wn_SSW, wn_SW, wn_WSW, wn_W, wn_WNW, wn_NW, wn_NNW, wn_N};

const char const_PlReady[] PROGMEM = "[kész]";
const char const_PlStopped[] PROGMEM = "[stop]";
const char const_PlConnect[] PROGMEM = "";
const char const_DlgVolume[] PROGMEM = "Hangerő";
const char const_DlgLost[] PROGMEM = "* LESZAKADT *";
const char const_DlgUpdate[] PROGMEM = "* FRISSÍTES *";
const char const_DlgNextion[] PROGMEM = "* NEXTION *";
const char const_getWeather[] PROGMEM = "";
const char const_waitForSD[] PROGMEM = "INDEX SD";

const char apNameTxt[] PROGMEM = "WiFi AP";
const char apPassTxt[] PROGMEM = "Jelszó";
const char bootstrFmt[] PROGMEM = "Csatlakozás: %s";
const char apSettFmt[] PROGMEM = "A rádió elérhetősége: HTTP://%s/";
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
const char weatherFmt[] PROGMEM =
  "%s, %.1f\011C \007 hőérzet: %.1f\011C \007 légnyomás: %d hPa \007 páratartalom: %d%% \007 szélsebesség: %.1f " WIND_UNIT " [%s]";
  #else
const char weatherFmt[] PROGMEM = "%s, %.1f\011C \007 %d hPa \007 %d%%";
  #endif
#endif

const char weatherUnits[] PROGMEM = "metric"; /* standard, metric, imperial */
const char weatherLang[] PROGMEM = "hu";      /* https://openweathermap.org/current#multi */

#endif
