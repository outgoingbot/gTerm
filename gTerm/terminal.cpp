#include "terminal.h"
//#define IMPLOT_IMPLEMENTATION //<is this needed by implot?

terminal::terminal(int width, int height) {
    _width = width;
    _height = height;
    //_window = window;

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

//Method to expose thread safe access to rx buffer
const std::deque<char>& terminal::getRxBuffer() const
{
    return _Term_rxBufferQueue;
}


int terminal::update(const char* title) {
    
    // Create ImGui window for terminal
    ImGui::Begin(title);

    // 1. Get new data + number of characters actually added
    size_t newCharCount = serialManObj->getNewData(_Term_rxBufferQueue);

    // 2. Process ONLY the new characters
    for (size_t i = 0; i < newCharCount; ++i)
    {
        // We know the new characters are at the end of the deque
        char c = _Term_rxBufferQueue[_Term_rxBufferQueue.size() - newCharCount + i];

        if (c == '\n')
        {
            _displayLines.push_back(std::move(_currentPartialLine));
            _currentPartialLine.clear();
        }
        else if (c != '\r')
        {
            _currentPartialLine += c;
        }
    }

    // 3. Limit display lines only
    //const size_t MAX_LINES = 500;

    if (_displayLines.size() > term_out.scroll_back_length)
    {
        _displayLines.erase(_displayLines.begin(),
            _displayLines.begin() + (_displayLines.size() - term_out.scroll_back_length));
    }

    // 4. Send to output
    term_out.update(_displayLines, serialManObj->isConnected());
    
       
    //-----------------------------Comm Port Entry Text-----------------------------|
    ImGui::Text("Comm Port:");
    ImGui::SameLine(); // Place the next widget on the same line
    strncpy(ui.input_buffer_Port, serialManObj->getCommPort().c_str(),IM_ARRAYSIZE(ui.input_buffer_Port) - 1);
    ui.input_buffer_Port[IM_ARRAYSIZE(ui.input_buffer_Port) - 1] = '\0';
    ImGui::SetNextItemWidth(150);  // Set width of the input field
    if (ImGui::InputText("##Comm_Port_Entry", ui.input_buffer_Port, IM_ARRAYSIZE(ui.input_buffer_Port))){
        serialManObj->setCommPort(ui.input_buffer_Port);
    }
    //-----------------------------Comm Port Entry Text-----------------------------|

    ImGui::SameLine(); // Place the next widget on the same line 

    //-----------------------------Comm Baud Entry Text-----------------------------|
    ImGui::Text("Baud Rate:");
    strncpy(ui.input_buffer_Baud, serialManObj->getBaudRate().c_str(), IM_ARRAYSIZE(ui.input_buffer_Baud) - 1);
    ui.input_buffer_Baud[IM_ARRAYSIZE(ui.input_buffer_Baud) - 1] = '\0';
    ImGui::SameLine(); // Place the next widget on the same line
    ImGui::SetNextItemWidth(150);  // Set width of the input field
    if (ImGui::InputText("##Comm_Baud_Entry", ui.input_buffer_Baud, IM_ARRAYSIZE(ui.input_buffer_Baud))) {
        serialManObj->setBaudRate(ui.input_buffer_Baud);
    }
    //-----------------------------Comm Baud Entry Text-----------------------------|

    ImGui::SameLine(); // Place the next widget on the same line

    //-----------------------------Connect Button-----------------------------|
    //static bool ConnectisClicked = false;
    bool pushingColor = false;    
    if (ui.ConnectisClicked) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));  // green
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));  // lighter green
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.8f, 0.0f, 1.0f));  // darker green
        pushingColor = true;
    }

    if (ImGui::Button("Connect")) {
        ui.ConnectisClicked = true;
        if (this->handle_connect_button() == 0) {
            std::cout << "SUCCESS: Terminal Class Connected!" << std::endl;
            std::cout << "" << serialManObj->getCommPort() << std::endl;
            std::cout << "" << serialManObj->getBaudRate() << std::endl;
        }else {
            std::cout << "ERROR: Terminal Class Not Connected!" << std::endl;
        }
    }

    if (pushingColor) ImGui::PopStyleColor(3);
    //-----------------------------Connect Button-----------------------------|
       
    ImGui::SameLine(); // Place the next widget on the same line

    //-----------------------------Disconnect Button--------------------------|
    if (ImGui::Button("Disconnect")) {
        if (this->handle_disconnect_button() == 0) {
            ui.ConnectisClicked = false;
        }
    }
    //-----------------------------Disconnect Button--------------------------|
        

    //------------------------------COM Port Drop Down-------------------------------|
    if (serialManObj != nullptr && serialManObj->isConnected() == false){
        // Button to trigger port scan and open popup
        if (ImGui::Button("Open COM Ports")){
            serialManObj->commPortNames.clear();
            serialManObj->listPorts(&serialManObj->commPortNames);
            ui.open_popup_port = true;
            ImGui::OpenPopup("SerialPortPopup");
        }

        // The popup itself
        if (ImGui::BeginPopup("SerialPortPopup")){
            ImGui::Text("Select a COM port:");
            ImGui::Separator();
            for (const auto& port : serialManObj->commPortNames){
                bool isSelected = (serialManObj->getCommPort() == port);
                if (ImGui::Selectable(port.c_str(), isSelected)){
                    serialManObj->setCommPort(port);
                    ui.open_popup_port = false;
                    ImGui::CloseCurrentPopup();
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndPopup();
        }
    }
    //------------------------------COM Port Drop Down-------------------------------|
    

    ImGui::SameLine(); // Place the next widget on the same line
    

    //------------------------------BAUD Port Drop Down-------------------------------|
    if (serialManObj != nullptr && serialManObj->isConnected() == false){
        if (ImGui::Button("Open BAUD Rates")){
            serialManObj->commBaudNames.clear();
            serialManObj->listBaudRates(&serialManObj->commBaudNames);
            ui.open_popup_baud = true;
            ImGui::OpenPopup("SerialBaudPopup");
        }

        if (ImGui::BeginPopup("SerialBaudPopup")){
            ImGui::Text("Select a BAUD rate:");
            ImGui::Separator();

            for (const auto& baud : serialManObj->commBaudNames){
                bool isSelected = (serialManObj->getBaudRate() == baud);

                if (ImGui::Selectable(baud.c_str(), isSelected)){
                    serialManObj->setBaudRate(baud);
                    ui.open_popup_baud = false;
                    ImGui::CloseCurrentPopup();
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndPopup();
        }
    }
    //------------------------------BAUD Port Drop Down-------------------------------|

    ImGui::NewLine();

    //------------------------------Scroll Back Entry-------------------------------|
    snprintf(ui.input_buffer_scrollback_len, IM_ARRAYSIZE(ui.input_buffer_scrollback_len),"%zu", term_out.scroll_back_length);
    ImGui::Text("Scroll Back Lines:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);  // Set width of the input field
    ImGui::InputText("##Scroll_Back_Entry", ui.input_buffer_scrollback_len, IM_ARRAYSIZE(ui.input_buffer_scrollback_len));
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        size_t new_value = strtoul(ui.input_buffer_scrollback_len, nullptr, 10);
        if (new_value > 0)
        {
            term_out.scroll_back_length = new_value;
        }
    }
    //------------------------------Scroll Back Entry-------------------------------|

    ImGui::End();

	return 0;
}

