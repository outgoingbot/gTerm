#include "dataPlotter.h"
#include <cstdio>
#include <cstring>

dataPlotter::dataPlotter() {
    // Initialize buffers if necessary
    memset(samples, 0, sizeof(samples));
}

//TODO: Move this into parser class. Most likely going to have to create a struct to pass to plotter with other control stuff
//like how many plots, data. a proper data strcture
void dataPlotter::ParseData(const std::deque<char>& deque) {
    if (deque.empty()) return;

    // Convert deque to string for sscanf processing
    std::string temp(deque.begin(), deque.end());
    const char* p = temp.c_str();

    sampleCount = 0; // Reset count for this update

    while (p && *p && sampleCount < MAX_BUFFER_SIZE) {
        float a = 0, b = 0, c = 0;
        // Parsing the CSV format: "val1,val2,val3"
        if (sscanf(p, "%f,%f,%f", &a, &b, &c) == 3) {
            samples[sampleCount++] = { a, b, c };
        }

        // Move to the next line
        const char* nl = strchr(p, '\n');
        if (!nl) break;
        p = nl + 1;
    }
}

void dataPlotter::update(const std::deque<char>& tempRxDeque) {
    ImGui::Begin("Live Serial Plot");

    // 1. UI Controls
    ImGui::Checkbox("Auto Y Scale", &autoScale);
    ImGui::SameLine();
    ImGui::Checkbox("Follow X", &follow_x);

    if (!autoScale) {
        //ImGui::SliderFloat("Y Min", &y_min, -5.0f, 0.0f);
        //ImGui::SliderFloat("Y Max", &y_max, 0.0f, 5.0f);
    }

    ImGui::SliderInt("Points to Display", &pointsToDisplay, 64, MAX_BUFFER_SIZE);

    // 2. Data Processing
    ParseData(tempRxDeque);

    if (sampleCount == 0) {
        ImGui::Text("No valid data received");
    }
    else {
        // Calculate window of data to show
        int count = (sampleCount < pointsToDisplay) ? sampleCount : pointsToDisplay;
        int start = sampleCount - count;

        // Fill plotting arrays
        for (int i = 0; i < count; i++) {
            int idx = start + i;
            x_data[i] = (float)i;
            y1_data[i] = samples[idx].sin;
            y2_data[i] = samples[idx].sq;
            y3_data[i] = samples[idx].saw;
        }

        // 3. Plotting
        ImVec2 sz = ImGui::GetContentRegionAvail();
        // Ensure minimum size
        if (sz.x < 100) sz.x = 100;
        if (sz.y < 100) sz.y = 100;

        if (ImPlot::BeginPlot("Waveforms", sz, ImPlotFlags_Crosshairs)) {
            ImPlot::SetupAxes("Samples", "Value");

            // X-Axis logic
            if (follow_x) {
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, count - 1, ImGuiCond_Always);
            }
            else {
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, count - 1, ImGuiCond_Once);
            }

            // Y-Axis logic
            if (autoScale) {
                ImPlot::SetupAxis(ImAxis_Y1, "Value", ImPlotAxisFlags_AutoFit);
            }
            else {
                ImPlot::SetupAxisLimits(ImAxis_Y1, y_min, y_max);
            }

            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.0f);

            ImPlot::SetNextLineStyle(ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            ImPlot::PlotLine("Sine", x_data, y1_data, count);

            ImPlot::SetNextLineStyle(ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
            ImPlot::PlotLine("Square", x_data, y2_data, count);

            ImPlot::SetNextLineStyle(ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
            ImPlot::PlotLine("Saw", x_data, y3_data, count);

            ImPlot::PopStyleVar();
            ImPlot::EndPlot();
        }
    }

    ImGui::End();
}