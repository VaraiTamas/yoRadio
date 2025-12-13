#ifndef AUDIOHANDLERS_H
#define AUDIOHANDLERS_H

#pragma once

#include <Arduino.h>
#include "../audioI2S/Audio.h"
#include "../displays/tools/l10n.h"
#include "audiohelpers.h"

#ifdef USE_NEXTION
extern decltype(nextion) nextion;  // Nextion kijelző objektum (extern)
#endif

// Globális vagy osztály szintű változók
String currentArtist = "";
String currentTitle = "";
uint16_t currentStationId = static_cast<uint16_t>(-1);
bool metaOff = false;

// Előre deklarációk
void my_audio_info(Audio::msg_t m);
void processID3(const char *msg);
void audio_bitrate(const char *info);
bool printable(const char *info);
void audio_showstation(const char *info);
void audio_showstreamtitle(const char *info);
void audio_error(const char *info);
void audio_id3artist(const char *info);
void audio_setTitleSafe(const char *info);
void audio_icy_description(const char *info);
void audio_beginSDread();
void audio_id3data(const char *info);
void audio_eof();
void audio_progress(uint32_t startpos, uint32_t endpos);
void seekSD();
void removeBOM(char *s);
bool cleanMeta(const char *src, char *dst, size_t dstSize);
void _utf8_clean(char *s);

static void safeStrCopy(char *dst, const char *src, size_t dstSize) {
  if (!dst || !src || dstSize == 0) {
    return;
  }
  strlcpy(dst, src, dstSize);  // biztos null-terminált tesz
}

/* 
 * Ha van esemény, ezt futtatja a Schreibfaul1 audio könyvtár.
 * Profi, event-alapú verzió: elsődlegesen m.e (event_t) alapján dolgozik,
 * m.s csak debugra / kompatra használjuk.
 */
