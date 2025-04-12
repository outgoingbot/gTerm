#include "dataParser.h"

dataParser::dataParser() {

    _autoScroll = true;
}


dataParser::~dataParser(void) {

}


int dataParser::update(std::deque<char> rxDequeObj) {

    //static float child_height = 600.0f; // Initial height
    //float min_size = 50.0f;             // Minimum allowed size
    //float resize_area = 10.0f;
    //static bool is_resizing = false;    // Flag to detect active resizing

    // Begin a scrollable child region
    //ImGui::BeginChild("ConsoleRegion", ImVec2(0, _window_params.height), true, ImGuiWindowFlags_HorizontalScrollbar);
    // Begin a NON-scrollable child region
    ImGui::BeginChild("Data_Manipulation_Region", ImVec2(0, _window_params.height), true);

    // Display all lines in the console
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));        // Light Gray


    ImVec2 region = ImGui::GetContentRegionAvail();
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + region.x); // wrap at the edge

    std::string tempString = "PLACE HOLDER STRING";

    ImGui::TextUnformatted(tempString.c_str(), tempString.c_str() + tempString.size());  // safer for large logs
    ImGui::PopTextWrapPos();  // Always pair it

    ImGui::PopStyleColor(1); // Restore previous colors

    // Auto-scroll to the bottom if enabled
    if (_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();

    return 0;
}


