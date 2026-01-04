# Nyomtatott áramkör – verzió: 2025.06.14

- **PCB mérete:** 99 × 90 mm
- **SMD alkatrészek mérete:** 1206
- A `GPIO_1`, `GPIO_2`, `GPIO_42`, `GPIO_17` kivezetések nem használt PIN-ek
- Az **R3–R6** ellenállások a nem használt pinek felhúzóellenállásai  
  Csak szükség esetén kell beforrasztani őket.
- A MISO 13 vezetéket nem kell a kijelzőre bekötni. A kijelző nem küld információt az ESP irányába és folyamatos hangerő képernyő megjelenést okoz!
- A T.IRQ jumpert nem kell összezárni.
### Forgó jeleadó - rotary encoder
- Az ENCODER-hez tartozó alkatrészek:
  - R7–R12: **10 kΩ**
  - C5–C8: **100 nF**
  - Beültetésük csak akkor szükséges, ha **nem a képen látható kék ENCODER-modult** használsz (a modul ezeket tartalmazza már!)

![](../../Rotary_encoder/rotary_encoder_modul_wire_diagram.png)

Az encoderek használata esetén a `myoptions.h` fájlban definiálni kell őket:

```cpp
/* ENCODER 1 */
#define ENC_BTNR 41  // S2
#define ENC_BTNL 40  // S1
#define ENC_BTNB 39  // KEY
// #define ENC_INTERNALPULLUP true

/* ENCODER 2 */
#define ENC2_BTNR 47  // S2
#define ENC2_BTNL 48  // S1
#define ENC2_BTNB 21  // KEY
// #define ENC2_INTERNALPULLUP true
```
### Érintő képernyő
- A T.CK, T.CS, T-MISO, T.MOSI érintkezők az LCD modulon az érintőképernyő SPI vezetékei. Használat esetén mindegyiket be kell kötni. Amennyiben nem akarsz használni érintő kijelzőt és nem kötöd be ezeket, úgy a myoptions.h fájlban kommenteld ki a sor elejére helyezett // jellel az ide vonatkozó definíciókat. Ellenkező esetben mindig a hangerő képernyő jelenhet meg.
```cpp
/* Touch */
// #define TS_MODEL TS_MODEL_XPT2046
// #define TS_CS    3
```

### SD kártya
- Az FSPI csatlakozóhoz lehet bekötni az SD kártya olvasót. Használata esetén a myoptions.h fájlban be kell kapcsolni a definíciókat.
```cpp
        /* DS CARD */
        #define SDC_CS 18
```
### RTC óramodul
- Az I2C csatlakozó alkalmas közvetlenül óramodul RTC DS3132 fogadására. Használata esetén a myoptions.h fájlban be kell kapcsolni a definíciókat.
```cpp
        /* CLOCK MODUL RTC DS3132 */
        #define RTC_SCL			     7
        #define RTC_SDA			     8
        #define RTC_MODULE DS3231
```
### Power-select jumperek       
A PWS_DAC, PWS_LCD, PWS_I2C power select jumperek csak tesztelési céllal kerültek fel. Amennyiben a modul tartalmaz saját 3.3V -os stabilizátor IC -t, úgy lehet választani az 5V -os táplálást.

- 5V_ESP32 zárása → az ESP a saját 3.3 V stabilizátorát használja

- 3.3V_ESP32 zárása → az ESP-t az alaplapi stabilizátor táplálja        
Ajánlás: csak az 5V_ESP32 ágat zárd

### Erősítő ki -be kapcsolása a képernyővédő és hangerő függvényében [olvasd el ezt!](../../docs/pwr_amp.md)

### Az ESP32-S3-DevKitC 1 N16R8 fejlesztőmodul lábkiosztása.      
<img src="../../PCB/esp32-S3-DevKitC-1-original-pinout-low.jpg" alt="esp32-S3-DevKitC-1-original-pinout-low" height="400"><br><br>      

### Az ESP32-S3-DevKitC 1 N16R8 fejlesztőmodul lábkiosztása ebben a projektben.      
<img src="../../PCB\esp_s3_devkit_c1_n16r8_pins_setup.png" alt="esp_s3_devkit_c1_n16r8_pins_setup" height="400"><br><br>

- Az ESP32-S3 kártyák esetében a HSPI busz által használt GPIO-k száma eltérhet (**SCK, MISO, MOSI**).  
  Általában **nincs elérhető VSPI busz**.

- **Ne használd a 35–37. GPIO-kat** 8 MB vagy nagyobb PSRAM-mal rendelkező alaplapokon.

- A **22–34, 45 és 46 GPIO-k** vagy foglaltak, vagy **nem használhatók**.

- A **GPIO 19 és 20** az USB porthoz tartozik,  
  a **GPIO 43 és 44** általában az USB- és a soros porthoz.  
  Egyes kártyák más GPIO-kat is használhatnak — **ellenőrizd kétszer**.

- A kijelző **VS RST tüskéjét** csatlakoztasd a panel **RST / EN / ST** tűjéhez  
  (érték: **-1**).

- A **GPIO 0 (BOOT)** és **GPIO 3 (JTAG)** rögzítőtüskék,  
  boot során **HIGH szinten kell lenniük** (nyomógombhoz csak körültekintéssel használhatók).

- Az SPI lábak ellenőrzéséhez itt találod a board-variánsokat:  
  https://github.com/espressif/arduino-esp32/tree/master/variants

- Fordítási konfigurációk:  
  https://github.com/sivar2311/ESP32-S3-PlatformIO-Flash-and-PSRAM-configurations

- Hardver beállítások:  
  https://github.com/e2002/yoradio#hardware-setup

- Flash-elési útmutató:  
  https://github.com/e2002/yoradio/wiki/How-to-flash


### Ez a PCB verzió legyártható a [jlcpcb.com](https://jlcpcb.com/) oldalon a [yoradio_gerber.zip](../../PCB/BCP_2025_06_14/yoradio_gerber.zip)   fájl feltöltésével.     

![PCB front](2D_top_99x90mm.jpg)<br><br>
![PCB back](2D_bottom_99x90mm.jpg)<br><br>
![3D top](3D_top.jpg)<br><br>



