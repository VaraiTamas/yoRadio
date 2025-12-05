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
const char mon[] PROGMEM = "pon";
const char tue[] PROGMEM = "wto";
const char wed[] PROGMEM = "śro";
const char thu[] PROGMEM = "czw";
const char fri[] PROGMEM = "pią";
const char sat[] PROGMEM = "sob";
const char sun[] PROGMEM = "nie";

const char monf[] PROGMEM = "poniedziałek";
const char tuef[] PROGMEM = "wtorek";
const char wedf[] PROGMEM = "środa";
const char thuf[] PROGMEM = "czwartek";
const char frif[] PROGMEM = "piątek";
const char satf[] PROGMEM = "sobota";
const char sunf[] PROGMEM = "niedziela";

const char jan[] PROGMEM = "styczeń";
const char feb[] PROGMEM = "luty";
const char mar[] PROGMEM = "marzec";
const char apr[] PROGMEM = "kwiecień";
const char may[] PROGMEM = "maj";
const char jun[] PROGMEM = "czerwiec";
const char jul[] PROGMEM = "lipiec";
const char aug[] PROGMEM = "sierpień";
const char sep[] PROGMEM = "wrzesień";
const char octt[] PROGMEM = "październik";
const char nov[] PROGMEM = "listopad";
const char decc[] PROGMEM = "grudzień";
/*
const char jan[] PROGMEM = "01";
const char feb[] PROGMEM = "02";
const char mar[] PROGMEM = "03";
const char apr[] PROGMEM = "04";
const char may[] PROGMEM = "05";
const char jun[] PROGMEM = "06";
const char jul[] PROGMEM = "07";
const char aug[] PROGMEM = "08";
const char sep[] PROGMEM = "09";
const char octt[] PROGMEM = "10";
const char nov[] PROGMEM = "11";
const char decc[] PROGMEM = "12";
*/
const char wn_N[]      PROGMEM = "NORTH"; //północ 
const char wn_NNE[]    PROGMEM = "NNE"; //północny północny wschód
const char wn_NE[]     PROGMEM = "NE"; //północny wschód
const char wn_ENE[]    PROGMEM = "ENE"; //wschodni północny wschód
const char wn_E[]      PROGMEM = "EAST"; //wschód 
const char wn_ESE[]    PROGMEM = "ESE"; //wschodni południowy wschód 
const char wn_SE[]     PROGMEM = "SE"; //południowy wschód 
const char wn_SSE[]    PROGMEM = "SSE"; //południowy południowy wschód 
const char wn_S[]      PROGMEM = "SOUTH"; //południe 
const char wn_SSW[]    PROGMEM = "SSW"; //południowy południowy zachód 
const char wn_SW[]     PROGMEM = "SW"; //południowy zachód 
const char wn_WSW[]    PROGMEM = "WSW"; //zachodni południowy zachód 
const char wn_W[]      PROGMEM = "WEST"; //zachód 
const char wn_WNW[]    PROGMEM = "WNW"; //zachodni północny zachód  
const char wn_NW[]     PROGMEM = "NW"; //północny zachód 
const char wn_NNW[]    PROGMEM = "NNW"; //północny północny zachód 

const char* const dow[]     PROGMEM = { sun, mon, tue, wed, thu, fri, sat };
const char* const dowf[]    PROGMEM = { sunf, monf, tuef, wedf, thuf, frif, satf };
const char* const mnths[]   PROGMEM = { jan, feb, mar, apr, may, jun, jul, aug, sep, octt, nov, decc };
const char* const wind[]    PROGMEM = { wn_N, wn_NNE, wn_NE, wn_ENE, wn_E, wn_ESE, wn_SE, wn_SSE, wn_S, wn_SSW, wn_SW, wn_WSW, wn_W, wn_WNW, wn_NW, wn_NNW, wn_N };

const char    const_PlReady[]    PROGMEM = "[ready]";
const char  const_PlStopped[]    PROGMEM = "[stopped]";
const char  const_PlConnect[]    PROGMEM = "[connecting]";
const char  const_DlgVolume[]    PROGMEM = "VOLUME";
const char    const_DlgLost[]    PROGMEM = "* LOST *";
const char  const_DlgUpdate[]    PROGMEM = "* UPDATING *";
const char const_DlgNextion[]    PROGMEM = "* NEXTION *";
const char const_getWeather[]    PROGMEM = "";
const char  const_waitForSD[]    PROGMEM = "INDEX SD";

const char        apNameTxt[]    PROGMEM = "AP NAME";
const char        apPassTxt[]    PROGMEM = "PASSWORD";
const char       bootstrFmt[]    PROGMEM = "Connecting %s";
const char        apSettFmt[]    PROGMEM = "SETTINGS PAGE ON: HTTP://%s/";
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
const char weatherFmt[] PROGMEM = "%s, %.1f\011C \007 odczuwalna: %.1f\011C \007 ciśnienie: %d hPa \007 wilgotność: %d%% \007 wiatr: %.1f " WIND_UNIT " [%s]";
  #else
const char weatherFmt[] PROGMEM = "%s \007 %.1f\011C \007 %d hPa \007 %d%% RH";
  #endif
#endif

const char weatherUnits[] PROGMEM = "metric"; /* standard, metric, imperial */
const char weatherLang[] PROGMEM = "pl";      /* https://openweathermap.org/current#multi */

#endif
