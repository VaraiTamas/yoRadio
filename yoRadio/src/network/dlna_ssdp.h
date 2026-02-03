#pragma once

#include <Arduino.h>
#include <WiFiUdp.h>

class DlnaSSDP {
public:
  bool resolve(const char* expectedHost, String& outDescUrl);

private:
  WiFiUDP _udp;

  bool sendSearch();
  bool receiveResponse(const char* expectedHost, String& outDescUrl);
  bool parseLocation(const String& response, String& outUrl);
};
