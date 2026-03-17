#include "options.h"
//#if DSP_MODEL == DSP_ILI9488

    #include "presets.h"

    #include <Arduino.h>
    #include <Preferences.h>

    #include "config.h"
    #include "display.h"
    #include "player.h"

    #include "../displays/dspcore.h"
    #include "../displays/tools/l10n.h"
    #include "../displays/tools/utf8To.h"
    #if (DSP_WIDTH == 480) && (DSP_HEIGHT == 320)
    #define GFX (*s_presetsCanvas)

static inline const char* uiText(const String& s, bool upper = false) {
    return utf8To(s.c_str(), upper);
}

static inline const char* uiText(const char* s, bool upper = false) {
    return utf8To(s, upper);
}

static bool     holdBarReady = false;
static int16_t  hold_xPlay, hold_wPlay;
static int16_t  hold_xSave, hold_wSave;
static int16_t  hold_xDel, hold_wDel;
static int16_t  hold_barY, hold_barH;
static String   toastText;
static uint32_t s_toastTopbarUntil = 0;
static int8_t   s_flashSlot = -1;
static uint32_t s_flashUntil = 0;

// ============================================ Presets offscreen framebuffer ============================
static Canvas* s_presetsCanvas = nullptr;
static int16_t s_presetsW = 0;
static int16_t s_presetsH = 0;

static void ensurePresetsCanvas() {
    int16_t w = dsp.width();
    int16_t h = dsp.height();

    if (!s_presetsCanvas || s_presetsW != w || s_presetsH != h) {
        if (s_presetsCanvas) {
            delete s_presetsCanvas;
            s_presetsCanvas = nullptr;
        }
        s_presetsCanvas = new Canvas(w, h); // RGB565 buffer (PSRAM)
        s_presetsW = w;
        s_presetsH = h;
    }
}

// Full-frame blit: Canvas -> LCD (egyben, mint a VU-widgetnél)
static void presetsBlitFull() {
    if (!s_presetsCanvas) { return; }

    uint32_t pixels = (uint32_t)s_presetsW * (uint32_t)s_presetsH;

    dsp.startWrite();
    dsp.setAddrWindow(0, 0, s_presetsW, s_presetsH);
    dsp.writePixels((uint16_t*)s_presetsCanvas->getBuffer(), pixels);
    dsp.endWrite();
}

// Rect blit: csak egy téglalap frissítése a full-frame bufferből
static void presetsBlitRect(int16_t x, int16_t y, int16_t w, int16_t h) {
    if (!s_presetsCanvas) { return; }

    // clip
    if (x < 0) {
        w += x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        y = 0;
    }
    if (x + w > s_presetsW) { w = s_presetsW - x; }
    if (y + h > s_presetsH) { h = s_presetsH - y; }
    if (w <= 0 || h <= 0) { return; }

    uint16_t* buf = (uint16_t*)s_presetsCanvas->getBuffer();

    dsp.startWrite();
    for (int16_t row = 0; row < h; row++) {
        uint16_t* p = buf + (uint32_t)(y + row) * (uint32_t)s_presetsW + (uint32_t)x;
        dsp.setAddrWindow(x, y + row, w, 1);
        dsp.writePixels(p, w);
    }
    dsp.endWrite();
}

// ---- SZÉLESSÉGMÉRÉS (mindig explicit textSize!) ----
static uint16_t textW(const char* s, uint8_t size) {
    if (!s) { return 0; }
    dsp.setTextSize(size); // KRITIKUS: ne függjön a dsp korábbi állapotától
    int16_t  x1, y1;
    uint16_t w, h;
    dsp.getTextBounds(s, 0, 0, &x1, &y1, &w, &h);
    return w;
}

static uint16_t textW_ui_sz(const char* s, uint8_t size) {
    const char* t = uiText(s);
    return textW(t, size);
}

// -------------------- Storage --------------------
static Preferences s_prefs;
static bool        s_inited = false;

static void ensurePrefs() {
    if (s_inited) { return; }
    s_prefs.begin("presets", false); // NVS namespace
    s_inited = true;
}
static const uint8_t SLOTS = 8; // A memóriagombok száma.
static char          presetName[SLOTS][64];
static char          presetUrl[SLOTS][256];
static uint16_t      presetId[SLOTS];
static bool          s_cacheValid = false; // cache érvényesség

// 6 banks (FAV1..FAV6), each has 12 presets
static uint8_t s_bank = 0;          // 0..4 (active)
static bool    s_kbdActive = false; // on-screen keyboard visible
static uint8_t s_kbdFav = 255;      // which fav is being edited
static String  s_kbdText;           // current edit buffer

// -------------------- Layout --------------------
static const uint16_t TOP_H = 40;
static const uint16_t FAV_H = 38;
static const uint8_t  FAVS = 5;
static const uint16_t GRID_FAV_GAP = 12;
static const uint16_t FAV_GAP = 9;
static const uint16_t FAV_BOTTOM_PAD = 5;
static const uint8_t  COLS = 2;
static const uint8_t  ROWS = 4;
static const uint8_t  KBD_MAX = 10; // Billentyűzet max beírható karakter.
static const uint16_t UI_MARGIN = 5;
static const uint16_t GAP = 10;
static const uint16_t Y0_PAD = 10;
static const uint16_t TOPBAR_MARGIN = 5;
static const uint16_t TOPBAR_Y = 2;
static const uint16_t TOPBAR_H = TOP_H;
static const uint16_t TOPBAR_BOTTOM = TOPBAR_Y + TOPBAR_H;

