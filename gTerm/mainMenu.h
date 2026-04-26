#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <imgui.h>
#include "ConfigManager.h"
class mainMenu {
public:
    mainMenu(ConfigManager& cfgManager);
    void update(); // Render the debug menu
    bool frame_rate_capped = false;
    bool v_sync_enabled = true;
    float currentFontSize = 12.0f;
    bool fontNeedsRebuild = false;
    bool show_debug = false;
    bool show_ball = false;

    bool exit_app = false;
    ConfigManager& cfgManagerP;
};

#endif // MAIN_MENU_H
