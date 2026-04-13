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
    // Prefer zenity, fallback to kdialog
    std::string cmd = "zenity --file-selection --save --confirm-overwrite "
        "--filename=\"" + lastPath + "/" + defaultName + "\" "
        "--file-filter=\"JSON files | *.json\" 2>/dev/null || "
        "kdialog --getsavefilename \"" + lastPath + "/" + defaultName + "\" "
        "\"*.json\" 2>/dev/null || echo ''";

    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        std::stringstream ss;
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), pipe)) ss << buffer;
        pclose(pipe);
        result = ss.str();
        if (!result.empty() && result.back() == '\n') result.pop_back();
    }
    if (!result.empty()) {
        lastPath = std::filesystem::path(result).parent_path().string();
    }
    return result;
}

std::string ConfigManager::OpenNativeLoadDialog() {
    std::string cmd = "zenity --file-selection "
        "--filename=\"" + lastPath + "\" "
        "--file-filter=\"JSON files | *.json\" 2>/dev/null || "
        "kdialog --getopenfilename \"" + lastPath + "\" "
        "\"*.json\" 2>/dev/null || echo ''";

    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        std::stringstream ss;
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), pipe)) ss << buffer;
        pclose(pipe);
        result = ss.str();
        if (!result.empty() && result.back() == '\n') result.pop_back();
    }
    if (!result.empty()) {
        lastPath = std::filesystem::path(result).parent_path().string();
    }
    return result;
}

void ConfigManager::ShowSaveButton() {
    std::string path = OpenNativeSaveDialog("gTerm_config.json");
    if (!path.empty()) {
        SaveToFile(path);
    }
}

void ConfigManager::ShowLoadButton() {
    std::string path = OpenNativeLoadDialog();
    if (!path.empty()) {
        bool success = LoadFromFile(path);
        if (success) {
            PrintConfigDebug();
            CopyConfigToVars();
        }
    }
}

void ConfigManager::HandleDialogs() {
    // no-op for native blocking dialogs
}

void ConfigManager::CopyConfigToVars() {
    nlohmann::json j = GetConfig();
    // Add your mappings here when needed
    // Example:
    // if (j.contains("_autoscroll")) {
    //     dataPlotter.controls.autoscroll = j["_autoscroll"].get<bool>();
    // }
}

void ConfigManager::PrintConfigDebug() const {
    nlohmann::json j = config;
    std::cout << "json loaded" << std::endl;
    std::cout << "=== ConfigManager Debug ===" << std::endl;
    for (auto& [key, value] : j.items()) {
        std::cout << key << " = " << value << std::endl;
    }
    std::cout << "===========================" << std::endl;
}