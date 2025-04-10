#include "terminal.h"

terminal::terminal(GLFWwindow* window, int width, int height) {
    _width = width;
    _height = height;
    _window = window;
    memset(buffer, '\0', sizeof(char));
    memset(input_buffer_Port, '\0', sizeof(char));
    memset(input_buffer_Baud, '\0', sizeof(char));

    //Instatiate the serialManager Object
    serialManObj = new serialManager();
}


terminal::~terminal(void) {
    //TODO: this should delte the serial Manager class and not the virtual comm class.
    delete serialManObj; // Cleanup
}



int terminal::handle_connect_button() {
    if (serialManObj->isConnected() == true) {
        std::cout << "ERROR: Already Connected to Serial Manager" << std::endl;
        return -1;
    }
    
    if (serialManObj->connect()) {
        std::cout << "SUCCESS: Serial Manager Connected!" << std::endl;
    }
    
    return 0;
}

int terminal::handle_disconnect_button() {
    if (serialManObj->isConnected() == false) {
        std::cout << "ERROR: Already Disconnected from Serial Manager" << std::endl;
        return -1;
    }
    serialManObj->stopThread();
    serialManObj->disconnect();
    return 0;
}



int terminal::update(const char* title) {
#define MAX_LINE_COUNT 1000 //Max Scroll back is 1000 lines
    static int lineCounter = 0;
    char lineBuff[1024];

    if (serialManObj->hasData()) {
        std::deque<char> receivedData = serialManObj->getData(0); //need to understand this parameter better
        // Process receivedData...
        serialManObj->copyToCharArray(lineBuff, sizeof(lineBuff) - 1);
        printf("Received Data: %s\n", lineBuff);
        term_out.addLine(lineBuff);
        lineCounter++;
    }
    
    //Limit the max number of text lines in the console window
    if (lineCounter >= MAX_LINE_COUNT) {
        lineCounter = MAX_LINE_COUNT;
        term_out.rmLine();
    }



        // Create ImGui window
        ImGui::Begin(title);

        //float scale_factor = (ImGui::GetWindowHeight() * ImGui::GetWindowWidth()) * 0.0000005f; //playing with autoscaling
        //std::cout << scale_factor << std::endl;
        //testing out scrolling text class
        //ImGui::SetWindowFontScale(scale_factor);

        term_out.update(); //scrolling Text class

        //Text Entry Test
        ImGui::SetWindowFontScale(1.0f); //looks like shit when scaled
        
        //Text Box
        ImGui::Text("Comm Port:");
        //Input Text Box
        ImGui::SameLine(); // Place the next widget on the same line
        
        ImGui::SetNextItemWidth(200);  // Set width of the input field
        if (ImGui::InputText("##TextEntry", input_buffer_Port, IM_ARRAYSIZE(input_buffer_Port))) {
            // Handle text input when the field changes
        }
        //Text Box
        ImGui::SameLine(); // Place the next widget on the same line
        ImGui::Text("Baud Rate:");

        //Input Text Box
        ImGui::SameLine(); // Place the next widget on the same line
        ImGui::SetNextItemWidth(200);  // Set width of the input field
        if (ImGui::InputText("##TextEntry2", input_buffer_Baud, IM_ARRAYSIZE(input_buffer_Baud))) {
            // Handle text input when the field changes
        }

        //-----------------------------Connect Button-----------------------------|
        ImGui::SameLine(); // Place the next widget on the same line
        //THIS SUCKS THAT I HAVE TO MANAGER ALL THE GUI ELEMENT STATES.!!!
        static bool ConnectisClicked = false;
        bool pushingColor = false;    
        
        // ImGui is a 'Immediate Mode GUI system'
        // This means I have to set the gui element state changes before its "called". 
        // If clicked, push red style
        if (ConnectisClicked) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));  // green
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));  // lighter green
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.8f, 0.0f, 1.0f));  // darker green
            pushingColor = true;
        }

        if (ImGui::Button("Connect")) {
            // This block executes when the button is clicked
            ConnectisClicked = true;

            if (this->handle_connect_button() == 0) {
                std::cout << "SUCCESS: Terminal Class Connected!" << std::endl;
                std::cout << "" << serialManObj->getCommPort() << std::endl;
                std::cout << "" << serialManObj->getCBaudRate() << std::endl;
                
            }else {
                std::cout << "ERROR: Terminal Class Not Connected!" << std::endl;
            }
        }

       if (pushingColor) ImGui::PopStyleColor(3);
       //-----------------------------Connect Button-----------------------------|
       

        //-----------------------------Disconnect Button--------------------------|
        //Disconnect Button
        ImGui::SameLine(); // Place the next widget on the same line
        if (ImGui::Button("Disconnect")) {
            // This block executes when the button is clicked
            if (this->handle_disconnect_button() == 0) {
                ConnectisClicked = false;
            }
        }
        //-----------------------------Disconnect Button--------------------------|
        
        //Just getting some basic serial status stuff to the GUI. this will change
        //Text Box
        ImGui::SameLine(); // Place the next widget on the same line
        ImGui::Text("Port:%s \r\n Baud:%s \r\n isConnected %i", serialManObj->getCommPort().c_str(), serialManObj->getCBaudRate().c_str(), serialManObj->isConnected());
        


        //List Comm Ports Button
        if (ImGui::Button("List Comm Ports")) {
            // This block executes when the button is clicked
            std::cout << "Getting Comm Ports from virtualComm.h " << std::endl;
            
            serialManObj->listPorts(&serialManObj->commPortNames);
            for (const std::string& portName : serialManObj->commPortNames) {
                std::cout << portName << std::endl;
            }


        }


       // Future Popup menu maybe?
        static bool open_popup = false;
        static int selected_item = -1;
        const char* items[] = { "Item 1", "Item 2", "Item 3" };

        // Button to trigger the popup
        if (ImGui::Button("Open Popup")) {
            open_popup = true;
            ImGui::OpenPopup("MyPopup");
        }

        // The actual popup window
        if (ImGui::BeginPopup("MyPopup")) {
            ImGui::Text("Select an item:");
            ImGui::Separator();
            for (int i = 0; i < 3; i++) {
                if (ImGui::Selectable(items[i], selected_item == i)) {
                    selected_item = i;
                    open_popup = false; // Close popup after selection
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }
        //End Popup Menu





        //ImGui::Text("You entered: %s", buffer);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 center = ImVec2(window_pos.x + 800, window_pos.y + 300); // Circle center relative to window
        drawCircle(draw_list, center, 50.0f, IM_COL32(255, 0, 0, 255));
        ImGui::End();

	return 0;
}


int terminal::render(void) {

	return 0;
}

void terminal::drawCircle(ImDrawList* draw_list, ImVec2 center, float radius, ImU32 color, int num_segments) {
    draw_list->AddCircle(center, radius, color, num_segments);
    draw_list->AddText(center, color, "Test");
}