static inline int16_t FAV_TOP() {
    return dsp.height() - FAV_H - 3;
}

// -------------------- Helpers --------------------
static int s_pressedSlot = -1;
static int s_lastDrawnPressed = -1;

void presets_setPressedSlot(int slot) {
    s_pressedSlot = slot;
}

static void makeKey(char* out, size_t outSz, uint8_t bank, uint8_t slot, const char* suffix) {
    if (bank == 0) {
        snprintf(out, outSz, "p%u_%s", slot, suffix);
    } else {
        snprintf(out, outSz, "b%u_p%u_%s", bank, slot, suffix);
    }
}

static void loadBankCache() {
    ensurePrefs();
    sanitizePresets(s_bank);
    for (uint8_t i = 0; i < SLOTS; i++) {
        presetName[i][0] = 0;
        presetUrl[i][0] = 0;
        presetId[i] = 0;
        char key[20];
        makeKey(key, sizeof(key), s_bank, i, "name");
        if (s_prefs.isKey(key)) {
            s_prefs.getString(key, presetName[i], sizeof(presetName[i]));
        } else {
            presetName[i][0] = 0;
        }
        makeKey(key, sizeof(key), s_bank, i, "url");
        if (s_prefs.isKey(key)) {
            s_prefs.getString(key, presetUrl[i], sizeof(presetUrl[i]));
        } else {
            presetUrl[i][0] = 0;
        }
        makeKey(key, sizeof(key), s_bank, i, "id");
        presetId[i] = s_prefs.getUShort(key, 0);
    }
    s_cacheValid = true;
}

// Removing faulty remnants from NVS memory.
void sanitizePresets(uint8_t bank) {
    Preferences prefs;
    prefs.begin("presets", false);                 // NVS open "presets"
    for (uint8_t slot = 0; slot < SLOTS; slot++) { // 0 - 7 slots
        char keyName[20], keyUrl[20], keyId[20];
        // Generate key e.g. b2_p5_name .
        makeKey(keyName, sizeof(keyName), bank, slot, "name");
        makeKey(keyUrl, sizeof(keyUrl), bank, slot, "url");
        makeKey(keyId, sizeof(keyId), bank, slot, "id");
        // Read data from NVS.
        String   name = prefs.getString(keyName, "");
        String   url = prefs.getString(keyUrl, "");
        uint16_t id = prefs.getUShort(keyId, 0);
        // Ha üresnek látszik, de ID van → ez egy szellem preset
        if (name.length() == 0 && url.length() == 0 && id != 0) {
            Serial.printf("Ghost preset fixed: bank %u slot %u (id=%u)\n", bank, slot, id);
            prefs.remove(keyId); // Törli a hibás bejegyzést az NVS memóriából.
        }
    }
    prefs.end();
}

// -------------------- Color helpers (RGB565) for gradients --------------------
static inline uint16_t blend565(uint16_t c1, uint16_t c2, uint8_t t) {
    uint16_t r1 = (c1 >> 11) & 0x1F, g1 = (c1 >> 5) & 0x3F, b1 = c1 & 0x1F;
    uint16_t r2 = (c2 >> 11) & 0x1F, g2 = (c2 >> 5) & 0x3F, b2 = c2 & 0x1F;
    uint16_t r = (uint16_t)((r1 * (255 - t) + r2 * t + 127) / 255);
    uint16_t g = (uint16_t)((g1 * (255 - t) + g2 * t + 127) / 255);
    uint16_t b = (uint16_t)((b1 * (255 - t) + b2 * t + 127) / 255);
    return (uint16_t)((r << 11) | (g << 5) | b);
}

static inline uint16_t lighten565(uint16_t c, uint8_t t) {
    return blend565(c, 0xFFFF, t);
}

static inline uint16_t darken565(uint16_t c, uint8_t t) {
    return blend565(c, 0x0000, t);
}

// -------------------- Two-line label (top + bottom), OEM style --------------------
static String normalizeSpaces(String s) {
    s.trim();
    while (s.indexOf("  ") >= 0) { s.replace("  ", " "); }
    return s;
}

static String fitToWidth(String s, int16_t maxW) {
    s = normalizeSpaces(s);
    if (textW(s.c_str(), 2) <= (uint16_t)maxW) { return s; }
    const char* dots = "...";
    while (s.length() > 0) {
        String t = s + dots;
        if (textW(t.c_str(), 2) <= (uint16_t)maxW) { return t; }
        s.remove(s.length() - 1);
        s.trim();
    }
    return String(dots);
}

