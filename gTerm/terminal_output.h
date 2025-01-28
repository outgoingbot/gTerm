#pragma once
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <deque>

class terminal_output {

public:
	terminal_output();
	~terminal_output();

	int update();
	int render(void);
	void addLine(const char* line); // Add a new line to the console
	void rmLine();
	void clear();

private:
	GLFWwindow* _window; // Pointer to the GLFW window
	int _width;          // Window width
	int _height;         // Window height

	char _buffer[128]; // Buffer to store the text input

	std::deque<std::string> _lines; // Stores each line of the console
	bool _autoScroll;                // Controls auto-scrolling behavior
	const float _maxHeight = 600.0f;
};