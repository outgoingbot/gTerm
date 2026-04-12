#include "dataParser.h"
#include <sstream>
#include <cctype>
#include <algorithm>

dataParser::dataParser() {
    compile();  // compile default format

    memset(input_text, '\0', sizeof(char)); //this makes no sense. memset one char? sizeof(char) * LEN_BUFF
    _window_params.min_height = 100.0f;
    _window_params.width; // Window width
    _window_params.height = 600.f; // Window height

    _window_params.drag_bar_height = 20.f;
    _autoScroll = true;
}


dataParser::~dataParser(void) {

}

void dataParser::compile() {
    specifiers = parse_specifiers(format);
}

size_t dataParser::getChannelCount() const {
    return specifiers.size();
}

void dataParser::parse(const std::deque<char>& deque, std::vector<ParsedSample>& outSamples) const
{
    outSamples.clear();

    if (deque.empty() || specifiers.empty()) return;

    std::string data(deque.begin(), deque.end());
    std::vector<ParsedSample> samples;

    size_t pos = 0;
    while (pos < data.size()) {
        size_t nl_pos = data.find(eol, pos);
        if (nl_pos == std::string::npos) break;

        std::string line = data.substr(pos, nl_pos - pos);
        pos = nl_pos + 1;

        if (line.empty()) continue;

        auto tokens = split_line(line);
        if (tokens.empty()) continue;

        ParsedSample sample;
        sample.values.reserve(std::min(tokens.size(), specifiers.size()));

        for (size_t i = 0; i < tokens.size() && i < specifiers.size(); ++i) {
            auto val = parse_token(tokens[i], specifiers[i]);
            sample.values.push_back(val.has_value() ? val.value() : 0.0);
        }

        if (!sample.values.empty()) {
            samples.push_back(std::move(sample));
        }
    }

    outSamples = std::move(samples);
}

std::vector<dataParser::FormatSpecifier> dataParser::parse_specifiers(const std::string& fmt) const {
    std::vector<FormatSpecifier> specs;
    size_t i = 0;
    while (i < fmt.length()) {
        if (fmt[i] == '%') {
            ++i;
            if (i < fmt.length() && fmt[i] == '%') {  // literal %%
                ++i;
                continue;
            }

            // Skip flags, width, precision, length modifiers (simple but effective for serial data)
            while (i < fmt.length() &&
                (std::isdigit(static_cast<unsigned char>(fmt[i])) ||
                    fmt[i] == '.' || fmt[i] == '-' || fmt[i] == '+' || fmt[i] == '*' || fmt[i] == 'l' || fmt[i] == 'L' || fmt[i] == 'h')) {
                ++i;
            }
            if (i >= fmt.length()) break;

            char c = std::tolower(static_cast<unsigned char>(fmt[i]));
            FormatSpecifier spec;
            if (c == 'f' || c == 'e' || c == 'g' || c == 'a') {
                spec.type = FormatSpecifier::Type::Float;
            }
            else if (c == 'd' || c == 'i') {
                spec.type = FormatSpecifier::Type::SignedInt;
            }
            else if (c == 'u') {
                spec.type = FormatSpecifier::Type::UnsignedInt;
            }
            else if (c == 'x' || c == 'X') {
                spec.type = FormatSpecifier::Type::Hex;
            }
            else {
                spec.type = FormatSpecifier::Type::Invalid;
            }
            specs.push_back(spec);
            ++i;
        }
        else {
            ++i;
        }
    }
    return specs;
}

std::vector<std::string> dataParser::split_line(const std::string& line) const {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        // trim whitespace
        size_t start = token.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            tokens.emplace_back("");
            continue;
        }
        size_t end = token.find_last_not_of(" \t\r\n");
        tokens.push_back(token.substr(start, end - start + 1));
    }
    return tokens;
}

std::optional<double> dataParser::parse_token(const std::string& token, const FormatSpecifier& spec) const {
    if (token.empty()) return std::nullopt;

    try {
        if (spec.type == FormatSpecifier::Type::Float) {
            return std::stod(token);
        }
        else if (spec.type == FormatSpecifier::Type::SignedInt) {
            return static_cast<double>(std::stoll(token));
        }
        else if (spec.type == FormatSpecifier::Type::UnsignedInt) {
            return static_cast<double>(std::stoull(token));
        }
        else if (spec.type == FormatSpecifier::Type::Hex) {
            return static_cast<double>(std::stoull(token, nullptr, 16));
        }
    }
    catch (...) {
        // Failed to parse -> return 0.0 or you can change to NAN
    }
    return std::nullopt;
}

