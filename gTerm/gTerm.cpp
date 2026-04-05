//gTerm entry point
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "gTerm.h"

using namespace std;

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define SCALE_FACTOR 1.7

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }

    // Configure OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA

    
    // Create a window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "gTerm", nullptr, nullptr);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    //Load stupid gTerm Icon that I wasted 3 hours getting to work.
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
        cerr << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    
 #define IGNORE_SAVED_IMU_INI 0
    #if IGNORE_SAVED_IMU_INI
    io.IniFilename = nullptr; // disables loading/saving window layout
    #endif
    
    ImGui::StyleColorsDark();

    // Set up platform/renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    glEnable(GL_MULTISAMPLE); //this doing anything? supposed to enable the 4x MSAA anti-aliasing set above
    //Scale the entire glfw window with this function
    glfwSetWindowSize(window, WINDOW_WIDTH * SCALE_FACTOR, WINDOW_HEIGHT * SCALE_FACTOR);
    //glfwSetWindowSize(window, 1200, 800);
    

    //========================================= FONT TESTING ==============================================
    ImFontConfig config;
    config.SizePixels = 18.0f; // Your starting size
    io.Fonts->AddFontDefaultVector(&config); // Explicitly load modern font
    io.Fonts->Build();
    //========================================= FONT TESTING ==============================================


    //Create Custom GUI Object
    mainMenu main_menu;
    DebugMenu debugMenu;
    terminal term(WINDOW_WIDTH, WINDOW_HEIGHT); //I dont think these size params are doing anything
    dataParser dParser;
    dataPlotter dPlotter;
     
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        if(main_menu.frame_rate_capped){
            glfwWaitEventsTimeout(0.008);        // ~8ms timeout → ~120 FPS max when idle
        }
        else {
            glfwPollEvents();                    // Stay fully responsive
        }

        // rebuild fonts  -  put this BEFORE ImGui::NewFrame()
        if (main_menu.fontNeedsRebuild) {
            ImGui::PushFont(nullptr, main_menu.currentFontSize);
            main_menu.fontNeedsRebuild = false;
        }

        // Start a new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::GetStyle().FontSizeBase = main_menu.currentFontSize;
        main_menu.update(); //gTerm Top Bar Menu Items (File, Edit, etc..)
        
        //ImGui::SetNextWindowPos(ImVec2(10, 50), ImGuiCond_FirstUseEver); // initial position only once
        //ImGui::SetNextWindowSize(ImVec2(1200, 800), ImGuiCond_FirstUseEver); // optional size
        term.update("Terminal"); //"main" terminal (realTerm like)

        //ImGui::SetNextWindowPos(ImVec2(1100, 100), ImGuiCond_FirstUseEver); // initial position only once
        //ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver); // optional size    
        //Parser Window
        dParser.update();
        
        //Plotter Window
        if (dParser.dataParse_enable && dParser.send_to_plot) {
            //ImGui::SetNextWindowPos(ImVec2(1100, 100), ImGuiCond_FirstUseEver); // initial position only once
            //ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver); // optional size
            dPlotter.update(dParser, term.getRxBuffer());
        }
        
        //Debug window
        ImVec2 windowSize(400, 500);
        ImVec2 windowPos(ImGui::GetIO().DisplaySize.x - windowSize.x, 0); // top-right
        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
        debugMenu.update(term.serialManObj); //shows fps and mouse position

        // Rendering
        ImGui::Render();
        //glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
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