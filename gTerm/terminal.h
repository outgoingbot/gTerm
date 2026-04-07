
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

	//Manages the serial data terminal display window.
	terminal_output term_out;
	
	//serial Manager class that has all the buffers, and higher level serial data manipulation.
	serialManager* serialManObj; //Serial Manager

	//public method to share thread safe rx deque buffer
	const std::deque<char>& getRxBuffer() const;
	//setTxBuffer

private:

	int _width;          // Window width //unused?
	int _height;         // Window height //unused?
	
	struct terminal_ui
	{
		char input_buffer_Port[64] = { 0 };   // Buffer for COM port
		char input_buffer_Baud[64] = { 0 };  // Buffer for baud rate
		char input_buffer_scrollback_len[64] = { 0 };  // Buffer for scroll back
		bool ConnectisClicked = false; //used to lock Connect button on click
		bool open_popup_port = false; //use for comm port popup
		bool open_popup_baud = false; //use for baudrate popup
	} ui;
	
	int handle_connect_button();
	int handle_disconnect_button();

	//this will hold a copy of the serial Manager deque. this copy does not need to be mutex locked when accessed.
	std::deque<char> _Term_rxBufferQueue;

	//lines vector control for term output
	std::vector<std::string> _displayLines; 
	std::string _currentPartialLine;
};