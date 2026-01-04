## 📄 ESP32 – LWIP és ESP-NETIF könyvtárak újrafordítása nagyobb TCP pufferekkel (Arduino + ESP-IDF)

Ez az útmutató bemutatja, hogyan lehet az Arduino-ESP32 környezetben használt ESP-IDF könyvtárakat újrafordítani úgy, hogy nagyobb TCP pufferbeállításokat használjanak.
Ez különösen hasznos nagy bitrátájú stream-lejátszáshoz (pl. FLAC, >1 Mbps).

🟡 Előfeltételek

ESP32-S3 (vagy kompatibilis) fejlesztőeszköz

Arduino-ESP32 core telepítve

ESP-IDF telepítőcsomag (ugyanaz a főverzió, mint amit az Arduino használ)

### 1️⃣ Az Arduino által használt ESP-IDF verzió ellenőrzése

Nyisd meg a mappát:     
C:\Users\<név>\AppData\Local\Arduino15\packages\esp32\tools\esp32-arduino-libs\
Itt találsz egy ilyen mappát:
idf-release_v5.5-xxxxxxx
Ez mutatja, hogy az Arduino ESP-IDF 5.5.x verziót használ.

Ezután nyisd meg:   
...C:\Users\<név>\AppData\Local\Arduino15\packages\esp32\tools\esp32-arduino-libs\idf-release_v5.5-xxxxxxx\esp32s3 mappát és abban találod az sdkconfig fájlt. Ez a fájl tartalmazza az összes beállítást, amelyet a könyvtárak fordításához használtak (alapértelmezett).  

### 2️⃣ Azonos verziójú ESP-IDF letöltése

Nyisd meg:

https://dl.espressif.com/dl/esp-idf/


Töltsd le ugyanazt a verziót (pl. 5.5.2), majd telepítsd a szoftvert (eltart egy ideig).
Én a C meghajtót használom.     


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

A telepítés a C:\Espressif mappába telepíti a fájlokat.
Ebbe a mappába hozz létre a munkakörnyezetnek mappát például  
C:\Espressif\Projects\ESP32S3\, majd a cél beállításához futtasd a PowerShell-ben az alábbi parancsokat:
```
$env:IDF_TARGET="esp32s3" 
```  
```
idf.py set-target esp32s3   
```
```
idf.py build
```
Ez lefordítja az alapértelmezett könyvtárakat.

### 4️⃣ Arduino-sdkconfig átmásolása és módosítása

Másold át az Arduino-ból az sdkconfig fájlt ide:

C:\Espressif\Projects\ESP32S3\


Majd nyisd meg, és módosítsd az alábbi értékeket (nagyobb TCP pufferek):

CONFIG_LWIP_MAX_ACTIVE_TCP=512  
CONFIG_LWIP_MAX_LISTENING_TCP=512   
CONFIG_LWIP_TCP_SND_BUF_DEFAULT=8192    
CONFIG_LWIP_TCP_WND_DEFAULT=32768   
CONFIG_LWIP_TCP_RECVMBOX_SIZE=32

Mentsd el, majd a fordításhoz futtasd újra:
```
idf.py build
```
Most már az új beállításokkal fordulnak a könyvtárak.

### 5️⃣ Az újonnan fordított könyvtárak kiemelése

A build után keresd meg a fájlokat:

F:\Espressif\Projects\ESP32S3\build\esp-idf\lwip\liblwip.a  
F:\Espressif\Projects\ESP32S3\build\esp-idf\esp_netif\libesp_netif.a


Ezek az új verziók.

### 6️⃣ A könyvtárak cseréje az Arduino környezetben

Biztonsági mentés ajánlott, majd másold be a fájlokat ide:

C:\Users\<név>\AppData\Local\Arduino15\packages\esp32\tools\esp32-arduino-libs\idf-release_v5.5-xxxx\esp32s3\lib\

### 7️⃣ Projekt újrafordítása Arduino alatt

Fordítsd újra a projektet (pl. YoRadio).
