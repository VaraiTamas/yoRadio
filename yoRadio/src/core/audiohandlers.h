#ifndef AUDIOHANDLERS_H
#define AUDIOHANDLERS_H

#pragma once

#include <Arduino.h>
#include "../audioI2S/Audio.h"  // helyes útvonal a projektedben
#include "../displays/tools/l10n.h"

#ifdef USE_NEXTION
extern decltype(nextion) nextion;  // helyettesd
#endif
//extern Stream &telnet; // vagy a megfelelő típus, ha más

// Bufferméretek
#ifndef BUFLEN
  #define BUFLEN 512
#endif

// Prototípusok (a header elérhetővé teszi a függvényeket más fájlok számára)
void my_audio_info(Audio::msg_t m);
//void audio_info(const char *info);
void audio_bitrate(const char *info);
bool printable(const char *info);
void audio_showstation(const char *info);
void audio_showstreamtitle(const char *info);
void audio_error(const char *info);
void audio_id3artist(const char *info);
void audio_id3album(const char *info);
void audio_icy_description(const char *info);
void audio_beginSDread();
void audio_id3data(const char *info);
void audio_eof_mp3(const char *info);
void audio_eof_stream(const char *info);
void audio_progress(uint32_t startpos, uint32_t endpos);

static void safeStrCopy(char *dst, const char *src, size_t dstSize) {
  if (!dst || dstSize == 0) {
    return;
  }
  dst[0] = '\0';
  if (!src) {
    return;
  }
  strncpy(dst, src, dstSize - 1);
  dst[dstSize - 1] = '\0';
}

void my_audio_info(Audio::msg_t m) {
  // Kiíratás a sorosra (debug)
  Serial.printf("##AUDIO -> m.s  : %s\n", m.s);
  Serial.printf("##AUDIO -> m.msg: %s\n", m.msg);
  if (m.s == nullptr || m.msg == nullptr) {
    return;
  }
  if (player.lockOutput) {
    return;
  }

  // Az üzenet típusától függően hívjuk meg a megfelelő feldolgozót

  if (strcmp(m.s, "info") == 0) {
    // Formátum felismerés
    if (strstr(m.msg, "MPEG-1 Layer III") != NULL) {
      config.setBitrateFormat(BF_MP3);
      display.putRequest(DBITRATE);
    } else if (strstr(m.msg, "AAC") != NULL) {
      config.setBitrateFormat(BF_AAC);
      display.putRequest(DBITRATE);
    } else if (strstr(m.msg, "FLAC") != NULL) {
      config.setBitrateFormat(BF_FLAC);
      display.putRequest(DBITRATE);
    } else if (strstr(m.msg, "WAV") != NULL) {
      config.setBitrateFormat(BF_WAV);
      display.putRequest(DBITRATE);
    } else if (strstr(m.msg, "OGG") != NULL || strstr(m.msg, "VORBIS") != NULL) {
      config.setBitrateFormat(BF_OGG);
      display.putRequest(DBITRATE);
    } else if (strstr(m.msg, "OPUS") != NULL) {
      config.setBitrateFormat(BF_OPU);
      display.putRequest(DBITRATE);
    }
  }

  if (strstr(m.msg, "skip metadata") != NULL) {
    config.setTitle(config.station.name);
  }
  if (strstr(m.msg, "Account already in use") != NULL || strstr(m.msg, "HTTP/1.0 401") != NULL) {
    player.setError(m.msg);
  }
  // A bitrate üzenet. ✅
  if (strcmp(m.s, "bitrate") == 0) {
    audio_bitrate(m.msg);
  }
  // Az állomás nevének kiolvasása. ✅
  if (strcmp(m.s, "station_name") == 0 || strcmp(m.s, "icy-name") == 0) {
    if (printable(m.msg)) {
      config.setStation(m.msg);
      display.putRequest(NEWSTATION);
      netserver.requestOnChange(STATION, 0);  // Nem frissül a web!
    }
  }
  // Streamtitle kiolvasása. ✅
  if (strcmp(m.s, "streamtitle") == 0 || strcmp(m.s, "StreamTitle") == 0) {
    audio_id3album(m.msg);
  }
  // icy-name kiolvasása
  const char *ici;
  if ((ici = strstr(m.msg, "icy-name: ")) != NULL) {
    char icyName[BUFLEN] = {0};
    safeStrCopy(icyName, ici + strlen("icy-name: "), sizeof(icyName));
#ifdef NAME_STRIM
    // ha kell, vágjuk le a " - " után jövő részt
    char *dash = strstr(icyName, " - ");
    if (dash) {
      *dash = '\0';
      config.setStation(icyName);
    } else {
      config.setStation(icyName);
    }
    display.putRequest(NEWSTATION);
    netserver.requestOnChange(STATION, 0);
#endif
    audio_id3album(icyName);
  }
  // Zenei stílus kiolvasása POP stb. ✅
  if (strcmp(m.s, "icy-genre") == 0) {
    if (config.store.audioinfo) {
    }
  }
  if (strcmp(m.s, "icy_url") == 0 || strcmp(m.s, "icy-url") == 0) {  //✅
    if (config.store.audioinfo) {
    }
  }
  if (strcmp(m.s, "icy_description") == 0) {  // ✅
    if (config.store.audioinfo) {
      audio_icy_description(m.msg);
    }
  }
}

