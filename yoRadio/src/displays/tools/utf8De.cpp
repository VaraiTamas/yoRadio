#include <Arduino.h>
#include "../../core/options.h"
#if L10N_LANGUAGE == DE
#include "../dspcore.h"
#include "utf8To.h"

#ifndef BUFLEN
#define BUFLEN 512
#endif

// ======================================================================
//  COMMON UTF8 → LCD (Nextion) MAPPING
//  HU + DE egyben
// ======================================================================

#ifndef DSP_LCD

char* utf8To(const char* str, bool uppercase)
{
    static char out[BUFLEN];
    strlcpy(out, str, BUFLEN);

    // HIÁNYZÓ RÉSZ – ASCII nagybetűsítés
    if (uppercase) {
        for (char *p = out; *p; p++)
            *p = toupper(*p);
    }

    int index = 0;

    while (out[index])
    {
        uint8_t b1 = (uint8_t)out[index];

        // ==================================================================
        // UTF-8 2-bytes:  C3 xx  → magyar + német betűk nagy része
        // ==================================================================
        if (b1 == 0xC3)
        {
            uint8_t b2 = (uint8_t)out[index + 1];

            switch (b2)
            {
                //
                // --- NÉMET ÉS MAGYAR KÖZÖS (C3 A4 / A9 / B6 / BC / stb.) ---
                //
                case 0xA4: out[index] = uppercase ? 0x8E : 0x84; break;  // ä / Ä
                case 0xB6: out[index] = uppercase ? 0x99 : 0x94; break;  // ö / Ö
                case 0xBC: out[index] = uppercase ? 0x9A : 0x81; break;  // ü / Ü
                case 0x9F: out[index] = 0xE1;            break;        // ß
                case 0x84: out[index] = 0x8E;            break;        // Ä
                case 0x96: out[index] = 0x99;            break;        // Ö
                case 0x9C: out[index] = 0x9A;            break;        // Ü

                //
                // --- MAGYAR LATIN-1 KITERJESZTÉSEK ---
                //
                case 0xA1: out[index] = uppercase ? 0x8F : 0x86; break; // á / Á
                case 0xAD: out[index] = uppercase ? 0x8D : 0x8C; break; // í / Í
                case 0xA9: out[index] = uppercase ? 0x90 : 0x82; break; // é / É
                case 0xB3: out[index] = uppercase ? 0x95 : 0x96; break; // ó / Ó
                case 0xBA: out[index] = uppercase ? 0x9C : 0x97; break; // ú / Ú
                //case 0xB6: /* már kezeltük németnél */ break;

                default:
                    break;
            }

            // UTF-8 második byte eltávolítása
            int j = index + 2;
            while (out[j]) { out[j - 1] = out[j]; j++; }
            out[j - 1] = 0;

            index++;
            continue;
        }

        // ==================================================================
        // UTF-8 2-bytes:  C5 xx  → magyar hosszú ő / ű + nagybetűk
        // ==================================================================
        if (b1 == 0xC5)
        {
            uint8_t b2 = (uint8_t)out[index + 1];

            switch (b2)
            {
                case 0x91: out[index] = 0x93; break;  // ő
                case 0x90: out[index] = 0x9B; break;  // Ő
                case 0xB1: out[index] = 0x8B; break;  // ű
                case 0xB0: out[index] = 0x98; break;  // Ű
                default:
                    index++;
                    continue;
            }

            int j = index + 2;
            while (out[j]) { out[j - 1] = out[j]; j++; }
            out[j - 1] = 0;

            index++;
            continue;
        }

        index++;
    }

    return out;
}

#else
// ======================================================================
//  NON-LCD BRANCH → UTF-8 passthrough (HU + DE működik változatlanul)
// ======================================================================
char* utf8To(const char* str, bool uppercase)
{
    static char out[BUFLEN];
    strlcpy(out, str, BUFLEN);

    if (uppercase)
        for (char* p = out; *p; p++)
            *p = toupper(*p);

    return out;
}
#endif

#endif // UTF8TO_COMMON_H

