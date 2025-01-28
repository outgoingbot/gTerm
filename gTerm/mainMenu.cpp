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

    ImGui::EndMainMenuBar(); // End the top menu bar


    
}
