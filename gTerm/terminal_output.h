#pragma once

#include <imgui.h>
#include "imgui_internal.h"
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <deque>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include "textselect.hpp"

class terminal_output {
public:
    terminal_output();
    ~terminal_output();

    int update(std::deque<char>& fullBufferqueue, size_t newCharCount, bool isConnected);
    void SetTextColor(float r, float g, float b, float a = 1.0f);
    void SetTextSize(float sz);

    //make private if needed
    size_t display_buff_num_chars = 100000;

    void clearDisplayText();

private:
    bool _autoScroll;
    bool _showControlChars;

    typedef struct window_params_t {
        float min_height = 100.0f;
        float width = 1000.0f;
        float height = 1200.0f;
        float drag_bar_height = 20.0f;
    } window_params_t;
    window_params_t _window_params;

    struct BallState {
        ImVec2 pos = ImVec2(50.0f, 50.0f);
        ImVec2 vel = ImVec2(100.0f, 60.0f);
        float radius = 5.0f;
    };
    BallState ball;

    void UpdateBall(float deltaTime, ImVec2 region, ImVec2 childMin, bool connected);


    float mTextColorR = 0.0f;
    float mTextColorG = 0.8f;
    float mTextColorB = 0.3f;
    float mTextColorA = 1.0f;
    float localFontSize = 0.0f;

    //string to be printed in InpuTextMultiLine. one continues block of ram
    std::string _displayLines;
    
};