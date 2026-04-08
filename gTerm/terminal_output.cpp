#include "terminal_output.h"


terminal_output::terminal_output() {
    _autoScroll = true;
    _showControlChars = false;

}

terminal_output::~terminal_output() {}



int terminal_output::update(std::deque<char>& _Term_rxBufferQueue, size_t newCharCount, bool isConnected) {

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
    


    //--------------------------------------- Print Serial Characters to Screen ---------------------------------------|
    for (size_t i = 0; i < newCharCount; ++i){
        char c = _Term_rxBufferQueue[_Term_rxBufferQueue.size() - newCharCount + i];
        //Parse each character is you want to. InputTextMultiLine will obey control characters ao no need to check for new linea, etc
        /*
        // control character checks
        switch (c){
        case '\0':
        case '\r':
        case '\n':
            _displayLines += ','; // Append the modified character to string
            break;
        case '\t':
        case '\v':
        case '\f':
        case '\b':
        default:
            _displayLines += c; // Append the new character directly to string
            break;    
        }
        */
        _displayLines += c; // Append only new characters directly to string
    }

    // limit total size of the string
    if (_displayLines.size() > display_buff_num_chars){
        _displayLines.erase(0, _displayLines.size() - display_buff_num_chars);
    }


    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));     // fully transparent
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::InputTextMultiline(
        "##terminal_output",
        const_cast<char*>(_displayLines.c_str()),
        _displayLines.size() + 1,
        ImVec2(-FLT_MIN, -FLT_MIN),
        ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_WordWrap | ImGuiInputTextFlags_NoHorizontalScroll
    );
    ImGui::PopStyleColor(3);
    ImGui::PopFont();
    if (_autoScroll) {
        // 1. Get the current window where the widget lives
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        // 2. Locate the child window by its auto-generated name
        // Format is: "ParentWindowName/Label_ID" (ID in hex)
        char child_name[128];
        ImFormatString(child_name, IM_ARRAYSIZE(child_name), "%s/##terminal_output_%08X", window->Name, ImGui::GetID("##terminal_output"));

        if (ImGuiWindow* child_window = ImGui::FindWindowByName(child_name)) {
            // 3. Directly set the scroll of that internal child
            child_window->Scroll.y = child_window->ScrollMax.y;
        }
    }
    //--------------------------------------- Print Serial Characters to Screen ---------------------------------------|
    
    //--------------------------------------- Copy Text PopUp ---------------------------------------|
    // Save selection before popup (critical)
    int sel_start = -1;
    int sel_end = -1;
    ImGuiID id = ImGui::GetID("##terminal_output");
    if (ImGuiInputTextState* state = ImGui::GetInputTextState(id))
    {
        if (state->HasSelection())
        {
            sel_start = ImMin(state->GetSelectionStart(), state->GetSelectionEnd());
            sel_end = ImMax(state->GetSelectionStart(), state->GetSelectionEnd());
        }
    }

    // Right-click Copy menu
    // Show right-click menu ONLY if text is selected
    if (sel_start != -1 && sel_end > sel_start){
        if (ImGui::BeginPopupContextItem("##terminal_output"))
        {
            if (ImGui::MenuItem("Copy", "Ctrl+C"))
            {
                std::string selected = _displayLines.substr(sel_start, sel_end - sel_start);
                ImGui::SetClipboardText(selected.c_str());
            }
            ImGui::EndPopup();
        }
    }else{

        //this is a little buggy. sometimes need to left click once if selection has been made, even if not visible
        // Register a context menu (optional)
        if (ImGui::BeginPopupContextItem("##terminal_output2")){

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


void terminal_output::clearDisplayText() {
    _displayLines.clear();

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
