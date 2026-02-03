#include "../core/options.h"
#ifdef USE_DLNA

#include "dlna_service.h"
#include "dlna_ssdp.h"
#include "dlna_desc.h"
#include "dlna_index.h"
#include "dlna_worker.h"
#include "../core/network.h"
#include "../core/player.h"

String g_dlnaControlUrl;

static bool s_serviceStarted = false;
static uint32_t s_reqId = 1;
bool g_dlnaReady = false;

bool dlnaInit(const String& rootObjectId, String& err) {

  g_dlnaReady = false;
  g_dlnaControlUrl = "";

  player.sendCommand({PR_STOP, 0});

  DlnaSSDP ssdp;
  DlnaDescription desc;
  DlnaIndex idx;

  String descUrl;
  String controlUrl;

  uint32_t lastYield = millis();

  if (!ssdp.resolve(dlnaHost, descUrl)) {
    err = "SSDP discover failed";
    return false;
  }

  if (millis() - lastYield > 50) {
    vTaskDelay(1);
    lastYield = millis();
  }

  bool cdOk = false;
  for (int i = 0; i < 3; i++) {
    if (desc.resolveControlURL(descUrl, controlUrl)) {
      cdOk = true;
      break;
    }

    vTaskDelay(pdMS_TO_TICKS(500));   // ✔ delay helyett
  }

  if (!cdOk || !controlUrl.length()) {
    err = "ContentDirectory not found";
    Serial.println("[DLNA] ERROR: ContentDirectory control URL not resolved");
    return false;
  }

  Serial.printf("[DLNA] ContentDirectory control URL: %s\n", controlUrl.c_str());

  if (!idx.buildContainerIndex(controlUrl, rootObjectId)) {
    err = "Root container browse failed";
    return false;
  }

  // biztos yield a végén is
  vTaskDelay(1);

  g_dlnaControlUrl = controlUrl;
  g_dlnaReady = true;

  return true;
}

void dlna_service_begin() {
  if (s_serviceStarted) return;
  s_serviceStarted = true;

  dlna_worker_start();
}

uint32_t dlna_next_reqId() {
  uint32_t v = s_reqId++;
  if (v == 0) v = s_reqId++; // 0 ne legyen
  return v;
}

bool dlna_isBusy() {
  return g_dlnaStatus.busy;
}

#endif   // USE_DLNA
