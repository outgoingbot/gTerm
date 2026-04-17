#include "DebugMenu.h"

DebugMenu::DebugMenu(const VersionInfo& versions) : m_versions(versions) {
//    debugserialManObj = nullptr;
}


void DebugMenu::update(terminal& terminalObj){
    // Start the debug menu window
    //ImGui::Begin("Debug Menu");
    ImGui::Begin("Debug Menu", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove);

    ImGui::Text("ImGui Version: %s", m_versions.imgui.c_str());
    ImGui::Text("ImPlot Version: %s", m_versions.implot.c_str());
    ImGui::Text("GLFW Version: %s", m_versions.glfw.c_str());
    ImGui::Text("OpenGL Version: %s", m_versions.opengl.c_str());
    ImGui::Text("GLSL Version: %s", m_versions.glsl.c_str());
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

    //copy the used serial manager address
    //debugserialManObj = sermanobj;
    ImGui::Separator();
    // Frame Rate debug plot ================================= START
    ImGui::BeginChild("Debug Frame Rate", ImVec2(0, 150));
    static float fpsHistory[1000] = { 0 };

    // Insert new value at the end and shift everything left (same as kernel buffer)
    float currentFPS = ImGui::GetIO().Framerate;
    memmove(&fpsHistory[0], &fpsHistory[1], sizeof(float) * 999);
    fpsHistory[999] = currentFPS;

    ImVec2 fill_size_fps = ImVec2(-1, -1);
    if (ImPlot::BeginPlot("Frame Rate History", fill_size_fps)) {
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, 1000, ImGuiCond_Always);
        ImPlot::SetupAxis(ImAxis_X1, "Samples");
        ImPlot::SetupAxis(ImAxis_Y1, "FPS", ImPlotAxisFlags_AutoFit);
        //ImPlot::SetNextLineStyle(ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        // New ImPlot 1.0 way
        ImPlotSpec spec;
        spec.LineColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
        spec.LineWeight = 1.0f;
        ImPlot::PlotLine("FPS", fpsHistory, 1000, 1.0, 0.0, spec);
        ImPlot::EndPlot();
    }
    ImGui::EndChild();
    // Frame Rate debug plot ================================= END

    ImGui::Separator();
    
    //Buffer size debug plot ================================= START
    ImGui::BeginChild("Debug Kernel Buffer");

    static float history[1000] = { 0 };
    size_t len = 0;
    terminalObj.debug_getKernelcharCount(&len);
    

    // Shift and insert data
    memmove(&history[0], &history[1], sizeof(float) * 999);
    history[999] = (float)len;

    // This makes the plot resize dynamically when you drag the window edges
    //ImVec2 fill_size_buf = ImVec2(-1, -1);

    if (ImPlot::BeginPlot("Kernel Buffer chars copied per thread loop", ImVec2(-1, 200))) {
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, 1000, ImGuiCond_Always);

        ImPlot::SetupAxis(ImAxis_X1, "Samples");
        // Auto-scale Y based on the 1000 points currently in history
        ImPlot::SetupAxis(ImAxis_Y1, "Bytes", ImPlotAxisFlags_AutoFit);
        // New ImPlot 1.0 way
        ImPlotSpec spec2;
        spec2.LineColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        spec2.LineWeight = 1.0f;
        ImPlot::PlotLine("Bytes", history, 1000, 1.0, 0.0, spec2);
        ImPlot::EndPlot();
    }

    //text with the fps
    char fpsBuf[32];
    snprintf(fpsBuf, sizeof(fpsBuf), "%.1f", (float)len);
    ImGui::Text("Kernel Buffer copy size: %s", fpsBuf);

    ImGui::EndChild();
    //Buffer size debug plot ================================= END

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