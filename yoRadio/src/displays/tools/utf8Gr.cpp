#include "Arduino.h"
#include "../../core/options.h"
#if L10N_LANGUAGE == GR
#include "../dspcore.h"
#include "utf8To.h"
/*
 * This code is to show Greek characters on screens that use Yoradio and not only. 
 * It is for the adafruit GFX library.
 * Created by Arkas and corrected by Gregory Smiaris
 * utf8RusGFX.h — UTF-8 → Windows-1253 Greek converter for 5x7 GLCD font
 * Corrected: handles uppercase/lowercase and accented/diaeresis Greek properly by Gregory Smiaris SV2RR 28 Oct 2025.
 */

#include <string.h>
#include <ctype.h>

// Ensure this matches your class signature. The original used DspCore::utf8Rus.
// If you don't compile inside that class, remove 'DspCore::' and use plain function.
char* utf8To(const char* str, bool uppercase)
{
    static char outbuf[BUFLEN];
    if (!str) {
        outbuf[0] = '\0';
        return outbuf;
    }

    size_t in_len = strlen(str);
    if (in_len >= BUFLEN) in_len = BUFLEN - 1;

    size_t i = 0;    // input index
    size_t o = 0;    // output index

    while (i < in_len && o < BUFLEN - 1) {
        unsigned char c1 = (unsigned char)str[i];

        // ASCII (including punctuation) — copy through
        if (c1 < 0xC0) {
            outbuf[o++] = (char)c1;
            i++;
            continue;
        }

        // Need at least one continuation byte for the 2-byte sequences we handle
        if (i + 1 >= in_len) {
            // Broken UTF-8 at end: copy byte as-is
            outbuf[o++] = (char)c1;
            i++;
            continue;
        }

        unsigned char c2 = (unsigned char)str[i + 1];
        unsigned char mapped = 0;
        
// Inside CE group mapping:
if (c1 == 0xCE) {
    switch (c2) {
        // Accented uppercase letters mapped to unaccented uppercase
        case 0x86: mapped = 0xC1; break; // Ά -> Α
        case 0x88: mapped = 0xC5; break; // Έ -> Ε
        case 0x89: mapped = 0xC7; break; // Ή -> Η
        case 0x8A: mapped = 0xC9; break; // Ί -> Ι
        case 0x8C: mapped = 0xCF; break; // Ό -> Ο
        case 0x8E: mapped = 0xD5; break; // Ύ -> Υ
        case 0x8F: mapped = 0xD9; break; // Ώ -> Ω
        case 0x90: mapped = 0xC9; break; // ΐ -> Ι
        	
        // Uppercase Α..Ο
        case 0x91: mapped = 0xC1; break; // Α
        case 0x92: mapped = 0xC2; break; // Β
        case 0x93: mapped = 0xC3; break; // Γ
        case 0x94: mapped = 0xC4; break; // Δ
        case 0x95: mapped = 0xC5; break; // Ε
        case 0x96: mapped = 0xC6; break; // Ζ
        case 0x97: mapped = 0xC7; break; // Η
        case 0x98: mapped = 0xC8; break; // Θ
        case 0x99: mapped = 0xC9; break; // Ι
        case 0x9A: mapped = 0xCA; break; // Κ
        case 0x9B: mapped = 0xCB; break; // Λ
        case 0x9C: mapped = 0xCC; break; // Μ
        case 0x9D: mapped = 0xCD; break; // Ν
        case 0x9E: mapped = 0xCE; break; // Ξ
        case 0x9F: mapped = 0xCF; break; // Ο
       
        
        // Uppercase Π..Φ..Ω
        case 0xA0: mapped = 0xD0; break; // Π
        case 0xA1: mapped = 0xD1; break; // Ρ
        case 0xA3: mapped = 0xD3; break; // Σ
        case 0xA4: mapped = 0xD4; break; // Τ
        case 0xA5: mapped = 0xD5; break; // Υ
        case 0xA6: mapped = 0xD6; break; // Φ
        case 0xA7: mapped = 0xD7; break; // Χ
        case 0xA8: mapped = 0xD8; break; // Ψ
        case 0xA9: mapped = 0xD9; break; // Ω
        case 0xAA: mapped = 0xC9; break; // Ϊ -> Ι
        case 0xAB: mapped = 0xD5; break; // Ϋ -> Υ
        	
        // Lowercase accented letters mapped to unaccented uppercase
        case 0xAC: mapped = 0xC1; break; // ά -> Α
        case 0xAD: mapped = 0xC5; break; // έ -> Ε
        case 0xAE: mapped = 0xC7; break; // ή -> Η
        case 0xAF: mapped = 0xC9; break; // ί -> Ι
        case 0xB1: mapped = 0xC1; break; // α -> Α
        case 0x85: mapped = 0xD5; break; // υ -> Υ
        case 0xBF: mapped = 0xCF; break; // ο -> Ο
        case 0xB5: mapped = 0xC5; break; // ε (force uppercase)

        // Other lowercase letters mapped to uppercase equivalents
        case 0xB2: mapped = 0xC2; break; // β
        case 0xB3: mapped = 0xC3; break; // γ
        case 0xB4: mapped = 0xC4; break; // δ
        case 0xB6: mapped = 0xC6; break; // ζ
        case 0xB7: mapped = 0xC7; break; // η
        case 0xB8: mapped = 0xC8; break; // θ
        case 0xB9: mapped = 0xC9; break; // ι
        case 0xBA: mapped = 0xCA; break; // κ
        case 0xBB: mapped = 0xCB; break; // λ
        case 0xBC: mapped = 0xCC; break; // μ
        case 0xBD: mapped = 0xCD; break; // ν
        case 0xBE: mapped = 0xCE; break; // ξ
                default: mapped = 0; break;
    }
}


// Add CF group Greek letter mappings here (including final sigma and accented versions)
if (c1 == 0xCF) {
    switch (c2) {
        case 0x80: mapped = 0xD0; break; // π
        case 0x81: mapped = 0xD1; break; // ρ
        case 0x82: mapped = 0xD3; break; // ς (final sigma)
        case 0x83: mapped = 0xD3; break; // σ
        case 0x84: mapped = 0xD4; break; // τ
        case 0x85: mapped = 0xD5; break; // υ
        case 0x86: mapped = 0xD6; break; // φ
        case 0x87: mapped = 0xD7; break; // χ
        case 0x88: mapped = 0xD8; break; // ψ
        case 0x89: mapped = 0xD9; break; // ω
        case 0x8A: mapped = 0xC9; break; // ϊ -> Ι
        case 0x8B: mapped = 0xD5; break; // ϋ -> Υ
        case 0x8C: mapped = 0xCF; break; // ό -> Ο
        case 0x8D: mapped = 0xD5; break; // ύ -> Υ
        case 0x8E: mapped = 0xD9; break; // ώ -> Ω
        default: mapped = 0; break;
    }
}
  // If we've found a mapping, consume two input bytes and write one output byte
        if (mapped) {
            outbuf[o++] = (char)mapped;
            i += 2;
            continue;
        }

        // If no mapping, copy first byte (this keeps punctuation/unknown bytes)
        outbuf[o++] = (char)c1;
        i++;
    }

    // Null-terminate
    outbuf[o] = '\0';

    // Optional uppercase transform AFTER mapping (so we uppercase mapped 8-bit codes)
    if (uppercase) {
        for (size_t k = 0; k < o; ++k) {
            outbuf[k] = (char)toupper((unsigned char)outbuf[k]);
        }
    }

    return outbuf;
}
#endif