#include "mainMenu.h"

//Main Menu with initialization list for cfgManager
mainMenu::mainMenu(ConfigManager& cfgManager) : cfgManagerP(cfgManager) {
    // Constructor (if you need to initialize anything, do it here)
}



// Call this every frame (can be at the end of your ImGui section)

void mainMenu::update() {

    ImGui::BeginMainMenuBar(); // Start the top menu bar

    if (ImGui::BeginMenu("File")) { // Add a "File" menu

        if (ImGui::MenuItem("New")) {
            // Handle "New" action        
        }
        if (ImGui::MenuItem("Open")) {
            // Handle "Open" action
            cfgManagerP.ShowOpenDiag();
            //cfgManagerP.open_diag = true;
        }
        if (ImGui::MenuItem("Save")) {
            // Handle "Save" action
            cfgManagerP.app_config_need_update = true;
        }
        ImGui::Separator(); // Add a separator between menu items
        if (ImGui::MenuItem("Exit")) {
            // Handle "Exit" action
            exit_app = true;
        }
        ImGui::EndMenu(); // End the "File" menu
    }


    if (ImGui::BeginMenu("Edit")) { // Add an "Edit" menu
        if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
            // Handle "Undo" action
        }
        if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {
            // Disabled menu item (e.g., if redo isn't available)
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Cut", "Ctrl+X")) {
            // Handle "Cut" action
        }
        if (ImGui::MenuItem("Copy", "Ctrl+C")) {
            // Handle "Copy" action
        }
        if (ImGui::MenuItem("Paste", "Ctrl+V")) {
            // Handle "Paste" action
        }
        ImGui::EndMenu(); // End the "Edit" menu
    }


    if (ImGui::BeginMenu("Settings")) {

        if (ImGui::Checkbox("Enable V-Sync (caps frame rate)", &v_sync_enabled)) {
            v_sync_need_update = true;
        }

        if (ImGui::Checkbox("Low Power on Disconnect", &low_power_on_disconnect)) {
            //something else of needed
        }

        if (ImGui::Checkbox("Turbo Mode", &turbo_mode)) {
            //something else of needed
        }

        ImGui::Separator();

        if (!turbo_mode) {
            ImGui::SliderFloat("Min FPS (experimental)", &frame_rate_slider_val, frame_rate_min_val, frame_rate_max_val, "%.0f");

            if (ImGui::IsItemDeactivatedAfterEdit()) {
                //do something
            }
        }

        if (ImGui::Checkbox("Show Ball", &show_ball)) {
            //something else of needed
        }

        if (ImGui::Checkbox("Show Debug", &show_debug)) {
            //something else of needed
        }

        ImGui::Separator();

        ImGui::SliderFloat("Font Size", &currentFontSize, 8.0f, 60.0f, "%.0f px");
        if (ImGui::IsItemDeactivatedAfterEdit())        // alternative (more precise)
        {
            fontNeedsRebuild = true;
        }

        if (show_debug) {
            ImGui::Separator();
            ImGui::SliderFloat("red", &globalColors.red, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("green", &globalColors.green, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("blue", &globalColors.blue, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("alpha", &globalColors.alpha, 0.0f, 1.0f, "%.2f");
        }
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar(); // End the top menu bar

}
