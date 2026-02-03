#include "../core/options.h"
#ifdef USE_DLNA

#include "../core/config.h"
#include "dlna_worker.h"
#include "dlna_service.h"
#include <SPIFFS.h>
#include "esp_task_wdt.h"
#include "dlna_index.h"
#include "dlna_http_guard.h" 

QueueHandle_t g_dlnaQueue = nullptr;
SemaphoreHandle_t g_spiffsMux = nullptr;
DlnaStatus g_dlnaStatus = {false,false,0,0,0,{0}};

static TaskHandle_t s_workerTask = nullptr;
static uint32_t s_playlistVer = 1;

uint32_t dlna_playlist_version() { return s_playlistVer; }

static void bumpPlaylistVer() {
  s_playlistVer++;
  if (s_playlistVer == 0) s_playlistVer = 1;
  g_dlnaStatus.playlistVer = s_playlistVer;
}

void dlna_status_setBusy(const DlnaJob& j, const char* msg) {
  g_dlnaStatus.busy = true;
  g_dlnaStatus.ok = false;
  g_dlnaStatus.err = 0;
  g_dlnaStatus.reqId = j.reqId;
  strncpy(g_dlnaStatus.msg, msg ? msg : "busy", sizeof(g_dlnaStatus.msg)-1);
  g_dlnaStatus.msg[sizeof(g_dlnaStatus.msg)-1] = 0;
}

void dlna_status_setDone(const DlnaJob& j, bool ok, int err, const char* msg) {
  g_dlnaStatus.busy = false;
  g_dlnaStatus.ok = ok;
  g_dlnaStatus.err = err;
  g_dlnaStatus.reqId = j.reqId;
  strncpy(g_dlnaStatus.msg, msg ? msg : (ok ? "ok" : "fail"), sizeof(g_dlnaStatus.msg)-1);
  g_dlnaStatus.msg[sizeof(g_dlnaStatus.msg)-1] = 0;
}

static bool write_playlist_atomic(const char* tmpPath, const char* finalPath, const String& content) {
  // mutex: egy időben ne olvassa/írja más
  xSemaphoreTake(g_spiffsMux, portMAX_DELAY);

  File f = SPIFFS.open(tmpPath, FILE_WRITE);
  if (!f) { xSemaphoreGive(g_spiffsMux); return false; }
  size_t n = f.print(content);
  f.close();
  if (n != content.length()) { SPIFFS.remove(tmpPath); xSemaphoreGive(g_spiffsMux); return false; }

  SPIFFS.remove(finalPath);
  bool ok = SPIFFS.rename(tmpPath, finalPath);

  xSemaphoreGive(g_spiffsMux);
  return ok;
}

static bool append_playlist_atomic(const char* tmpPath, const char* finalPath, const String& addLines) {
  xSemaphoreTake(g_spiffsMux, portMAX_DELAY);

  // 1) read old
  String old;
  {
    File f = SPIFFS.open(finalPath, FILE_READ);
    if (f) { old = f.readString(); f.close(); }
  }

  // 2) write merged to tmp
  File t = SPIFFS.open(tmpPath, FILE_WRITE);
  if (!t) { xSemaphoreGive(g_spiffsMux); return false; }
  t.print(old);
  if (old.length() && old[old.length()-1] != '\n') t.print("\n");
  t.print(addLines);
  t.close();

  // 3) swap
  SPIFFS.remove(finalPath);
  bool ok = SPIFFS.rename(tmpPath, finalPath);

  xSemaphoreGive(g_spiffsMux);
  return ok;
}

// WDT/yield helper hosszú ciklusokba
static inline void worker_yield() {
  //esp_task_wdt_reset();
  vTaskDelay(1); // 1 tick yield
}

