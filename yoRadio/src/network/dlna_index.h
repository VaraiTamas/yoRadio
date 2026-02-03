#pragma once
#include "../core/options.h"

#ifdef USE_DLNA

#include <Arduino.h>
#include <FS.h>
#include <vector>

class DlnaIndex {
public:
  DlnaIndex() = default;

  // Root container index file (for UI fast load, optional)
  bool buildContainerIndex(const String& controlUrl, const String& rootObjectId);

  // Used by /dlna/list endpoint
  bool listContainer(const String& controlUrl, const String& objectId, String& outJson, uint32_t start);

  // Used by /dlna/build endpoint decision
  bool browseAndDecide(const String& controlUrl, const String& objectId, bool& hasItems, bool& hasContainers);

  // Builds playlist from objectId (supports deep containers)
  bool autoBuildPlaylist(const String& controlUrl, const String& objectId, uint8_t maxDepth = 4, uint32_t hardLimit = 5000);

private:
  // SOAP browse one page, returns DIDL xml fragment (decoded from CDATA, entities simplified)
  bool browsePage(const String& controlUrl,
                  const String& objectId,
                  uint32_t start,
                  uint32_t count,
                  String& outDIDL,
                  uint32_t& returned,
                  uint32_t& total);

  String buildBrowseEnvelope(const String& objectId, uint32_t start, uint32_t count);
  void decodeEntities(String& s);
  String beautifyTitle(const String& raw);
  // DIDL parsers
  void extractContainersFromDIDL(const String& didl, std::vector<String>& ids, std::vector<String>& titles);
  void extractItemsFromDIDL(const String& didl, std::vector<String>& ids, std::vector<String>& titles, std::vector<String>& urls);

  // playlist helpers
  bool appendTracksToFile(const String& didl, fs::File& f, uint32_t& appended, uint32_t hardLimit);
};
#endif // USE_DLNA
