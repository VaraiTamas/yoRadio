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
const char mon[] PROGMEM = "Lu";
const char tue[] PROGMEM = "Ma";
const char wed[] PROGMEM = "Me";
const char thu[] PROGMEM = "Gi";
const char fri[] PROGMEM = "Ve";
const char sat[] PROGMEM = "Sa";
const char sun[] PROGMEM = "Do";

const char monf[] PROGMEM = "Lunedi";
const char tuef[] PROGMEM = "Martedi";
const char wedf[] PROGMEM = "Mercoledi";
const char thuf[] PROGMEM = "Giovedi";
const char frif[] PROGMEM = "Venerdi";
const char satf[] PROGMEM = "Sabato";
const char sunf[] PROGMEM = "Domenica";

const char jan[] PROGMEM = "Gennaio";
const char feb[] PROGMEM = "Febbraio";
const char mar[] PROGMEM = "Maezo";
const char apr[] PROGMEM = "Aprile";
const char may[] PROGMEM = "Maggio";
const char jun[] PROGMEM = "Giugno";
const char jul[] PROGMEM = "Luglio";
const char aug[] PROGMEM = "Agosto";
const char sep[] PROGMEM = "Settembre";
const char octc[] PROGMEM = "Ottobre";
const char nov[] PROGMEM = "Novembre";
const char decc[] PROGMEM = "Dicembre";

const char wn_N[]      PROGMEM = "NORD";
const char wn_NNE[]    PROGMEM = "NNE";
const char wn_NE[]     PROGMEM = "NE";
const char wn_ENE[]    PROGMEM = "ENE";
const char wn_E[]      PROGMEM = "EST";
const char wn_ESE[]    PROGMEM = "ESE";
const char wn_SE[]     PROGMEM = "SE";
const char wn_SSE[]    PROGMEM = "SSE";
const char wn_S[]      PROGMEM = "SUD";
const char wn_SSW[]    PROGMEM = "SSW";
const char wn_SW[]     PROGMEM = "SW";
const char wn_WSW[]    PROGMEM = "WSW";
const char wn_W[]      PROGMEM = "OVEST";
const char wn_WNW[]    PROGMEM = "WNW";
const char wn_NW[]     PROGMEM = "NW";
const char wn_NNW[]    PROGMEM = "NNW";

const char* const dow[]     PROGMEM = { sun, mon, tue, wed, thu, fri, sat };
const char* const dowf[]    PROGMEM = { sunf, monf, tuef, wedf, thuf, frif, satf };
const char* const mnths[]   PROGMEM = { jan, feb, mar, apr, may, jun, jul, aug, sep, octc, nov, decc };
const char* const wind[]    PROGMEM = { wn_N, wn_NNE, wn_NE, wn_ENE, wn_E, wn_ESE, wn_SE, wn_SSE, wn_S, wn_SSW, wn_SW, wn_WSW, wn_W, wn_WNW, wn_NW, wn_NNW, wn_N };

const char    const_PlReady[]    PROGMEM = "[Pronto]";
const char  const_PlStopped[]    PROGMEM = "[Stop]";
const char  const_PlConnect[]    PROGMEM = "[Connessione]";
const char  const_DlgVolume[]    PROGMEM = "VOLUME";
const char    const_DlgLost[]    PROGMEM = "* PERSO *";
const char  const_DlgUpdate[]    PROGMEM = "* AGGIORNAMENTO *";
const char const_DlgNextion[]    PROGMEM = "* NEXTION *";
const char const_getWeather[]    PROGMEM = "";
const char  const_waitForSD[]    PROGMEM = "INDICE SD";

const char        apNameTxt[]    PROGMEM = "NOME AP";
const char        apPassTxt[]    PROGMEM = "PASSWORD";
const char       bootstrFmt[]    PROGMEM = "Connessione a %s";
const char        apSettFmt[]    PROGMEM = "PAGINA IMPOSTAZIONI SU: HTTP://%s/";
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
const char weatherFmt[] PROGMEM = "%s, %.1f\011C \007 PERCEPITA: %.1f\011C \007 PRESSIONE: %d hPa \007 UMIDITA: %d%% \007 VENTO: %.1f " WIND_UNIT " [%s]";
  #else
const char weatherFmt[] PROGMEM = "%s, %.1f\011C \007 PRESSIONE: %d hPa \007 UMIDITA: %d%%";
  #endif
#endif
const char weatherUnits[] PROGMEM = "metric"; /* standard, metric, imperial */
const char weatherLang[] PROGMEM = "it";      /* https://openweathermap.org/current#multi */

#endif
