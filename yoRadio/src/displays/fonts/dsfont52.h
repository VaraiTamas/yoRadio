// Módosítás "font"   (320x240)
#ifndef dsfont_h
#define dsfont_h

//#if CLOCKFONT_MONO
//  #include "DS_DIGI42pt7b_mono.h"        // https://tchapi.github.io/Adafruit-GFX-Font-Customiser/
//#else
//  #include "DS_DIGI42pt7b.h"
//#endif
#if CLOCKFONT == VT_DIGI
 #include "VT_DIGI_27x15.h"        // A másodperc fontja
 #include "VT_DIGI_47x26.h"        // Módosítás az óra nagyméretű fontjának betöltése.
#else
 #include "VT_DIGI_OLD_27x15.h"
 #include "VT_DIGI_OLD_47x26.h"
#endif


#endif

