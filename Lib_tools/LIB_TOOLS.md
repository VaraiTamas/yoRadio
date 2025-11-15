# ESP32-S3 – Javított hálózati könyvtárak használata magas bitrátájú internetes rádiókhoz

Ha azt tapasztalod, hogy az ESP32-S3 alapú projekted (pl. yoRadio) bizonyos **magas bitrátájú** internetes rádióállomásokat (pl. 256–320 kbps MP3, AAC, FLAC stream) **szakadozva** játszik le, akkor a hiba nem feltétlenül a WiFi hálózatodban van.

A probléma gyakran az Arduino által használt **előre fordított ESP-IDF könyvtárakban** keresendő.

_Forrás: 4PDA – javított ESP-IDF könyvtárak_  
https://4pda.to/forum/index.php?showtopic=1010378&st=16700#entry132067099

---

## Miért történik a szakadás?

Az Arduino ESP32 Core valójában nem a „normál” ESP-IDF-et fordítja újra projektenként. Ehelyett **előre lefordított statikus .a könyvtárakat** használ, például:

- `libesp_netif.a`
- `liblwip.a`

Ezek **fix**, alapértelmezett konfigurációval készülnek, és **nem módosíthatók** Arduino IDE alól.

Ennek következményei:
- kisebb hálózati bufferek,
- kevésbé optimális LWIP beállítások,
- rosszabb csomagkezelés nagy adatfolyam esetén.

A közösség készített olyan ESP-IDF fordítást, amely javított hálózati beállításokkal rendelkezik. Ezekkel a `.a` fájlokkal **lényegesen stabilabban játszhatók le** a magas bitrátájú adások.

---

## Hol találhatók az Arduino által használt ESP-IDF könyvtárak?

Windows alatt:  
    - Arduino ESP32 Core 3.3.0 esetén   
    C:\Users<FELHASZNÁLÓNÉV>\AppData\Local\Arduino15\packages\esp32\tools\esp32-arduino-libs\idf-release_v5.5-b66b5448-v1\esp32s3\lib\  
    - Arduino ESP32 Core 3.3.3 esetén   
    C:\Users<FELHASZNÁLÓNÉV>\AppData\Local\Arduino15\packages\esp32\tools\esp32-arduino-libs\idf-release_v5.5-f1a1df9b-v3\esp32s3\lib 


Itt többek között megtalálod a következőket:

- `libesp_netif.a`
- `liblwip.a`

Ezeket kell lecserélni a javított változatokra.

---

## Mit javít a fájlcsere?

### ✔ Stabilabb streaming magas bitrátán  
A nagyobb TCP bufferek és optimalizált LWIP beállítások miatt.

### ✔ Kevesebb csomagvesztés  
Főleg lassabb routerek vagy ingadozó hálózat esetén.

### ✔ Megszűnik (vagy minimálisra csökken) a szakadozás  
FLAC, 320 kbps MP3 és erős tömörítésű AAC streameknél is.

### ✔ Nem szükséges az egész ESP-IDF újrafordítása  
Csak a két statikus könyvtár cseréje szükséges → kompatibilis marad az Arduino környezet.

---

## Fájlok cseréje – lépésről lépésre

⚠️ Fontos:
A javított libesp_netif.a és liblwip.a fájlok kifejezetten az Arduino ESP32 Core 3.3.0 vagy 3.3.3 verziójához készültek.
Más verziókkal nem kompatibilisek, és hibát vagy instabilitást okozhatnak.

1. **Zárd be az Arduino IDE-t.**
2. Navigálj ide (Windows), 
    - Arduino ESP32 Core 3.3.0 esetén   
    C:\Users<FELHASZNÁLÓNÉV>\AppData\Local\Arduino15\packages\esp32\tools\esp32-arduino-libs\idf-release_v5.5-b66b5448-v1\esp32s3\lib\  
    - Arduino ESP32 Core 3.3.3 esetén   
    C:\Users<FELHASZNÁLÓNÉV>\AppData\Local\Arduino15\packages\esp32\tools\esp32-arduino-libs\idf-release_v5.5-f1a1df9b-v3\esp32s3\lib 
3. **Készíts biztonsági másolatot** az eredeti fájlokról:
- `libesp_netif.a`
- `liblwip.a`
4. Másold be a **javított** `.a` fájlokat a [Lib_tools\esp32s3_5_4.zip fájlból](esp32s3_5_4.zip)    
5. Indítsd újra az Arduino IDE-t.
6. Fordítsd újra a projektet.

---

## Megjegyzések

- A csere **csak az ESP32-S3** platformot érinti (az `esp32s3\lib` mappában történik).
- Más ESP32 variánsok (ESP32, ESP32-C3) esetén külön könyvtár van.
- Ha az Arduino új ESP32 Core-t telepít, előfordulhat, hogy a fájlokat ismét cserélni kell.

---

