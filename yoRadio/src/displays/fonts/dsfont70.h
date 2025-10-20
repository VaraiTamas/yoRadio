// Módosítás "font"  (320x480)
#ifndef dsfont_h
#define dsfont_h

/*
#if CLOCKFONT_MONO
  #include "DS_DIGI56pt7b_mono.h"        // https://tchapi.github.io/Adafruit-GFX-Font-Customiser/
#else
  #include "DS_DIGI56pt7b.h"
#endif
*/
#if CLOCKFONT == VT_DIGI
 #include "VT_DIGI_34x19.h"        // A másodperc fontja
 #include "VT_DIGI_68x38.h"        // Módosítás az óra nagyméretű fontjának betöltése.
#else
 #include "VT_DIGI_OLD_35x21.h"
 #include "VT_DIGI_OLD_70x42.h"
#endif

#endif
