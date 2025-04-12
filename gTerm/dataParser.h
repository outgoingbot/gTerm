#pragma once
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <deque>
#include <algorithm>

class dataParser {

public:
	dataParser();
	~dataParser();

	int update(std::deque<char> charArray);


private:
	
	//char _buffer[128]; // Buffer to store the text input
	bool _autoScroll;                // Controls auto-scrolling behavior

	typedef struct window_params_t {
		float min_height = 100.0f;
		float width; // Window width
		float height = 600.f; // Window height

		float drag_bar_height = 20.f;
	};
	window_params_t _window_params;


};