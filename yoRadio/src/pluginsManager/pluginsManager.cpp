#include "pluginsManager.h"

pluginsManager pm;

Plugin::Plugin() {
}

void pluginsManager::init() {
  _ready = true;
  // pending plugin-ek átemelése
  for (auto* p : pending) {
    plugins.push_back(p);
  }
  pending.clear();
  Serial.printf("PM.init(): pending=%u\n", pending.size());
}

bool pluginsManager::ready() const {
  return _ready;
}

void Plugin::registerPlugin() {
  pm.add(this);
}

void pluginsManager::add(Plugin* plugin) {
  if (!_ready) {
    pending.push_back(plugin);   // <<< setup előtti regisztráció
    return;
  }
  plugins.push_back(plugin);
}

size_t pluginsManager::count() const {
  return plugins.size();
}

Plugin* pluginsManager::get(size_t index) {
  if (index < plugins.size()) {
    return plugins[index];
  }
  return nullptr;
}

