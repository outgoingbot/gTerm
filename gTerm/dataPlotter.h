#ifndef DATAPLOTTER_H
#define DATAPLOTTER_H

#include <deque>
#include <string>
#include <vector>
#include <imgui.h>
#include <implot.h>
#include "dataParser.h"

class dataPlotter {
public:
    dataPlotter(const dataParser& p);
    ~dataPlotter() = default;
    static constexpr size_t MAX_SAMPLES_OFFSET = 64; // hacky fix to give some headroom to number of samples to plot due to ascii conversion variation
    static constexpr size_t MAX_SAMPLES = 4096;   // default value
    static constexpr size_t MAX_CHANNELS = 128;    // safety limit

    const dataParser& parser;

    // Plotting buffers - these will be resized dynamically
    std::vector<ParsedSample> currentSamples;
    std::vector<float> x_data;

    // UI / settings
    int pointsToDisplay = 128; //default value. may consider adjusting dynamically based on currentSamples size
    size_t lastPointsToDisplay = 0;


    void update(const std::deque<char>& rxDeque);
    
    void clearSamples();

private:

    // UI State
    bool autoScale = true;
    bool follow_x = true;
    
};

#endif // DATAPLOTTER_H