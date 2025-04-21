#include "terminal.h"

terminal::terminal(int width, int height) {
    _width = width;
    _height = height;
    //_window = window;
    memset(buffer, '\0', sizeof(char));
    memset(input_buffer_Port, '\0', sizeof(char));
    memset(input_buffer_Baud, '\0', sizeof(char));

    //Instatiate the serialManager Object
    serialManObj = new serialManager();
}


terminal::~terminal(void) {
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
    serialManObj->disconnect();
    return 0;
}



int terminal::update(const char* title) {
    
    // Create ImGui window
    ImGui::Begin(title);
    
    //copy the serialBuffer into a the Terminal Class
    std::deque<char> tempRxDeque;
    serialManObj->copyData(&tempRxDeque);
    
    //need to feed _Term_rxBufferQueue into a function that returns a new buffer
    char filteredBuffer[10000] = { 0 };
    size_t bufferIndex = 0;
    size_t charCount = 0;

    if (!tempRxDeque.empty()) {
        while (!tempRxDeque.empty() && bufferIndex < sizeof(filteredBuffer) - 1) {
            char c = tempRxDeque.front();
            tempRxDeque.pop_front();

            filteredBuffer[bufferIndex++] = c;
            charCount++;
            if (c == '\n' || c == '\0') {
                charCount = 1;
            }

            int regionWidth = static_cast<int>(ImGui::GetWindowWidth());

            //insert a new line character every 100 chars
            if (!(charCount % (regionWidth/12))) {
                filteredBuffer[bufferIndex++] = '\n';
            }

            
        }

        if (bufferIndex >= sizeof(filteredBuffer) - 1) {
            // Reset buffer if full
            bufferIndex = 0;
            memset(filteredBuffer, 0, sizeof(filteredBuffer));
        }
    }



    //have to always draw the entire rxBuffer to the screen.
    term_out.update(filteredBuffer, serialManObj->isConnected()); //scrolling Text class


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
        




    //------------------------------COM Status Text-------------------------------|
    //Just getting some basic serial status stuff to the GUI. this will change
    //Text Box
    ImGui::SameLine(); // Place the next widget on the same line
    ImGui::Text("isConnected %i", serialManObj->isConnected());
    //------------------------------COM Status Text-------------------------------|

    if (serialManObj->isConnected() == true) {
        std::string portString = serialManObj->getCommPort();
        strncpy(input_buffer_Port, portString.c_str(), sizeof(input_buffer_Port) - 1);
        input_buffer_Port[sizeof(input_buffer_Port) - 1] = '\0'; // Always null-terminate

        std::string baudString = serialManObj->getCBaudRate();
        strncpy(input_buffer_Baud, baudString.c_str(), sizeof(input_buffer_Baud) - 1);
        input_buffer_Baud[sizeof(input_buffer_Baud) - 1] = '\0'; // Always null-terminate
    }
    

    //------------------------------COM Port Drop Down-------------------------------|
    static bool open_popup = false;
    if (serialManObj != nullptr && serialManObj->isConnected() == false) {
        // Button to trigger port scan and open popup
        if (ImGui::Button("Open COM Ports")) {
            serialManObj->commPortNames.clear();   
            //Build the list of comports
            serialManObj->listPorts(&serialManObj->commPortNames);
            open_popup = true;
            ImGui::OpenPopup("SerialPortPopup");
        }

        // The popup itself
        if (ImGui::BeginPopup("SerialPortPopup")) {
            ImGui::Text("Select a COM port:");
            ImGui::Separator();
            for (const auto& port : serialManObj->commPortNames) {
                if (ImGui::Selectable(port.c_str(), serialManObj->selectedPort == port)) {
                    serialManObj->selectedPort = port;
                    open_popup = false;
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }

    // Display selected result
        if (serialManObj->selectedPort.empty() == false) {
            ImGui::Text("Selected port: %s", serialManObj->selectedPort.c_str());

            if (serialManObj->selectedPort != serialManObj->getCommPort()) {
                serialManObj->setComPort(&serialManObj->selectedPort);
            }

        }
    }
    //------------------------------COM Port Drop Down-------------------------------|



    //ImGui::Text("You entered: %s", buffer);
    //ImDrawList* draw_list = ImGui::GetWindowDrawList();
    //ImVec2 window_pos = ImGui::GetWindowPos();
    //ImVec2 center = ImVec2(window_pos.x + 800, window_pos.y + 300); // Circle center relative to window
    //drawCircle(draw_list, center, 50.0f, IM_COL32(255, 0, 0, 255));
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