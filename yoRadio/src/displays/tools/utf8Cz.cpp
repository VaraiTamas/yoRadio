#include "Arduino.h"
#include "../../core/options.h"
#if L10N_LANGUAGE == CZ
#include "../dspcore.h"
#include "utf8To.h"

char* DspCore::utf8To(const char* str, bool uppercase) {
  int index = 0;
  static char strn[BUFLEN];
  bool E = false;
  strlcpy(strn, str, BUFLEN);

  String mystring(strn); 
  mystring.toUpperCase();
  mystring.toCharArray(strn, BUFLEN);

  if(L10N_LANGUAGE==EN) return strn;
  while (strn[index])
  {
    if (strn[index] >= 0xBF)
    {
      switch (strn[index]) {
        case 0xC3: {
            if (strn[index + 1] == 0xA1) {      // á
              strn[index] = 0x41;               // A
              break;
            }
            if (strn[index + 1] == 0xA4) {      // ä
              strn[index] = 0x41;               // A
              break;
            }
            if (strn[index + 1] == 0x81) {      // Á
              strn[index] = 0x41;               // A
              break;
            }
            if (strn[index + 1] == 0x84) {      // Ä
              strn[index] = 0x41;               // A
              break;
            }
            if (strn[index + 1] == 0x89) {      // É
              strn[index] = 0x45;               // E
              break;
            }
            if (strn[index + 1] == 0xA9) {      // é
              strn[index] = 0x45;               // E
              break;
            }
            if (strn[index + 1] == 0x8D) {      // Í
              strn[index] = 0x49;               // I
              break;
            }
            if (strn[index + 1] == 0xAD) {      // í
              strn[index] = 0x49;               // I
              break;
            }
            if (strn[index + 1] == 0x9D) {      // Ý
              strn[index] = 0x59;               // Y
              break;
            }
            if (strn[index + 1] == 0xBD) {      // ý
              strn[index] = 0x59;               // Y
              break;
            }
            if (strn[index + 1] == 0x93) {      // Ó
              strn[index] = 0x4F;               // O
              break;
            }
            if (strn[index + 1] == 0x94) {      // Ô
              strn[index] = 0x4F;               // O
              break;
            }
            if (strn[index + 1] == 0x96) {      // Ö
              strn[index] = 0x4F;               // O
              break;
            }
            if (strn[index + 1] == 0xB3) {      // ó
              strn[index] = 0x4F;               // O
              break;
            }
            if (strn[index + 1] == 0xB4) {      // ô
              strn[index] = 0x4F;               // O
              break;
            }
            if (strn[index + 1] == 0xB6) {      // ö
              strn[index] = 0x4F;               // O
              break;
            }
            if (strn[index + 1] == 0x9A) {      // Ú
              strn[index] = 0x55;               // U
              break;
            }
            if (strn[index + 1] == 0xBC) {      // Ü
              strn[index] = 0x55;               // U
              break;
            }
            if (strn[index + 1] == 0xBA) {      // ú
              strn[index] = 0x55;               // U
              break;
            }
            if (strn[index + 1] == 0xBC) {      // ü
              strn[index] = 0x55;               // U
              break;
            }
            if (strn[index + 1] >= 0x90 && strn[index + 1] <= 0xBF) strn[index] = strn[index + 1] + 0x30;
            break;
          }
        case 0xC4: {
            if (strn[index + 1] == 0x9A) {      // Ě
              strn[index] = 0x45;               // E
              break;
            }
            if (strn[index + 1] == 0x9B) {      // ě
              strn[index] = 0x45;               // E
              break;
            }
            if (strn[index + 1] == 0x8C) {      // Č
              strn[index] = 0x43;               // C
              break;
            }
            if (strn[index + 1] == 0x8D) {      // č
              strn[index] = 0x43;               // C
              break;
            }
            if (strn[index + 1] == 0x8E) {      // Ď
              strn[index] = 0x44;               // D
              break;
            }
            if (strn[index + 1] == 0x8F) {      // ď
              strn[index] = 0x44;               // D
              break;
            }
            if (strn[index + 1] >= 0x90 && strn[index + 1] <= 0xBF) strn[index] = strn[index + 1] + 0x30;
            break;
          }

        case 0xC5: {
            if (strn[index + 1] == 0x87) {    // Ń
              strn[index] = 0x4E;             // N
              break;
            }
            if (strn[index + 1] == 0x88) {    // ň
              strn[index] = 0x4E;             // N
              break;
            }
            if (strn[index + 1] == 0x98) {    // Ř
              strn[index] = 0x52;             // R
              break;
            }
            if (strn[index + 1] == 0x99) {    // ř
              strn[index] = 0x52;             // R
              break;
            }
            if (strn[index + 1] == 0xA0) {    // Š
              strn[index] = 0x53;             // S
              break;
            }
            if (strn[index + 1] == 0xA1) {    // š
              strn[index] = 0x53;             // S
              break;
            }
            if (strn[index + 1] == 0xA4) {    // Ť
              strn[index] = 0x54;             // T
              break;
            }
            if (strn[index + 1] == 0xA5) {    // ť
              strn[index] = 0x54;             // T
              break;
            }
            if (strn[index + 1] == 0xAE) {    // U s kroužkem
              strn[index] = 0x55;             // U
              break;
            }
            if (strn[index + 1] == 0xAF) {    // ů
              strn[index] = 0x55;             // U
              break;
            }
            if (strn[index + 1] == 0xBD) {    // Ž
              strn[index] = 0x5A;             // Z
              break;
            }
            if (strn[index + 1] == 0xBE) {    // ž
              strn[index] = 0x5A;             // Z
              break;
            }
            if (strn[index + 1] >= 0x80 && strn[index + 1] <= 0x8F) strn[index] = strn[index + 1] + 0x70;
            break;
          }
      }
      int sind = index + 2;
      while (strn[sind]) {
        strn[sind - 1] = strn[sind];
        sind++;
      }
      strn[sind - 1] = 0;
    }
    index++;
  }
  return strn;
}

#endif
