#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <imgui.h>

class mainMenu {
public:
    mainMenu();
    void update(); // Render the debug menu
    bool frame_rate_capped = false;
    float currentFontSize = 24.0f;
    bool fontNeedsRebuild = false;
};

#endif // MAIN_MENU_H
