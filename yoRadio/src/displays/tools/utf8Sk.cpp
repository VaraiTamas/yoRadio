#include "Arduino.h"
#include "../../core/options.h"
#if L10N_LANGUAGE == SK
  #include "../dspcore.h"
  #include "utf8To.h"

// ============================================================================
// Javítja a Shoutcast / Icecast által hibásan kódolt SZLOVÁK / LENGYEL UTF-8-at
//   – csak hibás karaktereket javít
//   – helyes UTF-8-hoz nem nyúl
//   – más nyelveket NEM érint
// ============================================================================
String fixSlovakUTF8(const String &in)
{
    String s = in;

    // ==========================
    // Általános UTF8 → latin javítás
    // ==========================
    s.replace("Ã¡", "á");   s.replace("Ã�", "Á");
    s.replace("Ã©", "é");   s.replace("Ã‰", "É");
    s.replace("Ãí", "í");   s.replace("Ã�", "Í");
    s.replace("Ã³", "ó");   s.replace("Ã“", "Ó");
    s.replace("Ãº", "ú");   s.replace("Ãš", "Ú");
    s.replace("Ã¼", "ü");   s.replace("Ãœ", "Ü");
    s.replace("Ã¶", "ö");   s.replace("Ã–", "Ö");

    // ==========================
    // Szlovák/Lengyel alap hibák
    // ==========================

    // ý / Ý
    s.replace("Ã½", "ý");
    s.replace("Ã�", "Ý");

    // ž / Ž
    s.replace("Å¾", "ž");
    s.replace("Å½", "Ž");

    // š / Š — sokféle torz forma RusynFM-nél
    s.replace("Å¡", "š");       // kis
    s.replace("Å ", "Š");       // nagy Š torzítva (C3 85 20)
    s.replace("Å\xa0", "Š");    // NBSP variáns
    s.replace("Ã… ", "Š");      // dupla kódolás
    s.replace("Ã…", "Š");

    // ÚJ: Ši speciális hibás minták
    s.replace("Åi", "Ši");
    s.replace("Å i", "Ši");

    // ť / Ť
    s.replace("Å¥", "ť");
    s.replace("Å¤", "Ť");

    // ľ / Ľ
    s.replace("Ä¾", "ľ");
    s.replace("Ä½", "Ľ");

    // č / Č — több fajta torz formával
    s.replace("Ä�", "č");            // alap hibás forma
    s.replace("Ä\u008D", "č");        // C4 8D
    s.replace("Ã„\u008D", "č");        // dupla hibás
    s.replace("Äč", "č");             // fallback
    s.replace("ÄŒ", "Č");             // hibás nagy Č
    s.replace("\xC4\x8C", "Č");       // tiszta UTF-8 Č is támogatott

    // ň / Ň
    s.replace("Åˆ", "ň");
    s.replace("Å‡", "Ň");

    // ď / Ď
    s.replace("Ä?", "ď");            // gyakori torzítás RusynFM-en
    s.replace("ÄŽ", "Ď");

    // ==========================
    // Rusyn / Szlovák speciális torzulások
    // ==========================

    // buÄka → bučka
    s.replace("Äa", "ča");           // régi torz forma
    s.replace("Ã„a", "ča");          // dupla konverzió
    s.replace("Ä\u008Dka", "čka");    // C4 8D + ka
    s.replace("Ä\u008D", "č");        // önmagában č

    // combining caron törlése (ˇ)
    s.replace("ˇ", "");
    s.replace("\xCC\x8C", "");        // combining caron
    s.replace("\xCB\x86", "");        // másik forma

    return s;
}



// ============================================================================
//  UTF-8 → GLCD belső kódtábla
//  (A TELJES ORIGINÁLIS FÁJL VÁLTOZATLANUL MEGTARTVA!)
// ============================================================================

