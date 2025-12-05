#include "namedays.h"

#ifdef NAMEDAYS_FILE     // "namedays"
#if NAMEDAYS_FILE == HU
  #include "../../locale/namedays/namedays_HU.h"
#elif NAMEDAYS_FILE == PL
  #include "../../locale/namedays/namedays_PL.h"
//#elif NAMEDAYS_FILE == RU
//  #include "../../locale/namedays/namedays_RU.h"
#elif NAMEDAYS_FILE == GR
  #include "../../locale/namedays/namedays_GR.h"
#elif NAMEDAYS_FILE == NL
  #include "../../locale/namedays/namedays_NL.h"
#elif NAMEDAYS_FILE == UA
  #include "../../locale/namedays/namedays_UA.h"  
#elif NAMEDAYS_FILE == DE
  #include "../../locale/namedays/namedays_DE.h"    
#else
  #error "Unsupported NAMEDAYS_FILE"
#endif

// --- Rotációs változók ---
uint32_t namedayLastRotation = 0;  // utolsó forgási idő
uint8_t  namedayCurrentIndex = 0;  // aktuális névindex
char     currentNamedayBuffer[30]; // puffer az aktuális névhez
int      lastNamedayDay = -1;      // utolsó nap a forgatás visszaállítására
int      lastNamedayMonth = -1;    // a rotáció visszaállításának utolsó hónapja

// Függvény, amely visszaadja az év egy adott napjának aktuális nevét, 4 másodpercenként váltakozva
const char *getNameDay(int month, int day) {
  const int daysInMonth[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int       dayOfYear = day - 1; // a hónap napja (0-alapú)

  for (int i = 0; i < month - 1; i++) {
    dayOfYear += daysInMonth[i];
  }

  if (dayOfYear < 0 || dayOfYear >= 366) {
    return ""; // üres karakterláncot ad vissza, ha a nap érvénytelen
  }

  // Ellenőrizd, hogy változott-e a nap - ha igen, állítsd vissza a rotációt
  if (lastNamedayDay != day || lastNamedayMonth != month) {
    lastNamedayDay = day;
    lastNamedayMonth = month;
    namedayCurrentIndex = 0;
    namedayLastRotation = millis();
    memset(currentNamedayBuffer, 0, sizeof(currentNamedayBuffer)); // napváltáskor törli a puffert
  }

  // A nap neveit tartalmazó karakterlánc lekérése
  char tempBuffer[80];
  //strcpy_P(tempBuffer, (const char *)pgm_read_ptr(&namedays[dayOfYear]));
  strcpy(tempBuffer, namedays[dayOfYear]);

  //  Serial.printf("displayILI9488.cpp -> A kiolvasott névnapok: %s%\n", tempBuffer);
  // Megszámoljuk a nevek számát a karakterláncban (vesszővel elválasztva)
  uint8_t nameCount = 1;
  for (int i = 0; tempBuffer[i] != '\0'; i++) {
    if (tempBuffer[i] == ',')
      nameCount++;
  }

  // Ha csak egy név van, ne cseréld le
  if (nameCount == 1) {
    memset(currentNamedayBuffer, 0, sizeof(currentNamedayBuffer)); // ürítsd ki a puffert
    strlcpy(currentNamedayBuffer, tempBuffer, sizeof(currentNamedayBuffer));
    return currentNamedayBuffer;
  }

  // Ellenőrzi, hogy eltelt-e 4 másodperc az utolsó forgatás óta
  uint32_t currentTime = millis();
  if (currentTime - namedayLastRotation >= 4000) { // 4 másodperc
    namedayLastRotation = currentTime;
    namedayCurrentIndex++;

    // Index visszaállítása, ha meghaladja a nevek számát
    if (namedayCurrentIndex >= nameCount) {
      namedayCurrentIndex = 0;
    }
  }

  // Keresd meg a megfelelő nevet a listából
  //strcpy_P(tempBuffer, (const char *)pgm_read_ptr(&namedays[dayOfYear])); // Másolja újra, mert az strtok megsemmisíti
 strcpy(tempBuffer, namedays[dayOfYear]);

  char *token = strtok(tempBuffer, ",");
  for (uint8_t i = 0; i < namedayCurrentIndex && token != NULL; i++) {
    token = strtok(NULL, ",");
  }

  if (token) {
    // Töröljük el a szóközöket az elejéről
    while (*token == ' ' || *token == '\t')
      token++;

    memset(currentNamedayBuffer, 0, sizeof(currentNamedayBuffer)); // puffer ürítése írás előtt
    strlcpy(currentNamedayBuffer, token, sizeof(currentNamedayBuffer));
    return currentNamedayBuffer;
  }

  return "";
}

#endif // NAMEDAY_ENABLED
