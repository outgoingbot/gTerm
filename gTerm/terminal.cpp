#include "terminal.h"

terminal::terminal(GLFWwindow* window, int width, int height) {
    _width = width;
    _height = height;
    _window = window;
    memset(buffer, '\0', sizeof(char));
    memset(input_buffer_Port, '\0', sizeof(char));
    memset(input_buffer_Baud, '\0', sizeof(char));

#ifdef IS_WINDOWS
    serialPort = new RS232Comm("\\\\.\\COM12"); // Windows serial port
#elif defined(IS_LINUX)
    serialPort = new LinuxSerialComm("/dev/ttyS0"); // Linux serial port
#endif

    serialHandler = new serial(serialPort);



}


terminal::~terminal(void) {
    delete serialPort; // Cleanup
}

int terminal::handle_connect_button() {
    //if (!(ser_obj.isConnected())) { //prevent trying to connect twice	
    //    ser_obj.connect(std::string(input_buffer_Port), std::string(input_buffer_Baud));
    //    return 0;
    //}
    //else {
    //    return -1;
    //}

    return 0;

}


int terminal::update(const char* title) {
#define MAX_LINE_COUNT 1000 //Max Scroll back is 1000 lines
    static int lineCounter = 0;
    char lineBuff[1024];

    if (serialHandler->hasData()) {
        std::deque<char> receivedData = serialHandler->getData(0); //need to understand this parameter better
        // Process receivedData...
        serialHandler->copyToCharArray(lineBuff, sizeof(lineBuff) - 1);
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
        ImGui::SameLine(); // Place the next widget on the same line
        ImGui::SetNextItemWidth(200);  // Set width of the input field
        if (ImGui::InputText("##TextEntry", input_buffer_Port, IM_ARRAYSIZE(input_buffer_Port))) {
            // Handle text input when the field changes
        }
        //Text Box
        ImGui::SameLine(); // Place the next widget on the same line
        ImGui::Text("Baud Rate:");
        ImGui::SameLine(); // Place the next widget on the same line
        ImGui::SetNextItemWidth(200);  // Set width of the input field
        if (ImGui::InputText("##TextEntry2", input_buffer_Baud, IM_ARRAYSIZE(input_buffer_Baud))) {
            // Handle text input when the field changes
        }

        //Connect Button
        ImGui::SameLine(); // Place the next widget on the same line
        if (ImGui::Button("Connect")) {
            // This block executes when the button is clicked
            this->handle_connect_button();
            //if (ser_obj.isConnected()) {
            //    std::cout << "Starting new thread for serial port " << std::endl;
            //    std::cout << "Connecting to " << input_buffer_Port << " @ " << input_buffer_Baud << " baud" << std::endl;
            //}
        }
        //Disconnect Button
        ImGui::SameLine(); // Place the next widget on the same line
        if (ImGui::Button("Disconnect")) {
            // This block executes when the button is clicked
            std::cout << "Connecting to " << input_buffer_Port << " @ " << input_buffer_Baud << " baud" << std::endl;
        }

        //List Comm Ports Button
        ImGui::SameLine(); // Place the next widget on the same line
        if (ImGui::Button("List Comm Ports")) {
            // This block executes when the button is clicked
            std::cout << "Getting Comm Ports from SerialComm.h " << std::endl;
            
            serialHandler->listPorts(&serialHandler->commPortNames);
            for (const std::string& portName : serialHandler->commPortNames) {
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