// A gombon két sorba tördeli a szöveget.
static void splitTwoLinesBalanced(const String& src, int16_t maxW, String& top, String& bottom) {
    String s = normalizeSpaces(src);
    if (textW(s.c_str(), 2) <= (uint16_t)maxW) {
        top = s;
        bottom = "";
        return;
    }
    // split into words (limit to 10 for safety)
    String  wds[10];
    uint8_t wc = 0;
    int     start = 0;
    for (int i = 0; i <= (int)s.length(); i++) {
        if (i == (int)s.length() || s[i] == ' ') {
            if (i > start && wc < 10) { wds[wc++] = s.substring(start, i); }
            start = i + 1;
        }
    }
    if (wc == 0) {
        top = fitToWidth(s, maxW);
        bottom = "";
        return;
    }
    if (wc == 1) {
        top = fitToWidth(wds[0], maxW);
        bottom = "";
        return;
    }
    int bestK = 1;
    int bestScore = 100000;
    for (int k = 1; k < wc; k++) {
        String a, b;
        for (int i = 0; i < k; i++) {
            if (i) { a += ' '; }
            a += wds[i];
        }
        for (int i = k; i < wc; i++) {
            if (i != k) { b += ' '; }
            b += wds[i];
        }
        int score = max((int)textW(a.c_str(), 2), (int)textW(b.c_str(), 2));
        if (score < bestScore) {
            bestScore = score;
            bestK = k;
        }
    }
    top = "";
    bottom = "";
    for (int i = 0; i < bestK; i++) {
        if (i) { top += ' '; }
        top += wds[i];
    }
    for (int i = bestK; i < wc; i++) {
        if (i != bestK) { bottom += ' '; }
        bottom += wds[i];
    }
    // csak akkor vágjuk, ha pixelben túl hosszú – egyetlen ponttal jelezzük
    auto trimWithDot = [&](String& s) {
        bool cut = false;
        while (textW(s.c_str(), 2) > (uint16_t)maxW && s.length()) {
            s.remove(s.length() - 1);
            cut = true;
        }
        if (cut && s.length()) {
            if (s[s.length() - 1] != '.') { s += '.'; }
        }
    };
    trimWithDot(top);
    trimWithDot(bottom);
}

static String keyFavSel() {
    return String("fav_sel");
}

static String keyFavLabel(uint8_t fav) {
    return String("fav") + String(fav) + String("_label");
}

static void readFavLabelC(uint8_t fav, char* out, size_t outSz) {
    ensurePrefs();
    char key[16];
    snprintf(key, sizeof(key), "fav%u_label", fav);
    if (s_prefs.isKey(key)) {
        s_prefs.getString(key, out, outSz);
    } else {
        snprintf(out, outSz, "FAV%u", fav + 1);
    }
}

static void writeFavLabel(uint8_t fav, const String& label) {
    ensurePrefs();
    String t = label;
    t.trim();
    if (t.length() == 0) { t = String("FAV") + String(fav + 1); }
    // allow long labels; UI will wrap/shrink as needed
    if (t.length() > 64) { t = t.substring(0, 64); }
    s_prefs.putString(keyFavLabel(fav).c_str(), t);
}

static void saveBank() {
    ensurePrefs();
    s_prefs.putUChar(keyFavSel().c_str(), s_bank);
}

static inline const char* readNameC(uint8_t slot) {
    if (!s_cacheValid) { loadBankCache(); }
    if (presetName[slot][0] == 0) { return "---"; }
    return presetName[slot];
}

static inline const char* readUrlC(uint8_t slot) {
    if (!s_cacheValid) { loadBankCache(); }
    return presetUrl[slot];
}

static inline uint16_t readIdC(uint8_t slot) {
    if (!s_cacheValid) { loadBankCache(); }
    return presetId[slot];
}

// Try to map a preset URL back to playlist station index (1-based).
// This keeps pause/play and NEXT/PREV behavior consistent.
static uint16_t findStationIdByUrl(const String& url) {
    if (url.length() == 0) { return 0; }
    File playlist = config.SDPLFS()->open(REAL_PLAYL, "r");
    if (!playlist) { return 0; }
    uint16_t id = 1;
    while (playlist.available()) {
        String line = playlist.readStringUntil('\n');
        line.trim();
        if (line.length() == 0) { continue; }
        char nameBuf[192];
        char urlBuf[512];
        int  ovol = 0;
        if (config.parseCSV(line.c_str(), nameBuf, urlBuf, ovol)) {
            if (url == String(urlBuf)) {
                playlist.close();
                return id;
            }
            id++;
        }
    }
    playlist.close();
    return 0;
}

static void drawBackground() {
    GFX.fillScreen(config.theme.background);
}

// A felső bár megrajzolása.
static void drawTopBar() {
    int16_t w = GFX.width();
    GFX.fillRect(0, 0, w, TOP_H, config.theme.metabg);
    GFX.drawFastHLine(0, TOP_H - 1, w, config.theme.prst_line);
    GFX.setTextSize(2);
    GFX.setTextColor(config.theme.prst_title1);
    static char p1[32], p2[32], p3[32];
    strncpy_P(p1, LANG::prstPlay, sizeof(p1) - 1);
    strncpy_P(p2, LANG::prstSave, sizeof(p2) - 1);
    strncpy_P(p3, LANG::prstDel, sizeof(p3) - 1);
    p1[sizeof(p1) - 1] = 0;
    p2[sizeof(p2) - 1] = 0;
    p3[sizeof(p3) - 1] = 0;
    hold_xPlay = 30;
    hold_wPlay = textW_ui_sz(p1, 2);
    hold_xSave = w / 2 - 20;
    hold_wSave = textW_ui_sz(p2, 2);
    hold_wDel = textW_ui_sz(p3, 2);
    hold_xDel = w - hold_wDel - 30;
    hold_barH = 3;
    hold_barY = TOPBAR_Y + 12 + 16;
    holdBarReady = true;
    int16_t y = 12;
    GFX.setCursor(30, y);
    GFX.print(utf8To(p1, false));
    GFX.setCursor(w / 2 - 20, y);
    GFX.print(utf8To(p2, false));
    GFX.setCursor(hold_xDel, y);
    GFX.print(utf8To(p3, false));
}

static void cancelTopToastAndRestore() {
    if (!s_toastTopbarUntil) {
        return; // nincs toast → nincs mit csinálni
    }
    s_toastTopbarUntil = 0; // toast megszüntetése
    ensurePresetsCanvas();
    drawTopBar();                             // Play / Save / Del újrarajzolása a canvasba
    presetsBlitRect(0, 0, s_presetsW, TOP_H); // csak a topbar frissül
}

