// Módosítva, átírva hibák javítva.
#include "clock_tts.h"
#include "../core/options.h"
#include "../../myoptions.h"
#include "../core/config.h"
#include "../core/network.h"
#include "../core/player.h"
#include <Arduino.h>
#include <time.h>

static int clock_tts_prev_volume = 0;
static bool clock_tts_fading_down = false;
static bool clock_tts_fading_up = false;
static unsigned long clock_tts_fade_timer = 0;
static int clock_tts_fade_volume = -1;
static unsigned long clock_lastTTSMillis = 0;
static bool clock_ttsActive = false;
static int clock_lastMinute = -1;
static int clock_tts_saved_station = -1;

// Konfigurációs változók
static bool clock_tts_enabled = CLOCK_TTS_ENABLED;
static int clock_tts_interval = CLOCK_TTS_INTERVAL_MINUTES;
static char clock_tts_language[32] = CLOCK_TTS_LANGUAGE;

void clock_tts_set_language(const char *lang) {
  if (lang && strlen(lang) < sizeof(clock_tts_language)) {
    strcpy(clock_tts_language, lang);
  }
}

void clock_tts_set_interval(int minutes) {
  if (minutes > 0) {
    clock_tts_interval = minutes;
  }
}

void clock_tts_enable(bool enable) {
  clock_tts_enabled = enable;
}

void clock_tts_setup() {
  clock_tts_prev_volume = 0;
  clock_tts_fading_down = false;
  clock_tts_fading_up = false;
  clock_tts_fade_timer = 0;
  clock_tts_fade_volume = -1;
  clock_lastTTSMillis = 0;
  clock_ttsActive = false;
  clock_lastMinute = -1;
  clock_tts_enabled = CLOCK_TTS_ENABLED;
}

void clock_tts_force(const char *text, const char *lang) {
  player.connecttospeech(text, lang ? lang : clock_tts_language);
  clock_lastTTSMillis = millis();
  clock_ttsActive = true;
}

static void clock_tts_announcement(char *buf, size_t buflen, int hour, int min, const char *lang) {
  if (strncmp(lang, "PL", 2) == 0) {
    snprintf(buf, buflen, "Jest godzina %d:%02d.", hour, min);
  } else if (strncmp(lang, "HU", 2) == 0) {
    snprintf(buf, buflen, "Az idő %d:%02d.", hour, min);
  } else if (strncmp(lang, "RU", 2) == 0) {
    snprintf(buf, buflen, "Сейчас %d:%02d.", hour, min);
  } else if (strncmp(lang, "DE", 2) == 0) {
    snprintf(buf, buflen, "Es ist %d Uhr %02d.", hour, min);
  } else if (strncmp(lang, "FR", 2) == 0) {
    snprintf(buf, buflen, "Il est %d:%02d.", hour, min);
  } else if (strncmp(lang, "GR", 2) == 0) {
    snprintf(buf, buflen, "I ora einai %d:%02d.", hour, min);
  } else if (strncmp(lang, "RO", 2) == 0) {
    snprintf(buf, buflen, "Este ora %d:%02d.", hour, min);
  } else if (strncmp(lang, "NL", 2) == 0) {
    snprintf(buf, buflen, "De tijd %d:%02d.", hour, min);
  } else {
    snprintf(buf, buflen, "The time is %d:%02d.", hour, min);
  }
}

void clock_tts_loop() {
  if (!clock_tts_enabled) {
    return;
  }
  if (config.getMode() == PM_SDCARD) {
    return;
  }
  //Serial.printf("player.isRuning %d \n", player.isRunning());
  unsigned long nowMillis = millis();
  time_t now = time(nullptr);
  struct tm *tm_struct = localtime(&now);

  // --- Fokozatos elhalkulás a TTS előtt ---
  if (clock_tts_fading_down) {
    if (clock_tts_fade_volume == -1) {
      clock_tts_fade_volume = player.getVolume();
      clock_tts_prev_volume = clock_tts_fade_volume;
    }
    if (nowMillis - clock_tts_fade_timer > 50 && clock_tts_fade_volume > 0) {
      clock_tts_fade_volume -= 1;
      if (clock_tts_fade_volume < 0) {
        clock_tts_fade_volume = 0;
      }
      player.setVolume(clock_tts_fade_volume);
      clock_tts_fade_timer = nowMillis;
    }
    if (clock_tts_fade_volume <= 0) {
      clock_tts_saved_station = config.lastStation();  // Aktuális állomás mentése
      config.isClockTTS = true;
      delay(150);
      char buf[48];
      clock_tts_announcement(buf, sizeof(buf), tm_struct->tm_hour, tm_struct->tm_min, clock_tts_language);
      player.setVolume(clock_tts_prev_volume);
      player.connecttospeech(buf, clock_tts_language);
      clock_lastTTSMillis = nowMillis;
      clock_tts_fading_down = false;
      clock_ttsActive = true;
      clock_lastMinute = tm_struct->tm_min;
      clock_tts_fade_volume = -1;
      config.isClockTTS = false;
    }
    return;
  }

  // --- Fokozatos hangerőnövekedés a TTS után ---
  if (clock_tts_fading_up) {
    if (clock_tts_fade_volume == -1) {
      clock_tts_fade_volume = 0;
    }
    if (nowMillis - clock_tts_fade_timer > 80 && clock_tts_fade_volume < clock_tts_prev_volume) {
      clock_tts_fade_volume += 1;
      player.setVolume(clock_tts_fade_volume);
      clock_tts_fade_timer = nowMillis;
    }
    if (clock_tts_fade_volume >= clock_tts_prev_volume) {
      player.setVolume(clock_tts_prev_volume);
      clock_tts_fading_up = false;
      clock_tts_fade_volume = -1;
    }
    return;
  }

  if (tm_struct) {
    if (tm_struct->tm_year + 1900 < 2020) {
      return;
    }
    if (tm_struct->tm_min % clock_tts_interval == 0 && tm_struct->tm_min != clock_lastMinute && tm_struct->tm_sec < 2 && !clock_ttsActive
        && player.isRunning()) {
      clock_tts_fading_down = true;  // Engedélyezi a halkítást.
      clock_tts_fade_timer = nowMillis;
      return;
    }
    if (clock_ttsActive && (nowMillis - clock_lastTTSMillis > 4500)) {
      player.sendCommand({PR_PLAY, clock_tts_saved_station});
      clock_tts_fading_up = true;
      clock_tts_fade_timer = nowMillis;
      clock_ttsActive = false;
      return;
    }
  } else {
    static unsigned long lastTTS = 0;
    if (nowMillis - lastTTS > 120000) {
      player.connecttospeech("Nem sikerült lekérni az időt", clock_tts_language);
      lastTTS = nowMillis;
    }
  }
}