/************************************* */
/*************** BITRATE ***************/
/************************************* */
void audio_bitrate(const char *info) {
  if (config.store.audioinfo) {
  }
  uint32_t br = atoi(info);
  if (br > 3000) {
    br = br / 1000;
  }
  config.station.bitrate = br;
  display.putRequest(DBITRATE);
#ifdef USE_NEXTION
  nextion.bitrate(config.station.bitrate);
#endif
  netserver.requestOnChange(BITRATE, 0);
}

/************************************* */
/*********** PRINTABLE *****************/
/************************************* */
bool printable(const char *info) {
  if (!info) {
    return false;
  }
  const unsigned char *p = (const unsigned char *)info;
  while (*p) {
    unsigned char c = *p;
    // ASCII 0x20–0x7E között biztosan nyomtatható
    if (c >= 0x20 && c <= 0x7E) {
      p++;
      continue;
    }
    // UTF-8 többbájtos karakterek vizsgálata
    // 110xxxxx 10xxxxxx → 2 bájtos
    // 1110xxxx 10xxxxxx 10xxxxxx → 3 bájtos
    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx → 4 bájtos
    if ((c & 0xE0) == 0xC0) {  // 2 bájtos kezdő
      if ((p[1] & 0xC0) != 0x80) {
        return false;
      }
      p += 2;
      continue;
    } else if ((c & 0xF0) == 0xE0) {  // 3 bájtos kezdő
      if ((p[1] & 0xC0) != 0x80 || (p[2] & 0xC0) != 0x80) {
        return false;
      }
      p += 3;
      continue;
    } else if ((c & 0xF8) == 0xF0) {  // 4 bájtos kezdő
      if ((p[1] & 0xC0) != 0x80 || (p[2] & 0xC0) != 0x80 || (p[3] & 0xC0) != 0x80) {
        return false;
      }
      p += 4;
      continue;
    }
    // Egyéb nem megengedett bájt
    return false;
  }
  return true;
}

// Kűlső meghívásra.
void audio_showstation(const char *info) {
  bool p = printable(info) && (strlen(info) > 0);
  (void)p;
  if (player.remoteStationName) {  //MQTT ről jön
    config.setStation(p ? info : config.station.name);
    display.putRequest(NEWSTATION);
    netserver.requestOnChange(STATION, 0);
  }
}

// Kűlső meghívásra.
void audio_showstreamtitle(const char *info) {
  if (strstr(info, "Account already in use") != NULL || strstr(info, "HTTP/1.0 401") != NULL) {
    player.setError(info);
  }
  bool p = (strlen(info) > 0) && printable(info);
#ifdef DEBUG_TITLES
  config.setTitle(DEBUG_TITLES);
#else
  if (p) {
    config.setTitle(info);
  } else if (strlen(config.station.title) == 0) {
    config.setTitle(config.station.name);
  }
#endif
}

void audio_error(const char *info) {
  player.setError(info);
  telnet.printf("##ERROR#:\t%s\n", info);
}

void audio_id3artist(const char *info) {
  if (printable(info)) {
    config.setStation(info);
  }
  display.putRequest(NEWSTATION);
  netserver.requestOnChange(STATION, 0);
}

/* config.setTitle() kötőjellel elválasztva kell kapnia Title1 és Title2 sort config.ccp -ben.
   Ez hívja a 
   netserver.requestOnChange(TITLE, 0); // frissíti a WEB -et.
   netserver.loop();
   display.putRequest(NEWTITLE); ami a display.ccp ben hívja
   Display::_title() függvényt és frissíti a kijelzőt _title1 és _title2 scrollwidgeteket.
*/
void audio_id3album(const char *info) {
  if (player.lockOutput) {
    return;
  }
  if (!info) {
    return;
  }
  if (!printable(info)) {
    return;
  }
  config.setTitle(info);
}

void audio_icy_description(const char *info) {
  if (player.lockOutput) {
    return;
  }
  if (!info) {
    return;
  }
  if (!printable(info)) {
    return;
  }
  if (strlen(config.station.title) == 0 ||                           // ha üres
      strcmp(config.station.title, config.station.name) == 0 ||      // ha a title megegyezik az állomás nevével
      strstr(config.station.title, "timeout") != NULL ||             // ha tartalmazza a timeout szót
      strcmp_P(config.station.title, LANG::const_PlConnect) == 0) {  // ha a title = "[csatlakozás]" vagy ami a displayL10n_hu.h ban van írva.
    config.setTitle(info);
  }
}

void audio_beginSDread() {
  config.setTitle("");
}

void audio_id3data(const char *info) {
  if (player.lockOutput) {
    return;
  }
  telnet.printf("##AUDIO.ID3#: %s\n", info);
}

void audio_eof_mp3(const char *info) {
  config.sdResumePos = 0;
  player.next();
}

void audio_eof_stream(const char *info) {
  player.sendCommand({PR_STOP, 0});
  if (!player.resumeAfterUrl) {
    return;
  }
  if (config.getMode() == PM_WEB) {
    player.sendCommand({PR_PLAY, config.lastStation()});
  } else {
    player.setResumeFilePos(config.sdResumePos == 0 ? 0 : config.sdResumePos - player.sd_min);
    player.sendCommand({PR_PLAY, config.lastStation()});
  }
}

void audio_progress(uint32_t startpos, uint32_t endpos) {
  player.sd_min = startpos;
  player.sd_max = endpos;
  netserver.requestOnChange(SDLEN, 0);
}

#endif  // AUDIOHANDLERS_H
