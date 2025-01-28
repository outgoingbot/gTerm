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

        //float scale_factor = (ImGui::GetWindowHeight() * ImGui::GetWindowWidth()) * 0.0000005f; //playing with autoscaling
        //std::cout << scale_factor << std::endl;
        //testing out scrolling text class
        //ImGui::SetWindowFontScale(scale_factor);

        term_out.update(); //scrolling Text class

        //Text Entry Test
        ImGui::SetWindowFontScale(1.0f); //looks like shit when scaled
        
        ImGui::Text("Enter your text below:");
        if (ImGui::InputText("##TextEntry", buffer, IM_ARRAYSIZE(buffer))) {
            // Handle text input when the field changes
        }
        

        ImGui::SameLine(); // Place the next widget on the same line
        //Button Test
        if (ImGui::Button("Click Me")) {
            // This block executes when the button is clicked
            std::cout << "Button clicked!" << std::endl;
        }
        ImGui::Text("You entered: %s", buffer);
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