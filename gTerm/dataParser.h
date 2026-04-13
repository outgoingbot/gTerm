
/*
Data parsing class.
*/

#pragma once
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <sstream>
#include <string>
#include <optional>

struct ParsedSample {
	std::vector<double> values;   // one value per field in the format string
};


class dataParser {

public:
	dataParser();
	~dataParser();

	//int update(std::deque<char> charArray);
	int update();
#ifndef DEBUG
	std::string format = "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f";   // User can change this at runtime
#else
	std::string format = "%f,%f,%f";   // User can change this at runtime
#endif
	
	char delimiter = ',';
	char eol = '\n';

	// Call this whenever the user changes format, delimiter, or eol
	void compile();

	// Main parsing function - replaces your old ParseData()
	// Returns all successfully parsed samples from the deque
	void parse(const std::deque<char>& deque, std::vector<ParsedSample>& outSamples) const;

	// Convenience: get current number of expected channels (after compile())
	size_t getChannelCount() const;

	// Add to DataParser class in DataParser.h
	const std::vector<ParsedSample>& getSamples() const {
		return lastSamples;
	}


	// Plot assignment - you control this here (channel index -> plot index)
	void setChannelToPlot(int channelIndex, int plotIndex);
	void removeChannelFromPlot(int channelIndex, int plotIndex);
	int getPlotCount() const;

	const std::vector<std::vector<int>>& getChannelToPlotMap() const;
	std::vector<std::vector<int>> channelToPlotMap;   // channelToPlotMap[channel] = list of plots it appears on

	//TODO: Put this in a struct
	bool dataParse_enable = true;
	bool send_to_plot = true;
	

private:
	
	//char _buffer[128]; // Buffer to store the text input
	bool _autoScroll;                // Controls auto-scrolling behavior

	typedef struct window_params_t {
		float min_height = 100.0f;
		float width = 600.0f; // Window width
		float height = 600.f; // Window height

		float drag_bar_height = 20.f;
	} window_params_t;

	window_params_t _window_params;

	struct FormatSpecifier {
		enum class Type { Float, SignedInt, UnsignedInt, Hex, Invalid };
		Type type = Type::Invalid;
	};

	std::vector<FormatSpecifier> specifiers;

	std::vector<FormatSpecifier> parse_specifiers(const std::string& fmt) const;
	std::vector<std::string> split_line(const std::string& line) const;
	std::optional<double> parse_token(const std::string& token, const FormatSpecifier& spec) const;
	std::vector<ParsedSample> lastSamples;   // add this



	std::deque<std::deque<char>> float_buffer_2d;

	char input_text[64]; // Buffer to store the text input
};