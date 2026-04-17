#include "dataParser.h"

dataParser::dataParser(AppConfig& cfg) : configRef(cfg) {
    compile();  // compile default format

    //memset(input_text, '\0', sizeof(char)); //this makes no sense. memset one char? sizeof(char) * LEN_BUFF
    _window_params.min_height = 100.0f;
    _window_params.width; // Window width
    _window_params.height = 600.f; // Window height

    _window_params.drag_bar_height = 20.f;
    //_autoScroll = true;
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

    //TODO: If parser is enabled and the serial data is junk it kills the FPS.
    //Something in parser is trying really hard to find numerical data in fuzzer text
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
 

    // ====================== Per-Plot Channel Selection (Add + Remove) ======================
    //static std::vector<char> channelSelected;
    //std::vector<bool> checked;
    size_t numChannels = getChannelCount();
    if (numChannels == 0) return 0;

    size_t totalSize = numChannels * numChannels;
    if (channelSelected.size() != totalSize) {
        channelSelected.resize(totalSize, 0);
    }

    for (size_t plot_idx = 0; plot_idx < numChannels; ++plot_idx) {
        std::string comboLabel = "##Select Channels for Plot " + std::to_string(plot_idx + 1) + "##Plot" + std::to_string(plot_idx);
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

void dataParser::ApplyConfig()
{
    format = configRef.d_parser_format;
    compile();
    channelSelected = configRef.p_channelSelected; //this may copy wrong. might be vect<int> -> vect<char>.
    
    //TODO: shitty copy paste from combo box
    //needs to be a method. prob crash if there are wrong channels saved like this
    size_t numChannels = getChannelCount();
    for (size_t plot_idx = 0; plot_idx < numChannels; ++plot_idx) {
        for (size_t chn_idx = 0; chn_idx < numChannels; ++chn_idx) {
            size_t idx = plot_idx * numChannels + chn_idx;

            bool isChecked = (channelSelected[idx] != 0);

            if (isChecked) {
                // Add channel to this plot
                setChannelToPlot(static_cast<int>(chn_idx), static_cast<int>(plot_idx));
            }else {
                // REMOVE channel from this plot
                removeChannelFromPlot(static_cast<int>(chn_idx), static_cast<int>(plot_idx));
            }
        }
    }//fix this shit

    // add every variable you need copied
}


void dataParser::StoreConfig()
{
    configRef.d_parser_format = format;
    configRef.p_channelSelected = channelSelected;
    // add every variable you need copied
}