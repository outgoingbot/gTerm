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
    dataPlotter(dataParser& p);
    ~dataPlotter() = default;
    static constexpr size_t MAX_SAMPLES = 4096;   // default value
    static constexpr size_t MAX_CHANNELS = 128;    // safety limit

    dataParser& parser;



    // UI / settings
    int pointsToDisplay = 128; //default value. may consider adjusting dynamically based on currentSamples size
    const size_t minDisplayable = 8;
    






    void update(const std::deque<char>& rxDeque, size_t newCharCount);
    
    void clearSamples();


private:
    
    //effort to imrove perfmance
    // Plotting buffers - these will be resized dynamically
    std::deque<ParsedSample> currentSamples;
    std::vector<ParsedSample> newSamples;
    std::vector<double> x_data;
    std::vector<double> y_data;
	std::uint64_t parserFormatRevision = 0;

    // UI State
    bool autoScale = true;
    bool follow_x = true;
    bool timeDomain = false;
    float timeWindowSeconds = 10.0f;
    bool show_advanced_syle_setting = false;
    
    float plot_line_weight_slider_var = 4.00f;
    const float plot_line_weight_min = 0.50f;
    const float plot_line_weight_max = 8.00f;

    //TODO: we dont need this implot already has this struct called ImPlotItemFlags
    // and that exists in the ImPlotSpec struct
    // Struct to hold runtime plot flags
    struct plot_flags_begin {
        bool NoTitle = false;
        bool NoLegend = false;
        bool NoMouseText = false;
        bool NoInputs = false;
        bool NoMenus = false;
        bool NoBoxSelect = false;
        bool NoFrame = false;
        bool Equal = false;
        bool Crosshairs = true;      // default on like before
    } plot_flags_begin;


    // ====================== FULL RUNTIME PLOT STYLE STRUCT ======================
    struct plot_style_vars {
        float  PlotBorderSize = 0.0f;
        float  MinorAlpha = 0.25f;
        float  DigitalPadding = 0.0f;
        float  DigitalSpacing = 4.0f;

        ImVec2 PlotPadding = ImVec2(0.0f, 0.0f);
        ImVec2 LabelPadding = ImVec2(5.0f, 0.0f); //This one was wasting vertical space
        ImVec2 LegendPadding = ImVec2(10.0f, 10.0f);
        ImVec2 LegendInnerPadding = ImVec2(5.0f, 5.0f);
        ImVec2 LegendSpacing = ImVec2(5.0f, 0.0f);
        ImVec2 MajorTickLen = ImVec2(10.0f, 10.0f);
        ImVec2 MinorTickLen = ImVec2(5.0f, 5.0f);
        ImVec2 MajorGridSize = ImVec2(1.0f, 1.0f);
        ImVec2 MinorGridSize = ImVec2(1.0f, 1.0f);
        ImVec2 MousePosPadding = ImVec2(5.0f, 5.0f);
        ImVec2 AnnotationPadding = ImVec2(4.0f, 4.0f);
        ImVec2 FitPadding = ImVec2(0.1f, 0.1f);
    } plotStyle;




    void applyStyle();
    void verticalSeparator();
};

#endif // DATAPLOTTER_H
