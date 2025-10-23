/* https://trip5.github.io/ehRadio_myoptions/generator.html */
/* https://github.com/e2002/yoradio */
/* Használat előtt olvasd el a readme.txt fájlt !!! */
/* Read the readme.txt file before use !!! */
#ifndef myoptions_h
#define myoptions_h

#ifndef ARDUINO_ESP32S3_DEV
#define ARDUINO_ESP32S3_DEV
#endif

/* Itt tudod beállítani a program nyelvét 
   You can set the program language here. 
   Supported languages: HU NL PL RU EN GR SK . */
#define L10N_LANGUAGE UA

/* -- Névnapok megjelenítése -- Display name days -- */
#define NAMEDAYS_FILE UA  // HU, PL, NL, GR, (UA Local/namedays/namedays_UA.h is not filled in.)

#define USE_BUILTIN_LED false /* The RGB LED does not turn on.. */

/* Arduino OTA Support */
// #define USE_OTA true                    /* Enable OTA updates from Arduino IDE */
// #define OTA_PASS "myotapassword12345"   /* OTA password for secure updates */

/* HTTP Authentication */
// #define HTTP_USER ""               /* HTTP basic authentication username */
// #define HTTP_PASS ""           /* HTTP basic authentication password */

/* LCD */
#define DSP_MODEL DSP_ILI9488
//#define DSP_MODEL DSP_ILI9341
//#define DSP_MODEL DSP_ST7796
#define TFT_DC         9
#define TFT_CS         10
#define TFT_RST        -1
#define BRIGHTNESS_PIN 14
/* 
   GPIO 11 - MOSI
   GPIO 12 - CLK
   GPIO 13 - MISO  // Ne csatlakoztasd a kijelzőhöz!!! - Do not connect to the LCD display!!! 
*/

/* Touch */
#define TS_MODEL TS_MODEL_XPT2046
#define TS_CS    3

// #define NEXTION_RX			15
// #define NEXTION_TX			16

/* PCM5102A  DAC */
#define I2S_DOUT 4
#define I2S_BCLK 5
#define I2S_LRC  6

/* ENCODER 1 */
#define ENC_BTNR 41  // S2
#define ENC_BTNL 40  // S1
#define ENC_BTNB 39  // KEY
// #define ENC_INTERNALPULLUP		true

/* ENCODER 2 */
#define ENC2_BTNR 47  // S2
#define ENC2_BTNL 48  // S1
#define ENC2_BTNB 21  // KEY
// #define ENC2_INTERNALPULLUP		true

/* CLOCK MODUL RTC DS3132 */
// #define RTC_SCL			     7
// #define RTC_SDA			     8
// #define RTC_MODULE DS3231

/* REMOTE CONTROL INFRARED RECEIVER */
#define IR_PIN 38

// #define WAKE_PIN	42

/* DS CARD */
#define SDC_CS 18

/* Az inaktív szegmens megjelenítése az óra számaiban true -> engedélyez, false -> nem engedélyez. 
   Inactive segments of the clock, true or false. */
#define CLOCKFONT_MONO true

/* Bekapcsolja az eredeti hétszegmenses óra betűtípust.
   Turn on the original seven-segment font. */
 #define CLOCKFONT  VT_DIGI_OLD

/* Google TTS hanggal mondja be az ídőt megadott nyelven és megadott percenként.
   Speaks the time using Google TTS voice in the specified language and every specified minute.
*/
#define CLOCK_TTS_ENABLED          true  // Enabled (true) or disabled (false)
#define CLOCK_TTS_LANGUAGE         "HU"  // Language ( EN, HU, PL, NL, DE, RU ,FR, GR)
#define CLOCK_TTS_INTERVAL_MINUTES 30    // Hány percenként mondja be. - How many times a minute does it say.

/* Ezzel a beállítással nincs görgetés az időjárás sávon.
   With this setting there is no scrolling on the weather bar.
*/
//#define WEATHER_FMT_SHORT

/* A VU méter két fajta kijelzési módot támogat.
BOOMBOX_STYLE stílusa, amikor középről két oldalra leng ki a kijelző. Azt itt tudod beállítani.
Ha a sor elején ott van // jel akkor az alap VU méter működik ami balról jobbra leng ki.
 The VU meter supports two types of display modes.
BOOMBOX_STYLE is the style when the display swings out from the center to two sides. You can set it here.
If there is a // sign at the beginning of the line, the basic VU meter is working, swinging out from left to right. 
*/
#define BOOMBOX_STYLE

/* A VU méter végén megjelenik egy fehér csík a csúcsértékeknél, ha ezt bekapcsolod. A sor elején a // jel letíltja.
 A white bar will appear at the end of the VU meter at the peak values ​​if you enable this. The // at the beginning of the line will disable it.
*/
#define VU_PEAK

/* Az állomások listájából való választásnál nem kell megnyomni a rotary encoder gombját, kilépéskor autómatikusan
átvált a csatorna. (Zsigmond Becskeházi által)
 When selecting from the station list, you do not need to press the rotary encoder button, the channel will automatically
change when you exit. (By Zsigmond Becskehazi) */
#define DIRECT_CHANNEL_CHANGE

/* Mennyi idő múlva lépjen vissza a főképernyőre az állomások listájából. (másodperc)
 How long to return to the main screen from the station list. (seconds) */
#define STATIONS_LIST_RETURN_TIME 2

#endif
