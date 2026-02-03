#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

enum DlnaJobType : uint8_t { DJ_INIT=0, DJ_BUILD=1, DJ_APPEND=2, DJ_CANCEL=3 };

struct DlnaJob {
  DlnaJobType type;
  char objectId[64];   // DLNA objectId
//  char key[32];        // opcionális: category/genre key
  uint32_t reqId;      // monoton növekvő request id
  uint32_t hardLimit;
};

struct DlnaStatus {
  volatile bool busy;
  volatile bool ok;
  volatile int  err;           // 0=OK, HTTP-szerű / belső hibakód
  volatile uint32_t reqId;
  volatile uint32_t playlistVer;   // fájlverzió a preview sync-hez
  char msg[96];
};

extern QueueHandle_t g_dlnaQueue;
extern SemaphoreHandle_t g_spiffsMux;
extern DlnaStatus g_dlnaStatus;

void dlna_worker_start();
void dlna_worker_enqueue(const DlnaJob& j);
void dlna_status_setBusy(const DlnaJob& j, const char* msg);
void dlna_status_setDone(const DlnaJob& j, bool ok, int err, const char* msg);
uint32_t dlna_playlist_version();