void my_audio_info(Audio::msg_t m) {
  // Biztonságos stringek a kiíráshoz
  const char *s = (m.s != nullptr) ? m.s : "";
  const char *msg = (m.msg != nullptr) ? m.msg : "";

  // Debug: mindent kiírunk
  Serial.printf("##AUDIO -> e:%d  m.s:'%s'  m.msg:'%s'\n", static_cast<int>(m.e), s, msg);

  // Ha a kimenet zárolva, semmit nem frissítünk
  if (player.lockOutput) {
    return;
  }

  // Ha nincs értelmes üzenet, nincs mit feldolgozni
  if (!msg) {
    return;
  }

  // META letiltás: ha a csatorna neve ponttal kezdődik,
  // akkor a title mindig a listában tárolt név lesz,
  // és nem használunk stream metaadatot.
  if (config.station.name[0] == '.') {
    config.setTitle(config.station.name + 1);
    metaOff = true;
  } else {
    metaOff = false;
  }

  // Általános hibák, amiket bármi eseményben figyelhetünk
  if (strstr(msg, "Account already in use") != nullptr || strstr(msg, "HTTP/1.0 401") != nullptr) {
    player.setError(msg);
  }

  // --------------------------------------------------------------------
  // EVENT-ALAPÚ FELDOLGOZÁS (m.e alapján)
  // --------------------------------------------------------------------
  switch (m.e) {

    // ----- Általános információk, formátum, SD hossz stb. -----
    case Audio::evt_info:
    {
      // Formátum felismerés
      if (strstr(msg, "MPEG-1 Layer III") != nullptr) {
        config.setBitrateFormat(BF_MP3);
        display.putRequest(DBITRATE);
      } else if (strstr(msg, "AAC") != nullptr) {
        config.setBitrateFormat(BF_AAC);
        display.putRequest(DBITRATE);
      } else if (strstr(msg, "FLAC") != nullptr) {
        config.setBitrateFormat(BF_FLAC);
        display.putRequest(DBITRATE);
      } else if (strstr(msg, "WAV") != nullptr) {
        config.setBitrateFormat(BF_WAV);
        display.putRequest(DBITRATE);
      } else if (strstr(msg, "OGG") != nullptr || strstr(msg, "VORBIS") != nullptr) {
        config.setBitrateFormat(BF_OGG);
        display.putRequest(DBITRATE);
      } else if (strstr(msg, "OPUS") != nullptr) {
        config.setBitrateFormat(BF_OPU);
        display.putRequest(DBITRATE);
      }

      // SD mód: "stream ready" → seek a mentett pozícióra
      if (strstr(msg, "stream ready") != nullptr) {
        seekSD();
      }
      // SD mód: Audio-Data-Start
      else if (strstr(msg, "Audio-Data-Start:") != nullptr) {
        player.sd_min = atoi(msg + strlen("Audio-Data-Start:"));
      }
      // SD mód: teljes hossz (Audio-Length:)
      else if (strstr(msg, "Audio-Length:") != nullptr) {
        uint32_t audioLength = static_cast<uint32_t>(atoi(msg + strlen("Audio-Length:")));
        player.sd_max = player.sd_min + audioLength;
        netserver.requestOnChange(SDLEN, 0);  // slider tartomány a web felé
      }

      // Ha a stream „skip metadata” módot jelez, akkor állomásnév kerül a title-be
      if (strstr(msg, "skip metadata") != nullptr) {
        if (config.station.name[0] == '.') {
          config.setTitle(config.station.name + 1);
        } else {
          config.setTitle(config.station.name);
        }
      }
    } break;

    // ----- Bitráta esemény -----
    case Audio::evt_bitrate:
    {
      // A könyvtár tipikusan szövegként adja meg a bitrátát (pl. "128000")
      audio_bitrate(msg);
    } break;

      // ----- Stream title (ICY) -----
    case Audio::evt_streamtitle:
    {
      //Serial.println();
      //hexDump("Eredeti: ", msg);
      char metaBuf[BUFLEN];
      if (!metaOff && cleanMeta(msg, metaBuf, sizeof(metaBuf))) {
        audio_setTitleSafe(metaBuf);
      }
      //Serial.println();
      //hexDump("cleanMeta után: ", metaBuf);
    } break;

    // ----- ID3 metaadatok (MP3) -----
    case Audio::evt_id3data:
    {
      // Log a telnetre
      audio_id3data(msg);

      // Track számból állomás ID (SD lejátszásnál fontos)
      if (strstr(msg, "Track:") != nullptr) {
        currentStationId = static_cast<uint16_t>(atoi(msg + strlen("Track:")));
      }
      char metaBuf[BUFLEN];
      if (!metaOff && cleanMeta(msg, metaBuf, sizeof(metaBuf))) {
        // processID3 kiszedi az Artist / Title sorokat
        processID3(metaBuf);
      }
    } break;

    // ----- Állomásnév esemény -----
    case Audio::evt_name:
    {
      char metaBuf[BUFLEN];
      if (!metaOff && cleanMeta(msg, metaBuf, sizeof(metaBuf))) {
        config.setStation(metaBuf);
        display.putRequest(NEWSTATION);
        netserver.requestOnChange(STATION, 0);
      }
    } break;

    // ----- ICY description (leírás) -----
    case Audio::evt_icydescription:
    {
      if (!metaOff) {
        audio_icy_description(msg);
      }
    } break;

    // ----- ICY URL -----
    case Audio::evt_icyurl:
    {
      // Jelenleg nincs külön feldolgozás, de ha kell, itt bővíthető
      // if (config.store.audioinfo) { ... }
    } break;

    // ----- Kép / borító (APIC) -----
    case Audio::evt_image:
    {
      // A msg általában pl.: "APIC found at pos 446"
      // Ha egyszer cover art feldolgozás lesz, az ide kerül.
      // Jelenleg csak logoljuk:
      // telnet.printf("##AUDIO.IMG#: %s\r\n", msg);
    } break;

    // ----- Fájl vége (SD mód) -----
    case Audio::evt_eof:
    {
      config.vuRefLevel =0;
      // audio_eof(); // TODO Nem mindig érkezik meg az eof.
    } break;

    // ----- Log események (hiba, diagnosztika) -----
    case Audio::evt_log:
    {
      // Ide jöhetne finomabb log-elemzés, de most az általános
      // hibaszűrést már a switch előtt elintéztük (Account in use / 401).
      // Ha kell, itt tovább bontható.
    } break;

    // ----- Nem használt / újonnan bejövő események -----
    case Audio::evt_lasthost:
    case Audio::evt_icylogo:
    case Audio::evt_lyrics:
    default:
      // Jelenleg nincs külön kezelés, de a debug logban látszik
      break;
  }

  // --------------------------------------------------------------------
  // 2) EXTRA SZÖVEG ALAPÚ ÉRTELMEZÉS (m.msg tartalma alapján),
  //    ami nem szorosan event-típushoz kötött.
  // --------------------------------------------------------------------

  // icy-name: ... → sok rádió ilyen formában küldi a nevet
  if (!metaOff) {
    const char *ici = strstr(msg, "icy-name: ");
    if (ici != nullptr) {
      char icyName[BUFLEN] = {0};
      safeStrCopy(icyName, ici + strlen("icy-name: "), sizeof(icyName));
      char metaBuf[BUFLEN];
      if (cleanMeta(icyName, metaBuf, sizeof(metaBuf)) && strlen(metaBuf) > 0) {
        audio_setTitleSafe(metaBuf);
      }
    }
  }
}

