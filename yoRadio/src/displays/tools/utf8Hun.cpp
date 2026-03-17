#include "Arduino.h"
#include "../../core/options.h"
#if L10N_LANGUAGE == HU
#include "../dspcore.h"
#include "utf8To.h"

// A DspCore korábbi logikáját megtartjuk, csak kiegészítjük a hosszú ő/ű támogatással,
// és az LCD ágban az egybájtos kódokra (0x93/0x9B/0x8B/0x98) fordítjuk.
// A javítást végezte: Botfai Tibor

char* utf8To(const char* str, bool uppercase) {
  int index = 0;
  static char strn[BUFLEN];
  bool E = false;
  strlcpy(strn, str, BUFLEN);

  if (uppercase) {
    bool next = false;
    for (char *iter = strn; *iter != '\0'; ++iter)
    {
      if (E) { E = false; continue; }
      uint8_t rus = (uint8_t)*iter;
      // ё kezelések
      if (rus == 208 && (uint8_t)*(iter + 1) == 129) {
        *iter = (char)209; *(iter + 1) = (char)145; E = true; continue;
      }
      if (rus == 209 && (uint8_t)*(iter + 1) == 145) {
        *iter = (char)209; *(iter + 1) = (char)145; E = true; continue;
      }
      if (next) {
        if (rus >= 128 && rus <= 143) *iter = (char)(rus + 32);
        if (rus >= 176 && rus <= 191) *iter = (char)(rus - 32);
        next = false;
      }
      if (rus == 208) next = true;
      if (rus == 209) { *iter = (char)208; next = true; }
      *iter = toupper(*iter);
    }
  }

  if (L10N_LANGUAGE == EN) return strn;

  while (strn[index])
  {
    // Latin-1 (C3 ..) – marad a korábbi magyar ékezetek mappingje
    if ((unsigned char)strn[index] == 0xC3)
    {
      switch ((unsigned char)strn[index + 1]) {
        case 0xA1: { strn[index] = uppercase ? (char)0x8F : (char)0x86; break; } // Á/á
        case 0x81: { strn[index] = (char)0x8F; break; }                           // Á
        case 0xAD: { strn[index] = uppercase ? (char)0x8D : (char)0x8C; break; } // Í/í
        case 0x8D: { strn[index] = (char)0x8D; break; }                           // Í
        case 0xBC: { strn[index] = uppercase ? (char)0x9A : (char)0x81; break; } // Ü/ü
        case 0x9C: { strn[index] = (char)0x9A; break; }                           // Ü
        case 0xB6: { strn[index] = uppercase ? (char)0x99 : (char)0x94; break; } // Ö/ö
        case 0x96: { strn[index] = (char)0x99; break; }                           // Ö
        case 0xBA: { strn[index] = uppercase ? (char)0x9C : (char)0x97; break; } // Ú/ú
        case 0x9A: { strn[index] = (char)0x9C; break; }                           // Ú
        case 0xB3: { strn[index] = uppercase ? (char)0x95 : (char)0x96; break; } // Ó/ó
        case 0x93: { strn[index] = (char)0x95; break; }                           // Ó
        case 0xA9: { strn[index] = uppercase ? (char)0x90 : (char)0x82; break; } // É/é
        case 0x89: { strn[index] = (char)0x90; break; }                           // É
        default: break;
      }
      // dobjuk a 2. UTF-8 bájtot
      int sind = index + 2;
      while (strn[sind]) { strn[sind - 1] = strn[sind]; sind++; }
      strn[sind - 1] = 0;
      index++;
      continue;
    }

    // Latin Extended-A (C5 ..) – HOSSZÚ ő/Ő és ű/Ű egybájtos LCD-kódokra
    if ((unsigned char)strn[index] == 0xC5)
    {
      unsigned char cont = (unsigned char)strn[index + 1];
      if (cont == 0x91) {            // ő
        strn[index] = (char)0x93;
      } else if (cont == 0x90) {     // Ő
        strn[index] = (char)0x9B;
      } else if (cont == 0xB1) {     // ű
        strn[index] = (char)0x8B;
      } else if (cont == 0xB0) {     // Ű
        strn[index] = (char)0x98;
      } else {
        index++;
        continue;
      }
      // dobjuk a 2. UTF-8 bájtot
      int sind = index + 2;
      while (strn[sind]) { strn[sind - 1] = strn[sind]; sind++; }
      strn[sind - 1] = 0;
      index++;
      continue;
    }

    index++;
  }
  return strn;
}

#endif  //#if L10N_LANGUAGE == HU