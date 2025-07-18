#include "terminal_output.h"

terminal_output::terminal_output() {
    _autoScroll = true;
    _showControlChars = false;
}

terminal_output::~terminal_output() {}

int terminal_output::update(const char* _rxBuff, size_t size, bool isConnected) {
    
    ImGui::BeginChild("ConsoleRegion", ImVec2(0, _window_params.height), true);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));
    ImVec2 region = ImGui::GetContentRegionAvail();
    ImVec2 childMin = ImGui::GetWindowPos();

    // Draw bouncing ball
    float deltaTime = ImGui::GetIO().DeltaTime;
    UpdateBall(deltaTime, region, childMin, isConnected);
    // Draw bouncing ball

    if (_rxBuff && _rxBuff[0] != '\0') {
        //need to copy buffer or else imgui crashes when trying to select the text. (I DONT THINK I NEED THIS.)
        static char inputTextBuffer[10000] = { 0 };
        strncpy(inputTextBuffer, _rxBuff, sizeof(inputTextBuffer) - 1);
        inputTextBuffer[sizeof(inputTextBuffer) - 1] = '\0';

        //THIS IS SELECTABLE TEXT. BUT IT CAUSING ISSUES (WELL KNOWN ISSUE TO THE IMGUI DEVS)
        //Draw the entire buffer. terminal.cpp is doint the heavy lifting to modify the display data for this multiline function
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;//ImGuiInputTextFlags_NoHorizontalScroll;
        ImGui::PushTextWrapPos(0.0f);
        /*
        ImGui::InputTextMultiline(
            "##SerialOutput",
            (char*)inputTextBuffer,
            IM_ARRAYSIZE(inputTextBuffer),
            ImVec2(-FLT_MIN, region.y),
            flags
        );
        */

        //THIS TEXT IS NOT SELECTABLE. IT WORKS VERY GOOD
        ImGui::TextUnformatted(_rxBuff, _rxBuff+size);
        //ImGui::Selectable(_rxBuff);
        //ImGui::PopTextWrapPos();

      

        if (_autoScroll) {
            ImGui::SetScrollY(ImGui::GetScrollMaxY());
        }
    }

    ImGui::PopStyleColor(1);
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