// A kártyalap nyomásakor az időcsík megjelenítése az aktuális funkció alatt.
void presets_drawHoldBar(uint32_t heldMs) {
    // 🔹 HA VAN TOAST → AZONNAL MEGSZÜNTETJÜK
    cancelTopToastAndRestore();
    if (!holdBarReady) { return; }
    ensurePresetsCanvas();
    auto&          gfx = *s_presetsCanvas;
    const uint32_t T_PLAY = BTN_PRESS_TICKS * 2;
    const uint32_t T_SAVE = BTN_PRESS_TICKS * 4;
    int16_t        fullX = hold_xPlay;
    int16_t        fullW = (hold_xDel + hold_wDel) - hold_xPlay;
    // törlés a canvasban
    gfx.fillRect(fullX, hold_barY, fullW, hold_barH, config.theme.background);
    int16_t  sx, sw;
    uint32_t segStart, segLen;
    if (heldMs < T_PLAY) {
        sx = hold_xPlay;
        sw = hold_wPlay;
        segStart = 0;
        segLen = T_PLAY;
    } else if (heldMs < T_SAVE) {
        sx = hold_xSave;
        sw = hold_wSave;
        segStart = T_PLAY;
        segLen = (T_SAVE - T_PLAY);
    } else {
        sx = hold_xDel;
        sw = hold_wDel;
        segStart = T_SAVE;
        segLen = 2000;
    }
    uint32_t t = (heldMs > segStart) ? (heldMs - segStart) : 0;
    if (t > segLen) { t = segLen; }
    int16_t filled = (int16_t)((uint32_t)sw * t / segLen);
    gfx.fillRect(sx, hold_barY, filled, hold_barH, config.theme.prst_fav);
    // CSAK a hold-bar téglalapot küldjük ki a kijelzőre
    presetsBlitRect(fullX, hold_barY, fullW, hold_barH);
}

// A memóriagombok méreteinek számítása.
static void slotRect(uint8_t slot, int16_t& x, int16_t& y, int16_t& bw, int16_t& bh) {
    const int16_t M = 12;
    const int16_t GAP = 10;
    const int16_t HEADER = TOP_H;
    const int16_t FAV = FAV_H + FAV_BOTTOM_PAD;
    int16_t       W = dsp.width();
    int16_t       H = dsp.height();
    int16_t       usableW = W - 2 * M;
    int16_t       usableH = H - HEADER - FAV - 2 * M;
    bw = (usableW - GAP) / 2;
    bh = (usableH - 3 * GAP) / 4;
    uint8_t col = slot % 2;
    uint8_t row = slot / 2;
    x = M + col * (bw + GAP);
    y = M + HEADER + row * (bh + GAP);
}

// A FAV gombok rajzolása.
static void favRect(uint8_t fav, int16_t& x, int16_t& y, int16_t& bw, int16_t& bh) {
    int16_t W = dsp.width();
    bh = FAV_H;
    y = FAV_TOP();
    int16_t usableW = W - UI_MARGIN * 2;
    int16_t gap = 8;
    int16_t avail = usableW - (FAVS - 1) * gap;
    int16_t base = avail / FAVS;
    int16_t rem = avail % FAVS;
    bw = base + (fav < rem ? 1 : 0);
    x = UI_MARGIN;
    for (uint8_t i = 0; i < fav; i++) { x += base + (i < rem ? 1 : 0) + gap; }
}

static void drawSlot(uint8_t slot, bool pressed = false, bool savedFlash = false) {
    int16_t x, y, bw, bh;
    slotRect(slot, x, y, bw, bh);
    bool     hasPreset = false;
    uint16_t id = readIdC(slot);
    if (id >= 1 && id <= config.playlistLength()) { hasPreset = true; }
    uint16_t bg = config.theme.prst_card;
    if (hasPreset) { // van mentett preset
        bg = config.theme.prst_accent;
    }
    if (pressed) {
        bg = lighten565(bg, 40); // nyomásra csak világosít
    }
    // --- kártya ---
    GFX.fillRoundRect(x, y, bw, bh, 10, bg);
    GFX.drawRoundRect(x, y, bw, bh, 10, darken565(bg, 40));
    const char* name = readNameC(slot);
    bool        empty = (name[0] == 0 || strcmp(name, "---") == 0);
    if (empty) {
        GFX.setTextSize(2);
        GFX.setTextColor(0x8410);
        GFX.setCursor(x + 16, y + bh / 2 - 8);
        char msg[32];
        strncpy_P(msg, LANG::prstEmptyPreset, sizeof(msg) - 1);
        msg[sizeof(msg) - 1] = 0;
        GFX.print(utf8To(msg, false));
        return;
    }
    // ---- Két soros állomásnév ----
    GFX.setTextSize(2);
    String top, bottom;
    splitTwoLinesBalanced(name, bw - 40, top, bottom);
    // főcím
    GFX.setTextSize(2);
    GFX.setTextColor(config.theme.prst_title1);
    GFX.setCursor(x + 16, y + 10);
    GFX.print(uiText(top));
    // alcím
    if (bottom.length()) {
        GFX.setTextSize(1);
        GFX.setTextColor(config.theme.prst_title2);
        GFX.setCursor(x + 16, y + 32);
        GFX.print(uiText(bottom));
    }
}

