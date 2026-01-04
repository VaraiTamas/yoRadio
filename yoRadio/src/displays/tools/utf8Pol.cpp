#include "Arduino.h"
#include "../../core/options.h"
#if L10N_LANGUAGE == PL
#include "../dspcore.h"
#include "utf8To.h"


// Polish chars: ąćęłńóśźż ĄĆĘŁŃÓŚŹŻ
// Execution of special characters PL, SK, DE, RO, S = A. Jaroszuk

#ifndef DSP_LCD
char* utf8To(const char* str, bool uppercase) {
  int index = 0;
  static char strn[BUFLEN];
  strlcpy(strn, str, BUFLEN); 

if(L10N_LANGUAGE==EN)  return strn;
  while (strn[index])
  { 
    bool converted = false;
    // ========================================================================
    // BLOK C5: Polski + Słowacki/Czeski
    // ========================================================================
    if (strn[index] == 0xC5)
    {
      switch (strn[index + 1]) {
        // ----------------------------------------------------------------
        // POLSKI (PL)
        // ----------------------------------------------------------------
        case 0x82: {    
			if (!uppercase){ 
			strn[index] = 0xCf;} // ł
			else {
			strn[index] = 0xD0;} // Ł
            converted = true;
            break;
        }
			case 0x81: { 
			strn[index] = 0xD0; // Ł
            converted = true;
            break;
            } 

		case 0x84: {
			if (!uppercase){ 
			strn[index] = 0xC0;} // ń
			else {
			strn[index] = 0xC1;} // Ń
            converted = true;
            break;
		}
			case 0x83: { 
			strn[index] = 0xC1; // Ń
            converted = true;
            break;
            } 
			
		case 0x9B: { 
			if (!uppercase){ 
			strn[index] = 0xCB;} // ś
			else {
			strn[index] = 0xCC;} // Ś
            converted = true;
            break;
        }
			case 0x9A: { 
			strn[index] = 0xCC; // Ś
            converted = true;
            break;
            } 
			
		case 0xBA: { 
			if (!uppercase){ 
			strn[index] = 0xBB;} // ź
			else {
			strn[index] = 0xBC;} // Ź
            converted = true;
            break;
        }
			case 0xB9: { 
			strn[index] = 0xBC; // Ź
            converted = true;
            break;
            } 

		case 0xBC: { 
			if (!uppercase){ 
			strn[index] = 0xB9;} // ż
			else {
			strn[index] = 0xBA;} // Ż
            converted = true;
            break;
        }
			case 0xBB: { 
			strn[index] = 0xBA; // Ż
            converted = true;
            break;
            } 

        // ----------------------------------------------------------------
        // SŁOWACKI / CZESKI (SK/CZ)
        // ----------------------------------------------------------------
        case 0x88: {    
			if (!uppercase){ 
			strn[index] = 0xB4;} // ň
			else {
			strn[index] = 0xB3;} // Ň
            converted = true;
            break;
        }
			case 0x87: { 
			strn[index] = 0xB3; // Ň
            converted = true;
            break;
            } 

		case 0x95: {
			if (!uppercase){ 
			strn[index] = 0xB6;} // ř
			else {
			strn[index] = 0xB5;} // Ŕ
            converted = true;
            break;
		}
			case 0x94: { 
			strn[index] = 0xB5; // Ŕ
            converted = true;
            break;
            } 
			
		case 0xA1: { 
			if (!uppercase){ 
			strn[index] = 0xC3;} // š
			else {
			strn[index] = 0xC2;} // Š
            converted = true;
            break;
        }
			case 0xA0: { 
			strn[index] = 0xC2; // Š
            converted = true;
            break;
            } 
			
		case 0xA5: { 
            // Uwaga: 0xC5 0xA5 może być zarówno ť (czeski/słowacki) lub źle zakodowane å (szwedzki)
            // Sprawdźmy kontekst lub domyślnie użyjmy ť dla zgodności
			if (!uppercase){ 
			strn[index] = 0xC6;} // ť
			else {
			strn[index] = 0xC5;} // Ť
            converted = true;
            break;
        }
			case 0xA4: { 
			strn[index] = 0xC5; // Ť
            converted = true;
            break;
            } 

		case 0xBE: { 
			if (!uppercase){ 
			strn[index] = 0xC8;} // ž
			else {
			strn[index] = 0xC7;} // Ž
            converted = true;
            break;
        }
			case 0xBD: { 
			strn[index] = 0xC7; // Ž
            converted = true;
            break;
            } 

		case 0xAE: { 
			if (!uppercase){ 
			strn[index] = 0xE8;} // ů
			else {
			strn[index] = 0x9D;} // Ů
            converted = true;
            break;
        }
		    case 0xAF: { 
			strn[index] = 0x9D; // Ů
            converted = true;
            break;
            } 
			 	
	  }
	  if (converted) {
		int sind = index + 2;
		while (strn[sind]) {
        strn[sind - 1] = strn[sind];
        sind++;
      }
    strn[sind - 1] = 0;
      }
    }

    // ========================================================================
    // BLOK C4: Polski + Słowacki/Czeski + Rumuński
    // ========================================================================
if (strn[index] == 0xC4)
    {
      converted = false;
	  switch (strn[index + 1]) {
        // ----------------------------------------------------------------
        // POLSKI (PL)
        // ----------------------------------------------------------------
		case 0x85: {
			if (!uppercase){ 
			strn[index] = 0xB8;} // ą
			else {
			strn[index] = 0xB7;} // Ą
            converted = true;
            break;
		}
			case 0x84	: { 
			strn[index] = 0xB7; // Ą
            converted = true;
            break;
            } 
		
		case 0x87: {
			if (!uppercase){ 
			strn[index] = 0xBD;} // ć
			else {
			strn[index] = 0xC4;} // Ć
            converted = true;
            break;
		}
			case 0x86: { 
			strn[index] = 0xC4; // Ć
            converted = true;
            break;
            } 

    case 0x99: {
			if (!uppercase){ 
			strn[index] = 0xD6;} // ę
			else {
			strn[index] = 0xD7;} // Ę
            converted = true;
            break;
		}
			case 0x98: { 
			strn[index] = 0xD7; // Ę
            converted = true;
            break;
            } 

        // ----------------------------------------------------------------
        // SŁOWACKI / CZESKI (SK/CZ)
        // ----------------------------------------------------------------
        case 0x8D: {
			if (!uppercase){ 
			strn[index] = 0xCA;} // č
			else {
			strn[index] = 0xC9;} // Č
            converted = true;
            break;
		}
			case 0x8C: { 
			strn[index] = 0xC9; // Č
            converted = true;
            break;
            } 

		case 0x8E: {
			if (!uppercase){ 
			strn[index] = 0xD1;} // ď
			else {
			strn[index] = 0xCE;} // Ď
            converted = true;
            break;
		}
			case 0x8F: { 
			strn[index] = 0xCE; // Ď
            converted = true;
            break;
            } 	

		case 0xBA: {
			if (!uppercase){ 
			strn[index] = 0xD3;} // ĺ
			else {
			strn[index] = 0xD2;} // Ĺ 
            converted = true;
            break;
		}
			case 0xB9: { 
			strn[index] = 0xD2; // Ĺ 
            converted = true;
            break;
            } 

		case 0xBE: {
			if (!uppercase){ 
			strn[index] = 0xD5;} // ľ
			else {
			strn[index] = 0xD4;} // Ľ 
            converted = true;
            break;
		}
			case 0xBD: { 
			strn[index] = 0xD4; // Ľ
            converted = true;
            break;
            }

        // ----------------------------------------------------------------
        // RUMUŃSKI (RO)
        // ----------------------------------------------------------------
		case 0x83: {  // ă (a with breve)
			if (!uppercase){ 
			strn[index] = 0xf4;} // ă
			else {
			strn[index] = 0xf5;} // Ă
            converted = true;
            break;
		}
		
		case 0x82: {  // Ă (A with breve)
			strn[index] = 0xf5;
            converted = true;
            break;
		}
      } 

      if (converted) {
		int sind = index + 2;
		while (strn[sind]) {
        strn[sind - 1] = strn[sind];
        sind++;
      }
      strn[sind - 1] = 0;
      }
	}

    // ========================================================================
    // BLOK C8: Rumuński (RO) - s i t z przecinkiem poniżej
    // ========================================================================
if (strn[index] == 0xC8)
    {
      converted = false;
	  switch (strn[index + 1]) {

		case 0x99: {  // ș (s with comma below)
			if (!uppercase){ 
			strn[index] = 0xef;} // ș
			else {
			strn[index] = 0xf0;} // Ș
            converted = true;
            break;
		}
		
		case 0x98: {  // Ș (S with comma below)
			strn[index] = 0xf0;
            converted = true;
            break;
		}

		case 0x9B: {  // ț (t with comma below)
			if (!uppercase){ 
			strn[index] = 0xf1;} // ț
			else {
			strn[index] = 0xf2;} // Ț
            converted = true;
            break;
		}
		
		case 0x9A: {  // Ț (T with comma below)
			strn[index] = 0xf2;
            converted = true;
            break;
		}

      } 

      if (converted) {
		int sind = index + 2;
		while (strn[sind]) {
        strn[sind - 1] = strn[sind];
        sind++;
      }
      strn[sind - 1] = 0;
      }
	}

    // ========================================================================
    // BLOK C3: Polski + Niemiecki + Słowacki + Rumuński
    // ========================================================================
if (strn[index] == 0xC3)
    {
      converted = false;
	  switch (strn[index + 1]) {

        // ----------------------------------------------------------------
        // POLSKI (PL)
        // ----------------------------------------------------------------
		case 0xB3: {
			if (!uppercase){ 
			strn[index] = 0xBE;} // ó
			else {
			strn[index] = 0xBF;} // Ó
            converted = true;
            break;
		}
			case 0x93: { 
			strn[index] = 0xBF; // Ó
            converted = true;
            break;
            } 

        // ----------------------------------------------------------------
        // NIEMIECKI (DE) + SZWEDZKI (SE)
        // ----------------------------------------------------------------
	        case 0xA4: {
		if (!uppercase){		// ä 
			strn[index] = 0x84;}
		  else {
			strn[index] = 0x8E;}	// Ä 
            converted = true;
            break;
            }
		case 0xB6: { 
		if (!uppercase){		// ö 
			strn[index] = 0x94;}
		  else {
			strn[index] = 0x99;}	// Ö 
            converted = true;
            break;
			}
		case 0xBC: {  
		if (!uppercase){		// ü 
			  strn[index] = 0x81;}
		  else {
			  strn[index] = 0x9A;}	// Ü 
            converted = true;
            break;
            }			
		case 0x84: {  			// Ä
              strn[index] = 0x8E;
              converted = true;
              break;
            }
		case 0x96: {  			// Ö
              strn[index] = 0x99;
              converted = true;
              break;
            }
		case 0x9C: {  			// Ü
              strn[index] = 0x9A;
              converted = true;
              break;
            }
		case 0x9F: {  			// ß
              strn[index] = 0xE1;
              converted = true;
              break;
            }
        case 0xA5: {                        // å (Swedish)
		if (!uppercase){					
			strn[index] = 0x86;}            // å small
		  else {
			strn[index] = 0x8F;}			// Å capital
            converted = true;
            break;
            }
		case 0x85: {  						// Å (Swedish capital)
              strn[index] = 0x8F;
              converted = true;
              break;
            }		       
         
        // ----------------------------------------------------------------
        // SŁOWACKI / CZESKI (SK/CZ)
        // ----------------------------------------------------------------
        case 0xA1: {
			if (!uppercase){ 
			strn[index] = 0xD9;} // á
			else {
			strn[index] = 0xD8;} // Á
            converted = true;
            break;
		}
			case 0x81: { 
			strn[index] = 0xD8; // Á
            converted = true;
            break;
            } 
		 
		case 0xA9: {
			if (!uppercase){ 
			strn[index] = 0x82;} // é
			else {
			strn[index] = 0x90;} // É
            converted = true;
            break;
		}
			case 0x89: { 
			strn[index] = 0x90; // É
            converted = true;
            break;
            } 

		case 0xAD: {
			if (!uppercase){ 
			strn[index] = 0xDB;} // í
			else {
			strn[index] = 0xDA;} // Í
            converted = true;
            break;
		}
			case 0x8D: { 
			strn[index] = 0xDA; // Í
            converted = true;
            break;
            } 	

		case 0xB4: {
			if (!uppercase){ 
			strn[index] = 0xDD;} // ô
			else {
			strn[index] = 0xDC;} // Ô
            converted = true;
            break;
		}
			case 0x94: { 
			strn[index] = 0xDC; // Ô
            converted = true;
            break;
            } 	

		case 0xBA: {
			if (!uppercase){ 
			strn[index] = 0xDF;} // ú
			else {
			strn[index] = 0xDE;} // Ú
            converted = true;
            break;
		}
			case 0x9A: { 
			strn[index] = 0xDE; // Ú
            converted = true;
            break;
            } 

		case 0xBD: {
			if (!uppercase){ 
			strn[index] = 0xE3;} // ý
			else {
			strn[index] = 0xE2;} // Ý
            converted = true;
            break;
		}
			case 0x9D: { 
			strn[index] = 0xE2; // Ý
            converted = true;
            break;
            } 

        // ----------------------------------------------------------------
        // RUMUŃSKI (RO)
        // ----------------------------------------------------------------
		case 0xA2: {  // â (small a with circumflex)
			if (!uppercase){ 
			strn[index] = 0x83;} // â
			else {
			strn[index] = 0xee;} // Â
            converted = true;
            break;
		}
		
		case 0x82: {  // Â (capital A with circumflex)
			strn[index] = 0xee;
            converted = true;
            break;
		}
		
		case 0xAE: {  // î (small i with circumflex)
			if (!uppercase){ 
			strn[index] = 0xed;} // î
			else {
			strn[index] = 0xf3;} // Î
            converted = true;
            break;
		}
		
		case 0x8E: {  // Î (capital I with circumflex)
			strn[index] = 0xf3;
            converted = true;
            break;
		}

          } 

      if (converted) {
		int sind = index + 2;
		while (strn[sind]) {
        strn[sind - 1] = strn[sind];
        sind++;
      }
      strn[sind - 1] = 0;
      }
	}

    index++;
  }

  // Konwertuj ASCII na wielkie litery gdy uppercase=true
  // NIE konwertuj na małe gdy uppercase=false (zachowaj oryginał)
  if (uppercase) {
    for (int i = 0; strn[i] != '\0'; i++) {
      if (strn[i] >= 'a' && strn[i] <= 'z') {
        strn[i] = toupper(strn[i]);
      }
    }
  }

return strn;
}
#endif //#ifndef DSP_LCD
#endif //#if L10N_LANGUAGE == PL