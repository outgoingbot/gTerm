#include "dataPlotter.h"
#include <cstdio>
#include <cstring>

dataPlotter::dataPlotter() {
    // Initialize buffers if necessary
    memset(samples, 0, sizeof(samples));
}

void dataPlotter::update(dataParser& parser, const std::deque<char>& rxDeque)
{
    ImGui::Begin("Live Serial Plot");

    // ====================== Format Configuration ======================
    static char formatBuf[256];

    // Sync buffer with current parser.format when it changes externally (or first time)
    if (strcmp(formatBuf, parser.format.c_str()) != 0) {
        strncpy(formatBuf, parser.format.c_str(), sizeof(formatBuf) - 1);
        formatBuf[sizeof(formatBuf) - 1] = '\0';
    }

    ImGui::InputText("Format string (e.g. %f,%i,%f,%d)", formatBuf, sizeof(formatBuf));

    // Use this more reliable pattern
    if (ImGui::IsItemDeactivatedAfterEdit() || ImGui::IsItemActivated() == false && ImGui::IsItemFocused() == false) {
        std::string newFormat(formatBuf);

        if (newFormat != parser.format) {
            parser.format = std::move(newFormat);
            parser.compile();
            printf("Format updated to: '%s' (%zu channels)\n", parser.format.c_str(), parser.getChannelCount());
        }
    }

    ImGui::SameLine();
    ImGui::Text(" | Channels: %zu", parser.getChannelCount());

    ImGui::Checkbox("Auto Y Scale", &autoScale);
    ImGui::SameLine();
    ImGui::Checkbox("Follow X", &follow_x);
    ImGui::SliderInt("Points to Display", &pointsToDisplay, 64, 4096);

    // ====================== Parse Data ======================
    parser.parse(rxDeque, currentSamples);

    if (currentSamples.empty()) {
        ImGui::Text("Waiting for valid data... Check format string and serial input.");
        ImGui::End();
        return;
    }

    // ====================== Prepare Plot Data ======================
    size_t displayCount = std::min(currentSamples.size(), static_cast<size_t>(pointsToDisplay));
    size_t startIdx = currentSamples.size() - displayCount;

    size_t numChannels = parser.getChannelCount();
    if (numChannels == 0) numChannels = 1;

    x_data.resize(displayCount);
    y_data.resize(numChannels);
    for (auto& ch : y_data) ch.resize(displayCount);

    for (size_t i = 0; i < displayCount; ++i) {
        size_t idx = startIdx + i;
        const auto& sample = currentSamples[idx];

        x_data[i] = static_cast<float>(i);

        for (size_t ch = 0; ch < numChannels; ++ch) {
            y_data[ch][i] = (ch < sample.values.size())
                ? static_cast<float>(sample.values[ch])
                : 0.0f;
        }
    }

    // ====================== Plotting ======================
    ImVec2 sz = ImGui::GetContentRegionAvail();
    if (sz.x < 100) sz.x = 100;
    if (sz.y < 100) sz.y = 100;

    if (ImPlot::BeginPlot("Live Serial Data", sz, ImPlotFlags_Crosshairs)) {
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

        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.0f);

        for (size_t ch = 0; ch < numChannels; ++ch) {
            std::string label = "Channel " + std::to_string(ch + 1);
            ImVec4 color = ImPlot::GetColormapColor(static_cast<int>(ch) % 10);
            ImPlot::SetNextLineStyle(color);

            ImPlot::PlotLine(label.c_str(), x_data.data(), y_data[ch].data(),
                static_cast<int>(displayCount));
        }

        ImPlot::PopStyleVar();
        ImPlot::EndPlot();
    }

    ImGui::End();
}