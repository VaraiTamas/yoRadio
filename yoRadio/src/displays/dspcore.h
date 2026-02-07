#ifndef dspcore_h
#define dspcore_h
#pragma once

#if DSP_MODEL==DSP_DUMMY
  #define DUMMYDISPLAY
  #define DSP_NOT_FLIPPED

#elif DSP_MODEL==DSP_ST7789       // https://k210.org/images/content/uploads/yoradio/ST7789.jpg
  #define TIME_SIZE           52
  #define PSFBUFFER
  #include "displayST7789.h"

#elif DSP_MODEL==DSP_ILI9341         // https://k210.org/images/content/uploads/yoradio/ILI9341.jpg
  #define TIME_SIZE           52
  #define PSFBUFFER
  #include "displayILI9341.h"

#elif DSP_MODEL==DSP_CUSTOM
  #define TIME_SIZE           0
  #include "displayCustom.h"

#elif DSP_MODEL==DSP_ST7796         // https://k210.org/images/content/uploads/yoradio/ST7796.jpg
  #define TIME_SIZE           70
  #define PSFBUFFER
  #include "displayST7796.h"

#elif DSP_MODEL==DSP_ILI9488 || DSP_MODEL==DSP_ILI9486  // https://k210.org/images/content/uploads/yoradio/ILI9488.jpg
  #define TIME_SIZE           70
  #define PSFBUFFER
  #include "displayILI9488.h"

#elif DSP_MODEL==DSP_SSD1322        
  #define TIME_SIZE           35
  #define DSP_OLED
  #include "displaySSD1322.h"

#endif

#endif
