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
**idf-release_v5.5-xxxxxxx**  
Ez mutatja, hogy az Arduino ESP-IDF 5.5.x verziót használ.

Ezután keresd meg az **'sdkconfig'** fájlt, erre később lesz szükség. Ez tartalmazza az összes beállítást, amelyet a könyvtárak fordításához használtak (alapértelmezetten). Az alábbi mappában találod.   
...C:\Users\<név>\AppData\Local\Arduino15\packages\esp32\tools\esp32-arduino-libs\idf-release_v5.5-xxxxxxx\esp32s3

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
C:\Espressif\Projects   
Majd ezen belül hozz létre egy projektet például ESP32S3 néven a PowerShell programmal!  
``` 
cd C:\Espressif\Projects
idf.py create-project ESP32S3
cd ESP32S3
```
majd a cél beállításához futtasd a PowerShell-ben az alábbi parancsokat:
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

Másold át az Arduino-ból az sdkconfig fájlt innen!

...C:\Users\<név>\AppData\Local\Arduino15\packages\esp32\tools\esp32-arduino-libs\idf-release_v5.5-xxxxxxx\esp32s3\

ide:

C:\Espressif\Projects\ESP32S3\

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

C:\Espressif\Projects\ESP32S3\build\esp-idf\lwip\liblwip.a  
C:\Espressif\Projects\ESP32S3\build\esp-idf\esp_netif\libesp_netif.a


Ezek az új verziók.

### 7️⃣ A fájlok cseréje

Biztonsági mentés ajánlott, majd másold be a fájlokat ide Arduino környezetben:

C:\Users\<név>\AppData\Local\Arduino15\packages\esp32\tools\esp32-arduino-libs\idf-release_v5.5-xxxx\esp32s3\lib\

Visual Studio Code PlatformIO környetetben:  
C:\Users\<név>\.platformio\packages\framework-arduinoespressif32-libs\esp32s3\lib

### 8️⃣ Projekt újrafordítása Arduino alatt

Fordítsd újra a projektet (pl. YoRadio).
