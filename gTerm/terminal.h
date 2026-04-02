
/*
Terminal is master class for all serial terminal related aspects of this program.
parsing and plotting will be done in other classes
*/

#pragma once
#include <imgui.h>
#include "implot.h"
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <deque>

#include "serial/serialManager.h"
#include "terminal_output.h" //Scolling Text Window

//testing imguitextselect library
#include <string_view>
#include <vector>
#include "textselect.hpp"



class terminal{

public:
	terminal(int width, int height);
	~terminal();

	int update(const char* title);
	int render(void);

	//Manages the serial data terminal display window.
	terminal_output term_out;
	
	//serial Manager class that has all the buffers, and higher level serial data manipulation.
	serialManager* serialManObj; //Serial Manager

	//public method to share thread safe rx deque buffer
	const std::deque<char>& getRxBuffer() const;

private:
	void drawCircle(ImDrawList* draw_list, ImVec2 center, float radius, ImU32 color, int num_segments = 100);

	//GLFWwindow* _window; // Pointer to the GLFW window
	int _width;          // Window width
	int _height;         // Window height
		
	char usr_text_buffer[128]; // Buffer to store the text input

	char input_buffer_Port[64]; // Buffer to store the port list
	char input_buffer_Baud[64]; // Buffer to store the baud
	
	int handle_connect_button();
	int handle_disconnect_button();

	//this will hold a copy of the serial Manager deque. this copy does not need to be mutex locked when accessed.
	std::deque<char> _Term_rxBufferQueue;
};