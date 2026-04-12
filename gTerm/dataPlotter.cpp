#include "dataPlotter.h"
#include <cstdio>
#include <cstring>

dataPlotter::dataPlotter() {
    // Initialize buffers if necessary
    //memset(samples, 0, sizeof(samples));
}

void dataPlotter::update(dataParser& parser, const std::deque<char>& rxDeque)
{
    ImGui::Begin("Live Serial Plot");

    ImGui::Checkbox("Auto Y Scale", &autoScale);
    ImGui::SameLine();
    ImGui::Checkbox("Follow X", &follow_x);
    ImGui::SliderInt("Points to Display", &pointsToDisplay, 64, static_cast<int>(MAX_SAMPLES));

    // ====================== Parser controls plot assignment ======================
    ImGui::Separator();
    if (ImGui::Button("Reset: One Channel per Plot")) {
        size_t numCh = parser.getChannelCount();
        for (size_t i = 0; i < numCh; ++i) {
            parser.setChannelToPlot(static_cast<int>(i), static_cast<int>(i));
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("All Channels in One Plot")) {
        size_t numCh = parser.getChannelCount();
        for (size_t i = 0; i < numCh; ++i) {
            parser.setChannelToPlot(static_cast<int>(i), 0);
        }
    }

    ImGui::Separator();

    // ====================== Parse Data ======================
    parser.parse(rxDeque, currentSamples);
    if (currentSamples.empty()) {
        ImGui::Text("Waiting for valid data...");
        ImGui::End();
        return;
    }

    size_t numChannels = parser.getChannelCount();
    size_t displayCount = std::min(currentSamples.size(), static_cast<size_t>(pointsToDisplay));
    size_t startIdx = currentSamples.size() > displayCount ? currentSamples.size() - displayCount : 0;

    if (displayCount != lastPointsToDisplay) {
        x_data.resize(displayCount);
        lastPointsToDisplay = displayCount;
    }
    for (size_t i = 0; i < displayCount; ++i) {
        x_data[i] = static_cast<float>(i);
    }

    // Build groups - one channel can now be on multiple plots
    const auto& map = parser.getChannelToPlotMap();
    int numPlots = parser.getPlotCount();

    std::vector<std::vector<int>> activeGroups(numPlots);

    for (size_t ch = 0; ch < numChannels; ++ch) {
        if (ch >= map.size()) continue;

        for (int plotId : map[ch]) {                    // loop over every plot this channel is assigned to
            if (plotId >= 0 && plotId < numPlots) {
                activeGroups[plotId].push_back(static_cast<int>(ch));
            }
        }
    }

    // ====================== Plotting - ONLY plots with assigned channels ======================
    ImVec2 plotSize = ImGui::GetContentRegionAvail();
    if (plotSize.x < 100) plotSize.x = 100;
    if (plotSize.y < 100) plotSize.y = 100;
    plotSize.x = -FLT_MIN;
    plotSize.y = 300;

    bool anyPlotDrawn = false;

    for (int p = 0; p < numPlots; ++p) {
        const auto& group = activeGroups[p];
        if (group.empty()) continue;        // Skip plots with no channels assigned

        anyPlotDrawn = true;

        std::string title = "Plot " + std::to_string(p + 1);

        if (ImPlot::BeginPlot(title.c_str(), plotSize, ImPlotFlags_Crosshairs)) {
            ImPlot::SetupAxes("Sample", "Value");

            if (follow_x) {
                ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, static_cast<double>(displayCount) - 1.0, ImGuiCond_Always);
            }
            else {
                ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, static_cast<double>(displayCount) - 1.0, ImGuiCond_Once);
            }

            if (autoScale) {
                ImPlot::SetupAxis(ImAxis_Y1, "Value", ImPlotAxisFlags_AutoFit);
            }

            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 4.0f);

            for (int ch : group) {
                std::vector<float> y(displayCount);
                for (size_t i = 0; i < displayCount; ++i) {
                    size_t idx = startIdx + i;
                    y[i] = (ch < static_cast<int>(currentSamples[idx].values.size()))
                        ? static_cast<float>(currentSamples[idx].values[ch])
                        : 0.0f;
                }

                std::string label = "Channel " + std::to_string(ch + 1);
                ImVec4 color = ImPlot::GetColormapColor(ch % 10);
                ImPlot::SetNextLineStyle(color);
                ImPlot::PlotLine(label.c_str(), x_data.data(), y.data(), static_cast<int>(displayCount));
            }

            ImPlot::PopStyleVar();
            ImPlot::EndPlot();
        }

        if (p < numPlots - 1) ImGui::Spacing();
    }

    if (!anyPlotDrawn) {
        ImGui::Text("No channels assigned to any plot yet.\nUse the checkboxes in Data_Manipulation_Region to assign channels.");
    }

    ImGui::End();
}