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
const char mon[] PROGMEM = "ma";
const char tue[] PROGMEM = "di";
const char wed[] PROGMEM = "wo";
const char thu[] PROGMEM = "do";
const char fri[] PROGMEM = "vr";
const char sat[] PROGMEM = "za";
const char sun[] PROGMEM = "zo";

const char monf[] PROGMEM = "maan";
const char tuef[] PROGMEM = "din";
const char wedf[] PROGMEM = "woe";
const char thuf[] PROGMEM = "don";
const char frif[] PROGMEM = "vrij";
const char satf[] PROGMEM = "zat";
const char sunf[] PROGMEM = "zon";

const char jan[] PROGMEM = "januari";
const char feb[] PROGMEM = "februari";
const char mar[] PROGMEM = "maart";
const char apr[] PROGMEM = "april";
const char may[] PROGMEM = "mei";
const char jun[] PROGMEM = "juni";
const char jul[] PROGMEM = "juli";
const char aug[] PROGMEM = "augustus";
const char sep[] PROGMEM = "september";
const char octc[] PROGMEM = "oktober";
const char nov[] PROGMEM = "november";
const char decc[] PROGMEM = "december";

const char wn_N[] PROGMEM = "noord";
const char wn_NNE[] PROGMEM = "noord noordwest";
const char wn_NE[] PROGMEM = "noordoost";
const char wn_ENE[] PROGMEM = "oost noordoost";
const char wn_E[] PROGMEM = "oost";
const char wn_ESE[] PROGMEM = "oost zuidoost";
const char wn_SE[] PROGMEM = "zuidoost";
const char wn_SSE[] PROGMEM = "zuid zuidoost";
const char wn_S[] PROGMEM = "zuid";
const char wn_SSW[] PROGMEM = "zuid zuidwest";
const char wn_SW[] PROGMEM = "zuidwest";
const char wn_WSW[] PROGMEM = "west zuidwest";
const char wn_W[] PROGMEM = "west";
const char wn_WNW[] PROGMEM = "west noordwest";
const char wn_NW[] PROGMEM = "noordwest";
const char wn_NNW[] PROGMEM = "noord noordwest";

const char *const dow[] PROGMEM = {sun, mon, tue, wed, thu, fri, sat};
const char *const dowf[] PROGMEM = {sunf, monf, tuef, wedf, thuf, frif, satf};
const char *const mnths[] PROGMEM = {jan, feb, mar, apr, may, jun, jul, aug, sep, octc, nov, decc};
const char *const wind[] PROGMEM = {wn_N, wn_NNE, wn_NE, wn_ENE, wn_E, wn_ESE, wn_SE, wn_SSE, wn_S, wn_SSW, wn_SW, wn_WSW, wn_W, wn_WNW, wn_NW, wn_NNW, wn_N};

const char const_PlReady[] PROGMEM = "[gereed]";
const char const_PlStopped[] PROGMEM = "[gestopt]";
const char const_PlConnect[] PROGMEM = "[verbonden]";
const char const_DlgVolume[] PROGMEM = "volume";
const char const_DlgLost[] PROGMEM = "* verbinding verbroken *";
const char const_DlgUpdate[] PROGMEM = "* update *";
const char const_DlgNextion[] PROGMEM = "* NEXTION *";
const char const_getWeather[] PROGMEM = "";
const char const_waitForSD[] PROGMEM = "INDEX SD";

const char apNameTxt[] PROGMEM = "WiFi AP";
const char apPassTxt[] PROGMEM = "wachtwoord";
const char bootstrFmt[] PROGMEM = "verbinden met: %s";
const char apSettFmt[] PROGMEM = "instellingen: HTTP://%s/";
// clang-format on
#ifdef WEATHER_FMT_SHORT
const char weatherFmt[] PROGMEM = "temp:%.1f\011C\007 druk:%d hPa\007 hum:%d%% RH";
#else
  #if EXT_WEATHER
    #ifdef WIND_SPEED_IN_KMH
      #define WIND_UNIT "km/h"
    #else
      #define WIND_UNIT "m/s"
    #endif
const char weatherFmt[] PROGMEM =
  "%s, %.1f\011C \007 gevoelstemperatuur: %.1f\011C \007 luchtdruk: %d hPa \007 luchtvochtigheid: %d%% \007 wind: %.1f " WIND_UNIT " [%s]";
  #else
const char weatherFmt[] PROGMEM = "%s, %.1f\011C \007 %d hPa \007 %d%%";
  #endif
#endif

const char weatherUnits[] PROGMEM = "metric"; /* standard, metric, imperial */
const char weatherLang[] PROGMEM = "nl";      /* https://openweathermap.org/current#multi */

#endif
