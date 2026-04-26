//gTerm entry point
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define IMPLOT_ENABLE_SPEC 1
#include "external/stb_image.h"
#include "gTerm.h"
#include "ConfigManager.h"

//#include "external/ImGuiFileDialog/ImGuiFileDialog.h"
using namespace std;
#define IGNORE_SAVED_IMGUI_INI 0
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define SCALE_FACTOR 1.0

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        LOG_ERROR_SYS("Failed to initialize GLFW");
        return -1;
    }

    // Configure OpenGL version and profile based on OS and Hardware
#if defined(IS_PI4)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API); //Uncomment for Pi 4 build
#endif
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#if defined(IS_PI4)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif


    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA

    
    // Create a window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "gTerm", nullptr, nullptr);
    if (!window) {
        LOG_ERROR_SYS("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    // Load stupid gTerm Icon that I wasted 3 hours getting to work.
    const char* iconPath = ICON_FILE_PATH; // Defined by CMake
    GLFWimage images[1];
    images[0].pixels = stbi_load(iconPath, &images[0].width, &images[0].height, 0, 4); //rgba channels 
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);


    // Make the OpenGL context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable VSync and caps framerate to moitor refresh rate

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG_ERROR_SYS("Failed to initialize GLAD");
        return -1;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    

    #if IGNORE_SAVED_IMGUI_INI
    io.IniFilename = nullptr; // disables loading/saving window layout
    #endif
    ImGuiStyle& style= ImGui::GetStyle();
    ImGui::StyleColorsDark();
    //setgTermStyles(style);
   
    // Set up platform/renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#if defined(IS_PI4)
    ImGui_ImplOpenGL3_Init("#version 300 es"); //use this if building on pi 4
#else
    ImGui_ImplOpenGL3_Init("#version 330");
#endif


    glEnable(GL_MULTISAMPLE); //enable the 4x MSAA anti-aliasing set above
    //Scale the entire glfw window with this function
    glfwSetWindowSize(window, WINDOW_WIDTH * SCALE_FACTOR, WINDOW_HEIGHT * SCALE_FACTOR);
    
    
    //----------------------------- Collect Various Version Info ------------------------------|
    VersionInfo versions;
    versions.imgui = IMGUI_VERSION;
    versions.implot = IMPLOT_VERSION;
    versions.glfw = glfwGetVersionString();
    versions.opengl = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    versions.glsl = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    //----------------------------- Collect Various Version Info ------------------------------|


    //------------------------------------- Font Setup ----------------------------------------|
    ImFontConfig config;
    config.SizePixels = 12.0f; // Font starting size
    io.Fonts->AddFontDefaultVector(&config); // Explicitly load modern font
    io.Fonts->Build();
    //------------------------------------- Font Setup ----------------------------------------|


    //------------------------------------ Create the Objects ---------------------------------|
    // Create config manager to handle json load/save
    ConfigManager cfgManager;

    // Create Custom GUI Object
    mainMenu main_menu(cfgManager);
    DebugMenu debugMenu(versions);
    terminal term(WINDOW_WIDTH, WINDOW_HEIGHT, cfgManager.GetConfig()); //I dont think these size params are doing anything
    dataParser dParser(cfgManager.GetConfig());
    dataPlotter dPlotter(dParser);
    //------------------------------------ Create the Objects ---------------------------------| 


    // Main loop
    while (!glfwWindowShouldClose(window) && !main_menu.exit_app) {
        
        //----------------------------- Update glfw from main menu settings -----------------------|
        if(main_menu.frame_rate_capped){
            glfwWaitEventsTimeout(0.004);        // ~240 FPS max
        }else {
            glfwPollEvents();                    // Stay fully responsive
        }

        if (main_menu.v_sync_enabled) {
            glfwSwapInterval(1);   // VSync ON  (locked to monitor refresh rate)
        }else {
            glfwSwapInterval(0);   // VSync OFF (uncapped)
        }
        //----------------------------- Update glfw from main menu settings -----------------------|


        //----------------------------- rebuild fonts from main menu settings ---------------------|
        if (main_menu.fontNeedsRebuild) {
            ImGui::PushFont(nullptr, main_menu.currentFontSize);
            main_menu.fontNeedsRebuild = false; // rest flag
        }
        //----------------------------- rebuild fonts from main menu settings ---------------------|
        

        //----------------------- Open JSON and copy to class members -----------------------------|
        if (cfgManager.config_need_update) {
            term.ApplyConfig();
            dParser.ApplyConfig();
            //dPlotter.ApplyConfig();
            cfgManager.config_need_update = false;  // reset flag
        }
        //----------------------- Open JSON and copy to class members -----------------------------|


        //----------------------------- Save class members to JSON --------------------------------|
        if (cfgManager.app_config_need_update) {
            term.StoreConfig();
            dParser.StoreConfig();
            //dPlotter.StoreConfig();
            cfgManager.ShowSaveDiag();
            cfgManager.app_config_need_update = false;  // reset flag
        }
        //----------------------------- Save class members to JSON --------------------------------|


        // Start a new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::GetStyle().FontSizeBase = main_menu.currentFontSize;
        main_menu.update(); //gTerm Top Bar Menu Items (File, Edit, etc..)
        
        ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver); // initial position only once
        ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver); // optional size
        term.update("Terminal"); //"main" terminal (realTerm like)

        //clear samples when file IO and connect()
        if (term.clear_samples) {
            dPlotter.clearSamples();
            term.clear_samples = false;
        }


        ImGui::SetNextWindowPos(ImVec2(10, 450), ImGuiCond_FirstUseEver); // initial position only once
        ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver); // optional size    
        //Parser Window
        dParser.update();
        
        //Plotter Window
        if (dParser.dataParse_enable && dParser.send_to_plot) {
            ImGui::SetNextWindowPos(ImVec2(350, 30), ImGuiCond_FirstUseEver); // initial position only once
            ImGui::SetNextWindowSize(ImVec2(1500, 800), ImGuiCond_FirstUseEver); // optional size
            dPlotter.update(term.getSafeRxQueue());
        }
        
        if (main_menu.show_debug) {
            debugMenu.update(term); //shows fps and mouse position
        }
        cfgManager.update();


        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);   
    } //End While


    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}




