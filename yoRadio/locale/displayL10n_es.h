#ifndef dsp_full_loc
#define dsp_full_loc
#include <pgmspace.h>
#include "../myoptions.h"
// nincs befejezve, hiányzik a utf8Es.cpp fájl és a glcdfont.c spanyol változata
// clang-format off
const char mon[] PROGMEM = "lu";
const char tue[] PROGMEM = "ma";
const char wed[] PROGMEM = "mi";
const char thu[] PROGMEM = "ju";
const char fri[] PROGMEM = "vi";
const char sat[] PROGMEM = "sa";
const char sun[] PROGMEM = "do";

const char monf[] PROGMEM = "lunes";
const char tuef[] PROGMEM = "martes";
const char wedf[] PROGMEM = "miercoles";
const char thuf[] PROGMEM = "jueves";
const char frif[] PROGMEM = "viernes";
const char satf[] PROGMEM = "sabado";
const char sunf[] PROGMEM = "domingo";

const char jan[] PROGMEM = "enero";
const char feb[] PROGMEM = "febrero";
const char mar[] PROGMEM = "marzo";
const char apr[] PROGMEM = "abril";
const char may[] PROGMEM = "mayo";
const char jun[] PROGMEM = "junio";
const char jul[] PROGMEM = "julio";
const char aug[] PROGMEM = "agosto";
const char sep[] PROGMEM = "septiembre";
const char octt[] PROGMEM = "octubre";
const char nov[] PROGMEM = "noviembre";
const char decc[] PROGMEM = "diciembre";

const char wn_N[]      PROGMEM = "NORTE";
const char wn_NNE[]    PROGMEM = "NNE";   // Norte-noreste
const char wn_NE[]     PROGMEM = "NE";    // Noreste
const char wn_ENE[]    PROGMEM = "ENE";   // Este-noreste
const char wn_E[]      PROGMEM = "ESTE";
const char wn_ESE[]    PROGMEM = "ESE";   // Este-sureste
const char wn_SE[]     PROGMEM = "SE";    // Sureste
const char wn_SSE[]    PROGMEM = "SSE";   // Sur-sureste
const char wn_S[]      PROGMEM = "SUR";
const char wn_SSW[]    PROGMEM = "SSO";   // Sur-suroeste (SSW → SSO en español)
const char wn_SW[]     PROGMEM = "SO";    // Suroeste (SW → SO)
const char wn_WSW[]    PROGMEM = "OSO";   // Oeste-suroeste
const char wn_W[]      PROGMEM = "OESTE";
const char wn_WNW[]    PROGMEM = "ONO";   // Oeste-noroeste
const char wn_NW[]     PROGMEM = "NO";    // Noroeste
const char wn_NNW[]    PROGMEM = "NNO";   // Norte-noroeste

const char* const dow[]     PROGMEM = { sun, mon, tue, wed, thu, fri, sat };
const char* const dowf[]    PROGMEM = { sunf, monf, tuef, wedf, thuf, frif, satf };
const char* const mnths[]   PROGMEM = { jan, feb, mar, apr, may, jun, jul, aug, sep, octt, nov, decc };
const char* const wind[]    PROGMEM = { wn_N, wn_NNE, wn_NE, wn_ENE, wn_E, wn_ESE, wn_SE, wn_SSE, wn_S, wn_SSW, wn_SW, wn_WSW, wn_W, wn_WNW, wn_NW, wn_NNW, wn_N };

const char    const_PlReady[]    PROGMEM = "[listo]";
const char  const_PlStopped[]    PROGMEM = "[stop]";
const char  const_PlConnect[]    PROGMEM = "";
const char  const_DlgVolume[]    PROGMEM = "VOLUMEN";
const char    const_DlgLost[]    PROGMEM = "* PERDIDO *";
const char  const_DlgUpdate[]    PROGMEM = "* UPDATING *";
const char const_DlgNextion[]    PROGMEM = "* NEXTION *";
const char const_getWeather[]    PROGMEM = "";
const char  const_waitForSD[]    PROGMEM = "INDEX SD";

const char        apNameTxt[]    PROGMEM = "AP NAME";
const char        apPassTxt[]    PROGMEM = "PASSWORD";
const char       bootstrFmt[]    PROGMEM = "Trying to %s";
const char        apSettFmt[]    PROGMEM = "SETTINGS PAGE ON: HTTP://%s/";
// clang-format on
#ifdef WEATHER_FMT_SHORT  // Módosítás
const char weatherFmt[] PROGMEM = "%.1f\011C  \007  %d hPa  \007  %d%% RH";
#else
  #if EXT_WEATHER
    #ifdef WIND_SPEED_IN_KMH
      #define WIND_UNIT "km/h"
    #else
      #define WIND_UNIT "m/s"
    #endif
const char weatherFmt[] PROGMEM = "%s, %.1f\011C \007 feels like: %.1f\011C \007 pressure: %d hPa \007 humidity: %d%% \007 wind: %.1f " WIND_UNIT " [%s]";
  #else
const char weatherFmt[] PROGMEM = "%s, %.1f\011C \007 pressure: %d hPa \007 humidity: %d%%";
  #endif
#endif
const char weatherUnits[] PROGMEM = "metric"; /* standard, metric, imperial */
const char weatherLang[] PROGMEM = "en";      /* https://openweathermap.org/current#multi */

#endif