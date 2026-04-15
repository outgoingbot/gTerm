#include "ConfigManager.h"
#include <fstream>
#include <sstream>
#include <filesystem>

ConfigManager::ConfigManager() {}

void ConfigManager::SaveToFile(const std::string& filepath) {
    std::ofstream file(filepath);
    if (file) {
        nlohmann::json j = config;
        file << j.dump(4);
    }
}

bool ConfigManager::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file) return false;
    nlohmann::json j;
    file >> j;
    config = j.get<AppConfig>();
    return true;
}

std::string ConfigManager::OpenNativeSaveDialog(const std::string& defaultName) {
    std::string cmd = "powershell -Command \"Add-Type -AssemblyName System.Windows.Forms; $sfd = New-Object System.Windows.Forms.SaveFileDialog; $sfd.Filter = 'JSON files (*.json)|*.json|All files (*.*)|*.*'; $sfd.FileName = '" + defaultName + "'; $sfd.InitialDirectory = '" + lastPath + "'; if ($sfd.ShowDialog() -eq 'OK') { $sfd.FileName } else { '' }\"";

    std::string result;
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (pipe) {
        std::stringstream ss;
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), pipe)) ss << buffer;
        _pclose(pipe);
        result = ss.str();
        if (!result.empty() && result.back() == '\n') result.pop_back();
    }
    if (!result.empty()) {
        lastPath = std::filesystem::path(result).parent_path().string();
    }
    return result;
}

std::string ConfigManager::OpenNativeLoadDialog() {
    std::string cmd = "powershell -Command \"Add-Type -AssemblyName System.Windows.Forms; $ofd = New-Object System.Windows.Forms.OpenFileDialog; $ofd.Filter = 'JSON files (*.json)|*.json|All files (*.*)|*.*'; $ofd.InitialDirectory = '" + lastPath + "'; if ($ofd.ShowDialog() -eq 'OK') { $ofd.FileName } else { '' }\"";

    std::string result;
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (pipe) {
        std::stringstream ss;
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), pipe)) ss << buffer;
        _pclose(pipe);
        result = ss.str();
        if (!result.empty() && result.back() == '\n') result.pop_back();
    }
    if (!result.empty()) {
        lastPath = std::filesystem::path(result).parent_path().string();
    }
    return result;
}

void ConfigManager::ShowSaveDiag() {
        std::string path = OpenNativeSaveDialog("gTerm_config.json");
        if (!path.empty()) {
            SaveToFile(path);
        }
}

void ConfigManager::ShowOpenDiag() {
        std::string path = OpenNativeLoadDialog();
        if (!path.empty()) {
            bool success = LoadFromFile(path);
            if (success){
                CopyConfigToVars();
                PrintConfigDebug();
                config_need_update = true;
            }

        }
}

void ConfigManager::HandleDialogs() {
    // no-op
}


void ConfigManager::CopyConfigToVars(){
    nlohmann::json j = GetConfig();   // convert struct back to json

    // Example for your "_autoscroll" key
    if (j.contains("comm_port")) {
        //dataPlotter.controls.autoscroll = j["_autoscroll"].get<bool>();
        config.comm_port = j["comm_port"].get<std::string>();
    }

    // Other examples:
    if (j.contains("comm_baud")) {
        //dataPlotter.some_int = j["some_int"].get<int>();
        config.comm_baud = j["comm_baud"].get<std::string>();
    }

    if (j.contains("d_parser_format")) {
        config.d_parser_format = j["d_parser_format"].get<std::string>();
    }
}



void ConfigManager::PrintConfigDebug() const {
    nlohmann::json j = config;   // convert struct to json

    std::cout << "json loaded" << std::endl;
    std::cout << "=== ConfigManager Debug ===" << std::endl;
    for (auto& [key, value] : j.items()) {
        std::cout << key << " = " << value << std::endl;
    }
    std::cout << "===========================" << std::endl;
}