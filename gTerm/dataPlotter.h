#ifndef DATAPLOTTER_H
#define DATAPLOTTER_H

#include <deque>
#include <string>
#include <vector>
#include "imgui.h"
#include "implot.h"
#include "dataParser.h"

class dataPlotter {
public:
    dataPlotter();
    ~dataPlotter() = default;

    static constexpr size_t MAX_SAMPLES = 4096;   // how many lines we keep for plotting
    static constexpr size_t MAX_CHANNELS = 128;    // safety limit
    dataParser parser;
    std::vector<ParsedSample> currentSamples;   // or process on the fly
    // Plotting buffers - these will be resized dynamically
    std::vector<float> x_data;
    std::vector<std::vector<float>> y_data;     // one vector per channel

    // UI / settings
    int pointsToDisplay = 512;                  // you can increase this
    int maxChannels = 128;                      // safety limit

    size_t lastNumChannels = 0;
    size_t lastPointsToDisplay = 0;

    // The main entry point called every frame
    void update(dataParser& parser, const std::deque<char>& rxDeque);
    // Helper to parse the deque into our sample buffer
    //void ParseData(const std::deque<char>& deque);

private:
    // Constants
    static constexpr int MAX_BUFFER_SIZE = 2048; // Adjust as needed

    // UI State (Replaces the 'static' variables from your snippet)
    bool autoScale = true;
    bool follow_x = true;
    float y_min = -20.f;
    float y_max = 20.f;


    // Data Storage
    struct Sample { float sin, sq, saw; };
    Sample samples[MAX_BUFFER_SIZE];
    int sampleCount = 0;


};

#endif // DATAPLOTTER_H