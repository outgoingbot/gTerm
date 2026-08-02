
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
#include <cctype>
#include <chrono>
#include <cstdint>
#include "ConfigManager.h"
#include "logger.h"

struct ParsedSample {
	std::vector<double> values;   // one value per field in the format string
	double timestampSeconds = 0.0; // monotonic host receive time, relative to the current capture
	std::uint64_t sampleNumber = 0;
};


class dataParser {

public:
	dataParser(AppConfig& cfg);
	~dataParser();

	//int update(std::deque<char> charArray);
	int update();
#ifdef DEBUG
	std::string format = "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f";   // User can change this at runtime
#else
	std::string format = "%f,%f,%f";   // User can change this at runtime
#endif
	
	//TODO: allow user to customize these
	char delimiter = ',';
	char eol = '\n';

	// Call this whenever the user changes format, delimiter, or eol
	void compile();

	// Parse only the newest bytes appended to deque. Incomplete lines are retained
	// between calls so the render loop never needs to reparse the receive history.
	void parse(const std::deque<char>& deque, size_t newCharCount, std::vector<ParsedSample>& outSamples);
	void resetStreamingState();
	void setTimestampClockRunning(bool running);
	double currentTimestampSeconds() const;
	std::uint64_t getFormatRevision() const { return formatRevision; }

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
	

	void ApplyConfig(); //apply configs from json to class memebers "OPEN button in mainMenu"
	void StoreConfig(); //save vars to AppConfig "SAVE button in mainMenu"

	//make this private
	
private:
	
	AppConfig& configRef;
	
	std::vector<char> channelSelected;

	typedef struct window_params_t {
		float min_height = 100.0f;
		float width = 600.0f; // Window width
		float height = 600.f; // Window height
		//float drag_bar_height = 20.f;
	} window_params_t;

	window_params_t _window_params;

	struct FormatSpecifier {
		enum class Type { Float, SignedInt, UnsignedInt, Hex, Invalid };
		Type type = Type::Invalid;
	};

	std::vector<FormatSpecifier> specifiers;
	std::string pendingLine;
	bool discardingOverlongLine = false;
	std::uint64_t nextSampleNumber = 0;
	std::uint64_t formatRevision = 0;
	bool timestampClockRunning = false;
	double accumulatedTimestampSeconds = 0.0;
	std::chrono::steady_clock::time_point timestampRunStartedAt = std::chrono::steady_clock::now();
	static constexpr size_t MAX_PENDING_LINE_LENGTH = 8192;

	std::vector<FormatSpecifier> parse_specifiers(const std::string& fmt) const;
	std::vector<std::string> split_line(const std::string& line) const;
	std::optional<double> parse_token(const std::string& token, const FormatSpecifier& spec) const;
	std::vector<ParsedSample> lastSamples;

};
