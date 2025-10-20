#include "Arduino.h"
#include "../../core/options.h"
#if L10N_LANGUAGE == HU
#include "../dspcore.h"
#include "utf8To.h"

// A DspCore korábbi logikáját megtartjuk, csak kiegészítjük a hosszú ő/ű támogatással,
// és az LCD ágban az egybájtos kódokra (0x93/0x9B/0x8B/0x98) fordítjuk.
// A javítást végezte: Botfai Tibor

#ifndef DSP_LCD
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
#else  // ============================== nem-LCD ág ==============================

char* utf8To(const char* str, bool uppercase) {
  int index = 0;
  static char strn[BUFLEN];
  static char newStr[BUFLEN];
  bool E = false;

  strlcpy(strn, str, BUFLEN);
  newStr[0] = '\0';

  // nagybetűsítés (eredeti logika)
  bool next = false;
  for (char *iter = strn; *iter != '\0'; ++iter)
  {
    if (E) { E = false; continue; }
    uint8_t rus = (uint8_t)*iter;
    if (rus == 208 && (uint8_t)*(iter + 1) == 129) { *iter = (char)209; *(iter + 1) = (char)145; E = true; continue; }
    if (rus == 209 && (uint8_t)*(iter + 1) == 145) { *iter = (char)209; *(iter + 1) = (char)145; E = true; continue; }
    if (next) {
      if (rus >= 128 && rus <= 143) *iter = (char)(rus + 32);
      if (rus >= 176 && rus <= 191) *iter = (char)(rus - 32);
      next = false;
    }
    if (rus == 208) next = true;
    if (rus == 209) { *iter = (char)208; next = true; }
    *iter = toupper(*iter);
  }

  while (strn[index])
  {
    if (strlen(newStr) > BUFLEN - 2) break;

    // Latin Extended-A: Ő/ő + Ű/ű – tényleges UTF-8-ra írjuk vissza
    if ((unsigned char)strn[index] == 0xC5) {
      unsigned char cont = (unsigned char)strn[index + 1];
      if (cont == 0x91) { strcat(newStr, uppercase ? "Ő" : "ő"); }
      else if (cont == 0x90) { strcat(newStr, "Ő"); }
      else if (cont == 0xB1) { strcat(newStr, uppercase ? "Ű" : "ű"); }
      else if (cont == 0xB0) { strcat(newStr, "Ű"); }
      else {
        char Temp[2] = { strn[index], 0 };
        strcat(newStr, Temp);
        index++;
        continue;
      }
      // dobjuk a 2. bájtot
      int sind = index + 2;
      while (strn[sind]) { strn[sind - 1] = strn[sind]; sind++; }
      strn[sind - 1] = 0;
      index++;
      continue;
    }

    // Cirill (D0/D1) → latin betűs átírás (meglévő logika rendezve)
    if ((unsigned char)strn[index] == 0xD0) {
      unsigned char c2 = (unsigned char)strn[index + 1];
      switch (c2) {
        case 0x81: strcat(newStr, "YO"); break; // Ё
        case 0x90: strcat(newStr, "A");  break; // А
        case 0x91: strcat(newStr, "B");  break; // Б
        case 0x92: strcat(newStr, "V");  break; // В
        case 0x93: strcat(newStr, "G");  break; // Г
        case 0x94: strcat(newStr, "D");  break; // Д
        case 0x95: strcat(newStr, "E");  break; // Е
        case 0x96: strcat(newStr, "ZH"); break; // Ж
        case 0x97: strcat(newStr, "Z");  break; // З
        case 0x98: strcat(newStr, "I");  break; // И
        case 0x99: strcat(newStr, "Y");  break; // Й
        case 0x9A: strcat(newStr, "K");  break; // К
        case 0x9B: strcat(newStr, "L");  break; // Л
        case 0x9C: strcat(newStr, "M");  break; // М
        case 0x9D: strcat(newStr, "N");  break; // Н
        case 0x9E: strcat(newStr, "O");  break; // О
        case 0x9F: strcat(newStr, "P");  break; // П
        case 0xA0: strcat(newStr, "R");  break; // Р
        case 0xA1: strcat(newStr, "S");  break; // С
        case 0xA2: strcat(newStr, "T");  break; // Т
        case 0xA3: strcat(newStr, "U");  break; // У
        case 0xA4: strcat(newStr, "F");  break; // Ф
        case 0xA5: strcat(newStr, "H");  break; // Х
        case 0xA6: strcat(newStr, "TS"); break; // Ц
        case 0xA7: strcat(newStr, "CH"); break; // Ч
        case 0xA8: strcat(newStr, "SH"); break; // Ш
        case 0xA9: strcat(newStr, "SHCH"); break; // Щ
        case 0xAA: strcat(newStr, "\""); break; // Ъ
        case 0xAB: strcat(newStr, "Y");  break; // Ы
        case 0xAC: strcat(newStr, "'");  break; // Ь
        case 0xAD: strcat(newStr, "E");  break; // Э
        case 0xAE: strcat(newStr, "YU"); break; // Ю
        case 0xAF: strcat(newStr, "YA"); break; // Я
        default: { char Temp[2] = { (char)0xD0, 0 }; strcat(newStr, Temp); index++; continue; }
      }
      // eltolás (eldobjuk a 2. bájtot)
      int sind = index + 2;
      while (strn[sind]) { strn[sind - 1] = strn[sind]; sind++; }
      strn[sind - 1] = 0;
      index++;
      continue;
    }

    if ((unsigned char)strn[index] == 0xD1) {
      unsigned char c2 = (unsigned char)strn[index + 1];
      if (c2 == 0x91) { strcat(newStr, "YO"); } // ё
      else { char Temp[2] = { (char)0xD1, 0 }; strcat(newStr, Temp); index++; continue; }
      int sind = index + 2;
      while (strn[sind]) { strn[sind - 1] = strn[sind]; sind++; }
      strn[sind - 1] = 0;
      index++;
      continue;
    }

    // Egyéb karakterek
    {
      if (strn[index] == 7) strn[index] = (char)165;
      if (strn[index] == 9) strn[index] = (char)223;
      char Temp[2] = { (char)strn[index], 0 };
      strcat(newStr, Temp);
      index++;
    }
  }
  return newStr;
}
#endif  // DSP_LCD
#endif  //#if L10N_LANGUAGE == HU