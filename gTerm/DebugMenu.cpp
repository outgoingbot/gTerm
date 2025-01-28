#include "DebugMenu.h"

DebugMenu::DebugMenu() {
    // Constructor (if you need to initialize anything, do it here)
}

void DebugMenu::update() {
    // Start the debug menu window
    ImGui::Begin("Debug Menu");


    // Display the frame rate
    ImGui::Text("Frame Rate: %.1f FPS", ImGui::GetIO().Framerate);

    // Get mouse position
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    ImGui::Text("Mouse Position: (%.1f, %.1f)", mousePos.x, mousePos.y);

    ImGui::End();
}
