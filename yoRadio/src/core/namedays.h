#ifndef NAMEDAYS_H
#define NAMEDAYS_H

#include "options.h"

// Csak akkor fordul, ha van NAMEDAYS_FILE definiálva
#ifdef NAMEDAYS_FILE

#include <Arduino.h>

// Tömb deklaráció
extern const char* namedays[];

// Label deklaráció
extern const char* nameday_label;

// Névnap-rotáció változói
extern uint32_t namedayLastRotation;      // utolsó forgási idő
extern uint8_t  namedayCurrentIndex;      // aktuális névindex
extern char     currentNamedayBuffer[30]; // puffer az aktuális névhez
extern int      lastNamedayDay;           // utolsó nap a forgatás visszaállítására
extern int      lastNamedayMonth;         // a rotáció visszaállításának utolsó hónapja

// Függvény, amely visszaadja az év egy adott napjának aktuális nevét, 4 másodpercenként váltakozva
const char *getNameDay(int month, int day);

#endif // NAMEDAYS_FILE

#endif // NAMEDAYS_H
