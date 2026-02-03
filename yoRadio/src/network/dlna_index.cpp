#include "../core/options.h"

#ifdef USE_DLNA

#include "dlna_index.h"
#include "dlna_http_guard.h"
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <SPIFFS.h>
#include "../core/config.h"     // TMP_PATH
extern Config config;

// Ajánlott: ha nálad máshol van, igazítsd!
#ifndef DLNA_LIST_JSON_PATH
  #define DLNA_LIST_JSON_PATH "/data/dlna_index.json"
#endif

static String jsonEscape(String s) {
  s.replace("\\", "\\\\");
  s.replace("\"", "\\\"");
  s.replace("\r", "\\r");
  s.replace("\n", "\\n");
  s.replace("\t", "\\t");
  // opcionális: egyéb problémás karakterek kiszűrése
  return s;
}

// ================================================================
// SOAP Browse - ONE PAGE
// ================================================================
extern SemaphoreHandle_t g_dlnaHttpMux;

bool DlnaIndex::browsePage(const String& controlUrl,
                           const String& objectId,
                           uint32_t start,
                           uint32_t count,
                           String& outDIDL,
                           uint32_t& returned,
                           uint32_t& total)
{
  returned = total = 0;
  outDIDL  = "";

  Serial.printf("[DLNA][BROWSE] mux=%p\n", (void*)g_dlnaHttpMux);

if (!controlUrl.length()) {
  Serial.println("[DLNA][BROWSE] controlUrl not ready");
  return false;
}

  bool ok = false;
  String xml;   // <-- KINT, hogy a lock után is elérd

  // ====== HTTP szakasz: CSAK EZ lockolt ======
  {
    DlnaHttpGuard lock;

    HTTPClient http;
    WiFiClient client;

    String soap = buildBrowseEnvelope(objectId, start, count);

    if (!http.begin(client, controlUrl)) {
      Serial.println("[DLNA][BROWSE] http.begin failed");
      return false;
    }

    http.addHeader("Content-Type", "text/xml; charset=\"utf-8\"");
    http.addHeader("SOAPACTION", "\"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"");

    int code = http.POST((uint8_t*)soap.c_str(), soap.length());
    if (code != HTTP_CODE_OK) {
      Serial.printf("[DLNA][BROWSE] HTTP error: %d\n", code);
      http.end();
      return false;
    }

    xml = http.getString();
    http.end();
  }
  // ====== innentől PARSE: lock nélkül ======

  // (opcionális) kis sanity
  if (!xml.length()) {
    Serial.println("[DLNA][BROWSE] empty XML");
    return false;
  }

  int nr1 = xml.indexOf("<NumberReturned>");
  int nr2 = xml.indexOf("</NumberReturned>");
  if (nr1 >= 0 && nr2 > nr1) returned = xml.substring(nr1 + 16, nr2).toInt();

  int tm1 = xml.indexOf("<TotalMatches>");
  int tm2 = xml.indexOf("</TotalMatches>");
  if (tm1 >= 0 && tm2 > tm1) total = xml.substring(tm1 + 14, tm2).toInt();

  int a = xml.indexOf("<Result>");
  int b = xml.indexOf("</Result>");
  if (a < 0 || b < 0 || b <= a) {
    // valid response but no Result block
    return true;
  }

  String didl = xml.substring(a + 8, b);
  didl.trim();

  if (didl.startsWith("<![CDATA[")) {
    didl.remove(0, 9);
    int cend = didl.lastIndexOf("]]>");
    if (cend >= 0) didl.remove(cend);
  }

  didl.replace("&lt;", "<");
  didl.replace("&gt;", ">");
  didl.replace("&quot;", "\"");
  didl.replace("&apos;", "'");
  didl.replace("&amp;", "&");

  outDIDL = didl;
  ok = true;

  return ok;
}

String DlnaIndex::buildBrowseEnvelope(const String& objectId, uint32_t start, uint32_t count) {
  String s;
  s.reserve(520);
  s += "<?xml version=\"1.0\"?>";
  s += "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" ";
  s += "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">";
  s += "<s:Body>";
  s += "<u:Browse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\">";
  s += "<ObjectID>" + objectId + "</ObjectID>";
  s += "<BrowseFlag>BrowseDirectChildren</BrowseFlag>";
  s += "<Filter>*</Filter>";
  s += "<StartingIndex>" + String(start) + "</StartingIndex>";
  s += "<RequestedCount>" + String(count) + "</RequestedCount>";
  s += "<SortCriteria></SortCriteria>";
  s += "</u:Browse>";
  s += "</s:Body></s:Envelope>";
  return s;
}

void DlnaIndex::decodeEntities(String& s) {
  s.replace("&amp;", "&");
  s.replace("&apos;", "'");
  s.replace("&quot;", "\"");
  s.replace("&lt;", "<");
  s.replace("&gt;", ">");
}

// ================================================================
// DIDL PARSERS
// ================================================================
void DlnaIndex::extractContainersFromDIDL(const String& didl, std::vector<String>& ids, std::vector<String>& titles) {
  ids.clear();
  titles.clear();

  int pos = 0;
  while ((pos = didl.indexOf("<container", pos)) >= 0) {
    int end = didl.indexOf("</container>", pos);
    if (end < 0) break;

    // id="..."
    String id, title;

    int idp = didl.indexOf("id=\"", pos);
    if (idp >= 0 && idp < end) {
      int idq = didl.indexOf("\"", idp + 4);
      if (idq > idp && idq < end) id = didl.substring(idp + 4, idq);
    }

    int t1 = didl.indexOf("<dc:title>", pos);
    int t2 = didl.indexOf("</dc:title>", t1);
    if (t1 >= 0 && t2 > t1 && t2 < end) {
      title = didl.substring(t1 + 10, t2);
      decodeEntities(title);
      title.trim();
    }

    if (id.length()) {
      ids.push_back(id);
      titles.push_back(title);
    }

    pos = end + 12;
  }
}

void DlnaIndex::extractItemsFromDIDL(const String& didl, std::vector<String>& ids, std::vector<String>& titles, std::vector<String>& urls) {
  ids.clear();
  titles.clear();
  urls.clear();

  int pos = 0;
  while ((pos = didl.indexOf("<item", pos)) >= 0) {
    int end = didl.indexOf("</item>", pos);
    if (end < 0) break;

    String id, title, url;

    int idp = didl.indexOf("id=\"", pos);
    if (idp >= 0 && idp < end) {
      int idq = didl.indexOf("\"", idp + 4);
      if (idq > idp && idq < end) id = didl.substring(idp + 4, idq);
    }

    int t1 = didl.indexOf("<dc:title>", pos);
    int t2 = didl.indexOf("</dc:title>", t1);
    if (t1 >= 0 && t2 > t1 && t2 < end) {
      title = didl.substring(t1 + 10, t2);
      decodeEntities(title);
      title.trim();
    }

    // first <res>...</res> (direct child)
    int r1 = didl.indexOf("<res", pos);
    int r2 = didl.indexOf(">", r1);
    int r3 = didl.indexOf("</res>", r2);
    if (r1 >= 0 && r2 > r1 && r3 > r2 && r3 < end) {
      url = didl.substring(r2 + 1, r3);
      url.trim();
    }

    if (title.length() && url.length()) {
      ids.push_back(id);
      titles.push_back(title);
      urls.push_back(url);
    }

    pos = end + 7;
  }
}

// ================================================================
// /dlna/list JSON (containers + items)
// IMPORTANT: dlna.html-hez ID + TITLE kell minden elemhez
// ================================================================
bool DlnaIndex::listContainer(const String& controlUrl, const String& objectId, String& outJson, uint32_t start) {
  String didl;
  uint32_t ret = 0, tot = 0;

  // 200 elég a UI-hoz, indexeléshez majd külön autoBuildPlaylist lapoz
  const uint32_t pageSize = 200;
  if (!browsePage(controlUrl, objectId, start, pageSize, didl, ret, tot)) return false;

  std::vector<String> cids, ctitles;
  extractContainersFromDIDL(didl, cids, ctitles);

  std::vector<String> iids, ititles, iurls;
  extractItemsFromDIDL(didl, iids, ititles, iurls);

  outJson.reserve(600 + (cids.size() + ititles.size()) * 80);
  outJson = String("{\"ok\":true,")
        + "\"start\":" + start + ","
        + "\"returned\":" + ret + ","
        + "\"total\":" + tot + ","
        + "\"items\":[";

  bool first = true;

  // containers first
  for (size_t i = 0; i < cids.size(); i++) {
    if (!first) outJson += ",";
    first = false;

    String t = (i < ctitles.size()) ? ctitles[i] : "";
    t = jsonEscape(t);
    String id = jsonEscape(cids[i]);
    outJson += "{\"type\":\"container\",\"id\":\"" + id + "\",\"title\":\"" + t + "\"}";
  }

  // items (tracks)
  for (size_t i = 0; i < ititles.size(); i++) {
    if (!first) outJson += ",";
    first = false;

   String t = jsonEscape(ititles[i]);
   String id = (i < iids.size()) ? jsonEscape(iids[i]) : "";
   outJson += "{\"type\":\"item\",\"id\":\"" + id + "\",\"title\":\"" + t + "\"}";
  }

  outJson += "]}";
  return true;
}

// ================================================================
// Quick decide: is there any item/container in this objectId
// ================================================================
bool DlnaIndex::browseAndDecide(const String& controlUrl, const String& objectId, bool& hasItems, bool& hasContainers) {
  hasItems = false;
  hasContainers = false;

  String didl;
  uint32_t ret = 0, tot = 0;
  if (!browsePage(controlUrl, objectId, 0, 25, didl, ret, tot)) return false;

  // lightweight checks
  if (didl.indexOf("<item") >= 0) hasItems = true;
  if (didl.indexOf("<container") >= 0) hasContainers = true;

  return true;
}

// ================================================================
// Playlist builder: deep traverse containers and append tracks
// Writes yoRadio format: title \t url \t ovol
// ================================================================
bool DlnaIndex::appendTracksToFile(const String& didl, fs::File& f, uint32_t& appended, uint32_t hardLimit) {
  std::vector<String> iids, ititles, iurls;
  extractItemsFromDIDL(didl, iids, ititles, iurls);

  for (size_t i = 0; i < ititles.size(); i++) {
    if (appended >= hardLimit) return true;
    const String& title = ititles[i];
    const String& url   = iurls[i];
    if (!title.length() || !url.length()) continue;

    String nice = beautifyTitle(title);
    // YO format: title \t url \t 0
    f.printf("%s\t%s\t0\n", nice.c_str(), url.c_str());
    appended++;
  }
  return true;
}

bool DlnaIndex::autoBuildPlaylist(const String& controlUrl,
                                  const String& objectId,
                                  uint8_t maxDepth,
                                  uint32_t hardLimit)
{
  if (hardLimit == 0) hardLimit = 20000;
  // ===== SPIFFS sanity check =====
  File ftest = SPIFFS.open(TMP_PATH, "w");
  if (!ftest) {
    Serial.println("[DLNA][PL] SPIFFS open failed");
    return false;
  }
  ftest.close();

  File f = SPIFFS.open(TMP_PATH, "w");
  if (!f) {
    Serial.println("[DLNA][PL] TMP open failed");
    return false;
  }

  uint32_t appended = 0;

  // ===== STACK NODE (NO String inside!) =====
  struct Node {
    char id[64];
    uint8_t depth;
  };

  std::vector<Node> stack;
  stack.reserve(256);

  Node root{};
  strlcpy(root.id, objectId.c_str(), sizeof(root.id));
  root.depth = 0;
  stack.push_back(root);

  // ===== DFS traversal =====
  while (!stack.empty() && appended < hardLimit) {
    Node n = stack.back();
    stack.pop_back();

    uint32_t start = 0;
    const uint32_t page = 20;

    while (true) {
      String didl;
      didl.reserve(8192);   // 🔑 heap stabilizálás

      uint32_t ret = 0, tot = 0;

      if (!browsePage(controlUrl, String(n.id), start, page, didl, ret, tot)) {
        f.close();
        return false;
      }

      // ---- append tracks ----
      if (ret > 0) {
        appendTracksToFile(didl, f, appended, hardLimit);
      }

      // ---- dive into containers ----
      if (n.depth < maxDepth) {
        std::vector<String> cids;
        cids.reserve(32);

        std::vector<String> ctitles; // nem kötelező, de extract ezt várja
        extractContainersFromDIDL(didl, cids, ctitles);

        for (size_t i = 0; i < cids.size() && appended < hardLimit; i++) {
          Node nn{};
          strlcpy(nn.id, cids[i].c_str(), sizeof(nn.id));
          nn.depth = n.depth + 1;
          stack.push_back(nn);
        }
      }

      if (ret == 0) break;

      start += ret;
      if (start >= tot) break;

      delay(5);   // ✔️ yield + watchdog barát
      if (appended >= hardLimit) break;
    }
  }

  f.close();

  Serial.printf("[DLNA][PL] Auto playlist ready (%lu tracks)\n",
                (unsigned long)appended);

  return (appended > 0);
}


// ================================================================
// buildContainerIndex: optional helper for root category cache
// ================================================================
bool DlnaIndex::buildContainerIndex(const String& controlUrl, const String& rootObjectId) {

  File ftest = SPIFFS.open(TMP_PATH, "w");
  if (!ftest) {
    Serial.println("[DLNA][PL] SPIFFS open failed");
    return false;
  }
  ftest.close();

  if (!controlUrl.length()) {
    Serial.println("[DLNA][IDX] ERROR: empty controlUrl");
    return false;
  }

  uint32_t lastYield = millis();

  String didl;
  uint32_t ret = 0, tot = 0;

  if (!browsePage(controlUrl, rootObjectId, 0, 200, didl, ret, tot))
    return false;

  // 🔑 yield browse után
  if (millis() - lastYield > 50) {
    vTaskDelay(1);
    lastYield = millis();
  }

  std::vector<String> cids, ctitles;
  extractContainersFromDIDL(didl, cids, ctitles);

  // 🔑 yield parse után
  if (millis() - lastYield > 50) {
    vTaskDelay(1);
    lastYield = millis();
  }

  File f = SPIFFS.open(DLNA_LIST_JSON_PATH, "w");
  if (!f) return false;

  f.print("{\"ok\":true,\"items\":[");

  for (size_t i = 0; i < cids.size(); i++) {
    if (i) f.print(",");

    String t = (i < ctitles.size()) ? ctitles[i] : "";
    t.replace("\"", "'");
    f.printf("{\"type\":\"container\",\"id\":\"%s\",\"title\":\"%s\"}",
             cids[i].c_str(), t.c_str());

    // 🔑 loop közbeni yield
    if ((i & 0x0F) == 0) {   // 16-onként
      vTaskDelay(1);
    }
  }

  f.print("]}");
  f.close();

  Serial.printf("[DLNA][IDX] %u containers written\n", (unsigned)cids.size());
  return true;
}


// ================================================================
// beautifyTitle  => "Album - [CD# -] TrackTitle"   (NO artist)
// ================================================================
static inline String _trimCopy(String s) { s.trim(); return s; }

static bool _looksLikeDiscToken(const String& s) {
  String t = s; t.trim();
  if (!t.length()) return false;

  String u = t; u.toUpperCase();

  // CD2, CD 2, DISC2, DISC 2, DISK2, VOL2, VOL.2, VOLUME 2
  if (u.startsWith("CD"))   return true;
  if (u.startsWith("DISC")) return true;
  if (u.startsWith("DISK")) return true;
  if (u.startsWith("VOL"))  return true;
  if (u.startsWith("VOLUME")) return true;

  return false;
}

static String _removeLeadingTrackNo(String s) {
  s.trim();
  if (!s.length()) return s;

  // "01 - " / "1 - " / "01." / "1."
  // remove leading digits + optional spaces + (" - " or ".")
  int i = 0;
  while (i < (int)s.length() && isdigit((uint8_t)s[i])) i++;
  if (i == 0) return s;

  // skip spaces
  while (i < (int)s.length() && s[i] == ' ') i++;

  // " - "
  if (i + 2 < (int)s.length() && s[i] == '-' ) {
    // handle "- " or "-   "
    i++;
    while (i < (int)s.length() && s[i] == ' ') i++;
    return _trimCopy(s.substring(i));
  }

  // "."
  if (i < (int)s.length() && s[i] == '.') {
    i++;
    while (i < (int)s.length() && s[i] == ' ') i++;
    return _trimCopy(s.substring(i));
  }

  return s;
}

static String _stripExtension(String s) {
  int dot = s.lastIndexOf('.');
  if (dot > 0) s = s.substring(0, dot);
  s.trim();
  return s;
}

static String _stripTrailingParenGroup(String s) {
  // remove trailing " (....)" if it's at the end
  s.trim();
  if (!s.endsWith(")")) return s;

  int lp = s.lastIndexOf(" (");
  int rp = s.lastIndexOf(')');
  if (lp >= 0 && rp == (int)s.length() - 1 && rp > lp + 2) {
    s = s.substring(0, lp);
    s.trim();
  }
  return s;
}

static void _splitBySlash(const String& in, String& left, String& right) {
  int slash = in.lastIndexOf('/');
  if (slash >= 0) {
    left  = in.substring(0, slash);
    right = in.substring(slash + 1);
  } else {
    left = in;
    right = "";
  }
  left.trim();
  right.trim();
}

static void _splitByDashTokens(const String& in, std::vector<String>& out) {
  out.clear();
  String s = in;
  s.trim();
  int from = 0;
  while (true) {
    int p = s.indexOf(" - ", from);
    if (p < 0) {
      String tail = s.substring(from);
      tail.trim();
      if (tail.length()) out.push_back(tail);
      break;
    }
    String part = s.substring(from, p);
    part.trim();
    if (part.length()) out.push_back(part);
    from = p + 3;
  }
}

String DlnaIndex::beautifyTitle(const String& raw) {
  String t = raw;
  t.trim();
  if (!t.length()) return t;

  // Normalize double spaces
  while (t.indexOf("  ") >= 0) t.replace("  ", " ");

  // Split by last '/'
  String left, right;
  _splitBySlash(t, left, right);

  // Track title candidate:
  // - If we have "01 - Title/..." => left has the track title part
  // - If FLAC filename: "Artist - Title (...).flac" is in left (or in t when no slash)
  String track = left.length() ? left : t;
  track = _stripExtension(track);
  track = _removeLeadingTrackNo(track);

  // If it's "Artist - Title" format, keep ONLY the "Title" part
  int dash = track.indexOf(" - ");
  if (dash >= 0) {
    track = track.substring(dash + 3);
    track.trim();
  }

  // Remove trailing " (....)" release tag from track title (FLAC case)
  track = _stripTrailingParenGroup(track);

  // Album candidate:
  // - If there is a right side (after '/'), that's usually album-ish
  // - Else fallback: try to derive from "Artist - Album" patterns (rare here)
  String album = right;

  // If right side contains "Artist - Album - CD2" or similar: take LAST meaningful token as album
  if (album.length()) {
    std::vector<String> toks;
    _splitByDashTokens(album, toks);

    if (toks.size() >= 2) {
      // last may be disc token
      String last = toks[toks.size() - 1];
      bool hasDisc = _looksLikeDiscToken(last);

      String disc = hasDisc ? last : "";
      String alb  = hasDisc ? toks[toks.size() - 2] : last;

      alb.trim();
      disc.trim();

      // result album part: "Album" or "Album - CD2"
      album = alb;
      if (disc.length()) album += " - " + disc;
    } else {
      // single token => treat as album as-is
      album.trim();
    }
  }

  // If still no album (no slash case), keep original behavior minimally
  if (!album.length()) {
    // last-resort: if original has " - " tokens, take the last token as album
    std::vector<String> toks;
    _splitByDashTokens(t, toks);
    if (toks.size() >= 2) {
      album = toks.back();
      album.trim();
    }
  }

  // Compose: "Album - Track"
  if (album.length() && track.length()) {
    return album + " - " + track;
  }

  // Fallback: just track
  if (track.length()) return track;

  return t;
}


#endif // USE_DLNA