static void drawFav(uint8_t fav) {
    int16_t x, y, bw, bh;
    favRect(fav, x, y, bw, bh);
    bool active = (fav == s_bank);
    GFX.fillRect(x, y, bw, bh, config.theme.background);
    GFX.drawFastHLine(x, y + bh - 1, bw, active ? config.theme.prst_fav : config.theme.prst_line);
    char lbl[32];
    readFavLabelC(fav, lbl, sizeof(lbl));
    uint8_t size = 2;
    GFX.setTextSize(size);
    uint16_t tw = textW_ui_sz(lbl, 2);
    if (tw > bw - 8) {
        size = 1;
        tw = textW_ui_sz(lbl, 1);
    }
    GFX.setTextColor(active ? config.theme.prst_fav : config.theme.prst_title3);
    int16_t tx = x + (bw - tw) / 2;
    int16_t ty = y + (bh - (size == 2 ? 16 : 8)) / 2;
    GFX.setCursor(tx, ty);
    GFX.print(uiText(lbl));
}

static void drawFavBar() {
    // FAV sáv háttere (fekete + felső elválasztó vonal)
    int16_t y = FAV_TOP();
    GFX.fillRect(0, y, GFX.width(), FAV_H, config.theme.background);
    GFX.drawFastHLine(0, y, GFX.width(), config.theme.prst_line);
    // FAV gombok
    for (uint8_t i = 0; i < FAVS; i++) { drawFav(i); }
}

static void drawToastTopBar(const char* msg) {
    ensurePresetsCanvas();
    auto&   gfx = *s_presetsCanvas;
    int16_t w = s_presetsW;
    // teljes topbar a CANVAS-ban
    gfx.fillRect(0, 0, w, TOP_H, config.theme.metabg);
    // slider sáv törlése is a CANVAS-ban
    gfx.fillRect(hold_xPlay, hold_barY, (hold_xDel + hold_wDel) - hold_xPlay, hold_barH, config.theme.background);
    gfx.drawFastHLine(0, TOP_H - 1, w, config.theme.prst_line);
    gfx.setTextSize(2);
    gfx.setTextColor(config.theme.prst_title1);
    uint16_t tw = textW_ui_sz(msg, 2);
    int16_t  tx = (w - tw) / 2;
    gfx.setCursor(tx, 12);
    gfx.print(utf8To(msg, false));
    s_toastTopbarUntil = millis() + 2000;
    // csak a topbar sáv kerül ki a kijelzőre
    presetsBlitRect(0, 0, w, TOP_H);
}

// ------------------------------------------------------------- Keyboard ------------------------------------------------------------------------
struct Key {
    const char* label;
    uint8_t     kind; // 0=char, 1=back, 2=space, 3=ok, 4=cancel
    char        ch;
};

static const Key ROW0[] = {{"0", 0, '0'}, {"1", 0, '1'}, {"2", 0, '2'}, {"3", 0, '3'}, {"4", 0, '4'}, {"5", 0, '5'}, {"6", 0, '6'}, {"7", 0, '7'}, {"8", 0, '8'}, {"9", 0, '9'}};
static const Key ROW1[] = {{"Q", 0, 'Q'}, {"W", 0, 'W'}, {"E", 0, 'E'}, {"R", 0, 'R'}, {"T", 0, 'T'}, {"Y", 0, 'Y'}, {"U", 0, 'U'}, {"I", 0, 'I'}, {"O", 0, 'O'}, {"P", 0, 'P'}};
static const Key ROW2[] = {{"A", 0, 'A'}, {"S", 0, 'S'}, {"D", 0, 'D'}, {"F", 0, 'F'}, {"G", 0, 'G'}, {"H", 0, 'H'}, {"J", 0, 'J'}, {"K", 0, 'K'}, {"L", 0, 'L'}};
static const Key ROW3[] = {{"Z", 0, 'Z'}, {"X", 0, 'X'}, {"C", 0, 'C'}, {"V", 0, 'V'}, {"B", 0, 'B'}, {"N", 0, 'N'}, {"M", 0, 'M'}, {"<-", 1, 0}};

static int16_t kbdX0() {
    return TOPBAR_MARGIN;
}

static int16_t kbdY0() {
    return (int16_t)(TOPBAR_BOTTOM + 10);
}

static int16_t kbdW() {
    return (int16_t)(s_presetsW - 2 * TOPBAR_MARGIN);
}

static int16_t kbdH() {
    return (int16_t)(s_presetsH - kbdY0() - 10);
}

// A FAV felirat szerkesztő box.
static void drawInputBox() {
    ensurePresetsCanvas();
    auto&    gfx = *s_presetsCanvas;
    uint16_t w = s_presetsW;
    int16_t  bw = (int16_t)(w - TOPBAR_MARGIN * 2);
    int16_t  bh = 40;
    gfx.fillRect(TOPBAR_MARGIN, TOPBAR_Y, bw, bh, config.theme.metabg);
    gfx.drawRect(TOPBAR_MARGIN, TOPBAR_Y, bw, bh, config.theme.div);
    gfx.setTextSize(2);
    gfx.setTextColor(0xFFFF, config.theme.metabg);
    String show = s_kbdText;
    if (show.length() > KBD_MAX) { show = show.substring(0, KBD_MAX); }
    if (show.length() == 0) { show = "_"; }
    gfx.setCursor(TOPBAR_MARGIN + 8, TOPBAR_Y + 12);
    gfx.print(show.c_str());
    // csak az input box frissül
    presetsBlitRect(TOPBAR_MARGIN, TOPBAR_Y, bw, bh);
}

