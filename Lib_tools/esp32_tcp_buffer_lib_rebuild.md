## 📄 ESP32 – LWIP és ESP-NETIF könyvtárak újrafordítása nagyobb TCP pufferekkel

Ez az útmutató bemutatja, hogyan lehet az ESP-IDF könyvtárakat újrafordítani úgy, hogy nagyobb TCP pufferbeállításokat használjanak.
Ez különösen hasznos nagy bitrátájú stream-lejátszáshoz (pl. FLAC, >1 Mbps).

🟡 Előfeltételek

ESP32-S3 (vagy kompatibilis) fejlesztőeszköz

- Arduino-ESP32 core telepítve vagy    
- VSC platformIO esetén az alábbi platform használata   
```
; framework-arduinoespressif32 @ 3.3.7
; framework-arduinoespressif32-libs @ 5.5.0+sha.87912cd291
; (v5.5.2.260206)
[env:esp32-s3-devkitc1-n16r8]
platform = https://github.com/pioarduino/platform-espressif32/releases/download/55.03.36/platform-espressif32.zip
board = esp32-s3-devkitc1-n16r8
framework = arduino
```

- ESP-IDF telepítőcsomag (ugyanaz a főverzió, mint amit az Arduino használ)

### 1️⃣ Az Arduino által használt ESP-IDF verzió ellenőrzése

Nyisd meg a mappát:  
```   
C:\Users\<név>\AppData\Local\Arduino15\packages\esp32\tools\esp32-arduino-libs\
```
Itt találsz egy ilyen mappát:   

**idf-release_v5.5-xxxxxxx***

Ez mutatja, hogy az Arduino ESP-IDF 5.5.x verziót használ.

Ezután keresd meg az **'sdkconfig'** fájlt, erre később lesz szükség. Ez tartalmazza az összes beállítást, amelyet a könyvtárak fordításához használtak (alapértelmezetten). Az alábbi mappában találod Arduino IDE esetén.   
```
C:\Users\<név>\AppData\Local\Arduino15\packages\esp32\tools\esp32-arduino-libs\idf-release_v5.5-xxxxxxx\esp32s3  
```

PlatformIO esetén megnyitva az sdkconfig fájl 
```
C:\Users\<név>\.platformio\packages\framework-arduinoespressif32-libs\esp32s3\sdkconfig
```
A fájl tetején van beírva
```
Espressif IoT Development Framework (ESP-IDF) 5.5.2 Project Configuration
```
### 2️⃣ Azonos verziójú ESP-IDF letöltése

Nyisd meg:

https://dl.espressif.com/dl/esp-idf/


Töltsd le ugyanazt a verziót (pl. 5.5.2), majd telepítsd a szoftvert "Futtatás rendszergazdaként" módban (eltarthat egy ideig).

Én a  C:\  meghajtót használom, ez nálad változhat.     


Telepítés után indítsd el az ESP-IDF PowerShell környezetet.
Normál esetben a környezet be lesz állítva, de ha nem, akkor tedd a következőket:
A PowerShell ablakban futtasd:
```
C:\Espressif\frameworks\esp-idf-v5.5.2> .\install.ps1
```
Majd
```
C:\Espressif\frameworks\esp-idf-v5.5.2> .\export.ps1
```

### 3️⃣ Fordítási projekt létrehozása

A telepítés a **C:\Espressif** mappába telepíti a fájlokat.
Ebbe a mappába hozz létre a munkakörnyezetnek egy mappát, például  
```
C:\Espressif\Projects   
```
Ezen belül hozz létre egy projektet például ESP32S3 néven a PowerShell programmal!  
``` 
cd C:\Espressif\Projects
idf.py create-project ESP32S3
cd ESP32S3
```
A cél beállításához futtasd a PowerShell-ben az alábbi parancsokat:
```
$env:IDF_TARGET="esp32s3"
idf.py set-target esp32s3
```
Fordítsd le a projektet!
```
PS C:\Espressif\projects\esp32s3> idf.py build
```
Ez lefordítja az alapértelmezett könyvtárakat.
### 4️⃣ Arduino-sdkconfig átmásolása és módosítása
Fontos megjegyezni, hogy mindkét sdkconfig fájl első pár sorában feltüntetett veriószámnak egyeznie kell !  
Ha egyeznek, másold át az Arduino-ból az sdkconfig fájlt innen!
```
C:\Users\<név>\AppData\Local\Arduino15\packages\esp32\tools\esp32-arduino-libs\idf-release_v5.5-xxxxxxx\esp32s3\
```
PlatformIO esetén innen
```
C:\Users\<név>\.platformio\packages\framework-arduinoespressif32-libs\esp32s3\sdkconfig
```
ide:
```
C:\Espressif\Projects\ESP32S3\
```


### 5️⃣ A projektben ki kell kapcsolni az egyedi partíció beállítást.

Indítsd el a PowerShell programban a menuconfigot!
```
PS C:\Espressif\projects\esp32s3> idf.py menuconfig
```
Választ a menüben:   
Partition Table --->  
Partition Table (Single factory app, no OTA)  --->  
és jelöld be    
(x) Single factory app, no OTA  
lehetőséget  

Majd szintén a menuconfigban módosítsd az alábbi értékeket ezekre vagy saját belátásod szerint kisérletezz!
|Új értékek az 'sdkconfig" fájlban    |         Eredeti érték |  Értékhatár (range)|  Menuconfig → Component config → LWIP → TCP →
|-------------------------------------|-----------------------|--------------------|----------------------------------------------|
|CONFIG_LWIP_MAX_ACTIVE_TCP=16        |         (16)          |  1-1024            | Maximum active TCP Connections
|CONFIG_LWIP_MAX_LISTENING_TCP=16     |         (16)          | 1-1024             | Maximum listening TCP Connections
|CONFIG_LWIP_TCP_SND_BUF_DEFAULT=8192 |         (5744)        | 2440-65535         | Default send buffer size  
|CONFIG_LWIP_TCP_WND_DEFAULT=32768    |         (5760)        | 2440-65535         |Default receive window size
|CONFIG_LWIP_TCP_RECVMBOX_SIZE=32     |         (6)           | 6-64               | Default TCP receive mail box size

Q - billentyűvel mentsd el a változásokat!  

Ezt követően fordítsd le a projektet az alábbi paranccsal aPowerShell programban.

```
PS C:\Espressif\projects\esp32s3> idf.py build
```
Most már az új beállításokkal fordulnak a könyvtárak.

### 6️⃣ Az újonnan fordított könyvtárak kiemelése

A build után keresd meg a fájlokat:
```
C:\Espressif\Projects\ESP32S3\build\esp-idf\lwip\liblwip.a  
C:\Espressif\Projects\ESP32S3\build\esp-idf\esp_netif\libesp_netif.a
```

Ezek az új verziók.

### 7️⃣ A fájlok cseréje

Biztonsági mentés ajánlott, majd másold be a fájlokat ide Arduino környezetben:
```
C:\Users\<név>\AppData\Local\Arduino15\packages\esp32\tools\esp32-arduino-libs\idf-release_v5.5-xxxx\esp32s3\lib\
```
Visual Studio Code PlatformIO környetetben:  
```
C:\Users\<név>\.platformio\packages\framework-arduinoespressif32-libs\esp32s3\lib
```
### 8️⃣ Projekt újrafordítása Arduino alatt

Fordítsd újra a projektet (pl. YoRadio).    

---

### Ha támogatni szeretnéd a munkámat itt meghívhatsz egy kávéra!!!     
<a href="https://buymeacoffee.com/vtom">
    <img src="../images/buymeacoffee.png" width="200">
</a>