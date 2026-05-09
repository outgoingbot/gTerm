#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <imgui.h>
#include "ConfigManager.h"
class mainMenu {
public:
    mainMenu(ConfigManager& cfgManager);
    void update(); // Render the debug menu

    bool v_sync_enabled = true;
    bool v_sync_need_update = true;


    bool turbo_mode = false;
    float frame_rate_slider_val = 120.0f;
    const float frame_rate_max_val = 240.0f;
    const float frame_rate_min_val = 5.0f;

    bool low_power_on_disconnect = true;
 

    float currentFontSize = 12.0f;
    bool fontNeedsRebuild = false;
    bool show_debug = false;
    bool show_ball = false;

    bool exit_app = false;
    ConfigManager& cfgManagerP;
};

#endif // MAIN_MENU_H
