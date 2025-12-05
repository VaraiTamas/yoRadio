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
const char mon[] PROGMEM = "Δε";
const char tue[] PROGMEM = "Τρ";
const char wed[] PROGMEM = "Τε";
const char thu[] PROGMEM = "Πε";
const char fri[] PROGMEM = "Πα";
const char sat[] PROGMEM = "Σα";
const char sun[] PROGMEM = "Κυ";

const char monf[] PROGMEM = "Δευτέρα";
const char tuef[] PROGMEM = "Τρίτη";
const char wedf[] PROGMEM = "Τετάρτη";
const char thuf[] PROGMEM = "Πέμπτη";
const char frif[] PROGMEM = "Παρασκευή";
const char satf[] PROGMEM = "Σάββατο";
const char sunf[] PROGMEM = "Κυριακή";

const char jan[] PROGMEM = "Ιανουάριος";
const char feb[] PROGMEM = "Φεβρουάριος";
const char mar[] PROGMEM = "Μάρτιος";
const char apr[] PROGMEM = "Απρίλιος";
const char may[] PROGMEM = "Μάιος";
const char jun[] PROGMEM = "Ιούνιος";
const char jul[] PROGMEM = "Ιούλιος";
const char aug[] PROGMEM = "Αύγουστος";
const char sep[] PROGMEM = "Σεπτέμβριος";
const char octt[] PROGMEM = "Οκτώβριος";
const char nov[] PROGMEM = "Νοέμβριος";
const char decc[] PROGMEM = "Δεκέμβριος";

const char wn_N[]      PROGMEM = "Β";
const char wn_NNE[]    PROGMEM = "ΒΒΑ";
const char wn_NE[]     PROGMEM = "ΒΑ";
const char wn_ENE[]    PROGMEM = "ΑΒΑ";
const char wn_E[]      PROGMEM = "Α";
const char wn_ESE[]    PROGMEM = "ΑΝΑ";
const char wn_SE[]     PROGMEM = "ΝΑ";
const char wn_SSE[]    PROGMEM = "ΝΝΑ";
const char wn_S[]      PROGMEM = "Ν";
const char wn_SSW[]    PROGMEM = "ΝΝΔ";
const char wn_SW[]     PROGMEM = "ΝΔ";
const char wn_WSW[]    PROGMEM = "ΔΝΔ";
const char wn_W[]      PROGMEM = "Δ";
const char wn_WNW[]    PROGMEM = "ΔΒΔ";
const char wn_NW[]     PROGMEM = "ΒΔ";
const char wn_NNW[]    PROGMEM = "ΒΒΔ";

const char* const dow[]     PROGMEM = { sun, mon, tue, wed, thu, fri, sat };
const char* const dowf[]    PROGMEM = { sunf, monf, tuef, wedf, thuf, frif, satf };
const char* const mnths[]   PROGMEM = { jan, feb, mar, apr, may, jun, jul, aug, sep, octt, nov, decc };
const char* const wind[]    PROGMEM = { wn_N, wn_NNE, wn_NE, wn_ENE, wn_E, wn_ESE, wn_SE, wn_SSE, wn_S, wn_SSW, wn_SW, wn_WSW, wn_W, wn_WNW, wn_NW, wn_NNW, wn_N };

const char    const_PlReady[]    PROGMEM = "[έτοιμο]";
const char  const_PlStopped[]    PROGMEM = "[σταμάτησε]";
const char  const_PlConnect[]    PROGMEM = "[σύνδεση]";
const char  const_DlgVolume[]    PROGMEM = "ΕΝΤΑΣΗ";
const char    const_DlgLost[]    PROGMEM = "* χάθηκε το σήμα *";
const char  const_DlgUpdate[]    PROGMEM = "* ΕΝΗΜΕΡΩΣΗ *";
const char const_DlgNextion[]    PROGMEM = "* NEXTION *";
const char const_getWeather[]    PROGMEM = "";
const char  const_waitForSD[]    PROGMEM = "INDEX SD";

const char        apNameTxt[]    PROGMEM = "ΟΝΟΜΑ AP";
const char        apPassTxt[]    PROGMEM = "ΚΩΔΙΚΟΣ";
const char       bootstrFmt[]    PROGMEM = "σύνδεση με %s";
const char        apSettFmt[]    PROGMEM = "ΣΕΛΙΔΑ ΡΥΘΜΙΣΕΩΝ: HTTP://%s/";
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
const char weatherFmt[] PROGMEM =
  "%s, %.1f\011C \007 αίσθηση θερμοκρασίας: %.1f\011C \007 πίεση: %d hPa \007 υγρασία: %d%% \007 άνεμος: %.1f " WIND_UNIT " [%s]";
  #else
const char weatherFmt[] PROGMEM = "%s, %.1f\011C \007 Πίεση: %d hPa \007 Υγρασία: %d%%";
  #endif
#endif
const char weatherUnits[] PROGMEM = "metric"; /* standard, metric, imperial */
const char weatherLang[] PROGMEM = "el";      /* https://openweathermap.org/current#multi */

#endif