int dataParser::update()
{
    ImGui::Begin("Data_Manipulation_Region");

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));

    ImVec2 region = ImGui::GetContentRegionAvail();
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + region.x);

    // ==================== Parser Controls ====================
    ImGui::Checkbox("Parser Enabled", &dataParse_enable);
    ImGui::Checkbox("Plots Enabled", &send_to_plot);

    // ====================== Format String Input ======================
    static char formatBuf[256] = { 0 };

    // Sync buffer with current format (only when they differ)
    if (strcmp(formatBuf, format.c_str()) != 0) {
        strncpy(formatBuf, format.c_str(), sizeof(formatBuf) - 1);
        formatBuf[sizeof(formatBuf) - 1] = '\0';
    }

    if (dataParse_enable) {
        ImGui::TextUnformatted("Format string (e.g. %f,%i,%f,%d)");
        ImGui::PushItemWidth(200.0f);
        ImGui::InputText("Input String", formatBuf, sizeof(formatBuf));
        
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            std::string newFormat = formatBuf;
            if (newFormat != format) {
                format = std::move(newFormat);
                compile();
                printf("Format updated to: '%s'  (%zu channels)\n",
                    format.c_str(), getChannelCount());
            }
        }

        //ImGui::SameLine();
        //ImGui::NewLine();
        ImGui::Text("Channels: %zu", getChannelCount());

        // Optional: Show current active format below
        ImGui::Text("Active format: %s", format.c_str());
    }
    // ==================== Your existing placeholder / debug text ====================
    //std::string tempString = dataParse_enable ? "True" : "False";
    //ImGui::TextUnformatted(tempString.c_str(), tempString.c_str() + tempString.size());

    ImGui::PopTextWrapPos();
    ImGui::PopStyleColor(1);

    
    
    //static int selectedChannel = 0;   // this will hold the value from the first dropdown

    //// Populate first dropdown with channels 0 to getChannelCount()-1
    //// (example using ImGui - adjust for your UI framework)
    //if (ImGui::BeginCombo("Channel", ("Channel " + std::to_string(selectedChannel)).c_str()))
    //{
    //    for (size_t ch = 0; ch < getChannelCount(); ++ch)
    //    {
    //        bool isSelected = (selectedChannel == (int)ch);
    //        if (ImGui::Selectable(("Channel " + std::to_string(ch)).c_str(), isSelected))
    //            selectedChannel = (int)ch;
    //        if (isSelected)
    //            ImGui::SetItemDefaultFocus();
    //    }
    //    ImGui::EndCombo();
    //}



    
    static std::vector<char> channelSelected; 
    std::vector<bool> checked;
