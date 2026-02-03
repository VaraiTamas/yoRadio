#pragma once
#include <Arduino.h>

class DlnaDescription {
public:
  // descUrl = pl. http://IP:PORT/desc/device.xml
  bool resolveControlURL(const String& descUrl, String& outControlUrl);

private:
  bool parseStream(Stream& s, String& outControlPath);
  String extractBaseUrl(const String& fullUrl);
};
