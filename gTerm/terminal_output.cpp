#include "terminal_output.h"

terminal_output::terminal_output() {
    _autoScroll = true;
    _showControlChars = false;

    //allocate Selactable Text object
    textSelect = new TextSelect(
        [this](size_t idx) { return this->getLineAtIdx(idx); },
        [this]() { return this->getNumLines();     },
        true
    );

}

terminal_output::~terminal_output() {}


//these fucking dumb and need to be removed or changed. why are we forced to have a hard coded variable type here?
// It look like they are needed to be pushed into the selectText constructor but we need a better solution
// Prob need to modify the selectText class
//Text slect methods
std::string_view terminal_output::getLineAtIdx(size_t idx) {
    return lines[idx];
}

size_t terminal_output::getNumLines() {
    return lines.size();
}


int terminal_output::update(const std::vector<std::string>& new_lines, bool isConnected) {

    // Create a window to contain the text
    ImGui::BeginChild("ConsoleRegion", ImVec2(0, _window_params.height), true, ImGuiWindowFlags_NoMove);
    //ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(mTextColorR, mTextColorG, mTextColorB, mTextColorA));
    
    ImGui::PushFont(nullptr, ImGui::GetFontSize() + localFontSize);
    ImVec2 region = ImGui::GetContentRegionAvail();
    ImVec2 childMin = ImGui::GetWindowPos();

    // Draw bouncing ball
    float deltaTime = ImGui::GetIO().DeltaTime;
    UpdateBall(deltaTime, region, childMin, isConnected);
    
    // Update TextSelect instance (all text selection is handled in this method)
    textSelect->update(); //Moved this above the for loop below

    //send the lines into
    for (const auto& line : new_lines) {
        ImGui::TextWrapped("%s", line.c_str());
    }

    ImGui::PopFont();

    // Register a context menu (optional)
    if (ImGui::BeginPopupContextWindow()) {
        ImGui::BeginDisabled(!textSelect->hasSelection());
        if (ImGui::MenuItem("Copy", "Ctrl+C")) {
            textSelect->copy();
        }
        ImGui::EndDisabled();

        if (ImGui::MenuItem("Select all", "Ctrl+A")) {
            textSelect->selectAll();
        }

        if (ImGui::MenuItem("Clear selection")) {
            textSelect->clearSelection();
        }

        //------------------------------set the text color-------------------------------|
        // Local static values so sliders remember their positions
        static float r = 0.0f;
        static float g = 0.8f;
        static float b = 0.3f;
        static float a = 1.0f;
        static float sz = 0.0f;

        // Wrap in a collapsible header or your own UI block
        ImGui::Separator();
        ImGui::Text("Text Color Controls");
        ImGui::PushItemWidth(150.0f);
        // Four sliders (each 0.0 -> 1.0)
        ImGui::SliderFloat("Red", &r, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Green", &g, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Blue", &b, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Alpha", &a, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Size", &sz, -20.f, 20.0f, "%.0f");
        ImGui::PopItemWidth();
        SetTextColor(r, g, b, a);
        SetTextSize(sz);
        //END ------------------------------set the text color-------------------------------|



        ImGui::EndPopup();
    }

    if (_autoScroll) {
        ImGui::SetScrollY(ImGui::GetScrollMaxY());
    }
    
    ImGui::PopStyleColor(1);
    ImGui::EndChild();
    //------------------------Selectable and wrapable text!!!! lets go-------------------------------------------


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
#define BALL_SPEED_SCALE 3.0f

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

void terminal_output::SetTextColor(float r, float g, float b, float a)
{
    mTextColorR = r;
    mTextColorG = g;
    mTextColorB = b;
    mTextColorA = a;
}

void terminal_output::SetTextSize(float sz)
{
    localFontSize = sz;
}