static void dlna_worker_task(void* ) {
  //esp_task_wdt_add(nullptr); // add current task to WDT (ha használod)
  DlnaJob j{};

  for (;;) {
    if (xQueueReceive(g_dlnaQueue, &j, portMAX_DELAY) != pdTRUE) continue;

    Serial.printf("[DLNA][WORK] job=%d objectId='%s'\n", (int)j.type, j.objectId);

    if (j.type == DJ_CANCEL) {
      dlna_status_setDone(j, true, 0, "cancelled");
      continue;
    }

    dlna_status_setBusy(j, "working");

    bool ok = false;
    int err = 0;

    // !!! FONTOS: itt semmilyen AsyncWebServerRequest nincs, csak paraméterek
    if (j.type == DJ_BUILD) {
      dlna_status_setBusy(j, "build");

      if (!g_dlnaControlUrl.length()) {
        dlna_status_setDone(j, false, 503, "DLNA not initialized");
        continue;
      }

      DlnaIndex idx;
      bool hasItems = false, hasContainers = false;

      ok = idx.browseAndDecide(g_dlnaControlUrl, j.objectId, hasItems, hasContainers);

      
      if (!ok) {
        dlna_status_setDone(j, false, 500, "browse failed");
        continue;
      }

      if (!hasItems) { 
       // van konténer, de track nincs közvetlenül -> UI-nak jelezzük szépen
       dlna_status_setDone(j, false, 422, hasContainers ? "No tracks in this container (only subfolders)" : "Empty container");
       continue;
      } 

      uint8_t depth;
      if (hasItems) depth = 2;
      else depth = 6;

      uint32_t limit = j.hardLimit ? j.hardLimit : 20000;

      ok = idx.autoBuildPlaylist(
             g_dlnaControlUrl,
             j.objectId,
             depth,
             limit    // ha nincs: add meg defaultnak
           );

      if (!ok) {
        dlna_status_setDone(j, false, 500, "build failed");
        continue;
      }

      // ===== ATOMIKUS CSERE =====
      xSemaphoreTake(g_spiffsMux, portMAX_DELAY);
      SPIFFS.remove(PLAYLIST_DLNA_PATH);
      ok = SPIFFS.rename(TMP_PATH, PLAYLIST_DLNA_PATH);
      xSemaphoreGive(g_spiffsMux);

      if (!ok) {
        dlna_status_setDone(j, false, 550, "rename failed");
        continue;
      }

      config.sdResumePos = 0;
      config.resumeAfterModeChange = false;

      // 🔑 DLNA build -> reset DLNA index ONLY
      config.store.lastDlnaStation = 1;
      config.saveValue(&config.store.lastDlnaStation, (uint16_t)1);

      dlna_status_setDone(j, true, 0, "build ok");
    }

    else if (j.type == DJ_APPEND) {
      dlna_status_setBusy(j, "append");

      if (!g_dlnaControlUrl.length()) {
        dlna_status_setDone(j, false, 503, "DLNA not initialized");
        continue;
      }

      DlnaIndex idx;
      bool hasItems = false, hasContainers = false;

      ok = idx.browseAndDecide(g_dlnaControlUrl, j.objectId, hasItems, hasContainers);
      if (!ok) {
        dlna_status_setDone(j, false, 500, "browse failed");
        continue;
      }

      if (!hasItems) { 
       // van konténer, de track nincs közvetlenül -> UI-nak jelezzük szépen
       dlna_status_setDone(j, false, 422, hasContainers ? "No tracks in this container (only subfolders)" : "Empty container");
       continue;
      } 

      uint8_t depth;
      if (hasItems) depth = 2;
      else depth = 6;

      uint32_t limit = j.hardLimit ? j.hardLimit : 20000;

      ok = idx.autoBuildPlaylist(
             g_dlnaControlUrl,
             j.objectId,
             depth,
             limit
           );

      if (!ok) {
        dlna_status_setDone(j, false, 500, "append build failed");
        continue;
      }

      // ===== APPEND =====
      xSemaphoreTake(g_spiffsMux, portMAX_DELAY);

      File out = SPIFFS.open(PLAYLIST_DLNA_PATH, FILE_APPEND);
      File in  = SPIFFS.open(TMP_PATH, FILE_READ);

      if (out && in) {
        if (out.size() > 0) out.print("\n");
        while (in.available()) out.write(in.read());
        ok = true;
      } else {
        ok = false;
      }

      if (out) out.close();
      if (in)  in.close();
      SPIFFS.remove(TMP_PATH);

      xSemaphoreGive(g_spiffsMux);

      if (!ok) {
        dlna_status_setDone(j, false, 551, "append failed");
        continue;
      }

      bumpPlaylistVer();
      dlna_status_setDone(j, true, 0, "append ok");
    }

else if (j.type == DJ_INIT) {
  dlna_status_setBusy(j, "init");

  String errStr;
  String rootId = String(dlnaIDX);

  vTaskDelay(1);

  bool okInit = dlnaInit(rootId, errStr);

  vTaskDelay(1);

  dlna_status_setDone(
    j,
    okInit,
    okInit ? 0 : 503,
    okInit ? "init ok" : errStr.c_str()
  );
}

    worker_yield();
  }
}

void dlna_worker_start() {
  if (s_workerTask) return;                 // már fut
  if (!g_dlnaHttpMux) {
	  g_dlnaHttpMux = xSemaphoreCreateMutex();
	  Serial.println("[DLNA] HTTP mutex created");}
  if (!g_spiffsMux) g_spiffsMux = xSemaphoreCreateMutex();
  if (!g_dlnaQueue) g_dlnaQueue = xQueueCreate(6, sizeof(DlnaJob));

  BaseType_t ok = xTaskCreatePinnedToCore(
    dlna_worker_task,
    "dlna_worker",
    24 * 1024,     // 12KB elég (ha kell, később feljebb)
    nullptr,
    2,             // közepes prio
    &s_workerTask,
    0              // !!! CORE0 (nálad core1 halál)
  );

  if (ok != pdPASS) {
    s_workerTask = nullptr;
    Serial.println("[DLNA] worker task create FAILED");
  } else {
    Serial.println("[DLNA] worker task started");
  }
}


void dlna_worker_enqueue(const DlnaJob& j) {
  if (!g_dlnaQueue) return;
  xQueueSend(g_dlnaQueue, &j, 0);
}
#endif   // USE_DLNA