static void keyRowMetrics(int totalW, int keys, int gap, int& baseW, int& rem) {
    int avail = totalW - (keys - 1) * gap;
    baseW = avail / keys;
    rem = avail % keys;
}

static void drawKeyboard() {
    ensurePresetsCanvas();
    auto& gfx = *s_presetsCanvas;
    gfx.fillScreen(config.theme.background);
    drawInputBox();
    int16_t x0 = kbdX0();
    int16_t y0 = kbdY0();
    int16_t W = kbdW();
    int16_t H = kbdH();
    int16_t keyH = (H - 3 * 8) / 5;
    int16_t gap = 8;
    auto    drawKey = [&](int16_t x, int16_t y, int16_t w, int16_t h, const char* lbl, bool special = false) {
        uint16_t bg = special ? config.theme.prst_fav : config.theme.prst_button;
        uint16_t border = darken565(bg, 40);
        gfx.setTextColor(config.theme.prst_title1);
        gfx.fillRoundRect(x, y, w, h, 8, bg);
        gfx.drawRoundRect(x, y, w, h, 8, border);
        gfx.setTextSize(2);
        uint16_t tw = textW(lbl, 2);
        int16_t  tx = x + (w - (int16_t)tw) / 2;
        int16_t  ty = y + (h - 16) / 2;
        gfx.setCursor(tx, ty);
        gfx.print(lbl);
    };
    int     base, rem;
    int16_t x;
    // Row 0
    keyRowMetrics(W, 10, gap, base, rem);
    x = x0;
    for (int i = 0; i < 10; i++) {
        int w = base + (i < rem ? 1 : 0);
        drawKey(x, y0, w, keyH, ROW0[i].label);
        x += w + gap;
    }
    // Row 1
    int16_t y1 = y0 + keyH + gap;
    keyRowMetrics(W, 10, gap, base, rem);
    x = x0;
    for (int i = 0; i < 10; i++) {
        int w = base + (i < rem ? 1 : 0);
        drawKey(x, y1, w, keyH, ROW1[i].label);
        x += w + gap;
    }
    // Row 2
    int16_t y2 = y1 + keyH + gap;
    keyRowMetrics(W, 9, gap, base, rem);
    x = x0;
    for (int i = 0; i < 9; i++) {
        int w = base + (i < rem ? 1 : 0);
        drawKey(x, y2, w, keyH, ROW2[i].label);
        x += w + gap;
    }
    // Row 3
    int16_t y3 = y2 + keyH + gap;
    keyRowMetrics(W, 8, gap, base, rem);
    x = x0;
    for (int i = 0; i < 8; i++) {
        int w = base + (i < rem ? 1 : 0);
        drawKey(x, y3, w, keyH, ROW3[i].label);
        x += w + gap;
    }
    // Row 4
    int16_t     y4 = y3 + keyH + gap;
    int16_t     wSpace = (W * 55) / 100;
    int16_t     wOk = (W * 18) / 100;
    int16_t     wCan = W - wSpace - wOk - 2 * gap;
    static char kSpace[16], kOk[16], kCancel[16];
    strncpy_P(kSpace, LANG::prstSpace, sizeof(kSpace) - 1);
    strncpy_P(kOk, LANG::prstOk, sizeof(kOk) - 1);
    strncpy_P(kCancel, LANG::prstCancel, sizeof(kCancel) - 1);
    kSpace[15] = 0;
    kOk[15] = 0;
    kCancel[15] = 0;
    drawKey(x0, y4, wSpace, keyH, utf8To(kSpace, false));
    #ifdef THEME_GRAY
    drawKey(x0 + wSpace + gap, y4, wOk, keyH, utf8To(kOk, false), false);
    #else
    drawKey(x0 + wSpace + gap, y4, wOk, keyH, utf8To(kOk, false), true);
    #endif
    drawKey(x0 + wSpace + gap + wOk + gap, y4, wCan, keyH, utf8To(kCancel, false));
    // teljes billentyűzet egy frame-ben megy ki
    presetsBlitFull();
}

// A numerikus billentyűzet érintésének kezelése.
static bool hitKeyRow(int16_t tx, int16_t ty, int16_t rx, int16_t ry, int keyCount, int totalW, int gap, int keyH, const Key* row, Key& out) {
    int base, rem;
    keyRowMetrics(totalW, keyCount, gap, base, rem);
    int16_t x = rx;
    for (int i = 0; i < keyCount; i++) {
        int w = base + (i < rem ? 1 : 0);
        if (tx >= x && tx < x + w && ty >= ry && ty < ry + keyH) {
            out = row[i];
            return true;
        }
        x += w + gap;
    }
    return false;
}

