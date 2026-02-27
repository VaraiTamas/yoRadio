#ifndef BACKLIGHT_PLUGIN_H
#define BACKLIGHT_PLUGIN_H

#include "../../pluginsManager/pluginsManager.h"

class BacklightPlugin : public Plugin {
  public:
    BacklightPlugin();

    void on_setup() override;
    void on_ticker() override;
    void on_start_play() override;
    void on_stop_play() override;
    void on_track_change() override;
    void on_display_player() override;
    void on_btn_click(controlEvt_e& btnid) override;

    bool isDimmed() const;
    bool isFading() const;
    bool isFadeControl();
    void wake();
    void activity();
    void setBacklight( uint8_t backLight );


  private:
    void tick();

    uint32_t      lastActivity = 0;
    uint32_t      lastFadeStep = 0;
    uint32_t      lastUiWakeMs = 0;
    uint8_t       currentBrightness = 0;
    uint8_t       targetBrightness = 0;
    uint8_t       savedBrightness = 0;
    bool          brightnessCaptured = false;
    displayMode_e lastMode = PLAYER;

    enum State { WAIT, FADING, DIMMED };

    State state = WAIT;
};

void backlightPluginInit();
extern BacklightPlugin backlightPlugin;


#endif
