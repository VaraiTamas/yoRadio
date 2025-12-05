#ifndef dsp_full_loc
#define dsp_full_loc
#include <pgmspace.h>
#include "../myoptions.h"
// clang-format off
const char mon[] PROGMEM = "пн";
const char tue[] PROGMEM = "вт";
const char wed[] PROGMEM = "ср";
const char thu[] PROGMEM = "чт";
const char fri[] PROGMEM = "пт";
const char sat[] PROGMEM = "сб";
const char sun[] PROGMEM = "нд";

const char monf[] PROGMEM = "понеділок";
const char tuef[] PROGMEM = "вівторок";
const char wedf[] PROGMEM = "середа";
const char thuf[] PROGMEM = "четвер";
const char frif[] PROGMEM = "п'ятниця";
const char satf[] PROGMEM = "субота";
const char sunf[] PROGMEM = "неділя";

const char jan[] PROGMEM = "січня";
const char feb[] PROGMEM = "лютого";
const char mar[] PROGMEM = "березня";
const char apr[] PROGMEM = "квітня";
const char may[] PROGMEM = "травня";
const char jun[] PROGMEM = "червня";
const char jul[] PROGMEM = "липня";
const char aug[] PROGMEM = "серпня";
const char sep[] PROGMEM = "вересня";
const char octt[] PROGMEM = "жовтня";
const char nov[] PROGMEM = "листопада";
const char decc[] PROGMEM = "грудня";

const char wn_N[]      PROGMEM = "Пн";
const char wn_NNE[]    PROGMEM = "ПнПнСх";
const char wn_NE[]     PROGMEM = "ПнСх";
const char wn_ENE[]    PROGMEM = "СхПнСх";
const char wn_E[]      PROGMEM = "Сх";
const char wn_ESE[]    PROGMEM = "СхПдСх";
const char wn_SE[]     PROGMEM = "ПдЗх";
const char wn_SSE[]    PROGMEM = "ПдПдСх";
const char wn_S[]      PROGMEM = "Пд";
const char wn_SSW[]    PROGMEM = "ПдПдЗх";
const char wn_SW[]     PROGMEM = "ПдЗх";
const char wn_WSW[]    PROGMEM = "ЗхПдЗх";
const char wn_W[]      PROGMEM = "Зх";
const char wn_WNW[]    PROGMEM = "ЗхПнЗх";
const char wn_NW[]     PROGMEM = "ПнЗх";
const char wn_NNW[]    PROGMEM = "ПнПнЗх";

const char* const dow[]     PROGMEM = { sun, mon, tue, wed, thu, fri, sat };
const char* const dowf[]    PROGMEM = { sunf, monf, tuef, wedf, thuf, frif, satf };
const char* const mnths[]   PROGMEM = { jan, feb, mar, apr, may, jun, jul, aug, sep, octt, nov, decc };
const char* const wind[]    PROGMEM = { wn_N, wn_NNE, wn_NE, wn_ENE, wn_E, wn_ESE, wn_SE, wn_SSE, wn_S, wn_SSW, wn_SW, wn_WSW, wn_W, wn_WNW, wn_NW, wn_NNW, wn_N };

const char    const_PlReady[]    PROGMEM = "[готовий]";
const char  const_PlStopped[]    PROGMEM = "[зупинено]";
const char  const_PlConnect[]    PROGMEM = "[з'єднання]";
const char  const_DlgVolume[]    PROGMEM = "ГУЧНІСТЬ";
const char    const_DlgLost[]    PROGMEM = "ВИМКНЕНО";
const char  const_DlgUpdate[]    PROGMEM = "ОНОВЛЕННЯ";
const char const_DlgNextion[]    PROGMEM = "NEXTION";
const char const_getWeather[]    PROGMEM = "";
const char  const_waitForSD[]    PROGMEM = "ІНДЕКС SD";

const char        apNameTxt[]    PROGMEM = "ТОЧКА ДОСТУПУ";
const char        apPassTxt[]    PROGMEM = "ГАСЛО";
const char       bootstrFmt[]    PROGMEM = "З'єднуюсь з %s";
const char        apSettFmt[]    PROGMEM = "НАЛАШТУВАННЯ: HTTP://%s/";
// clang-format on
#ifdef WEATHER_FMT_SHORT
const char weatherFmt[] PROGMEM = "%.1f\011C  \007  %d hPa  \007  %d%% RH";
#else
  #if EXT_WEATHER
    #ifdef WIND_SPEED_IN_KMH
      #define WIND_UNIT "км/ч"
    #else
      #define WIND_UNIT "M/с"
    #endif
const char weatherFmt[] PROGMEM = "%s, %.1f\011C \007 відчувається: %.1f\011C \007 тиск: %d hPa \007 вологість: %d%% \007 вітер: %.1f " WIND_UNIT " [%s]";
  #else
const char weatherFmt[] PROGMEM = "%s, %.1f\011C \007 тиск: %d hPa \007 вологість: %d%%";
  #endif
#endif

const char weatherUnits[] PROGMEM = "metric"; /* standard, metric, imperial */
const char weatherLang[] PROGMEM = "ua";      /* https://openweathermap.org/current#multi */

#endif
