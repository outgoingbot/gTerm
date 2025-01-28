#include "terminal.h"

terminal::terminal(GLFWwindow* window, int width, int height) {
    _width = width;
    _height = height;
    _window = window;
    memset(buffer, '\0', sizeof(char));
}


terminal::~terminal(void) {

}


int terminal::update(const char* title) {
        // Create ImGui window
        ImGui::Begin(title);

        //Text Entry Test
        ImGui::SetWindowFontScale(2.0f);
        ImGui::Text("Enter your text below:");
        if (ImGui::InputText("##TextEntry", buffer, IM_ARRAYSIZE(buffer))) {
            // Handle text input when the field changes
        }
        ImGui::Text("You entered: %s", buffer);


        //Button Test
        if (ImGui::Button("Click Me")) {
            // This block executes when the button is clicked
            std::cout << "Button clicked!" << std::endl;
        }

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 center = ImVec2(window_pos.x + 400, window_pos.y + 300); // Circle center relative to window
        drawCircle(draw_list, center, 250.0f, IM_COL32(255, 0, 0, 255));
        ImGui::End();

	return 0;
}


int terminal::render(void) {

	return 0;
}

void terminal::drawCircle(ImDrawList* draw_list, ImVec2 center, float radius, ImU32 color, int num_segments) {
    draw_list->AddCircle(center, radius, color, num_segments);
    draw_list->AddText(center, color, "Test");
}