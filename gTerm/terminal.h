
/*
Terminal is master class for all serial terminal related aspects of this program.
parsing and plotting will be done in other classes
*/

#pragma once
#include <imgui.h>
#include <implot.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <deque>
#include <string_view>
#include <vector>
#include "ConfigManager.h"
#include "serial/serialManager.h"
#include "terminal_output.h" //Scolling Text Window




class terminal{

public:
	terminal(int width, int height, AppConfig& cfg);
	~terminal();

	int update(const char* title);

	//public method to share thread safe rxQueue copy
	const std::deque<char>& getSafeRxQueue() const;
	
	void ApplyConfig(); //apply configs from json to class memebers "OPEN button in mainMenu"
	void StoreConfig(); //save vars to AppConfig "SAVE button in mainMenu"

	void clearRxQueue();
	bool clear_samples = false;

	void debug_getKernelcharCount(size_t* len);

private:
	//Manages the serial data terminal display window.
	terminal_output term_out;

	//serial Manager class that has all the buffers, and higher level serial data manipulation.
	serialManager serialManObj; //Serial Manager

	AppConfig& configRef;
	
	struct terminal_ui {
		char input_buffer_Port[64] = { 0 };   // Buffer for COM port from user entry in text box
		char input_buffer_Baud[64] = { 0 };  // Buffer for baud rate from user entry in text box
		char input_buffer_scrollback_len[64] = { 0 };  // Buffer for scroll back
		char input_buffer_Tx_Data[4096] = { 0 }; //Buffer for tx Data from user entry in text box
		bool open_popup_port = false; //use for comm port popup
		bool open_popup_baud = false; //use for baudrate popup
		bool button_pause_serial = false; //use for pausing updating screen
	} ui;
	
	struct controls {
		bool tx_eol_cr = false; //use for addeding eol to tx strings
		bool tx_eol_lf = false; //use for addeding eol to tx strings
	} controls;


	int handle_connect_button();
	int handle_disconnect_button();
	int handle_send_button();

	//holds a copy of the serial Manager rxQueue.
	std::deque<char> _Term_rxQueue;
};