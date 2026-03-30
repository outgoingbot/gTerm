#ifndef DATAPLOTTER_H
#define DATAPLOTTER_H

#include <deque>
#include <string>
#include <vector>
#include "imgui.h"
#include "implot.h"

class dataPlotter {
public:
    dataPlotter();
    ~dataPlotter() = default;

    // The main entry point called every frame
    void update(std::deque<char>& tempRxDeque);
    // Helper to parse the deque into our sample buffer
    void ParseData(std::deque<char>& deque);

private:
    // Constants
    static constexpr int MAX_BUFFER_SIZE = 2048; // Adjust as needed

    // UI State (Replaces the 'static' variables from your snippet)
    bool autoScale = false;
    bool follow_x = true;
    float y_min = -20.f;
    float y_max = 20.f;
    int pointsToDisplay = 512;

    // Data Storage
    struct Sample { float sin, sq, saw; };
    Sample samples[MAX_BUFFER_SIZE];
    int sampleCount = 0;

    // Internal Plotting Buffers
    float x_data[MAX_BUFFER_SIZE];
    float y1_data[MAX_BUFFER_SIZE];
    float y2_data[MAX_BUFFER_SIZE];
    float y3_data[MAX_BUFFER_SIZE];

};

#endif // DATAPLOTTER_H