#include "terminal_output.h"

terminal_output::terminal_output() {
    
    _autoScroll = true;
}


terminal_output::~terminal_output(void) {

}

void terminal_output::addLine(const char* line) {
    // Add the new line to the vector
    _lines.push_back(std::string(line));
}

void terminal_output::rmLine() {
    // remove
    _lines.pop_front();
    
}

void terminal_output::clear() {
    _lines.clear(); // Clear all lines
}

int terminal_output::update() {
    // Begin a scrollable child region
    ImGui::BeginChild("ConsoleRegion", ImVec2(0, _maxHeight), true, ImGuiWindowFlags_HorizontalScrollbar);

    // Display all lines in the console
    for (const std::string& line : _lines) {
        ImGui::TextUnformatted(line.c_str());
    }

    // Auto-scroll to the bottom if enabled
    if (_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();

    return 0;
}


int terminal_output::render(void) {

    return 0;
}

