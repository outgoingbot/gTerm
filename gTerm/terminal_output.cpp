#include "terminal_output.h"

terminal_output::terminal_output() { 
    _autoScroll = true;
}


terminal_output::~terminal_output(void) {

}


int terminal_output::update(std::deque<char> rxDequeObj) {
    // Begin a scrollable child region
    ImGui::BeginChild("ConsoleRegion", ImVec2(0, _window_params.height), true, ImGuiWindowFlags_HorizontalScrollbar);
  
    // Display all lines in the console
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));        // Light Gray

    // Optional: Push wrapping to match child window width
    //ImGui::PushTextWrapPos(0.0f);  // 0.0f means wrap at the right edge of the current window
    ImVec2 region = ImGui::GetContentRegionAvail();
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + region.x); // wrap at the edge

    // === FUN: Update and draw the ball ===
    float deltaTime = ImGui::GetIO().DeltaTime;
    ImVec2 childMin = ImGui::GetWindowPos(); // top-left corner of child window
    UpdateBall(deltaTime, region, childMin);
    // === FUN: Update and draw the ball ===

    //TODO: I should have a mutex lock here I think
    std::string textString(rxDequeObj.begin(), rxDequeObj.end());//copy all the chars in the deque<char> object into a string
    ImGui::TextUnformatted(textString.c_str(), textString.c_str() + textString.size());  // safer for large logs
    ImGui::PopTextWrapPos();  // Always pair i
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


void terminal_output::addLine(const char* line) {

}


void terminal_output::rmLine() {

}


void terminal_output::clear() {
}


void terminal_output::UpdateBall(float deltaTime, ImVec2 region, ImVec2 childMin) {
    ball.pos.x += ball.vel.x * deltaTime;
    ball.pos.y += ball.vel.y * deltaTime;

    if (ball.pos.x - ball.radius < 0.0f) {
        ball.pos.x = ball.radius;
        ball.vel.x = -ball.vel.x;
    }
    if (ball.pos.x + ball.radius > region.x) {
        ball.pos.x = region.x - ball.radius;
        ball.vel.x = -ball.vel.x;
    }
    if (ball.pos.y - ball.radius < 0.0f) {
        ball.pos.y = ball.radius;
        ball.vel.y = -ball.vel.y;
    }
    if (ball.pos.y + ball.radius > region.y) {
        ball.pos.y = region.y - ball.radius;
        ball.vel.y = -ball.vel.y;
    }

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 ballScreenPos = ImVec2(childMin.x + ball.pos.x, childMin.y + ball.pos.y);
    draw_list->AddCircleFilled(ballScreenPos, ball.radius, IM_COL32(255, 0, 0, 255));
}