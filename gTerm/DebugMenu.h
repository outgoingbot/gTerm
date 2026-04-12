#ifndef DEBUG_MENU_H
#define DEBUG_MENU_H

#include <imgui.h>
#include "implot.h"
#include <iostream>
#include "serial/serialManager.h"

struct VersionInfo {
    std::string imgui;
    std::string implot;
    std::string glfw;
    std::string opengl;
    std::string glsl;
};

class DebugMenu {
public:
    DebugMenu(const VersionInfo& versions);
    void update(serialManager*); // Render the debug menu
private:
    serialManager* debugserialManObj; //Serial Manager
    //GLuint DebugMenu::loadTexture(const char* filename);

    VersionInfo m_versions;

};

#endif // DEBUG_MENU_H
