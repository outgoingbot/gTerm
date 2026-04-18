#include "ConfigManager.h"
#include <fstream>
#include <filesystem>
#include "external/ImGuiFileDialog/ImGuiFileDialog.h"

ConfigManager::ConfigManager() {
}

void ConfigManager::ShowSaveDiag() {
    open_diag = true;
    open_diag_config_flag = true;   // will trigger save dialog next update()
}

void ConfigManager::ShowOpenDiag() {
    open_diag = true;
    open_diag_config_flag = false;  // will trigger open dialog next update()
}

void ConfigManager::update() {
    if (!open_diag) return;

    ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_FirstUseEver);

    if (open_diag_config_flag) {
        // Save dialog
        IGFD::FileDialogConfig cfg;
        cfg.path = lastPath;
        ImGuiFileDialog::Instance()->OpenDialog("ConfigSaveDlgKey", "Save Config", ".json", cfg);
    }
    else {
        // Open dialog
        IGFD::FileDialogConfig cfg;
        cfg.path = lastPath;
        ImGuiFileDialog::Instance()->OpenDialog("ConfigOpenDlgKey", "Load Config", ".json", cfg);
    }

    // Display logic
    const char* dlgKey = open_diag_config_flag ? "ConfigSaveDlgKey" : "ConfigOpenDlgKey";

    if (ImGuiFileDialog::Instance()->Display(dlgKey)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            if (open_diag_config_flag) {
                // Save
                SaveToFile(filePathName);
            }
            else {
                // Load
                if (LoadFromFile(filePathName)) {
                    CopyConfigToVars();
                    PrintConfigDebug();
                    config_need_update = true;
                }
            }

            // remember last path
            if (!filePath.empty()) {
                lastPath = filePath;
            }
        }

        ImGuiFileDialog::Instance()->Close();
        open_diag = false;
        open_diag_config_flag = true;  // reset to default (save)
    }
}

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

void ConfigManager::CopyConfigToVars() {
    nlohmann::json j = config;

    if (j.contains("comm_port")) {
        config.comm_port = j["comm_port"].get<std::string>();
    }
    if (j.contains("comm_baud")) {
        config.comm_baud = j["comm_baud"].get<std::string>();
    }
    if (j.contains("d_parser_format")) {
        config.d_parser_format = j["d_parser_format"].get<std::string>();
    }
}

void ConfigManager::PrintConfigDebug() const {
    nlohmann::json j = config;
    LOG_INFO("json loaded");
    LOG_INFO("Printing Keys = values");
    for (auto& [key, value] : j.items()) {
        LOG_INFO(key << " = " << value);
    }
}