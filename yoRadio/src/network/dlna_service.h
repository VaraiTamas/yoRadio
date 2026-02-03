#pragma once
#include "../core/options.h"
#include <Arduino.h>

#ifdef USE_DLNA

#include "dlna_worker.h"

bool dlnaInit(const String& rootObjectId, String& err);
bool dlnaList(const String& objectId, String& outJson, String& err);
bool dlnaBuild(const String& objectId, bool activate, int& count, String& err);

void dlna_service_begin();
uint32_t dlna_next_reqId();
bool dlna_isBusy();

extern String g_dlnaControlUrl;

#endif // USE_DLNA
