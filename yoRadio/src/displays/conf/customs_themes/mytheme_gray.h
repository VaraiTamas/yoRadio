#ifndef _my_theme_h
#define _my_theme_h

#define ENABLE_THEME
#ifdef  ENABLE_THEME
// clang-format off
/*                               R    G    B  */
/*----- SCREEN -----*/
#define COLOR_BACKGROUND          0,   0,   0

/*----- META -----*/
#define COLOR_STATION_NAME      255, 255, 255  
#define COLOR_STATION_BG          0,   0,   0   
#define COLOR_STATION_FILL      125, 125, 125  
#define COLOR_SNG_TITLE_1       220, 220, 220
#define COLOR_SNG_TITLE_2       162, 162, 162
#define COLOR_BITRATE           200, 200, 200

/*----- WEATHER -----*/
#define COLOR_WEATHER           220, 220, 220

/*---- VOLUME SCREEN -----*/
#define COLOR_DIGITS            255, 255, 255

/*----- NAMEDAY -----*/
#define COLOR_NAMEDAY           200, 200, 200

/*----- CLOCK, DATE -----*/
#define COLOR_CLOCK             255, 255, 255 
#define COLOR_CLOCK_BG           28,  28,  28
#define COLOR_SECONDS           255, 255, 255
#define COLOR_DIVIDER           162, 162, 162
#define COLOR_DATE              255, 255, 255

/*----- VU WIDGET ----*/
#define COLOR_VU_MAX            255, 255, 255
#define COLOR_VU_MID            200, 200, 200
#define COLOR_VU_MIN            130, 130, 130

/*----- FOOTER -----*/
#define COLOR_VOLBAR_OUT        200, 200, 200
#define COLOR_VOLBAR_IN         200, 200, 200
#define COLOR_VOLUME_VALUE      162, 162, 162
#define COLOR_IP                162, 162, 162
#define COLOR_CH                162, 162, 162
#define COLOR_RSSI              162, 162, 162
#define COLOR_HEAP               41,  40,  41
#define COLOR_BUFFER              90, 90, 90

/*----- PLAYLIST -----*/
#ifdef PLAYLIST_SCROLL_MOVING_CURSOR
    #define COLOR_PL_CURRENT        255, 255, 255
    #define COLOR_PL_CURRENT_BG       0,   0,   0
    #define COLOR_PL_CURRENT_FILL     0,   0,   0
    #define COLOR_PLAYLIST_0        165, 165, 165
    #define COLOR_PLAYLIST_1        165, 165, 165
    #define COLOR_PLAYLIST_2        165, 165, 165
    #define COLOR_PLAYLIST_3        165, 165, 165
    #define COLOR_PLAYLIST_4        165, 165, 165
#else
    #define COLOR_PL_CURRENT        255, 255, 255
    #define COLOR_PL_CURRENT_BG      10,  10,  10
    #define COLOR_PL_CURRENT_FILL    10,  10,  10
    #define COLOR_PLAYLIST_0        115, 115, 115
    #define COLOR_PLAYLIST_1         89,  89,  89
    #define COLOR_PLAYLIST_2         56,  56,  56
    #define COLOR_PLAYLIST_3         35,  35,  35
    #define COLOR_PLAYLIST_4         25,  25,  25
#endif

/*----- PRESETS -----*/
#define COLOR_PRST_BUTTON        21,  21,  21  // "presets" buttons background
#define COLOR_PRST_CARD          21,  21,  21  // "presets" kards
#define COLOR_PRST_ACCENT        50,  50,  50
#define COLOR_PRST_FAV          255, 255, 255
#define COLOR_PRST_TITLE_1      255, 255, 255
#define COLOR_PRST_TITLE_2      200, 200, 200
#define COLOR_PRST_TITLE_3      150, 150, 150
#define COLOR_PRST_LINE         162, 162, 162
#endif  /* #ifdef  ENABLE_THEME */
#endif  /* #define _my_theme_h  */