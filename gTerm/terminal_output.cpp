#include "terminal_output.h"
#include <cstring>

terminal_output::terminal_output() {
    _autoScroll = true;
}

terminal_output::~terminal_output() {}

int terminal_output::update(std::deque<char>& rxDequeObj, bool isConnected) {
    ImGui::BeginChild("ConsoleRegion", ImVec2(0, _window_params.height), true, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));

    ImVec2 region = ImGui::GetContentRegionAvail();
    ImVec2 childMin = ImGui::GetWindowPos();

    // Draw bouncing ball
    float deltaTime = ImGui::GetIO().DeltaTime;
    UpdateBall(deltaTime, region, childMin, isConnected);

    // Display incoming serial data
    if (!rxDequeObj.empty()) {
        std::string textString(rxDequeObj.begin(), rxDequeObj.end());
        ImGui::TextUnformatted(textString.c_str(), textString.c_str() + textString.size());
    }

    ImGui::PopStyleColor(1);

    if (_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();

    // Resizable drag bar
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

    ImGui::Button("|||", ImVec2(ImGui::GetContentRegionAvail().x, _window_params.drag_bar_height));

    ImGui::PopStyleColor(3);

    if (ImGui::IsItemActive()) {
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        _window_params.height = std::max(_window_params.height + delta.y, _window_params.min_height);
    }

    return 0;
}

void terminal_output::UpdateBall(float deltaTime, ImVec2 region, ImVec2 childMin, bool connected) {
#define BALL_SPEED_SCALE 2.0f

    deltaTime *= BALL_SPEED_SCALE;

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

    ImU32 ballColor = connected ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 ballScreenPos = ImVec2(childMin.x + ball.pos.x, childMin.y + ball.pos.y);
    draw_list->AddCircleFilled(ballScreenPos, ball.radius, ballColor);
}
