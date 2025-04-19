// terminal_output.h
#pragma once

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <deque>
#include <string>
#include <algorithm>

class terminal_output {
public:
    terminal_output();
    ~terminal_output();

    int update(std::deque<char> rxDequeObj, bool isConnected);
    void addLine(const char* line); // Add a new line to the console
    void clear();

private:
    bool _autoScroll;

    typedef struct window_params_t {
        float min_height = 100.0f;
        float width = 1000.0f;
        float height = 500.0f;
        float drag_bar_height = 20.0f;
    } window_params_t;
    window_params_t _window_params;

    struct BallState {
        ImVec2 pos = ImVec2(50.0f, 50.0f);
        ImVec2 vel = ImVec2(100.0f, 60.0f);
        float radius = 5.0f;
    };
    BallState ball;

    std::deque<std::string> logBuffer;
    char inputBuffer[256];

    void UpdateBall(float deltaTime, ImVec2 region, ImVec2 childMin, bool connected);
};