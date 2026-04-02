//gTerm entry point
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "gTerm.h"

//TODO: update imgui to 1.92.6 (im on 1.92.5 this has inputTextMultiLine() as wrapping and selectable)

using namespace std;

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define SCALE_FACTOR 1.0

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }

    // Configure OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
    glfwSwapInterval(1); // Enable VSync

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
    
    (void)io;

    //========================================= FONT TESTING ==============================================
    //resize the default font
    //ImFontConfig config;
    //config.SizePixels = 24.0f; // make ProggyClean bigger
    //io.Fonts->AddFontDefault(&config);


    //Set the Glass_TTY_VT220 Font
    /*
    io.Fonts->Clear(); // Clear existing fonts
    const char* fontPath = FONT_FILE_PATH; // Defined by CMake
    io.Fonts->AddFontFromFileTTF(fontPath, 24.0f);
    */

    //ImGui_ImplOpenGL3_CreateFontsTexture(); // Docs say this should be called but it crashes the app

    // Keep ImGui default font (automatically added by AddFontDefault)
    ImFont* fontDefault = io.Fonts->AddFontDefault();

    // Add your custom font
    const char* fontPath = FONT_FILE_PATH;  // e.g. "assets/Glass_TTY_VT220.ttf"
    ImFont* fontVT220 = io.Fonts->AddFontFromFileTTF(fontPath, 24.0f);

    // (Optional) Add more sizes or variants
    ImFont* fontVT220_Large = io.Fonts->AddFontFromFileTTF(fontPath, 32.0f);
    ImFont* fontVT220_Small = io.Fonts->AddFontFromFileTTF(fontPath, 16.0f);

    // Build the font atlas
    //io.Fonts->Build(); <-- causing crash on start
    //========================================= FONT TESTING ==============================================


    ImGui::StyleColorsDark();

    // Set up platform/renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    //glEnable(GL_MULTISAMPLE); //this doing anything? supposed to enable the 4x MSAA anti-aliasing set above

    //Create Custom GUI Object
    mainMenu main_menu;
    DebugMenu debugMenu;
    terminal term(WINDOW_WIDTH, WINDOW_HEIGHT); //I dont think these size params are doing anything
    dataParser dParser;
    dataPlotter dPlotter;
    
    //Scale the entire glfw window with this function
    glfwSetWindowSize(window, WINDOW_WIDTH * 1.5, WINDOW_HEIGHT * 1.5);

 
    // Main loop
    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents(); //I think this handles the user inputs

        // Start a new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        main_menu.update(); //gTerm Top Bar Menu Items (File, Edit, etc..)
        //ImGui::SetNextWindowPos(ImVec2(10, 50), ImGuiCond_FirstUseEver); // initial position only once
        //ImGui::SetNextWindowSize(ImVec2(1200, 800), ImGuiCond_FirstUseEver); // optional size
        term.update("Terminal"); //going to be the "main" terminal (realTerm like)

        //ImGui::SetNextWindowPos(ImVec2(1100, 100), ImGuiCond_FirstUseEver); // initial position only once
        //ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver); // optional size
        
        //Going to need smaller 'term' objects that have graphs, logging, settings, DSP options, etc (bulk of code)
        
        //Parser Window
        dParser.update();
        
        //Plotter Window
        dPlotter.update(dParser, term.getRxBuffer());

        //Debug window
        ImVec2 windowSize(400, 250);
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