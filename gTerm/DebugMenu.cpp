#include "DebugMenu.h"

DebugMenu::DebugMenu(const VersionInfo& versions) : m_versions(versions) {
//    debugserialManObj = nullptr;
}

/*
void DebugMenu::update(terminal& terminalObj){
    // Start the debug menu window
    ImGui::Begin("Debug Menu", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove);
    ImGui::PushFont(nullptr, ImGui::GetFontSize() / 1.5);
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

    ImGui::Separator();
    // Frame Rate debug plot ================================= START
    ImGui::BeginChild("Debug Frame Rate", ImVec2(0, 100));
    static float fpsHistory[1000] = { 0 };

    // Insert new value at the end and shift everything left (same as kernel buffer)
    float currentFPS = ImGui::GetIO().Framerate;
    memmove(&fpsHistory[0], &fpsHistory[1], sizeof(float) * 999);
    fpsHistory[999] = currentFPS;

    //ImVec2 fill_size_fps = ImVec2(-1, -1);
    if (ImPlot::BeginPlot("Frame Rate History", ImVec2(-1, 100))) {
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, 1000, ImGuiCond_Always);
        //ImPlot::SetupAxis(ImAxis_X1, "Samples");
        ImPlot::SetupAxis(ImAxis_Y1, "FPS", ImPlotAxisFlags_AutoFit);
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
    ImGui::BeginChild("Debug Kernel Buffer", ImVec2(0, 100));
    static float history[1000] = { 0 };
    size_t len = 0;
    terminalObj.debug_getKernelcharCount(&len);
    // Shift and insert data
    memmove(&history[0], &history[1], sizeof(float) * 999);
    history[999] = (float)len;

    if (ImPlot::BeginPlot("Kernel Buffer chars copied per thread loop", ImVec2(-1, 100))) {
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, 1000, ImGuiCond_Always);

        //ImPlot::SetupAxis(ImAxis_X1, "Samples");
        // Auto-scale Y based on the 1000 points currently in history
        ImPlot::SetupAxis(ImAxis_Y1, "Bytes", ImPlotAxisFlags_AutoFit);
        // New ImPlot 1.0 way
        ImPlotSpec spec2;
        spec2.LineColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        spec2.LineWeight = 1.0f;
        ImPlot::PlotLine("Bytes", history, 1000, 1.0, 0.0, spec2);
        ImPlot::EndPlot();
    }
    ImGui::EndChild();
    //Buffer size debug plot ================================= END
        //text with the fps
    char fpsBuf[32];
    snprintf(fpsBuf, sizeof(fpsBuf), "%.1f", (float)len);
    ImGui::Text("Kernel Buffer copy size: %s", fpsBuf);

    char sizeBuf[32];
    snprintf(sizeBuf, sizeof(sizeBuf), "%zu", terminalObj.getSafeRxQueue().size());
    ImGui::Text("rxQueue size: %s", sizeBuf);
    ImGui::PopFont();
    ImGui::End();
    
}
*/

void DebugMenu::update(terminal& terminalObj)
{
#define DEBUG_BAR_HEIGHT  68
#define PLOT_HEIGHT       42
    
    size_t Klen = 0;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 pos = ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - DEBUG_BAR_HEIGHT);
    ImVec2 size = ImVec2(viewport->Size.x, DEBUG_BAR_HEIGHT);

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);

    ImGui::Begin("Debug Menu", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing);

    // === Ultra tight layout ===
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 2));   // almost no padding
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 2));   // minimal gaps
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 1));   // text feels tighter

    ImGui::PushFont(nullptr, ImGui::GetFontSize() / 1.5f);

    // Info line - minimal spacing
    ImGui::Text("ImGui %s | ImPlot %s | GLFW %s | OpenGL %s | GLSL %s",
        m_versions.imgui.c_str(), m_versions.implot.c_str(),
        m_versions.glfw.c_str(), m_versions.opengl.c_str(), m_versions.glsl.c_str());

    ImGui::SameLine(0.0f, 4.0f);   // very tight
    ImGui::Text(" | %.1f FPS", ImGui::GetIO().Framerate);


    //rx Queue size
    ImGui::SameLine(0.0f, 4.0f);
    char sizeBuf[32];
    snprintf(sizeBuf, sizeof(sizeBuf), "%zu", terminalObj.getSafeRxQueue().size());
    ImGui::Text(" | rxQueue size: %s", sizeBuf);


    //kernel copy
    ImGui::SameLine(0.0f, 4.0f);
    snprintf(sizeBuf, sizeof(sizeBuf), "%.1f", (float)Klen);
    ImGui::Text(" | Kernel Buffer copy size: %s", sizeBuf);


    ImGui::SameLine(0.0f, 4.0f);
    ImVec2 mouse = ImGui::GetIO().MousePos;
    ImGui::Text(" | Mouse: (%.0f, %.0f)", mouse.x, mouse.y);

    // ====================== Tiny Plots ======================
    ImVec2 avail = ImGui::GetContentRegionAvail();
    float plotWidth = (avail.x - 8.0f) * 0.5f;

    // FPS Plot
    ImGui::BeginChild("FPSPlot", ImVec2(plotWidth, PLOT_HEIGHT), false, ImGuiWindowFlags_NoScrollbar);
    static float fpsHistory[1000] = { 0 };
    float currentFPS = ImGui::GetIO().Framerate;
    memmove(&fpsHistory[0], &fpsHistory[1], sizeof(float) * 999);
    fpsHistory[999] = currentFPS;

    if (ImPlot::BeginPlot("##FrameRate", ImVec2(-1, PLOT_HEIGHT),
        ImPlotFlags_NoTitle | ImPlotFlags_NoMenus | ImPlotFlags_NoFrame))
    {
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, 1000, ImGuiCond_Always);
        ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoLabel);
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_AutoFit);
        ImPlotSpec spec;
        spec.LineColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
        spec.LineWeight = 1.2f;
        ImPlot::PlotLine("FPS", fpsHistory, 1000, 1.0, 0.0, spec);
        ImPlot::EndPlot();
    }
    ImGui::EndChild();

    ImGui::SameLine(0.0f, 6.0f);

    // Kernel Plot
    ImGui::BeginChild("KernelPlot", ImVec2(plotWidth, PLOT_HEIGHT), false, ImGuiWindowFlags_NoScrollbar);
    static float history[1000] = { 0 };
    //size_t len = 0;
    terminalObj.debug_getKernelcharCount(&Klen);
    memmove(&history[0], &history[1], sizeof(float) * 999);
    history[999] = (float)Klen;

    if (ImPlot::BeginPlot("##KernelBuffer", ImVec2(-1, PLOT_HEIGHT),
        ImPlotFlags_NoTitle | ImPlotFlags_NoMenus | ImPlotFlags_NoFrame))
    {
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, 1000, ImGuiCond_Always);
        ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoLabel);
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_AutoFit);
        ImPlotSpec spec2;
        spec2.LineColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        spec2.LineWeight = 1.2f;
        ImPlot::PlotLine("Bytes", history, 1000, 1.0, 0.0, spec2);
        ImPlot::EndPlot();
    }
    ImGui::EndChild();

    ImGui::PopStyleVar(3);   // pop the 3 style vars we pushed
    ImGui::PopFont();
    ImGui::End();
}

