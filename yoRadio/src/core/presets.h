#pragma once
#include <Arduino.h>

// Presets / Favorites screen (8 slots per bank, 6 banks selectable via FAV1..FAV6).
// Usage (implemented in touchscreen.cpp):
// - Tap top bar (PLAYER) -> open presets screen (PG_PRESETS)
// - Tap preset: play + return to PLAYER
// - Long press preset (2s): save current station into slot (stays on presets)
// - Long press preset (5s): delete current station to desired location (remains at set values)
// - Tap FAV: switch bank
// - Long press FAV: edit its label using on-screen keyboard
void presets_drawScreen();
void presets_drawPressed();
void presets_clearPressed();
void presets_setPressedSlot(int slot);

// Hit-test helpers:
int  presets_hitTest(uint16_t x, uint16_t y);        // returns 0..11 or -1
int  presets_hitTestFav(uint16_t x, uint16_t y);     // returns 0..4  or -1

// Bank (FAV) selection:
void presets_selectBank(uint8_t fav);

// Keyboard (FAV label edit):
bool presets_keyboardActive();
void presets_keyboardOpen(uint8_t fav);
bool presets_keyboardTap(uint16_t x, uint16_t y);

// Preset actions:
bool presets_save(uint8_t slot);
bool presets_play(uint8_t slot);

bool presets_clear(uint8_t slot);
void presets_drawHoldBar(uint32_t heldMs);
void presets_clearHoldBar();
bool presets_toastExpired();

void sanitizePresets(uint8_t bank);


