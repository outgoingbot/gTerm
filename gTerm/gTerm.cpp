// gTerm.cpp : Defines the entry point for the application.
//

#include "gTerm.h"

using namespace std;

#define SCROLL_BACK 1000
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

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
    terminal term(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    terminal term_B(window, WINDOW_WIDTH, WINDOW_HEIGHT);

    DebugMenu debugMenu;
    //create random junk char array
    

    // Main loop
    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents(); //I think this handles the user inputs

        // Start a new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //Update Our Custom GUI elements
            // Simulate adding new lines to the console
        static int counter = 0;
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "This is line %d", counter++);
        term.term_out.addLine(buffer);
        if (counter > SCROLL_BACK) {
            term.term_out.rmLine();
        }

        
        main_menu.update(); //gTerm Top Bar Menu Items (File, Edit, etc..)
        debugMenu.update();
        term.update("One");
        term_B.update("Two");
        
        

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
