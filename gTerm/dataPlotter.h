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

    static constexpr size_t MAX_SAMPLES = 4096;   // how many lines we keep for plotting
    static constexpr size_t MAX_CHANNELS = 128;    // safety limit

    const dataParser& parser;

    std::vector<ParsedSample> currentSamples;   // or process on the fly
    // Plotting buffers - these will be resized dynamically
    std::vector<float> x_data;

    // UI / settings
    //TODO: we need to figure out how to associate how many samples we actually have with a full rxQueue buffer
    //This is hard because ascii-> float (or int) can be n chars = 1 number
    //i.e. i could have '1.0' ascii chars, so thats 3 chars = 1 number
    //or i could have '1.0000' ascii chars, and thats still = 1 number
    //so the slider would have to adjust for that somehow/
    //maybe we once the rqQueue buffer is full we can then figure our max points to display (MAX_SAMPELS)
    int pointsToDisplay = 128; //slider from 8 to MAX_SAMPLES
    size_t lastPointsToDisplay = 0;

    // The main entry point called every frame
    void update(const std::deque<char>& rxDeque);
    

private:

    // UI State
    bool autoScale = true;
    bool follow_x = true;
    
};

#endif // DATAPLOTTER_H