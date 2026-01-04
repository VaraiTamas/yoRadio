# Erősítő vezérlése képernyővédővel ( by Łukasz Antoszewski )

Sok yoRadio készülék beépített hang­erősítővel, egyetlen dobozban kerül megépítésre.  
Ez a fejlesztés egy dedikált GPIO kimeneten keresztül képes **vezérelni az erősítő táp- vagy relé bemenetét**.

## Hogyan működik?

- A rádió bekapcsolásakor a beállított kimeneti lábon **magas szint (HIGH)** jelenik meg  
  → az erősítő bekapcsol
- Ha a képernyővédő aktiválódik **(while not playing)** módban, a láb **alacsony szintre (LOW)** vált  
  → az erősítő kikapcsol
- A képernyővédő **(while not playing)** módban akkor is aktiválódik, ha a **hangerő 0**

Ez a megoldás különösen hasznos beépített, izolált relével vezérelt erősítős rendszereknél.     
https://www.aliexpress.com/item/1005005514531757.html

## Előnyök

- **Alacsonyabb energiafogyasztás**
- **Hosszabb hangszóró-élettartam**  
  A csendben is jelen lévő finom erősítő-zaj megszűnik, ami hosszú távon kíméli a hangszórókat.
- A készülék képes akár **24/7 óraként is működni**, zenelejátszás nélkül felesleges erősítő-üzem nélkül.

## Beállítás (`myoptions.h`)

A funkció **alapértelmezés szerint ki van kapcsolva**.  
A `myoptions.h` fájlban lehet engedélyezni az alábbi sor beírásával, ahol a vezérelni akart GPIO számát kell megadni:

```cpp
#define PWR_AMP 2
