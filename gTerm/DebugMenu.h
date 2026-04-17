#ifndef DEBUG_MENU_H
#define DEBUG_MENU_H

#include <imgui.h>
#include <implot.h>
#include <iostream>
#include "terminal.h"

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
    void update(terminal&); // Render the debug menu
private:
    VersionInfo m_versions;

};

#endif // DEBUG_MENU_H
