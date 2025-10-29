// Módosítás "font"  (320x480) https://tchapi.github.io/Adafruit-GFX-Font-Customiser/
#ifndef dsfont_h
#define dsfont_h
#include "../../core/options.h"

#if CLOCKFONT == VT_DIGI_OLD
  #ifdef AM_PM_STYLE
    #include "VT_DIGI_OLD_27x15s.h"
    #include "VT_DIGI_OLD_70x42.h"
  #else
    #include "VT_DIGI_OLD_35x21s.h"  // A másodperc fontja
    #include "VT_DIGI_OLD_70x42.h"   // Az óra nagyméretű fontjának betöltése.
  #endif
#else  // CLOCKFONT == VT_DIGI
  #ifdef AM_PM_STYLE
    #include "VT_DIGI_27x15s.h"  // A másodperc fontja
    #include "VT_DIGI_68x38.h"   // Az óra nagyméretű fontjának betöltése.
  #else
    #include "VT_DIGI_34x19s.h"  // A másodperc fontja
    #include "VT_DIGI_68x38.h"   // Az óra nagyméretű fontjának betöltése.
  #endif
#endif

#endif
