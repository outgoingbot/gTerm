
/*
Terminal is master class for all serial related aspects of this program.
*/

#pragma once
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <deque>

#include "serial/serialManager.h"
//#include "serial/virtualComm.h"



#include "terminal_output.h" //Scolling Text Window

class terminal{

public:
	terminal(GLFWwindow* window, int width, int height);
	~terminal();

	int update(const char* title);
	int render(void);

	//Manages the serial data terminal display window.
	terminal_output term_out;
	

	//TODO: This virtual comm port should not be here. All we should instantiat is the serial class.
	// (the serial class should be the only interface to terminal class.
	


	//This is higher level serial class that has all the buffers, and higher level serial data manipulation.
	serialManager* serialManObj; //change name to Serialclass

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