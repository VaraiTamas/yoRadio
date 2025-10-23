#include "Arduino.h"
#include "../../core/options.h"
#if L10N_LANGUAGE == UA
#include "../dspcore.h"
#include "utf8To.h"

// Convert UTF-8 Cyrillic (including Ukrainian) to single-byte font index
char* utf8To(const char* str, bool uppercase) {
  static char out[BUFLEN];
  int outPos = 0;

  for (int i = 0; str[i] && outPos < BUFLEN - 1; i++) {
    uint8_t c = (uint8_t)str[i];

    // Handle two-byte UTF-8 Cyrillic sequences
    if ((c == 0xD0 || c == 0xD1) && str[i + 1]) {
      uint8_t n = (uint8_t)str[++i];
      uint8_t ch = 0;

      // --- Capital letters (D0 range) ---
      if (c == 0xD0) {
        if (n == 0x81) ch = 0xA8;       // Є
        else if (n == 0x84) ch = 0xAA;  // Ґ
        else if (n == 0x86) ch = 0xB2;  // І
        else if (n == 0x87) ch = 0xAF;  // Ї
        else if (n >= 0x90 && n <= 0xBF) ch = n + 0x30; // А–п
      }

      // --- Lowercase (D1 range) ---
      else if (c == 0xD1) {
        if (n == 0x91) ch = 0xB8;       // є
        else if (n == 0x94) ch = 0xBA;  // ґ
        else if (n == 0x96) ch = 0xB3;  // і
        else if (n == 0x97) ch = 0xBF;  // ї
        else if (n >= 0x80 && n <= 0x8F) ch = n + 0x70; // р–я
      }

      if (ch) out[outPos++] = ch;
    }
    // --- ASCII ---
    else {
      out[outPos++] = uppercase ? toupper(c) : c;
    }
  }

  out[outPos] = '\0';
  return out;
}
#endif  //#if L10N_LANGUAGE == UA