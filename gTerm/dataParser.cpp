#include "dataParser.h"

dataParser::dataParser() {
    
    _window_params.min_height = 100.0f;
    _window_params.width; // Window width
    _window_params.height = 600.f; // Window height

    _window_params.drag_bar_height = 20.f;
    _autoScroll = true;
}


dataParser::~dataParser(void) {

}


//int dataParser::update(std::deque<char> rxDequeObj) {
int dataParser::update() {

    //static float child_height = 600.0f; // Initial height
    //float min_size = 50.0f;             // Minimum allowed size
    //float resize_area = 10.0f;
    //static bool is_resizing = false;    // Flag to detect active resizing

    // Begin a scrollable child region
    //ImGui::BeginChild("ConsoleRegion", ImVec2(0, _window_params.height), true, ImGuiWindowFlags_HorizontalScrollbar);
    // Begin a NON-scrollable child region
    ImGui::Begin("Data_Manipulation_Region");

    // Display all lines in the console
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));        // Light Gray


    ImVec2 region = ImGui::GetContentRegionAvail();
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + region.x); // wrap at the edge

    ImGui::Checkbox("Parser Enabled", &dataParse_enable);

    std::string tempString = "PLACE HOLDER STRING";
    if (dataParse_enable) {
        tempString = "True";
    }
    else {
        tempString = "False";
    }

    ImGui::TextUnformatted(tempString.c_str(), tempString.c_str() + tempString.size());  // safer for large logs
    ImGui::PopTextWrapPos();  // Always pair it
    ImGui::PopStyleColor(1); // Restore previous colors

    // Auto-scroll to the bottom if enabled
    if (_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::End();

    return 0;
}

std::deque<float> dataParser::ParseFloatArrayFromAscii(const std::deque<char>& asciiBuffer)
{
    std::deque<float> result;
    std::string token;
    std::stringstream ss;

    // Build a full string from the deque
    for (char ch : asciiBuffer)
        ss << ch;

    // Parse comma-separated values
    while (std::getline(ss, token, ','))
    {
        try {
            // Convert to float and store
            result.push_back(std::stof(token));
        }
        catch (const std::invalid_argument& e) {
            // Ignore invalid tokens (non-numeric)
            continue;
        }
        catch (const std::out_of_range& e) {
            // Ignore values that don't fit in float
            continue;
        }
    }

    return result;
}