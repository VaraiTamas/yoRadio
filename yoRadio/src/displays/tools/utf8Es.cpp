#include "Arduino.h"
#include "../../core/options.h"

#if L10N_LANGUAGE == ES
#include "../dspcore.h"
#include "utf8To.h"

#ifndef DSP_LCD
char* utf8To(const char* str, bool uppercase)
{
    static char out[BUFLEN];
    int i = 0, o = 0;

    memset(out, 0, BUFLEN);

    while (str[i] && o < BUFLEN - 1)
    {
        uint8_t c = (uint8_t)str[i++];

        // ===============================
        // UTF-8 C3 (acentos español)
        // ===============================
        if (c == 0xC3 && str[i])
        {
            uint8_t n = (uint8_t)str[i++];

            switch (n)
            {
                case 0xA1: out[o++] = 0xA0; break; // á
                case 0xAD: out[o++] = 0xA1; break; // í
                case 0xB3: out[o++] = 0xA2; break; // ó
                case 0xBA: out[o++] = 0xA3; break; // ú
                case 0xB1: out[o++] = 0xA4; break; // ñ
                case 0x91: out[o++] = 0xA5; break; // Ñ
                default:   out[o++] = '?';  break;
            }
        }
        // ===============================
        // UTF-8 C2 (¿ ¡)
        // ===============================
        else if (c == 0xC2 && str[i])
        {
            uint8_t n = (uint8_t)str[i++];

            if (n == 0xBF) out[o++] = 0xA8; // ¿
            else if (n == 0xA1) out[o++] = 0xAD; // ¡
            else out[o++] = '?';
        }
        // ===============================
        // ASCII normal
        // ===============================
        else
        {
            if (uppercase && c >= 'a' && c <= 'z')
                c -= 32;

            out[o++] = c;
        }
    }

    out[o] = 0;
    return out;
}
#endif
#endif