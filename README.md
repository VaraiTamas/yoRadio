# yoRadio VTom
<img src="images/front_radio.png" alt="Projekt logó" width="800">

### A program alapja a ёRadio v0.9.720 https://github.com/e2002/yoradio
Ezt mindenképpen olvasd végig.
https://github.com/e2002/yoradio

---
## yoRadio VTom
- [Telepítési tanácsok](#telepitesi-tanacsok)
- [Nyelvek, területi beállítások](#nyelvek-teruleti-beallitasok)
- [Névnapok megjelenítése](#nevnapok-megjelenitese)
- [PCB nyomtatott áramkör](#pcb-nyomtatott-aramkor)
- [Version history](#version-history)

---
## Telepitesi tanacsok
!!! Figyelem !!!
Ez a verzió kizárólag az ESP32-S3-devkit-C1 N16R8, 44 lábú modulhoz és
- ILI9488 480x320 felbontású SPI
- ILI9341 320x240 felbontású SPI 
- ST7796  480x320 felbontású SPI

kijelzőhöz készült és csak az audioI2S DAC eszközzel működik megfelelően, [PCM5102A](PCM5102A) -val tesztelve!
- Amennyiben mindig a hangerő jelenik meg ellenőrízd a következőket:
   - Az LCD kijelzőn nem szabad bekötni a MISO_13 vezetéket, mert arra nincs szökség!
   - Ha nem használsz touch funkciót, akkor ne definiáld a myoptions.h fájlban, kommenteld ki!  

         /* Touch */
         // #define TS_MODEL TS_MODEL_XPT2046
         // #define TS_CS    3

- Nem támogatja az ESP32-t PSRAM memória nélkül.   
- Az ESP32 board v.3.3.0 vagy újabb verziókra van szükség.  
- A partíció sémánál válaszd "8M with spiffs (3MB APP/1.5MB SPIFFS)" 
- A memória szerkezete és a vele való munka teljesen megváltozott, az összes puffer és sok változó átkerült a PSRAM-ba. 
- Megnövelt audio adatpuffer 655 KB-ra. Az I2S hangmintavételezéssel rendelkezik 48 kHz-ig.  
- Nagyon fontos, hogy a data mappa feltöltése után töröld a böngésző előzményeit és a web megnyitása után nyomd meg a Ctrl+Shift+R gombokat a web felület valódi frissítéséhez (Google chrome)!!!




Ez a konfiguráció néhány további könyvtártól függ. Kérlek, telepítsd őket a könyvtárkezelővel vagy PlatformIO esetén használd a mellékelt platformio.ini fájlt.
- Adafruit GFX Library  1.12.3  https://github.com/adafruit/Adafruit-GFX-Library.git
- XPT2046_Touchscreen   1.4     https://github.com/PaulStoffregen/XPT2046_Touchscreen.git
- RTCLib                2.1.4   https://github.com/adafruit/RTClib.git
- Adafruit_ILI9341      1.6.2   https://github.com/adafruit/Adafruit_ILI9341.git   (szükség esetén)

### Nyelvek, teruleti beallitasok:

Aprogram beépített nyelveket és területi beállításokat tartalmaz HU, PL, GR, EN, RU, NL, SK, UA nyelveken.   
A myoptions.h fájlban az alábbi paranccsal állíthatod be.   
#define L10N_LANGUAGE HU

A program az Adafruit_GFX librarit használja, ahol egy 5x7 pixel méretű fontot skáláz fel a kért méret függvényében. Ez a font a glcdfont.c fájlban van megrajzolva.    
A fájlok helye:   
Arduino IDE esetén a számítógép Dokumentumok/Arduino/libraries/Adafruit_GFX_Library/.glcdfont.c    
PlatformIO esetén a \yoRadio\\.pio\libdeps\esp32-s3-devkitc1-n16r8\Adafruit GFX Library\glcdfont.c

Ha nálad nem jelennek meg helyesen a karakterek, akkor ezt a fájlt le kell cserélni a nyelvedhez tartozó fájlra. A WiFi kijelzés és hangszóró kijelzés helytelenül jelenik meg, valamint azoknál a nyelveknél, melyek az angoltól eltérő karakterkészletet használnak (ékezetest), különböző a nyelvekhez szerkesztett fájlt kell használni és arra lecserélni az eredetit.
Ezek itt találhatóak a programban:

      yoRadio/locale/glcdfont/EN, NL, CZ/glcdfont.c
      yoRadio/locale/glcdfont/GR/glcdfont.c
      yoRadio/locale/glcdfont/HU/glcdfont.c
      yoRadio/locale/glcdfont/PL, SK/glcdfont.c
      yoRadio/locale/glcdfont/RU/glcdfont.c
      yoRadio/locale/glcdfont/UA/glcdfont.c



A myoptions.h fájlban beállított pin-ek ajánlottak a helyes működéshez és a mellékelt PCB 
alaplap szerint van konfigurálva.
Itt tovább alakítható.
https://trip5.github.io/ehRadio_myoptions/generator.html

Az ESP modulról itt olvasható:
esp32-S3-devkit-C1 44 pins https://randomnerdtutorials.com/esp32-s3-devkitc-pinout-guide


### Nevnapok megjelenitese:
A program képes megjeleníteni a HU, PL, GR nyelvű névnapokat.
A myoptions.h fájlban az alábbi paranccsal lehet bekapcsolni és beállítani a kívánt nyelvet.

#define NAMEDAYS_FILE HU

A névnapok tárolása az alábbi fájlokban történik.

      local/namedays/namedays_HU.h
      local/namedays/namedays_PL.h
      local/namedays/namedays_GR.h

Ha más nyelven szeretnéd használni vedd fel velem a kapcsolatot.

Ha nem szeretnéd megjeleníteni, akkor kommenteld ki a sort,    
// #define NAMEDAYS_FILE HU   
vagy a WEB-es felületen kikapcsolható options/tools-> Namedays gombbal.


### PCB nyomtatott aramkor:
- A PCB gyártáshoz szükséges gerber fájl, kapcsolási rajz, és egyéb információ a [PCB](PCB) mappában található. 
- Építési javaslatok [PCB_2025.06.14. oldalon láthatóak.](PCB/BCP_2025_06_14/PCB.md) 
---


### Version history:
### v7.4
   - Ukrán(UA) nyelv hozzáadva (by Vadim Poljakovszkij)
   - A dátum kifejezés meghosszabítva 38 karakterre
   
### v7.3
   - Elérhetővé váltak a görög névnapok és görög nyelv. (by Antreas Mpokas)
   - Beállítható az eredeti hét szegmenses óra betűtípus a myoptions.h -ban. #define CLOCKFONT VT_DIGI_OLD
   - A területi beállításokhoz tartozó fájlok glcdfont, displayL10n, namedays egységesen a local mappába kerültek.
   - Lengyel nyelv és névnapok javítása. (by Andrzej Jaroszuk)
   - [Új WEB kereső hozzáadva 2.0 (by Mirosław Bubka)](images/MB_2.0/MB_search.md)

### v7.2
   - IR hangerő túlfutott a 0-100 tartományon, ez lett javítva. 
   - Új beállítási lehtőség a myoptions.h fájlban
   - #define DIRECT_CHANNEL_CHANGE
      Az állomások listájából való választásnál nem kell megnyomni a rotary encoder gombját, kilépéskor autómatikusan átvált a csatorna. (by Zsigmond Becskeházi)
   - #define STATIONS_LIST_RETURN_TIME 2  
      Mennyi idő múlva lépjen vissza a főképernyőre az állomások listájából. (másodperc) Eredeti érték: 30 másodperc. 
### v7.1
   - Amikor a weben ki-be kapcsoljuk a névnapokat, akkor a teljes képernyő frissült és
   nem csak a bitrateWidget. Ez lett javítva.
### v6.0
   - bekerült a Maleksm audio könyvtár (csak PSRAM -al működik) https://github.com/schreibfaul1/ESP32-audioI2S.git    
### v5.0
   - Két formátumú kivezérlésmérő   

