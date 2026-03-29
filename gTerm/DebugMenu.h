#ifndef DEBUG_MENU_H
#define DEBUG_MENU_H

#include <imgui.h>
//#include "GLFW/glfw3.h"
//#include "glad/glad.h"
//#include "stb_image.h"
#include "implot.h"
#include <iostream>
#include "serial/serialManager.h"

class DebugMenu {
public:
    DebugMenu();
    void update(serialManager*); // Render the debug menu
private:
    serialManager* debugserialManObj; //Serial Manager
    //GLuint DebugMenu::loadTexture(const char* filename);
};

#endif // DEBUG_MENU_H