void setgTermStyles(ImGuiStyle& style) {
    // === Main accent color (change these 3 numbers to your desired color) ===
    ImVec4 accent = ImVec4(0.26f, 0.59f, 0.58f, 1.0f);   // ← change to red, purple, green, etc.

    // Combo, Text inputs, Scrollbars, Sliders, etc.
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);

    style.Colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, 0.6f);

    // Combo box, Text input, Drag, etc.
    style.Colors[ImGuiCol_Button] = accent;
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(accent.x * 1.1f, accent.y * 1.1f, accent.z * 1.1f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(accent.x * 0.9f, accent.y * 0.9f, accent.z * 0.9f, 1.0f);

    // Tabs
    style.Colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = accent;
    style.Colors[ImGuiCol_TabActive] = accent;
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(accent.x * 0.85f, accent.y * 0.85f, accent.z * 0.85f, 1.0f);

    // Plot borders / lines / frames
    style.Colors[ImGuiCol_PlotLines] = accent;
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(accent.x * 1.2f, accent.y * 1.2f, accent.z * 1.2f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = accent;
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(accent.x * 1.2f, accent.y * 1.2f, accent.z * 1.2f, 1.0f);
    //style.Colors[ImGuiCol_PlotBorder] = ImVec4(0.40f, 0.40f, 0.40f, 0.8f);  // or use accent if you want

    // Text input fields (InputText, Combo, etc.)
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(accent.x * 0.5f, accent.y * 0.5f, accent.z * 0.5f, 0.5f);

    // Checkmark / Slider grab (to match)
    style.Colors[ImGuiCol_CheckMark] = accent;
    style.Colors[ImGuiCol_SliderGrab] = accent;
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(accent.x * 1.1f, accent.y * 1.1f, accent.z * 1.1f, 1.0f);


}