#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <imgui.h>
#include "ConfigManager.h"
class mainMenu {
public:
    mainMenu();
    void update(); // Render the debug menu
    bool frame_rate_capped = false;
    bool v_sync_enabled = true;
    float currentFontSize = 18.0f;
    bool fontNeedsRebuild = false;

    ConfigManager cfgManager;
};

#endif // MAIN_MENU_H
