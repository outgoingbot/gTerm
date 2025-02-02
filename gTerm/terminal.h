#pragma once
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

#include "serial/serial.h"

#include "terminal_output.h" //Scolling Text Window

class terminal{

public:
	terminal(GLFWwindow* window, int width, int height);
	~terminal();

	int update(const char* title);
	int render(void);

	terminal_output term_out;
	serial ser_obj;

private:
	GLFWwindow* _window; // Pointer to the GLFW window
	int _width;          // Window width
	int _height;         // Window height
	void drawCircle(ImDrawList* draw_list, ImVec2 center, float radius, ImU32 color, int num_segments = 100);

	char buffer[128]; // Buffer to store the text input

	char input_buffer_Port[64]; // Buffer to store the text input
	char input_buffer_Baud[64]; // Buffer to store the text input
	
	int handle_connect_button();

};