/* 
 * Ha megállítottuk a zene lejátszását SD módban és újraindítjuk, 
 * akkor a lejátszás az elejéről kezdődne. 
 * Ha megérkezik a "stream ready" üzenet, akkor vissza kell ugrani
 * a mentett stop pozícióra.
 */
void seekSD() {
  if (config.getMode() == PM_SDCARD && config.sdResumePos > 0) {
    if (currentStationId == config.stopedSdStationId) {
      uint32_t offset = 0;
      if (config.sdResumePos > player.sd_min) {
        offset = config.sdResumePos - player.sd_min;
      }
      player.setAudioFilePosition(offset);
    }
  }
}

void processID3(const char *msg) {
  bool updated = false;
  if (!msg) {
    return;
  }
  // "Artist: " → hossz 8
  if (strstr(msg, "Artist") == msg) {
    String s = String(msg).substring(8);
    s.trim();
    currentArtist = s;
    updated = true;
  }
  // "Title: " → hossz 7
  else if (strstr(msg, "Title") == msg) {
    String s = String(msg).substring(7);
    s.trim();
    currentTitle = s;
    updated = true;
  }
  if (updated) {
    String info;
    if (currentArtist.length() > 0 && currentTitle.length() > 0) {
      info = currentArtist + " - " + currentTitle;
    } else if (currentArtist.length() > 0) {
      info = currentArtist + " -  ";  // cím még nincs
    }
    if (info.length() > 0) {
      config.setTitle(info.c_str());
    }
  }
}

