#include "../core/options.h"
#ifdef USE_DLNA
#include <WiFi.h>
#include "dlna_ssdp.h"

static const IPAddress SSDP_ADDR(239,255,255,250);
static const uint16_t SSDP_PORT = 1900;

bool DlnaSSDP::resolve(const char* expectedHost, String& outDescUrl) {
  outDescUrl = "";

 
_udp.stop();

  // sima UDP socket (nem multicast API!)
  if (!_udp.begin(0)) {
    Serial.println("[DLNA][SSDP] UDP begin failed");
    return false;
  }

  if (!sendSearch()) {
    _udp.stop();
    return false;
  }

  unsigned long start = millis();
  while (millis() - start < 3500) {
    if (receiveResponse(expectedHost, outDescUrl)) {
      _udp.stop();
      return true;
    }
    delay(10);
  }

  _udp.stop();
  Serial.println("[DLNA][SSDP] timeout");
  return false;
}

bool DlnaSSDP::sendSearch() {
  const char* msg =
    "M-SEARCH * HTTP/1.1\r\n"
    "HOST: 239.255.255.250:1900\r\n"
    "MAN: \"ssdp:discover\"\r\n"
    "MX: 2\r\n"
    "ST: urn:schemas-upnp-org:device:MediaServer:1\r\n"
    "\r\n";

  _udp.beginPacket(SSDP_ADDR, SSDP_PORT);
  _udp.write((const uint8_t*)msg, strlen(msg));
  _udp.endPacket();

  Serial.println("[DLNA][SSDP] M-SEARCH sent");
  return true;
}

bool DlnaSSDP::receiveResponse(const char* expectedHost, String& outDescUrl) {
  int size = _udp.parsePacket();
  if (!size) return false;

/*  String response;
  while (_udp.available()) {
    response += (char)_udp.read();
  }*/

  // FIX: String összefűzés helyett fix buffer (heap-fragmentáció ellen)
  static char buf[1600];
  int n = 0;
  while (_udp.available() && n < (int)sizeof(buf) - 1) {
    buf[n++] = (char)_udp.read();
  }
  buf[n] = 0;
  String response(buf);

  if (!response.startsWith("HTTP/1.1 200")) return false;

  String url;
  if (!parseLocation(response, url)) return false;

  if (expectedHost && strlen(expectedHost)) {
    if (!url.startsWith(String("http://") + expectedHost)) {
      Serial.println("[DLNA][SSDP] response ignored (host mismatch)");
      return false;
    }
  }

  outDescUrl = url;
  Serial.printf("[DLNA][SSDP] FOUND: %s\n", outDescUrl.c_str());
  return true;
}

bool DlnaSSDP::parseLocation(const String& response, String& outUrl) {
/*  int idx = response.indexOf("\nLOCATION:");
  if (idx < 0) idx = response.indexOf("\nLocation:");*/

  // case-insensitive keresés durván, de stabilan
  int idx = response.indexOf("\nLOCATION:");
  if (idx < 0) idx = response.indexOf("\nLocation:");
  if (idx < 0) idx = response.indexOf("\nlocation:");

  if (idx < 0) return false;

  int start = response.indexOf(":", idx) + 1;
  int end   = response.indexOf("\r", start);
  if (start <= 0 || end <= start) return false;

  outUrl = response.substring(start, end);
  outUrl.trim();
  return true;
}
#endif // USE_DLNA