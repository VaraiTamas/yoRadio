#include "Arduino.h"
#include "../../core/options.h"
#if L10N_LANGUAGE == RU || L10N_LANGUAGE == EN || L10N_LANGUAGE == NL // Módosítás plussz sor. "utf8To"
    #include "../dspcore.h"
    #include "utf8To.h"

size_t strlen_utf8(const char* s) {
    size_t count = 0;
    while (*s) {
        count++;
        if ((*s & 0xF0) == 0xF0) { // 4-byte character
            s += 4;
        } else if ((*s & 0xE0) == 0xE0) { // 3-byte character
            s += 3;
        } else if ((*s & 0xC0) == 0xC0) { // 2-byte character
            s += 2;
        } else { // 1-byte character (ASCII)
            s += 1;
        }
    }
    return count;
}

char* utf8To(const char* str, bool uppercase) {
    static char out[BUFLEN];
    int         outPos = 0;
    for (int i = 0; str[i] && outPos < BUFLEN - 1; i++) {
        uint8_t c = (uint8_t)str[i];
        if (c == 0xD0 && str[i + 1]) {
            uint8_t n = (uint8_t)str[++i];
            if (n == 0x81) { // Ё
                out[outPos++] = uppercase ? 0xA8 : 0xB8;
            } else if (n >= 144 && n <= 191) {
                uint8_t ch = n + 48;
                if (n >= 176 && uppercase) ch -= 32;
                out[outPos++] = ch;
            }
        } else if (c == 0xD1 && str[i + 1]) {
            uint8_t n = (uint8_t)str[++i];
            if (n == 0x91) { // ё
                out[outPos++] = uppercase ? 0xA8 : 0xB8;
            } else if (n >= 128 && n <= 143) {
                uint8_t ch = n + 112;
                if (uppercase) ch -= 32;
                out[outPos++] = ch;
            }
        } else { // ASCII
            out[outPos++] = uppercase ? toupper(c) : c;
        }
    }
    out[outPos] = 0;
    return out;
}
#endif // L10N_LANGUAGE == RU