char *utf8To(const char *str, bool uppercase) {

  // ← ÚJ: először kijavítjuk a hibás UTF-8 karaktereket
  String fixed = fixSlovakUTF8(str);

  static char strn[BUFLEN];
  strlcpy(strn, fixed.c_str(), BUFLEN);
  
  int index = 0;

  if (uppercase) {
    for (char *iter = strn; *iter != '\0'; ++iter) {
      *iter = toupper(*iter);
    }
  }

  //Serial.println();
  //hexDump("fixSlovakUTF8 után:", strn);

  if (L10N_LANGUAGE == EN) {
    return strn;
  }

  while (strn[index]) {
    // ==== C5 blokk ======================================================
    if (strn[index] == 0xC5) {
      switch (strn[index + 1]) {

        // -------- Lengyel ł / Ł --------
        case 0x82: strn[index] = uppercase ? 0xD0 : 0xCf; break;
        case 0x81: strn[index] = 0xD0; break;

        // -------- ń / Ń --------
        case 0x84: strn[index] = uppercase ? 0xC1 : 0xC0; break;
        case 0x83: strn[index] = 0xC1; break;

        // -------- ś / Ś --------
        case 0x9B: strn[index] = uppercase ? 0xCC : 0xCB; break;
        case 0x9A: strn[index] = 0xCC; break;

        // -------- ź / Ź --------
        case 0xBA: strn[index] = uppercase ? 0xBC : 0xBB; break;
        case 0xB9: strn[index] = 0xBC; break;

        // -------- ż / Ż --------
        case 0xBC: strn[index] = uppercase ? 0xBA : 0xB9; break;
        case 0xBB: strn[index] = 0xBA; break;

        // -------- Szlovák: ň / Ň --------
        case 0x88: strn[index] = uppercase ? 0xB3 : 0xB4; break;
        case 0x87: strn[index] = 0xB3; break;

        // -------- ř / Ř --------
        case 0x95: strn[index] = uppercase ? 0xB5 : 0xB6; break;
        case 0x94: strn[index] = 0xB5; break;

        // -------- š / Š --------
        case 0xA1: strn[index] = uppercase ? 0xC2 : 0xC3; break;
        case 0xA0: strn[index] = 0xC2; break;

        // -------- ť / Ť --------
        case 0xA5: strn[index] = uppercase ? 0xC5 : 0xC6; break;
        case 0xA4: strn[index] = 0xC5; break;

        // -------- ž / Ž --------
        case 0xBE: strn[index] = uppercase ? 0xC7 : 0xC8; break;
        case 0xBD: strn[index] = 0xC7; break;

        // -------- ů / Ů --------
        case 0xAE: strn[index] = uppercase ? 0x9D : 0xE8; break;
        case 0xAF: strn[index] = 0x9D; break;
      }

      // törli a második UTF-8 byte-ot
      int sind = index + 2;
      while (strn[sind]) {
        strn[sind - 1] = strn[sind];
        sind++;
      }
      strn[sind - 1] = 0;
    }

    // ==== C4 blokk ======================================================
    else if (strn[index] == 0xC4) {
      switch (strn[index + 1]) {

        // -------- ą / Ą --------
        case 0x85: strn[index] = uppercase ? 0xB7 : 0xB8; break;
        case 0x84: strn[index] = 0xB7; break;

        // -------- ć / Ć --------
        case 0x87: strn[index] = uppercase ? 0xC4 : 0xBD; break;
        case 0x86: strn[index] = 0xC4; break;

        // -------- ę / Ę --------
        case 0x99: strn[index] = uppercase ? 0xD7 : 0xD6; break;
        case 0x98: strn[index] = 0xD7; break;

        // -------- č / Č --------
        case 0x8D: strn[index] = uppercase ? 0xC9 : 0xCA; break;
        case 0x8C: strn[index] = 0xC9; break;

        // -------- ď / Ď --------
        case 0x8E: strn[index] = uppercase ? 0xCE : 0xD1; break;
        case 0x8F: strn[index] = 0xCE; break;

        // -------- ĺ / Ĺ --------
        case 0xBA: strn[index] = uppercase ? 0xD2 : 0xD3; break;
        case 0xB9: strn[index] = 0xD2; break;

        // -------- ľ / Ľ --------
        case 0xBE: strn[index] = uppercase ? 0xD4 : 0xD5; break;
        case 0xBD: strn[index] = 0xD4; break;
      }

      int sind = index + 2;
      while (strn[sind]) {
        strn[sind - 1] = strn[sind];
        sind++;
      }
      strn[sind - 1] = 0;
    }

    // ==== C3 blokk ======================================================
    else if (strn[index] == 0xC3) {
      switch (strn[index + 1]) {

        // -------- ó / Ó --------
        case 0xB3: strn[index] = uppercase ? 0xBF : 0xBE; break;
        case 0x93: strn[index] = 0xBF; break;

        // -------- ä, ö, ü + nagybetűk --------
        case 0xA4: strn[index] = uppercase ? 0x8E : 0x84; break;
        case 0x96: strn[index] = 0x99; break;
        case 0xB6: strn[index] = uppercase ? 0x99 : 0x94; break;
        case 0x9C: strn[index] = 0x9A; break;
        case 0xBC: strn[index] = uppercase ? 0x9A : 0x81; break;

        // -------- ß --------
        case 0x9F: strn[index] = 0xE1; break;

        // -------- SK: á / Á --------
        case 0xA1: strn[index] = uppercase ? 0xD8 : 0xD9; break;
        case 0x81: strn[index] = 0xD8; break;

        // -------- é / É --------
        case 0xA9: strn[index] = uppercase ? 0x90 : 0x82; break;
        case 0x89: strn[index] = 0x90; break;

        // -------- í / Í --------
        case 0xAD: strn[index] = uppercase ? 0xDA : 0xDB; break;
        case 0x8D: strn[index] = 0xDA; break;

        // -------- ô / Ô --------
        case 0xB4: strn[index] = uppercase ? 0xDC : 0xDD; break;
        case 0x94: strn[index] = 0xDC; break;

        // -------- ú / Ú --------
        case 0xBA: strn[index] = uppercase ? 0xDE : 0xDF; break;
        case 0x9A: strn[index] = 0xDE; break;

        // -------- ý / Ý --------
        case 0xBD: strn[index] = uppercase ? 0xE2 : 0xE3; break;
        case 0x9D: strn[index] = 0xE2; break;
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
