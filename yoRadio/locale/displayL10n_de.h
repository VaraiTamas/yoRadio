#ifndef dsp_full_loc
#define dsp_full_loc
#include <pgmspace.h>
#include "../myoptions.h"

const char mon[] PROGMEM = "Mo";
const char tue[] PROGMEM = "Di";
const char wed[] PROGMEM = "Mi";
const char thu[] PROGMEM = "Do";
const char fri[] PROGMEM = "Fr";
const char sat[] PROGMEM = "Sa";
const char sun[] PROGMEM = "So";

const char monf[] PROGMEM = "Montag";
const char tuef[] PROGMEM = "Dienstag";
const char wedf[] PROGMEM = "Mittwoch";
const char thuf[] PROGMEM = "Donnerstag";
const char frif[] PROGMEM = "Freitag";
const char satf[] PROGMEM = "Samstag";
const char sunf[] PROGMEM = "Sonntag";

const char jan[] PROGMEM = "Januar";
const char feb[] PROGMEM = "Februar";
const char mar[] PROGMEM = "März";
const char apr[] PROGMEM = "April";
const char may[] PROGMEM = "Mai";
const char jun[] PROGMEM = "Juni";
const char jul[] PROGMEM = "Juli";
const char aug[] PROGMEM = "August";
const char sep[] PROGMEM = "September";
const char octt[] PROGMEM = "Oktober";
const char nov[] PROGMEM = "November";
const char decc[] PROGMEM = "Dezember";

const char wn_N[] PROGMEM = "NORD";
const char wn_NNE[] PROGMEM = "NNO";
const char wn_NE[] PROGMEM = "NO";
const char wn_ENE[] PROGMEM = "ONO";
const char wn_E[] PROGMEM = "OST";
const char wn_ESE[] PROGMEM = "OSO";
const char wn_SE[] PROGMEM = "SO";
const char wn_SSE[] PROGMEM = "SSO";
const char wn_S[] PROGMEM = "SÜD";
const char wn_SSW[] PROGMEM = "SSW";
const char wn_SW[] PROGMEM = "SW";
const char wn_WSW[] PROGMEM = "WSW";
const char wn_W[] PROGMEM = "WEST";
const char wn_WNW[] PROGMEM = "WNW";
const char wn_NW[] PROGMEM = "NW";
const char wn_NNW[] PROGMEM = "NNW";

const char *const dow[] PROGMEM = {sun, mon, tue, wed, thu, fri, sat};
const char *const dowf[] PROGMEM = {sunf, monf, tuef, wedf, thuf, frif, satf};
const char *const mnths[] PROGMEM = {jan, feb, mar, apr, may, jun, jul, aug, sep, octt, nov, decc};
const char *const wind[] PROGMEM = {wn_N, wn_NNE, wn_NE, wn_ENE, wn_E, wn_ESE, wn_SE, wn_SSE, wn_S, wn_SSW, wn_SW, wn_WSW, wn_W, wn_WNW, wn_NW, wn_NNW, wn_N};

const char const_PlReady[] PROGMEM = "[bereit]";
const char const_PlStopped[] PROGMEM = "[gestoppt]";
const char const_PlConnect[] PROGMEM = "[verbinden]";
const char const_DlgVolume[] PROGMEM = "Lautstärke";
const char const_DlgLost[] PROGMEM = "* Signal verloren *";
const char const_DlgUpdate[] PROGMEM = "* Aktualisierung *";
const char const_DlgNextion[] PROGMEM = "* NEXTION *";
const char const_getWeather[] PROGMEM = "";
const char const_waitForSD[] PROGMEM = "INDEX SD";

const char apNameTxt[] PROGMEM = "AP NAME";
const char apPassTxt[] PROGMEM = "PASSWORT";
const char bootstrFmt[] PROGMEM = "VERBINDEN %s";
const char apSettFmt[] PROGMEM = "SEITE MIT EINSTELLUNGEN: HTTP://%s/";

#ifdef WEATHER_FMT_SHORT  // Módosítás
const char weatherFmt[] PROGMEM = "%.1f\011C  \007  %d hPa  \007  %d%% RH";
#else
  #if EXT_WEATHER
    #ifdef WIND_SPEED_IN_KMH
      #define WIND_UNIT "km/h"
    #else
      #define WIND_UNIT "m/s"
    #endif
const char weatherFmt[] PROGMEM = "%s, %.1f\011C \007 gefühlte Temperatur: %.1f\011C \007 Druck: %d hPa \007 Luftfeuchtigkeit: %d%% \007 Wind: %.1f " WIND_UNIT " [%s]";
  #else
const char weatherFmt[] PROGMEM = "%s, %.1f\011C \007 Druck: %d hPa \007 Luftfeuchtigkeit: %d%%";
  #endif
#endif
const char weatherUnits[] PROGMEM = "metric"; /* standard, metric, imperial */
const char weatherLang[] PROGMEM = "de";      /* https://openweathermap.org/current#multi */

#endif
