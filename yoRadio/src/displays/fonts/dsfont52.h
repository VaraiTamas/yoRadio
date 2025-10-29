// Módosítás "font"   (320x240)   https://tchapi.github.io/Adafruit-GFX-Font-Customiser/
#ifndef dsfont_h
#define dsfont_h
#include "../../core/options.h"

#if CLOCKFONT == VT_DIGI_OLD
  #ifdef AM_PM_STYLE
    #include "VT_DIGI_OLD_20x11s.h"
    #include "VT_DIGI_OLD_47x26.h"
  #else
    #include "VT_DIGI_OLD_27x15s.h"
    #include "VT_DIGI_OLD_47x26.h"
  #endif
#else  // CLOCKFONT == VT_DIGI
  #ifdef AM_PM_STYLE
    #include "VT_DIGI_20x11s.h"  // A másodperc fontja
    #include "VT_DIGI_47x26.h"   // Az óra nagyméretű fontjának betöltése.
  #else
    #include "VT_DIGI_27x15s.h"  // A másodperc fontja
    #include "VT_DIGI_47x26.h"   // Az óra nagyméretű fontjának betöltése.
  #endif
#endif

#endif
