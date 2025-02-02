#include "DebugMenu.h"

DebugMenu::DebugMenu() {
    // Constructor (if you need to initialize anything, do it here)
}

void DebugMenu::update(){
    // Start the debug menu window
    ImGui::Begin("Debug Menu");


    // Display the frame rate
    ImGui::Text("Frame Rate: %.1f FPS", ImGui::GetIO().Framerate);

    // Get mouse position
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    ImGui::Text("Mouse Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
    

    //// Load texture
    //const char* logoPath = LOGO_FILE_PATH; // Defined by CMake
    //GLuint myTexture = loadTexture(logoPath);
    ////ImGui::Image(myTexture, ImVec2(256, 256)); // Display the texture
    //ImGui::Image((ImTextureID)(intptr_t)myTexture, ImVec2(256, 256)); // Display the texture

    ImGui::End();
}


//GLuint DebugMenu::loadTexture(const char* filename) {
//    int width, height, channels;
//    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4); // Force RGBA
//
//    if (!data) {
//        std::cerr << "Failed to load image: " << stbi_failure_reason() << std::endl;
//        return 0;
//    }
//
//    GLuint texture;
//    glGenTextures(1, &texture);
//    glBindTexture(GL_TEXTURE_2D, texture);
//
//    // Upload texture to GPU
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//    glGenerateMipmap(GL_TEXTURE_2D);
//
//    // Texture parameters
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    stbi_image_free(data);  // Free CPU-side memory
//
//    return texture;
//}