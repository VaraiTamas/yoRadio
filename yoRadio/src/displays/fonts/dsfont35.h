#ifndef dsfont_h
#define dsfont_h
#include "../../core/options.h"
#if DSP_MODEL == DSP_SSD1322
  #if CLOCKFONT == VT_DIGI_OLD
    #include "VT_DIGI_OLD_20x11s.h"
    #include "TinyFont5.h"
  #else
    #include "VT_DIGI_20x11s.h"
    #include "TinyFont5.h"
  #endif
  #include "DS_DIGI28pt7b.h"
#endif
#endif
