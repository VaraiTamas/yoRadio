#include "Arduino.h"
#include "../../core/options.h"
#if L10N_LANGUAGE == PL
#include "../dspcore.h"
#include "utf8To.h"


// Polish chars: ąćęłńóśźż ĄĆĘŁŃÓŚŹŻ

// ============================================================================
// Naprawia zniekształcone znaki UTF-8 z Shoutcast / Icecast
//   – naprawia tylko błędnie zakodowane znaki
//   – nie modyfikuje prawidłowych znaków UTF-8
//   – obsługuje polskie i słowackie znaki
// ============================================================================
String fixSlovakUTF8(const String &in)
{
    String s = in;

    // ==========================
    // Ogólne naprawy UTF8 → latin
    // ==========================
    s.replace("Ã¡", "á");   s.replace("Ã\x81", "Á");
    s.replace("Ã©", "é");   s.replace("Ã‰", "É");
    s.replace("Ãí", "í");   s.replace("Ã\x8D", "Í");
    s.replace("Ã³", "ó");   s.replace("Ã\x93", "Ó");
    s.replace("Ãº", "ú");   s.replace("Ãš", "Ú");
    s.replace("Ã¼", "ü");   s.replace("Ãœ", "Ü");
    s.replace("Ã¶", "ö");   s.replace("Ã–", "Ö");
    
    // Swedish characters
    s.replace("Ã¥", "å");   s.replace("Ã…", "Å");
    s.replace("Ã¤", "ä");   s.replace("Ã„", "Ä");
    

    // ==========================
    // Polskie/Słowackie podstawowe błędy
    // ==========================

    // ý / Ý
    s.replace("Ã½", "ý");
    s.replace("Ã ", "Ý");

    // ž / Ž
    s.replace("Å¾", "ž");
    s.replace("Å½", "Ž");

    // š / Š — różne zniekształcone formy
    s.replace("Å¡", "š");       // małe
    s.replace("Å ", "Š");       // wielkie Š zniekształcone (C3 85 20)
    s.replace("Å\xa0", "Š");    // wariant NBSP
    s.replace("Ã… ", "Š");      // podwójne kodowanie
    s.replace("Ã…", "Š");

    // Specjalne zniekształcone wzorce Ši
    s.replace("Åi", "Ši");
    s.replace("Å i", "Ši");

    // ť / Ť
    s.replace("Å¥", "ť");
    s.replace("Å¤", "Ť");

    // ľ / Ľ
    s.replace("Ä¾", "ľ");
    s.replace("Ä½", "Ľ");

    // č / Č — różne zniekształcone formy
    s.replace("Ä ", "č");            // podstawowa błędna forma
    s.replace("Ä\u008D", "č");       // C4 8D
    s.replace("Ã„\u008D", "č");      // podwójne błędne
    s.replace("Äč", "č");            // fallback
    s.replace("ÄŒ", "Č");            // błędne wielkie Č
    s.replace("\xC4\x8C", "Č");      // czyste UTF-8 Č też obsługiwane

    // ň / Ň
    s.replace("Åˆ", "ň");
    s.replace("Å‡", "Ň");

    // ď / Ď
    s.replace("Ä?", "ď");            // częste zniekształcenie
    s.replace("ÄŽ", "Ď");

    // ==========================
    // Polskie specjalne zniekształcenia
    // ==========================

    // ą / Ą
    s.replace("Ä…", "ą");
    s.replace("Ä„", "Ą");

    // ć / Ć
    s.replace("Ä‡", "ć");
    s.replace("Ä†", "Ć");

    // ę / Ę
    s.replace("Ä™", "ę");
    s.replace("Ä˜", "Ę");

    // ł / Ł
    s.replace("Å‚", "ł");
    s.replace("Å ", "Ł");

    // ń / Ń
    s.replace("Å„", "ń");
    s.replace("Åƒ", "Ń");

    // ś / Ś
    s.replace("Å›", "ś");
    s.replace("Åš", "Ś");

    // ź / Ź
    s.replace("Åº", "ź");
    s.replace("Å¹", "Ź");

    // ż / Ż
    s.replace("Å¼", "ż");
    s.replace("Å»", "Ż");

    // ==========================
    // Rusyn / Słowackie specjalne zniekształcenia
    // ==========================

    // buÄka → bučka
    s.replace("Äa", "ča");           // stara zniekształcona forma
    s.replace("Ã„a", "ča");          // podwójna konwersja
    s.replace("Ä\u008Dka", "čka");   // C4 8D + ka
    s.replace("Ä\u008D", "č");       // samodzielne č

    // usuwanie combining caron (ˇ)
    s.replace("ˇ", "");
    s.replace("\xCC\x8C", "");       // combining caron
    s.replace("\xCB\x86", "");       // inna forma

    return s;
}

#ifndef DSP_LCD
char* utf8To(const char* str, bool uppercase) {
  // ← NOWE: najpierw naprawiamy zniekształcone znaki UTF-8
  String fixed = fixSlovakUTF8(str);

  int index = 0;
  static char strn[BUFLEN];
  strlcpy(strn, fixed.c_str(), BUFLEN); 

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

// Wstaw tutaj swoją korektę na dalsze czcionki...

// Konwersja pojedynczych bajtów ISO-8859-1/Windows-1252 (nie-UTF-8)
// dla znaków szwedzkich å/Å, ä/Ä, ö/Ö
    if ((unsigned char)strn[index] == 0xE5) {  // å (ISO-8859-1)
      strn[index] = 0x86;
    } else if ((unsigned char)strn[index] == 0xC5) {  // Å (ISO-8859-1)
      strn[index] = 0x8F;
    } else if ((unsigned char)strn[index] == 0xE4) {  // ä (ISO-8859-1)
      strn[index] = 0x84;
    } else if ((unsigned char)strn[index] == 0xC4) {  // Ä (ISO-8859-1)
      strn[index] = 0x8E;
    } else if ((unsigned char)strn[index] == 0xF6) {  // ö (ISO-8859-1)
      strn[index] = 0x94;
    } else if ((unsigned char)strn[index] == 0xD6) {  // Ö (ISO-8859-1)
      strn[index] = 0x99;
    }

    index++;
  }

  // Normalizuj wielkość liter ASCII (po konwersji UTF-8)
  for (int i = 0; strn[i] != '\0'; i++) {
    // Konwertuj tylko znaki ASCII a-z lub A-Z, nie dotykaj zakodowanych znaków specjalnych
    if (uppercase) {
      if (strn[i] >= 'a' && strn[i] <= 'z') {
        strn[i] = toupper(strn[i]);
      }
    } else {
      if (strn[i] >= 'A' && strn[i] <= 'Z') {
        strn[i] = tolower(strn[i]);
      }
    }
  }

return strn;
}
#endif //#ifndef DSP_LCD
#endif //#if L10N_LANGUAGE == PL