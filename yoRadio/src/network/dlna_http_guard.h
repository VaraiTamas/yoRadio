#pragma once
#ifdef USE_DLNA
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t g_dlnaHttpMux;

struct DlnaHttpGuard {
  DlnaHttpGuard()  { if (g_dlnaHttpMux) xSemaphoreTake(g_dlnaHttpMux, portMAX_DELAY); }
  ~DlnaHttpGuard() { if (g_dlnaHttpMux) xSemaphoreGive(g_dlnaHttpMux); }
};
#endif