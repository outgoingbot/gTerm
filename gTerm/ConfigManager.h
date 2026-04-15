#pragma once
#include <imgui.h>
#include <string>
#include "external/json.hpp"
#include <iostream> //for cout debuggins

//TODO: Going to need to add saved variables here
struct AppConfig {
    std::string comm_port = R"(\\.\COM11)";
    std::string comm_baud = "115200";
    std::string ser_buff_num_chars = "alot";
    // add your variables here
};

//This list must be kept updated with the AppConfig struct for some dumb reason
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AppConfig, comm_port, comm_baud, ser_buff_num_chars)

class ConfigManager {
public:
    ConfigManager();

    void ShowSaveDiag();
    void ShowOpenDiag();
    void HandleDialogs();

    AppConfig& GetConfig() { return config; }
    const AppConfig& GetConfig() const { return config; }
    
    void CopyConfigToVars();

    //Debugging function to print json key/values to terminal
    void PrintConfigDebug() const;

    //used to trigger json save/load after copy operations
    bool config_need_update = false;
    bool app_config_need_update = false;

private:
    AppConfig config;
    std::string lastPath = ".";

    std::string OpenNativeSaveDialog(const std::string& defaultName = "gTerm_config.json");
    std::string OpenNativeLoadDialog();
    void SaveToFile(const std::string& filepath);
    bool LoadFromFile(const std::string& filepath);
};