static bool keyboardTap(uint16_t x, uint16_t y) {
    int16_t x0 = kbdX0();
    int16_t y0 = kbdY0();
    int16_t W = kbdW();
    int16_t H = kbdH();
    int16_t keyH = (H - 4 * 8) / 5;
    int16_t gap = 8;
    Key     k;
    // Row 0 – numbers
    if (hitKeyRow(x, y, x0, y0, 10, W, gap, keyH, ROW0, k)) {
        if (s_kbdText.length() < KBD_MAX) { s_kbdText += k.ch; }
        return true;
    }
    // Row 1 – QWERTY
    int16_t y1 = y0 + keyH + gap;
    if (hitKeyRow(x, y, x0, y1, 10, W, gap, keyH, ROW1, k)) {
        if (s_kbdText.length() < KBD_MAX) { s_kbdText += k.ch; }
        return true;
    }
    // Row 2 – ASDF
    int16_t y2 = y1 + keyH + gap;
    if (hitKeyRow(x, y, x0, y2, 9, W, gap, keyH, ROW2, k)) {
        if (s_kbdText.length() < KBD_MAX) { s_kbdText += k.ch; }
        return true;
    }
    // Row 3 – ZXC
    int16_t y3 = y2 + keyH + gap;
    if (hitKeyRow(x, y, x0, y3, 8, W, gap, keyH, ROW3, k)) {
        if (k.kind == 1) {
            if (s_kbdText.length()) { s_kbdText.remove(s_kbdText.length() - 1); }
        } else {
            if (s_kbdText.length() < KBD_MAX) { s_kbdText += k.ch; }
        }
        return true;
    }
    // Row 4
    int16_t y4 = y3 + keyH + gap;
    int16_t wSpace = (W * 55) / 100;
    int16_t wOk = (W * 18) / 100;
    int16_t wCan = W - wSpace - wOk - 2 * gap;
    // Space
    if (x >= x0 && x < x0 + wSpace && y >= y4 && y < y4 + keyH) {
        if (s_kbdText.length() < KBD_MAX) {
            if (s_kbdText.length() > 0 && s_kbdText[s_kbdText.length() - 1] != ' ') { s_kbdText += ' '; }
        }
        return true;
    }
    // OK
    int16_t xOk = x0 + wSpace + gap;
    if (x >= xOk && x < xOk + wOk && y >= y4 && y < y4 + keyH) {
        writeFavLabel(s_kbdFav, s_kbdText);
        s_kbdActive = false;
        s_kbdFav = 255;
        s_kbdText = "";
        // back to presets grid
        return true;
    }
    // Cancel
    int16_t xCan = xOk + wOk + gap;
    if (x >= xCan && x < xCan + wCan && y >= y4 && y < y4 + keyH) {
        s_kbdActive = false;
        s_kbdFav = 255;
        s_kbdText = "";
        return true;
    }
    return false;
}

// -------------------- Public API --------------------
void presets_drawPressed() {
    if (s_pressedSlot == s_lastDrawnPressed) { return; }
    ensurePresetsCanvas();
    if (s_lastDrawnPressed >= 0) {
        int16_t x, y, w, h;
        slotRect(s_lastDrawnPressed, x, y, w, h);
        drawSlot(s_lastDrawnPressed, false, false);
        presetsBlitRect(x, y, w, h);
    }
    if (s_pressedSlot >= 0) {
        int16_t x, y, w, h;
        slotRect(s_pressedSlot, x, y, w, h);
        drawSlot(s_pressedSlot, true, false);
        presetsBlitRect(x, y, w, h);
    }

    s_lastDrawnPressed = s_pressedSlot;
}

void presets_clearPressed() {
    if (s_lastDrawnPressed < 0) { return; }
    ensurePresetsCanvas();
    int16_t x, y, w, h;
    slotRect(s_lastDrawnPressed, x, y, w, h);
    drawSlot(s_lastDrawnPressed, false, false);
    // csak ezt a kártyát küldjük ki
    presetsBlitRect(x, y, w, h);
    s_lastDrawnPressed = -1;
}

void presets_drawScreen() {
    ensurePrefs();
    ensurePresetsCanvas();
    if (s_kbdActive) {
        drawKeyboard(); // ezt később canvasosítjuk
        presetsBlitFull();
        return;
    }
    drawBackground();
    drawTopBar();
    for (uint8_t i = 0; i < SLOTS; i++) {
        bool pressed = (i == s_pressedSlot);
        drawSlot(i, pressed, false);
    }
    drawFavBar();
    presetsBlitFull(); // ← egyetlen frame megy ki
}

bool presets_toastExpired() {
    if (s_toastTopbarUntil && millis() > s_toastTopbarUntil) {
        s_toastTopbarUntil = 0;
        ensurePresetsCanvas();
        drawTopBar();
        presetsBlitRect(0, 0, s_presetsW, TOP_H);
        // Ha volt villogó slot, vissza normálra
        if (s_flashSlot >= 0 && millis() >= s_flashUntil) {
            int16_t x, y, w, h;
            slotRect(s_flashSlot, x, y, w, h);
            drawSlot(s_flashSlot, false, false); // canvasba
            presetsBlitRect(x, y, w, h);         // csak azt a kártyát küldjük ki
            s_flashSlot = -1;
        }
    }
    return false; // továbbra sem kérünk full redraw-t
}

int presets_hitTest(uint16_t x, uint16_t y) {
    // ne engedjük, hogy a slotok belenyúljanak a FAV sávba
    if (y >= FAV_TOP()) { return -1; }
    if (y < TOP_H) { return -1; }
    for (uint8_t i = 0; i < SLOTS; i++) {
        int16_t rx, ry, rw, rh;
        slotRect(i, rx, ry, rw, rh);
        if (x >= rx && x < (rx + rw) && y >= ry && y < (ry + rh)) { return (int)i; }
    }
    return -1;
}

int presets_hitTestFav(uint16_t x, uint16_t y) {
    if (y < FAV_TOP()) { return -1; }
    for (uint8_t i = 0; i < FAVS; i++) {
        int16_t rx, ry, rw, rh;
        favRect(i, rx, ry, rw, rh);
        if (x >= rx && x < (rx + rw) && y >= ry && y < (ry + rh)) { return (int)i; }
    }
    return -1;
}

