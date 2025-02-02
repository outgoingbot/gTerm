#ifndef DEBUG_MENU_H
#define DEBUG_MENU_H

#include <imgui.h>
//#include "GLFW/glfw3.h"
//#include "glad/glad.h"
//#include "stb_image.h"

#include <iostream>

class DebugMenu {
public:
    DebugMenu();
    void update(); // Render the debug menu
private:
    //GLuint DebugMenu::loadTexture(const char* filename);
};

#endif // DEBUG_MENU_H
