#include "../core/options.h"
#ifdef USE_DLNA
#include "dlna_http_guard.h"
SemaphoreHandle_t g_dlnaHttpMux = nullptr;
#endif