void presets_selectBank(uint8_t fav) {
    if (fav >= FAVS) { return; }
    ensurePrefs();
    s_bank = fav;
    saveBank();
    s_cacheValid = false;
    loadBankCache();
}

bool presets_keyboardActive() {
    return s_kbdActive;
}

void presets_keyboardOpen(uint8_t fav) {
    if (fav >= FAVS) { return; }
    ensurePrefs();
    char buf[32];
    readFavLabelC(fav, buf, sizeof(buf));
    s_kbdActive = true;
    s_kbdFav = fav;
    s_kbdText = buf; // itt OK a String, ritka művelet
}

bool presets_keyboardTap(uint16_t x, uint16_t y) {
    if (!s_kbdActive) { return false; }
    bool changed = keyboardTap(x, y);
    if (s_kbdActive) {
        // csak az input box frissül
        drawInputBox();
        // drawInputBox() végén presetsBlitRect(...) van
    } else {
        // billentyűzet bezárult → teljes Presets UI újrarajzolása
        presets_drawScreen();
    }
    return changed;
}

bool presets_save(uint8_t slot) {
    if (slot >= SLOTS) { return false; }
    ensurePrefs();
    if (strlen(config.station.url) == 0) {
        char msg[32];
        strncpy_P(msg, LANG::prstNoUrl, sizeof(msg) - 1);
        msg[sizeof(msg) - 1] = 0;
        drawToastTopBar(msg);
        return true; // ne lépjünk ki
    }
    const char* url = config.station.url;
    const char* name = config.station.name;
    if (!name || name[0] == 0) { name = "RADIO"; }
    char key[20];
    makeKey(key, sizeof(key), s_bank, slot, "url");
    s_prefs.putString(key, url);
    makeKey(key, sizeof(key), s_bank, slot, "name");
    s_prefs.putString(key, name);
    // also store playlist station index (1-based) for correct pause/play behavior
    uint16_t id = config.lastStation();
    if (id == 0 || id > config.playlistLength()) {
        // if current station was started from URL, try to resolve it back to playlist
        id = findStationIdByUrl(url);
    }
    if (id >= 1 && id <= config.playlistLength()) {
        char key[20];
        makeKey(key, sizeof(key), s_bank, slot, "id");
        s_prefs.putUShort(key, id);
    }
    // ---- cache frissítése ----
    strncpy(presetName[slot], name, sizeof(presetName[slot]) - 1);
    presetName[slot][sizeof(presetName[slot]) - 1] = 0;
    strncpy(presetUrl[slot], url, sizeof(presetUrl[slot]) - 1);
    presetUrl[slot][sizeof(presetUrl[slot]) - 1] = 0;
    presetId[slot] = id;
    drawSlot(slot, false, true);
    int16_t x, y, w, h;
    slotRect(slot, x, y, w, h);
    // AZONNAL kirakjuk a mentett gombot
    presetsBlitRect(x, y, w, h);
    s_flashSlot = slot;
    s_flashUntil = millis() + 2500;
    char msg[32];
    strncpy_P(msg, LANG::prstAssigned, sizeof(msg) - 1);
    msg[sizeof(msg) - 1] = 0;
    drawToastTopBar(msg);
    return true;
}

bool presets_play(uint8_t slot) {
    if (slot >= SLOTS) { return false; }
    ensurePrefs();
    const char* url = readUrlC(slot);
    const char* name = readNameC(slot);
    if (!url || url[0] == 0) {
        char msg[32];
        strncpy_P(msg, LANG::prstEmptyPreset, sizeof(msg) - 1);
        msg[sizeof(msg) - 1] = 0;
        drawToastTopBar(msg);
        return true; // esemény kezelve, maradunk itt
    }
    // Prefer playlist ID if valid
    uint16_t id = readIdC(slot);
    if (id >= 1 && id <= config.playlistLength()) {
        config.lastStation(id);
        player.sendCommand({PR_PLAY, (int)id});
        return true;
    }
    // Try to resolve URL back to playlist
    uint16_t resolved = findStationIdByUrl(url);
    if (resolved >= 1 && resolved <= config.playlistLength()) {
        char key[20];
        makeKey(key, sizeof(key), s_bank, slot, "id");
        s_prefs.putUShort(key, resolved);
        presetId[slot] = resolved; // cache update
        config.lastStation(resolved);
        player.sendCommand({PR_PLAY, (int)resolved});
        return true;
    }
    // Fallback: direct URL play
    player.playUrl(url, name);
    return true;
}

bool presets_clear(uint8_t slot) {
    if (slot >= SLOTS) { return false; }
    ensurePrefs();
    char key[20];
    makeKey(key, sizeof(key), s_bank, slot, "url");
    s_prefs.remove(key);
    makeKey(key, sizeof(key), s_bank, slot, "name");
    s_prefs.remove(key);
    makeKey(key, sizeof(key), s_bank, slot, "id");
    s_prefs.remove(key);
    // cache ürítése is!
    presetUrl[slot][0] = 0;
    presetName[slot][0] = 0;
    presetId[slot] = 0;
    drawSlot(slot, false, false);
    int16_t x, y, w, h;
    slotRect(slot, x, y, w, h);
    // AZONNAL kirakjuk a törölt kártyát
    presetsBlitRect(x, y, w, h);
    char msg[32];
    strncpy_P(msg, LANG::prstDeleted, sizeof(msg) - 1);
    msg[sizeof(msg) - 1] = 0;
    drawToastTopBar(msg);
    return true;
}
#endif