/************************************* */
/*************** BITRATE ***************/
/************************************* */
void audio_bitrate(const char *info) {
  if (!info) {
    return;
  }
  if (config.store.audioinfo) {
    // Itt lehetne plusz log, ha szükséges
  }
  uint32_t br = static_cast<uint32_t>(atoi(info));
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

/***************************************/
/*********** PRINTABLE *****************/
/***************************************/
bool printable(const char *info) {
  if (!info) {
    return false;
  }
  const unsigned char *p = reinterpret_cast<const unsigned char *>(info);
  while (*p) {
    unsigned char c = *p;
    // Kontroll karakterek tiltása (0x00–0x1F), TAB opcionálisan engedhető
    if (c < 0x20) {
      if (c != 0x09) {
        return false;
      }
    }
    // ASCII (nyomtatható)
    if (c >= 0x20 && c <= 0x7E) {
      p++;
      continue;
    }
    // UTF-8 multi-byte validálás
    // 2 bájtos
    if ((c & 0xE0) == 0xC0) {
      if ((p[1] & 0xC0) != 0x80) {
        return false;
      }
      p += 2;
      continue;
    }
    // 3 bájtos
    if ((c & 0xF0) == 0xE0) {
      if ((p[1] & 0xC0) != 0x80 || (p[2] & 0xC0) != 0x80) {
        return false;
      }
      p += 3;
      continue;
    }
    // 4 bájtos karakterek tiltása
    if ((c & 0xF8) == 0xF0) {
      return false;
    }
    // Minden más hibás
    return false;
  }
  return true;
}

// Külső meghívásra.
void audio_showstation(const char *info) {
  bool p = printable(info) && (info && strlen(info) > 0);
  if (player.remoteStationName) {  // MQTT-ről jön
    config.setStation(p ? info : config.station.name);
    display.putRequest(NEWSTATION);
    netserver.requestOnChange(STATION, 0);
  }
}

// Külső meghívásra.
void audio_showstreamtitle(const char *info) {
  if (!info) {
    return;
  }

  if (strstr(info, "Account already in use") != nullptr || strstr(info, "HTTP/1.0 401") != nullptr) {
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
  if (!info) {
    return;
  }
  player.setError(info);
  telnet.printf("##ERROR#:\t%s\r\n", info);
}

void audio_id3artist(const char *info) {
  config.setStation(info);
  display.putRequest(NEWSTATION);
  netserver.requestOnChange(STATION, 0);
}

/*
 * config.setTitle() kötőjellel elválasztva kell kapnia Title1 és Title2 sort.
 * Ez hívja a:
 *   netserver.requestOnChange(TITLE, 0); // frissíti a WEB-et
 *   netserver.loop();
 *   display.putRequest(NEWTITLE);
 * Display::_title() szétválasztja a kötőjel mentén a _title1 / _title2 sorokat
 * és frissíti a scrollwidgeteket.
 */
void audio_setTitleSafe(const char *info) {
  if (player.lockOutput) {
    return;
  }
  if (!info) {
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
  if (strlen(config.station.title) == 0 ||                           // ha üres
      strcmp(config.station.title, config.station.name) == 0 ||      // ha a title megegyezik az állomás nevével
      strstr(config.station.title, "timeout") != nullptr ||          // ha tartalmazza a "timeout" szót
      strcmp_P(config.station.title, LANG::const_PlConnect) == 0) {  // ha title = "[csatlakozás]" (lokalizált)
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
  if (!info) {
    return;
  }
  telnet.printf("##AUDIO.ID3#: %s\r\n", info);
}

void audio_eof() {
  if (!config.isClockTTS && config.getMode() == PM_SDCARD) {
    config.sdResumePos = 0;
    player.next();
  }
}

void audio_progress(uint32_t startpos, uint32_t endpos) {
  player.sd_min = startpos;
  player.sd_max = endpos;
  netserver.requestOnChange(SDLEN, 0);
}
// Az audiohelpers.h fájlban van deklarálva.
// Hexadecimális kiiratás debug használatra.
void hexDump(const char *label, const char *s) {
  Serial.printf("%s (len=%u): %s --> ", label, strlen(s), s);
  const unsigned char *p = (const unsigned char *)s;
  while (*p) {
    Serial.printf("%02X ", *p);
    p++;
  }
  Serial.println();
}

void removeBOM(char *s) {
  if (!s) {
    return;
  }
  if (strlen(s) < 3) {
    return;
  }

  if ((unsigned char)s[0] == 0xEF && (unsigned char)s[1] == 0xBB && (unsigned char)s[2] == 0xBF) {
    memmove(s, s + 3, strlen(s + 3) + 1);
  }
}

bool cleanMeta(const char *src, char *dst, size_t dstSize) {
  if (!src || !dst || dstSize == 0) {
    return false;
  }
  // bemásoljuk lokális bufferbe
  strlcpy(dst, src, dstSize);
  // BOM eltávolítás
  removeBOM(dst);
  // UTF-8 takarítás
  _utf8_clean(dst);
  // csak ellenőrzés (nem módosít):
  if (!printable(dst)) {
    return false;
  }
  return true;
}

void _utf8_clean(char *s) {
  char *in = s;
  char *out = s;
  while (*in) {
    unsigned char c = (unsigned char)*in;
    // --- ZERO-WIDTH karakterek kiszűrése ---
    if (c == 0xE2 && (unsigned char)in[1] == 0x80 && ((unsigned char)in[2] == 0x8B || (unsigned char)in[2] == 0x8C || (unsigned char)in[2] == 0x8D)) {
      in += 3;
      continue;
    }
    // Soft hyphen
    if (c == 0xC2 && (unsigned char)in[1] == 0xAD) {
      in += 2;
      continue;
    }
    // --- MINDEN UTF-8 maradjon érintetlen ---
    // Csak másoljuk byte-onként
    *out++ = *in++;
  }
  *out = '\0';
}

#endif  // AUDIOHANDLERS_H
