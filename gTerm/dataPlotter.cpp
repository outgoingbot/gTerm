#include "dataPlotter.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <utility>

dataPlotter::dataPlotter(dataParser& p) : parser(p), parserFormatRevision(p.getFormatRevision()) {
    newSamples.reserve(256);
    x_data.reserve(MAX_SAMPLES);
    y_data.reserve(MAX_SAMPLES);
}

void dataPlotter::update(const std::deque<char>& rxDeque, size_t newCharCount, bool captureRunning)
{
    parser.setTimestampClockRunning(captureRunning);

    // A format change starts a new capture because old samples may have a
    // different channel layout.
    if (parserFormatRevision != parser.getFormatRevision()) {
        currentSamples.clear();
        parserFormatRevision = parser.getFormatRevision();
    }

    parser.parse(rxDeque, newCharCount, newSamples);
    for (auto& sample : newSamples) {
        currentSamples.push_back(std::move(sample));
    }
    while (currentSamples.size() > MAX_SAMPLES) {
        currentSamples.pop_front();
    }

    if (!ImGui::Begin("Live Serial Plot")) {
        ImGui::End();
        return;
    }

    ImGui::Checkbox("Auto Y Scale", &autoScale);
    ImGui::SameLine();
    ImGui::Checkbox("Follow X", &follow_x);

    verticalSeparator();

    ImGui::Checkbox("Time X Axis", &timeDomain);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Off: sample number\nOn: elapsed host receive time");
    }
    
    verticalSeparator();
    
    ImGui::PushItemWidth(300.0f);
    ImGui::SliderFloat("Line Width", &plot_line_weight_slider_var, plot_line_weight_min, plot_line_weight_max, "%.2f");
    ImGui::PopItemWidth();

    verticalSeparator();

    ImGui::PushItemWidth(300.0f);
    if (ImGui::BeginCombo("Plot Flags##config", "Click to edit flags")) {

        ImGui::Checkbox("No Title", &plot_flags_begin.NoTitle);
        ImGui::Checkbox("No Legend", &plot_flags_begin.NoLegend);
        ImGui::Checkbox("No Mouse Text", &plot_flags_begin.NoMouseText);
        ImGui::Checkbox("No Inputs", &plot_flags_begin.NoInputs);
        ImGui::Checkbox("No Menus", &plot_flags_begin.NoMenus);
        ImGui::Checkbox("No Box Select", &plot_flags_begin.NoBoxSelect);
        ImGui::Checkbox("No Frame", &plot_flags_begin.NoFrame);
        ImGui::Checkbox("Equal Aspect", &plot_flags_begin.Equal);
        ImGui::Checkbox("Crosshairs", &plot_flags_begin.Crosshairs);

        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();


    verticalSeparator();

    ImGui::Checkbox("Show Advanced Style", &show_advanced_syle_setting);


    // Cleaner version - direct bitwise OR (no if statements)
    ImPlotFlags flags_begin = ImPlotFlags_None
        | (plot_flags_begin.NoTitle ? ImPlotFlags_NoTitle : 0)
        | (plot_flags_begin.NoLegend ? ImPlotFlags_NoLegend : 0)
        | (plot_flags_begin.NoMouseText ? ImPlotFlags_NoMouseText : 0)
        | (plot_flags_begin.NoInputs ? ImPlotFlags_NoInputs : 0)
        | (plot_flags_begin.NoMenus ? ImPlotFlags_NoMenus : 0)
        | (plot_flags_begin.NoBoxSelect ? ImPlotFlags_NoBoxSelect : 0)
        | (plot_flags_begin.NoFrame ? ImPlotFlags_NoFrame : 0)
        | (plot_flags_begin.Equal ? ImPlotFlags_Equal : 0)
        | (plot_flags_begin.Crosshairs ? ImPlotFlags_Crosshairs : 0);



    //===============================================================================
    if (show_advanced_syle_setting) {
        if (ImGui::CollapsingHeader("Plot Style Variables")) {
            ImGui::SliderFloat("Border Size", &plotStyle.PlotBorderSize, 0.0f, 8.0f, "%.1f");
            ImGui::SliderFloat("Minor Alpha", &plotStyle.MinorAlpha, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("Digital Padding", &plotStyle.DigitalPadding, 0.0f, 50.0f, "%.0f");
            ImGui::SliderFloat("Digital Spacing", &plotStyle.DigitalSpacing, 0.0f, 20.0f, "%.0f");

            ImGui::Separator();
            ImGui::Text("Padding & Spacing");
            ImGui::SliderFloat2("Plot Padding", &plotStyle.PlotPadding.x, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat2("Label Padding", &plotStyle.LabelPadding.x, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("Legend Padding", &plotStyle.LegendPadding.x, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat2("Legend Inner", &plotStyle.LegendInnerPadding.x, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("Legend Spacing", &plotStyle.LegendSpacing.x, 0.0f, 20.0f, "%.0f");

            ImGui::Separator();
            ImGui::Text("Ticks & Grid");
            ImGui::SliderFloat2("Major Tick Len", &plotStyle.MajorTickLen.x, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat2("Minor Tick Len", &plotStyle.MinorTickLen.x, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("Major Grid Size", &plotStyle.MajorGridSize.x, 0.0f, 5.0f, "%.1f");
            ImGui::SliderFloat2("Minor Grid Size", &plotStyle.MinorGridSize.x, 0.0f, 5.0f, "%.1f");

            ImGui::Separator();
            ImGui::Text("Other");
            ImGui::SliderFloat2("Mouse Pos Pad", &plotStyle.MousePosPadding.x, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("Annotation Pad", &plotStyle.AnnotationPadding.x, 0.0f, 15.0f, "%.0f");
            ImGui::SliderFloat2("Fit Padding", &plotStyle.FitPadding.x, 0.0f, 0.5f, "%.2f");
        }
    }
    //===============================================================================







    // ====================== Parser controls plot assignment ======================
    //ImGui::Separator();
    //if (ImGui::Button("Reset: One Channel per Plot")) {
    //    size_t numCh = parser.getChannelCount();
    //    for (size_t i = 0; i < numCh; ++i) {
    //        parser.setChannelToPlot(static_cast<int>(i), static_cast<int>(i));
    //    }
    //}
    //ImGui::SameLine();
    //if (ImGui::Button("All Channels in One Plot")) {
    //    size_t numCh = parser.getChannelCount();
    //    for (size_t i = 0; i < numCh; ++i) {
    //        parser.setChannelToPlot(static_cast<int>(i), 0);
    //    }
    //}

    ImGui::Separator();

    if (currentSamples.empty()) {
        ImGui::Text("Waiting for valid data...");
        ImGui::End();
        return;
    }
    

    const size_t numAvailable = currentSamples.size();
    const size_t numChannels = std::min(parser.getChannelCount(), MAX_CHANNELS);
    size_t startIdx = 0;
    size_t displayCount = 0;
    const double nowSeconds = parser.currentTimestampSeconds();

    if (timeDomain) {
        ImGui::PushItemWidth(300.0f);
        ImGui::SliderFloat("Time Window (s)", &timeWindowSeconds, 0.5f, 30.0f, "%.1f");
        ImGui::PopItemWidth();

        if (follow_x) {
            const double oldestVisibleTime = nowSeconds - static_cast<double>(timeWindowSeconds);
            auto firstVisible = std::lower_bound(
                currentSamples.begin(), currentSamples.end(), oldestVisibleTime,
                [](const ParsedSample& sample, double timestamp) {
                    return sample.timestampSeconds < timestamp;
                });
            if (firstVisible != currentSamples.begin()) {
                --firstVisible; // preserve line continuity at the left edge
            }
            startIdx = static_cast<size_t>(std::distance(currentSamples.begin(), firstVisible));
        }
        displayCount = currentSamples.size() - startIdx;
    }
    else {
        ImGui::PushItemWidth(300.0f);
        ImGui::SliderInt("Points to Display", &pointsToDisplay,
            static_cast<int>(minDisplayable), static_cast<int>(MAX_SAMPLES));
        ImGui::PopItemWidth();

        displayCount = std::min(numAvailable, static_cast<size_t>(pointsToDisplay));
        startIdx = numAvailable - displayCount;
    }

    x_data.resize(displayCount);
    for (size_t i = 0; i < displayCount; ++i) {
        x_data[i] = timeDomain
            ? currentSamples[startIdx + i].timestampSeconds
            : static_cast<double>(currentSamples[startIdx + i].sampleNumber);
    }

    // Build groups - one channel can now be on multiple plots
    const auto& map = parser.getChannelToPlotMap();
    int numPlots = parser.getPlotCount();

    ImGui::Text("Plots: %d | Buffered samples: %zu / %zu | Visible: %zu",
        numPlots, numAvailable, MAX_SAMPLES, displayCount);

    std::vector<std::vector<int>> activeGroups(numPlots);

    for (size_t ch = 0; ch < numChannels; ++ch) {
        if (ch >= map.size()) continue;

        for (int plotId : map[ch]) {                    // loop over every plot this channel is assigned to
            if (plotId >= 0 && plotId < numPlots) {
                activeGroups[plotId].push_back(static_cast<int>(ch));
            }
        }
    }


    // ====================== Apply Style ONCE per frame ======================
    applyStyle();   // moved outside the plot loop

    // ====================== Plotting - ONLY plots with assigned channels ======================
    ImVec2 plotSize = ImGui::GetContentRegionAvail();

    plotSize.x = -FLT_MIN;
    if (numPlots > 0) {
        plotSize.y = (plotSize.y / numPlots) - 8; //fill window with plots. -8 for float rounding error (removes scroll bar)
    }
    //if (plotSize.x < 100) plotSize.x = 100;
    if (plotSize.y < 100) plotSize.y = 100;

    bool anyPlotDrawn = false;

    for (int p = 0; p < numPlots; ++p) {
        const auto& group = activeGroups[p];
        if (group.empty()) continue;        // Skip plots with no channels assigned

        anyPlotDrawn = true;

        std::string title = "Plot " + std::to_string(p + 1);
        if (ImPlot::BeginPlot(title.c_str(), plotSize, flags_begin)) {
            const char* xAxisLabel = timeDomain ? "Time (s)" : "Sample";
            const ImPlotAxisFlags yAxisFlags = ImPlotAxisFlags_NoLabel |
                (autoScale ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None);
            ImPlot::SetupAxes(xAxisLabel, nullptr, ImPlotAxisFlags_None, yAxisFlags);

            if (timeDomain) {
                ImPlot::SetupAxisFormat(ImAxis_X1, "%.2f");
                if (follow_x) {
                    ImPlot::SetupAxisLimits(ImAxis_X1,
                        nowSeconds - static_cast<double>(timeWindowSeconds), nowSeconds, ImGuiCond_Always);
                }
                else if (displayCount > 0) {
                    const double firstTime = currentSamples[startIdx].timestampSeconds;
                    const double lastTime = currentSamples[startIdx + displayCount - 1].timestampSeconds;
                    ImPlot::SetupAxisLimits(ImAxis_X1, firstTime,
                        std::max(lastTime, firstTime + 0.001), ImGuiCond_Once);
                }
            }
            else {
                const double firstBufferedSample = static_cast<double>(currentSamples.front().sampleNumber);
                const double lastBufferedSample = static_cast<double>(currentSamples.back().sampleNumber);
                ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, firstBufferedSample, lastBufferedSample + 8.0);
                const double firstVisibleSample = static_cast<double>(currentSamples[startIdx].sampleNumber);
                const double lastVisibleSample = static_cast<double>(currentSamples[startIdx + displayCount - 1].sampleNumber);
                ImPlot::SetupAxisLimits(ImAxis_X1, firstVisibleSample,
                    std::max(lastVisibleSample, firstVisibleSample + 1.0),
                    follow_x ? ImGuiCond_Always : ImGuiCond_Once);
            }

            for (int ch : group) {
                std::string label = "Channel " + std::to_string(ch + 1);
                ImVec4 color = ImPlot::GetColormapColor(ch % 10);

                ImPlotSpec spec;
                spec.LineColor = color;
                spec.LineWeight = plot_line_weight_slider_var;

                y_data.resize(displayCount);

                for (size_t i = 0; i < displayCount; ++i) {
                    size_t idx = startIdx + i;
                    y_data[i] = (ch < static_cast<int>(currentSamples[idx].values.size()))
                        ? currentSamples[idx].values[ch]
                        : 0.0;
                }

                if (displayCount > 0) {
                    ImPlot::PlotLine(label.c_str(), x_data.data(), y_data.data(), static_cast<int>(displayCount), spec);
                }
            }

            ImPlot::EndPlot();
        }

        if (p < numPlots - 1) ImGui::Spacing();
    }

    if (!anyPlotDrawn) {
        ImGui::Text("No channels assigned to any plot yet.\nUse the checkboxes in Data Parser to assign channels.");
    }
    ImPlot::PopStyleVar(16);   // match your applyStyle() count
    ImGui::End();
}




void dataPlotter::clearSamples() {
    currentSamples.clear();
    newSamples.clear();
    x_data.clear();
    y_data.clear();
    parser.resetStreamingState();
    parserFormatRevision = parser.getFormatRevision();
    pointsToDisplay = 128;
}


void dataPlotter::verticalSeparator() {
    //ImVec4(0.65f, 0.35f, 0.00f, 1.00f); //add Orange Accent
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.65f, 0.35f, 0.0f, 1.0f));
    ImGui::SameLine();
    ImGui::Text(" | ");
    ImGui::SameLine();
    ImGui::PopStyleColor(1);
}

void dataPlotter::applyStyle() {
    // === Apply Runtime Plot Style ===
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotBorderSize, plotStyle.PlotBorderSize);
    ImPlot::PushStyleVar(ImPlotStyleVar_MinorAlpha, plotStyle.MinorAlpha);
    ImPlot::PushStyleVar(ImPlotStyleVar_DigitalPadding, plotStyle.DigitalPadding);
    ImPlot::PushStyleVar(ImPlotStyleVar_DigitalSpacing, plotStyle.DigitalSpacing);

    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, plotStyle.PlotPadding);
    ImPlot::PushStyleVar(ImPlotStyleVar_LabelPadding, plotStyle.LabelPadding);
    ImPlot::PushStyleVar(ImPlotStyleVar_LegendPadding, plotStyle.LegendPadding);
    ImPlot::PushStyleVar(ImPlotStyleVar_LegendInnerPadding, plotStyle.LegendInnerPadding);
    ImPlot::PushStyleVar(ImPlotStyleVar_LegendSpacing, plotStyle.LegendSpacing);

    ImPlot::PushStyleVar(ImPlotStyleVar_MajorTickLen, plotStyle.MajorTickLen);
    ImPlot::PushStyleVar(ImPlotStyleVar_MinorTickLen, plotStyle.MinorTickLen);
    ImPlot::PushStyleVar(ImPlotStyleVar_MajorGridSize, plotStyle.MajorGridSize);
    ImPlot::PushStyleVar(ImPlotStyleVar_MinorGridSize, plotStyle.MinorGridSize);

    ImPlot::PushStyleVar(ImPlotStyleVar_MousePosPadding, plotStyle.MousePosPadding);
    ImPlot::PushStyleVar(ImPlotStyleVar_AnnotationPadding, plotStyle.AnnotationPadding);
    ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, plotStyle.FitPadding);
}
