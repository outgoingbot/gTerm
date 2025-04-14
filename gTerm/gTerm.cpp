//gTerm entry point
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "gTerm.h"

using namespace std;

//#define SCROLL_BACK 1000
#define WINDOW_WIDTH 1400
#define WINDOW_HEIGHT 900

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
    // glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA


    
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
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr; // disables loading/saving window layout
    (void)io;

    //Set the Default Font
    io.Fonts->Clear(); // Clear existing fonts
    const char* fontPath = FONT_FILE_PATH; // Defined by CMake
    io.Fonts->AddFontFromFileTTF(fontPath, 24.0f);
    //ImGui_ImplOpenGL3_CreateFontsTexture(); // Docs say this should be called but it crashes the app

    ImGui::StyleColorsDark();

    // Set up platform/renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    //glEnable(GL_MULTISAMPLE); //this doing anything? supposed to enable the 4x MSAA anti-aliasing set above


    //Create Custom GUI Object
    mainMenu main_menu;
    terminal term(WINDOW_WIDTH, WINDOW_HEIGHT); //I dont think these size params are doing anything
    DebugMenu debugMenu;
    dataParser dataParsObj;
    
    //Scale the entire glfw window with this function
   // glfwSetWindowSize(window, WINDOW_WIDTH * 0.8, WINDOW_HEIGHT * 0.8);


    // Main loop
    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents(); //I think this handles the user inputs

        // Start a new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        main_menu.update(); //gTerm Top Bar Menu Items (File, Edit, etc..)
        ImGui::SetNextWindowPos(ImVec2(10, 50), ImGuiCond_FirstUseEver); // initial position only once
        ImGui::SetNextWindowSize(ImVec2(1200, 800), ImGuiCond_FirstUseEver); // optional size
        term.update("Terminal"); //going to be the "main" terminal (realTerm like)

        ImGui::SetNextWindowPos(ImVec2(1100, 100), ImGuiCond_FirstUseEver); // initial position only once
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver); // optional size
        dataParsObj.update();
        
        //Going to need smaller 'term' objects that have graphs, logging, settings, DSP options, etc (bulk of code)

        ImVec2 windowSize(400, 150);
        ImVec2 windowPos(ImGui::GetIO().DisplaySize.x - windowSize.x, 0); // top-right
        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
        debugMenu.update(); //shows fps and mouse position


        // Rendering
        ImGui::Render();
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);   
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
