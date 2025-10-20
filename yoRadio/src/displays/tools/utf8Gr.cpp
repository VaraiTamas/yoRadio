#include "Arduino.h"
#include "../../core/options.h"
#if L10N_LANGUAGE == GR
#include "../dspcore.h"
#include "utf8To.h"
// Polish chars: ąćęłńóśźż ĄĆĘŁŃÓŚŹŻ

char* utf8To(const char* str, bool uppercase) {
	int index = 0;
	static char strn[BUFLEN];
	strlcpy(strn, str, BUFLEN); 

	if (uppercase) { // Εναλλαγή κεφαλαίων και πεζών γραμμάτων
		for (char *iter = strn; *iter != '\0'; ++iter)
		*iter = toupper(*iter); 
	}

	if(L10N_LANGUAGE == EN) return strn;
	
	while (strn[index])
	{ 
		// Εάν το πρώτο byte χαρακτήρων UTF-8 είναι C5, βάλτε τους όλους σε αυτήν την ομάδα!
		// https://www.utf8-chartable.de/unicode-utf8-table.pl?start=256&names=-&utf8=0x
		if (strn[index] == 0xC5) 
		{
			switch (strn[index + 1]) {
			case 0x82: {    
					if (!uppercase){ 
						strn[index] = 0xCf;} // *ł
					else {
						strn[index] = 0xD0;} // *Ł
					break;
				}
			case 0x81: { 
					strn[index] = 0xD0; // *Ł
					break;
				} 
			case 0x84: {
					if (!uppercase){ 
						strn[index] = 0xC0;} // *ń
					else {
						strn[index] = 0xC1;} // *Ń
					break;
				}
			case 0x83: { 
					strn[index] = 0xC1; // *Ń
					break;
				} 			
			case 0x9B: { 
					if (!uppercase){ 
						strn[index] = 0xCB;} // *ś
					else {
						strn[index] = 0xCC;} // *Ś
					break;
				}
			case 0x9A: { 
					strn[index] = 0xCC; // *Ś
					break;
				} 				
			case 0xBA: { 
					if (!uppercase){ 
						strn[index] = 0xBB;} // *ź
					else {
						strn[index] = 0xBC;} // *Ź
					break;
				}
			case 0xB9: { 
					strn[index] = 0xBC; // *Ź
					break;
				} 
			case 0xBC: { 
					if (!uppercase){ 
						strn[index] = 0xB9;} // *ż
					else {
						strn[index] = 0xBA;} // *Ż
					break;
				}
			case 0xBB: { 
					strn[index] = 0xBA; // *Ż
					break;
				} 
			//slovakia
			case 0x88: {    
					if (!uppercase){ 
						strn[index] = 0xB4;} // *ň
					else {
						strn[index] = 0xB3;} // *Ň
					break;
				}
			case 0x87: { 
					strn[index] = 0xB3; // *Ň
					break;
				} 
			case 0x95: {
					if (!uppercase){ 
						strn[index] = 0xB6;} // *ř
					else {
						strn[index] = 0xB5;} // *Ŕ
					break;
				}
			case 0x94: { 
					strn[index] = 0xB5; // *Ŕ
					break;
				} 				
			case 0xA1: { 
					if (!uppercase){ 
						strn[index] = 0xC3;} // *š
					else {
						strn[index] = 0xC2;} // *Š
					break;
				}
			case 0xA0: { 
					strn[index] = 0xC2; // *Š
					break;
				} 				
			case 0xA5: { 
					if (!uppercase){ 
						strn[index] = 0xC6;} // *ť
					else {
						strn[index] = 0xC5;} // *Ť
					break;
				}
			case 0xA4: { 
					strn[index] = 0xC5; // *Ť
					break;
				} 
			case 0xBE: { 
					if (!uppercase){ 
						strn[index] = 0xC8;} // *ž
					else {
						strn[index] = 0xC7;} // *Ž
					break;
				}
			case 0xBD: { 
					strn[index] = 0xC7; // *Ž
					break;
				} 
			case 0xAE: { 
					if (!uppercase){ 
						strn[index] = 0xE8;} // *ů
					else {
						strn[index] = 0x9D;} // *Ů
					break;
				}
			case 0xAF: { 
					strn[index] = 0x9D; // *Ů
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

		// Εάν το πρώτο byte χαρακτήρων UTF-8 είναι C4, βάλτε τους όλους σε αυτήν την ομάδα!
		// https://www.utf8-chartable.de/unicode-utf8-table.pl?start=256&names=-&utf8=0x
		if (strn[index] == 0xC4)  
		{
			switch (strn[index + 1]) {

			case 0x85: {
					if (!uppercase){ 
						strn[index] = 0xB8;} // *ą
					else {
						strn[index] = 0xB7;} // *Ą
					break;
				}
			case 0x84	: { 
					strn[index] = 0xB7; // *Ą
					break;
				} 				
			case 0x87: {
					if (!uppercase){ 
						strn[index] = 0xBD;} // *ć
					else {
						strn[index] = 0xC4;} // *Ć
					break;
				}
			case 0x86: { 
					strn[index] = 0xC4; // *Ć
					break;
				} 
			case 0x99: {
					if (!uppercase){ 
						strn[index] = 0xD6;} // *ę
					else {
						strn[index] = 0xD7;} // *Ę
					break;
				}
			case 0x98: { 
					strn[index] = 0xD7; // *Ę
					break;
				} 
			// Slovakia chars:
			case 0x8D: {
					if (!uppercase){ 
						strn[index] = 0xCA;} // *č
					else {
						strn[index] = 0xC9;} // *Č
					break;
				}
			case 0x8C: { 
					strn[index] = 0xC9; // *Č
					break;
				} 
			case 0x8E: {
					if (!uppercase){ 
						strn[index] = 0xD1;} // *ď
					else {
						strn[index] = 0xCE;} // *Ď
					break;
				}
			case 0x8F: { 
					strn[index] = 0xCE; // *Ď
					break;
				} 	
			case 0xBA: {
					if (!uppercase){ 
						strn[index] = 0xD3;} // *ĺ
					else {
						strn[index] = 0xD2;} // *Ĺ 
					break;
				}
			case 0xB9: { 
					strn[index] = 0xD2; // *Ĺ 
					break;
				} 
			case 0xBE: {
					if (!uppercase){ 
						strn[index] = 0xD5;} // *ľ
					else {
						strn[index] = 0xD4;} // *Ľ 
					break;
				}
			case 0xBD: { 
					strn[index] = 0xD4; // *Ľ
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

		// Εάν το πρώτο byte χαρακτήρων UTF-8 είναι C3, βάλτε τους όλους σε αυτήν την ομάδα!
		// https://www.utf8-chartable.de/unicode-utf8-table.pl?start=128&number=128&names=-&utf8=0x
		if (strn[index] == 0xC3)  
		{
			switch (strn[index + 1]) {

			case 0xB3: {
					if (!uppercase){ 
						strn[index] = 0xBE;} // *ó
					else {
						strn[index] = 0xBF;} // *Ó
					break;
				}
			case 0x93: { 
					strn[index] = 0xBF; // *Ó
					break;
				} 
				// deutschland chars: äöü ÄÖÜ ß é				
			case 0xA4: {
					if (!uppercase){					// ä 
						strn[index] = 0x84;}
					else {
						strn[index] = 0x8E;}			// Ä 
					break;
				}
			case 0xB6: { 
					if (!uppercase){					// ö 
						strn[index] = 0x94;}
					else {
						strn[index] = 0x99;}			// Ö 
					break;
				}
			case 0xBC: {  
					if (!uppercase){					// ü 
						strn[index] = 0x81;}
					else {
						strn[index] = 0x9A;}			// Ü 
					break;
				}			
			case 0x84: {  						// Ä
					strn[index] = 0x8E;
					break;
				}
			case 0x96: {  						// Ö
					strn[index] = 0x99;
					break;
				}
			case 0x9C: {  						// Ü
					strn[index] = 0x9A;
					break;
				}
			case 0x9F: {  						// ß
					strn[index] = 0xE1;
					break;
				}		       				
			// Slovakia
			case 0xA1: {
					if (!uppercase){ 
						strn[index] = 0xD9;} // *á
					else {
						strn[index] = 0xD8;} // *Á
					break;
				}
			case 0x81: { 
					strn[index] = 0xD8; // *Á
					break;
				} 
			case 0xA9: {
					if (!uppercase){ 
						strn[index] = 0x82;} // *é
					else {
						strn[index] = 0x90;} // *É
					break;
				}
			case 0x89: { 
					strn[index] = 0x90; // *É
					break;
				} 
			case 0xAD: {
					if (!uppercase){ 
						strn[index] = 0xDB;} // *í
					else {
						strn[index] = 0xDA;} // *Í
					break;
				}
			case 0x8D: { 
					strn[index] = 0xDA; // *Í
					break;
				} 	
			case 0xB4: {
					if (!uppercase){ 
						strn[index] = 0xDD;} // *ô
					else {
						strn[index] = 0xDC;} // *Ô
					break;
				}
			case 0x94: { 
					strn[index] = 0xDC; // *Ô
					break;
				} 	
			case 0xBA: {
					if (!uppercase){ 
						strn[index] = 0xDF;} // *ú
					else {
						strn[index] = 0xDE;} // *Ú
					break;
				}
			case 0x9A: { 
					strn[index] = 0xDE; // *Ú
					break;
				} 
			case 0xBD: {
					if (!uppercase){ 
						strn[index] = 0xE3;} // *ý
					else {
						strn[index] = 0xE2;} // *Ý
					break;
				}
			case 0x9D: { 
					strn[index] = 0xE2; // *Ý
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

		// Εισαγάγετε τη διόρθωση εδώ για περαιτέρω γραμματοσειρές...

		// ****************************** Ελληνικά ******************************
		// Εάν το πρώτο byte χαρακτήρων UTF-8 είναι CE, βάλτε τους όλους σε αυτήν την ομάδα!
		// https://www.utf8-chartable.de/unicode-utf8-table.pl?start=768&utf8=0x
		if (strn[index] == 0xCE)  
		{
			switch (strn[index + 1]) {

			case 0x86: { //Ά
					strn[index] = 0xA2; // Ά
					break;
				}
			case 0x88: { //Έ
					strn[index] = 0xB8;
					break;
				}
			case 0x89: { //Ή
					strn[index] = 0xB9;
					break;
				}
			case 0x8A: { //Ί
					strn[index] = 0xBA;
					break;
				}
			case 0x8C: { //Ό
					strn[index] = 0xBC;
					break;
				}
			case 0x8E: { //Ύ
					strn[index] = 0xBE;
					break;
				}
			case 0x8f: { //Ώ
					strn[index] = 0xBF;
					break;
				}
			case 0x90: { //ΐ
					if (uppercase){ 
						strn[index] = 0xDA;} // Ϊ
					else {
						strn[index] = 0xC0;} // ΐ
					break;				
				}
			case 0x91: { //Α
					strn[index] = 0xC1;
					break;
				}		
			case 0x92: { //Β
					strn[index] = 0xC2;
					break;
				}		
			case 0x93: { //Γ
					strn[index] = 0xC3;
					break;
				}			
			case 0x94: { //Δ
					strn[index] = 0xC4;
					break;
				}			
			case 0x95: { //Ε
					strn[index] = 0xC5;
					break;
				}			
			case 0x96: { //Ζ
					strn[index] = 0xC6;
					break;
				}			
			case 0x97: { //Η
					strn[index] = 0xC7;
					break;
				}			
			case 0x98: { //Θ
					strn[index] = 0xC8;
					break;
				}			
			case 0x99: { //Ι
					strn[index] = 0xC9;
					break;
				}			
			case 0x9A: { //Κ
					strn[index] = 0xCA;
					break;
				}			
			case 0x9B: { //Λ
					strn[index] = 0xCB;
					break;
				}			
			case 0x9C: { //Μ
					strn[index] = 0xCC;
					break;
				}			
			case 0x9D: { //Ν
					strn[index] = 0xCD;
					break;
				}			
			case 0x9E: { //Ξ
					strn[index] = 0xCE;
					break;
				}			
			case 0x9F: { //Ο
					strn[index] = 0xCF;
					break;
				}			
			case 0xA0: { //Π
					strn[index] = 0xD0;
					break;
				}			
			case 0xA1: { //Ρ
					strn[index] = 0xD1;
					break;
				}			
			case 0xA3: { //Σ
					strn[index] = 0xD3;
					break;
				}			
			case 0xA4: { //Τ
					strn[index] = 0xD4;
					break;
				}			
			case 0xA5: { //Υ
					strn[index] = 0xD5;
					break;
				}			
			case 0xA6: { //Φ
					strn[index] = 0xD6;
					break;
				}			
			case 0xA7: { //Χ
					strn[index] = 0xD7;
					break;
				}			
			case 0xA8: { //Ψ
					strn[index] = 0xD8;
					break;
				}			
			case 0xA9: { //Ω
					strn[index] = 0xD9;
					break;
				}		
			case 0xAA: { //Ϊ
					strn[index] = 0xDA;
					break;
				}			
			case 0xAB: { //Ϋ
					strn[index] = 0xDB;
					break;
				}			
			case 0xAC: { //ά
					if (uppercase){ 
						strn[index] = 0xA2;} // Ά
					else {
						strn[index] = 0xDC;} // ά
					break;
				}				
			case 0xAD: { //έ
					if (uppercase){ 
						strn[index] = 0xB8;} // Έ
					else {
						strn[index] = 0xDD;} // έ
					break;
				}			
			case 0xAE: { //ή
					if (uppercase){ 
						strn[index] = 0xB9;} // 
					else {
						strn[index] = 0xDE;} // 
					break;
				}	
			case 0xAF: { //ί
					if (uppercase){ 
						strn[index] = 0xBA;} // 
					else {
						strn[index] = 0xDF;} // 
					break;
				}			
			case 0xB0: { //ΰ
					if (uppercase){ 
						strn[index] = 0xDB;} // 
					else {
						strn[index] = 0xE0;} // 
					break;
				}			
			case 0xB1: { //α
					if (uppercase){ 
						strn[index] = 0xC1;} // 
					else {
						strn[index] = 0xE1;} // 
					break;
				}			
			case 0xB2: { //β
					if (uppercase){ 
						strn[index] = 0xC2;} // 
					else {
						strn[index] = 0xE2;} // 
					break;
				}			
			case 0xB3: { //γ
					if (uppercase){ 
						strn[index] = 0xC3;} // 
					else {
						strn[index] = 0xE3;} // 
					break;
				}	
			case 0xB4: { //δ
					if (uppercase){ 
						strn[index] = 0xC4;} // 
					else {
						strn[index] = 0xE4;} // 
					break;
				}			
			case 0xB5: { //ε
					if (uppercase){ 
						strn[index] = 0xC5;} // 
					else {
						strn[index] = 0xE5;} // 
					break;
				}			
			case 0xB6: { //ζ
					if (uppercase){ 
						strn[index] = 0xC6;} // 
					else {
						strn[index] = 0xE6;} // 
					break;
				}	
			case 0xB7: { //η
					if (uppercase){ 
						strn[index] = 0xC7;} // 
					else {
						strn[index] = 0xE7;} // 
					break;
				}			
			case 0xB8: { //θ
					if (uppercase){ 
						strn[index] = 0xC8;} // 
					else {
						strn[index] = 0xE8;} // 
					break;
				}					
			case 0xB9: { //ι
					if (uppercase){ 
						strn[index] = 0xC9;} // 
					else {
						strn[index] = 0xE9;} // 
					break;
				}	
			case 0xBA: { //κ
					if (uppercase){ 
						strn[index] = 0xCA;} // 
					else {
						strn[index] = 0xEA;} // 
					break;
				}			
			case 0xBB: { //λ
					if (uppercase){ 
						strn[index] = 0xCB;} // 
					else {
						strn[index] = 0xEB;} // 
					break;
				}			
			case 0xBC: { //μ
					if (uppercase){ 
						strn[index] = 0xCC;} // 
					else {
						strn[index] = 0xEC;} // 
					break;
				}	
			case 0xBD: { //ν
					if (uppercase){ 
						strn[index] = 0xCD;} // 
					else {
						strn[index] = 0xED;} // 
					break;
				}			
			case 0xBE: { //ξ
					if (uppercase){ 
						strn[index] = 0xCE;} // 
					else {
						strn[index] = 0xEE;} // 
					break;
				}			
			case 0xBF: { //ο
					if (uppercase){ 
						strn[index] = 0xCF;} // 
					else {
						strn[index] = 0xEF;} // 
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
		
		// ****************************** Ελληνικά ******************************
		// Εάν το πρώτο byte χαρακτήρων UTF-8 είναι CF, βάλτε τους όλους σε αυτήν την ομάδα!
		// https://www.utf8-chartable.de/unicode-utf8-table.pl?start=768&utf8=0x
		if (strn[index] == 0xCF)  
		{
			switch (strn[index + 1]) {
		
			case 0x80: { //π
					if (uppercase){ 
						strn[index] = 0xD0;} // 
					else {
						strn[index] = 0xF0;} // 
					break;				
				}			
			case 0x81: { //ρ
					if (uppercase){ 
						strn[index] = 0xD1;} // 
					else {
						strn[index] = 0xF1;} // 
					break;				
				}			
			case 0x82: { //ς
					if (uppercase){ 
						strn[index] = 0xD3;} // 
					else {
						strn[index] = 0xF2;} // 
					break;				
				}	
			case 0x83: { //σ
					if (uppercase){ 
						strn[index] = 0xD3;} // 
					else {
						strn[index] = 0xF3;} // 
					break;				
				}			
			case 0x84: { //τ
					if (uppercase){ 
						strn[index] = 0xD4;} // 
					else {
						strn[index] = 0xF4;} // 
					break;				
				}			
			case 0x85: { //υ
					if (uppercase){ 
						strn[index] = 0xD5;} // 
					else {
						strn[index] = 0xF5;} // 
					break;				
				}	
			case 0x86: { //φ
					if (uppercase){ 
						strn[index] = 0xD6;} // 
					else {
						strn[index] = 0xF6;} // 
					break;				
				}			
			case 0x87: { //χ
					if (uppercase){ 
						strn[index] = 0xD7;} // 
					else {
						strn[index] = 0xF7;} // 
					break;				
				}			
			case 0x88: { //ψ
					if (uppercase){ 
						strn[index] = 0xD8;} // 
					else {
						strn[index] = 0xF8;} // 
					break;				
				}
			case 0x89: { //ω
					if (uppercase){ 
						strn[index] = 0xD9;} // 
					else {
						strn[index] = 0xF9;} // 
					break;				
				}			
			case 0x8A: { //ϊ
					if (uppercase){ 
						strn[index] = 0xDA;} // 
					else {
						strn[index] = 0xFA;} // 
					break;				
				}			
			case 0x8B: { //ϋ
					if (uppercase){ 
						strn[index] = 0xDB;} // 
					else {
						strn[index] = 0xFB;} // 
					break;				
				}	
			case 0x8C: { //ό
					if (uppercase){ 
						strn[index] = 0xBC;} // 
					else {
						strn[index] = 0xFC;} // 
					break;				
				}			
			case 0x8D: { //ύ
					if (uppercase){ 
						strn[index] = 0xBE;} // 
					else {
						strn[index] = 0xFD;} // 
					break;				
				}			
			case 0x8E: { //ώ
					if (uppercase){ 
						strn[index] = 0xBF;} // 
					else {
						strn[index] = 0xFE;} // 
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
