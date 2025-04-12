#include "terminal_output.h"

terminal_output::terminal_output() {
    
    _autoScroll = true;
}


terminal_output::~terminal_output(void) {

}

//
void terminal_output::addLine(const char* line) {
    //ToDo: add an upper limit to how large the queue can get.
    //if(_lines.size < SCROLL_BACK) push_back();
    // Add the new line to the vector
   // _lines.push_back(std::string(line));
}

//void terminal_output::addChar(const char* key) {
//    //ToDo: add an upper limit to how large the queue can get.
//    //if(_lines.size < SCROLL_BACK) push_back();
//    // Add the new line to the vector
//    _lines.push_back(std::string(line));
//}


void terminal_output::rmLine() {
    // remove elements to prevent memory overflow
   // _lines.pop_front();
    
}

void terminal_output::clear() {
   // _lines.clear(); // Clear all lines
}

int terminal_output::update(std::deque<char> rxDequeObj) {

    //static float child_height = 600.0f; // Initial height
    //float min_size = 50.0f;             // Minimum allowed size
    //float resize_area = 10.0f;
    //static bool is_resizing = false;    // Flag to detect active resizing

    // Begin a scrollable child region
    //ImGui::BeginChild("ConsoleRegion", ImVec2(0, _window_params.height), true, ImGuiWindowFlags_HorizontalScrollbar);
    // Begin a NON-scrollable child region
    ImGui::BeginChild("ConsoleRegion", ImVec2(0, _window_params.height), true, ImGuiWindowFlags_HorizontalScrollbar);
  
    // Display all lines in the console
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));        // Light Gray

    // Optional: Push wrapping to match child window width
    //ImGui::PushTextWrapPos(0.0f);  // 0.0f means wrap at the right edge of the current window
    ImVec2 region = ImGui::GetContentRegionAvail();
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + region.x); // wrap at the edge

    //TODO I should have a mutex lock here I think
    //copy all the chars in the deque<char> object into a string
    std::string textString(rxDequeObj.begin(), rxDequeObj.end());
   // ImGui::Text("%s", textString.c_str()); //first working method
    ImGui::TextUnformatted(textString.c_str(), textString.c_str() + textString.size());  // safer for large logs
    ImGui::PopTextWrapPos();  // Always pair it

    ////This needs to maintain the serial buffer
    //for (const std::string& line : _lines) {
    //    //ImGui::TextUnformatted(line.c_str());
    //    ImGui::Text(line.c_str());
    //}
    ImGui::PopStyleColor(1); // Restore previous colors

    // Auto-scroll to the bottom if enabled
    if (_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

   ImGui::EndChild();


//add dragable button to resize output window
    // Set button colors: Light Gray (Default), Gray (Hovered), Dark Gray (Pressed)
   ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));        // Light Gray
   ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Gray
   ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));  // Dark Gray


   ImGui::Button("|||", ImVec2(ImGui::GetItemRectSize().x, _window_params.drag_bar_height));
   ImGui::PopStyleColor(3); // Restore previous colors

    if (ImGui::IsItemActive()) { // If dragging the resize handle
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        //child_width = std::max(child_width + delta.x, min_size);
        _window_params.height = std::max(_window_params.height + delta.y, _window_params.min_height);
    }
  
    

    return 0;
}


int terminal_output::render(void) {

    return 0;
}

