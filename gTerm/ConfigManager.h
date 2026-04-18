#pragma once
#include <imgui.h>
#include <string>
#include <iostream> //for cout debuggins
#include "external/json.hpp"
#include "logger.h"


//struct used in json file
struct AppConfig {
    std::string comm_port = R"(\\.\COM11)";
    std::string comm_baud = "115200";
    std::string d_parser_format = R"(%f,%f,%f)";
    std::vector<char> p_channelSelected;
    // add variables here
};

//This list must be kept updated with the AppConfig struct for some dumb reason
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AppConfig, comm_port, comm_baud, d_parser_format, p_channelSelected)

class ConfigManager {
public:
    ConfigManager();

    void update();

    void ShowSaveDiag();
    void ShowOpenDiag();

    AppConfig& GetConfig() { return config; }
    const AppConfig& GetConfig() const { return config; }
    
    //used to trigger json save/load after copy operations
    bool config_need_update = false;
    bool app_config_need_update = false;

    bool open_diag = false;
    bool open_diag_config_flag = true;

private:
    AppConfig config;
    std::string lastPath = ".";

    //copy the loaded json to our class members
    void CopyConfigToVars();

    //Debugging function to print json key/values to terminal
    void PrintConfigDebug() const;

    std::string OpenNativeSaveDialog(const std::string& defaultName = "gTerm_config.json");
    std::string OpenNativeLoadDialog();
    void SaveToFile(const std::string& filepath);
    bool LoadFromFile(const std::string& filepath);
};