#if 0
    for (size_t plot_idx = 0; plot_idx < getChannelCount(); ++plot_idx){
        // Initialize once
        if (channelSelected.size() != getChannelCount())
            channelSelected.resize(getChannelCount(), 0);

        // Multi-select Combo with checkboxes
        if (ImGui::BeginCombo(("Select Channels " + std::to_string(plot_idx)).c_str(), "Select Channels")){
            for (size_t chn_idx = 0; chn_idx < getChannelCount(); ++chn_idx){
                bool checked = channelSelected[chn_idx] != 0;
                if (ImGui::Checkbox(("Channel " + std::to_string(chn_idx)).c_str(), &checked))
                    channelSelected[chn_idx] = checked ? 1 : 0;
                    if(channelSelected[chn_idx]) setChannelToPlot(plot_idx, channelSelected[chn_idx]);
            }
            ImGui::EndCombo();
        }

#else
    // ====================== Per-Plot Channel Selection (Add + Remove) ======================
    size_t numChannels = getChannelCount();
    if (numChannels == 0) return 0;

    size_t totalSize = numChannels * numChannels;
    if (channelSelected.size() != totalSize) {
        channelSelected.resize(totalSize, 0);
    }

    for (size_t plot_idx = 0; plot_idx < numChannels; ++plot_idx) {
        std::string comboLabel = "Select Channels for Plot " + std::to_string(plot_idx + 1) + "##Plot" + std::to_string(plot_idx);
        std::string preview = "Plot " + std::to_string(plot_idx + 1) + " channels";

        if (ImGui::BeginCombo(comboLabel.c_str(), preview.c_str())) {

            for (size_t chn_idx = 0; chn_idx < numChannels; ++chn_idx) {
                size_t idx = plot_idx * numChannels + chn_idx;

                bool isChecked = (channelSelected[idx] != 0);

                if (ImGui::Checkbox(("Channel " + std::to_string(chn_idx + 1)).c_str(), &isChecked)) {
                    channelSelected[idx] = isChecked ? 1 : 0;

                    if (isChecked) {
                        // Add channel to this plot
                        setChannelToPlot(static_cast<int>(chn_idx), static_cast<int>(plot_idx));
                    }
                    else {
                        // REMOVE channel from this plot
                        removeChannelFromPlot(static_cast<int>(chn_idx), static_cast<int>(plot_idx));
                    }
                }
            }

            ImGui::EndCombo();
        }
    }
#endif




        // Later, evaluate which ones are checked:
        //for (size_t i = 0; i < getChannelCount(); ++i)
        //{
        //    if (channelSelected[i]) {
        //        //setChannelToPlot((int)i, yourPlotIndex);
        //    }
        //}
    //}

    // Example usage anywhere in your code
    //setChannelToPlot(0, 0);   // Channel 1 -> Plot 1
    //setChannelToPlot(0, 1);   // Channel 2 -> Plot 1
    //setChannelToPlot(1, 1);   // Channel 2 -> Plot 2
    //setChannelToPlot(2, 2);   // Channel 3 -> Plot 3


    ImGui::End();

    return 0;
}


void dataParser::setChannelToPlot(int channelIndex, int plotIndex)
{
    if (channelIndex < 0) return;

    // Resize if needed
    if (static_cast<size_t>(channelIndex) >= channelToPlotMap.size()) {
        channelToPlotMap.resize(channelIndex + 1);
    }

    // Get reference to the list of plots for this channel
    auto& plots = channelToPlotMap[channelIndex];

    // Add plot only if not already present (allows same channel on multiple plots)
    if (std::find(plots.begin(), plots.end(), plotIndex) == plots.end()) {
        plots.push_back(plotIndex);
    }
}



int dataParser::getPlotCount() const
{
    if (channelToPlotMap.empty()) {
        return 1;
    }

    int maxPlot = -1;   // start from -1 so we can detect if anything was assigned

    for (const auto& plots : channelToPlotMap) {
        for (int p : plots) {
            if (p > maxPlot) {
                maxPlot = p;
            }
        }
    }

    return (maxPlot < 0) ? 1 : maxPlot + 1;
}


const std::vector<std::vector<int>>& dataParser::getChannelToPlotMap() const
{
    return channelToPlotMap;
}

void dataParser::removeChannelFromPlot(int channelIndex, int plotIndex)
{
    if (channelIndex < 0 || static_cast<size_t>(channelIndex) >= channelToPlotMap.size()) {
        return;
    }

    auto& plots = channelToPlotMap[channelIndex];

    // Remove the plot from the list if present
    auto it = std::find(plots.begin(), plots.end(), plotIndex);
    if (it != plots.end()) {
        plots.erase(it);
    }
}



//this seriously looks like some ai bullshit. drugs are bad
//std::deque<float> dataParser::ParseFloatArrayFromAscii(const std::deque<char>& asciiBuffer)
//{
//    std::deque<float> result;
//    std::string token;
//    std::stringstream ss;
//
//    // Build a full string from the deque
//    for (char ch : asciiBuffer)
//        ss << ch;
//
//    // Parse comma-separated values
//    while (std::getline(ss, token, ','))
//    {
//        try {
//            // Convert to float and store
//            result.push_back(std::stof(token));
//        }
//        catch (const std::invalid_argument& e) {
//            // Ignore invalid tokens (non-numeric)
//            continue;
//        }
//        catch (const std::out_of_range& e) {
//            // Ignore values that don't fit in float
//            continue;
//        }
//    }
//
//    return result;
//}

