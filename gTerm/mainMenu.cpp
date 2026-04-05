#include "mainMenu.h"

mainMenu::mainMenu() {
    // Constructor (if you need to initialize anything, do it here)
}

void mainMenu::update() {

    ImGui::BeginMainMenuBar(); // Start the top menu bar

    if (ImGui::BeginMenu("File")) { // Add a "File" menu
        if (ImGui::MenuItem("New")) {
            // Handle "New" action
        }
        if (ImGui::MenuItem("Open")) {
            // Handle "Open" action
        }
        if (ImGui::MenuItem("Save")) {
            // Handle "Save" action
        }
        ImGui::Separator(); // Add a separator between menu items
        if (ImGui::MenuItem("Exit")) {
            // Handle "Exit" action
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


    if (ImGui::BeginMenu("Settings"))
    {
        if (ImGui::MenuItem("cap frame rate = true")) {
            frame_rate_capped = true;
        }
        if (ImGui::MenuItem("cap frame rate = false")) {
            frame_rate_capped = false;
        }

        ImGui::Separator();

        ImGui::SliderFloat("Font Size", &currentFontSize, 8.0f, 60.0f, "%.0f px");
        if (ImGui::IsItemDeactivatedAfterEdit())        // alternative (more precise)
        {
            fontNeedsRebuild = true;
        }

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar(); // End the top menu bar


    
}
