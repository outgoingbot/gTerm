#include "terminal.h"
//#define IMPLOT_IMPLEMENTATION //<is this needed by implot?

terminal::terminal(int width, int height, AppConfig& cfg) : configRef(cfg) {
    _width = width;
    _height = height;
    //_window = window;

    serialManObj = new serialManager();
}


terminal::~terminal(void) {
    delete serialManObj; // Cleanup
}


void terminal::ApplyConfig()
{
    serialManObj->setCommPort(configRef.comm_port);
    serialManObj->setBaudRate(configRef.comm_baud);
    //ser_buff_num_chars = configRef.ser_buff_num_chars;
    // add every variable you need copied
}


void terminal::StoreConfig()
{
    configRef.comm_port = serialManObj->getCommPort();
    configRef.comm_baud = serialManObj->getBaudRate();
    //ser_buff_num_chars = configRef.ser_buff_num_chars;
    // add every variable you need copied
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

int terminal::handle_send_button() {
    if (serialManObj->isConnected() == false) {
        std::cout << "ERROR: Not Connected" << std::endl;
        return -1;
    }

    std::string toSend = ui.inputBuffer;   // copy the current text

    // Append EOL only once, on this send
    if (controls.tx_eol_cr) {
        toSend += '\r';
    }
    if (controls.tx_eol_lf) {
        toSend += '\n';
    }

    if (!toSend.empty()) {
        serialManObj->queueForTransmit(toSend);
    }

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
    
    //get the new characters pushed from the serial thread
    size_t newCharCount = serialManObj->getNewData(_Term_rxBufferQueue);
    //Send the buffer and number of new chars to be displayed
    term_out.update(_Term_rxBufferQueue, newCharCount, serialManObj->isConnected());
    


    if (ImGui::BeginTabBar("TermTabBar")) {

        if (ImGui::BeginTabItem("Port Config")) {
            // Put content for Tab 1 here
            //-----------------------------Comm Port Entry Text-----------------------------|
            ImGui::Text("Comm Port:");
            ImGui::SameLine(); // Place the next widget on the same line
            strncpy(ui.input_buffer_Port, serialManObj->getCommPort().c_str(), IM_ARRAYSIZE(ui.input_buffer_Port) - 1);
            ui.input_buffer_Port[IM_ARRAYSIZE(ui.input_buffer_Port) - 1] = '\0';
            ImGui::SetNextItemWidth(100);  // Set width of the input field
            if (ImGui::InputText("##Comm_Port_Entry", ui.input_buffer_Port, IM_ARRAYSIZE(ui.input_buffer_Port))) {
                serialManObj->setCommPort(ui.input_buffer_Port);
            }
            //-----------------------------Comm Port Entry Text-----------------------------|

            ImGui::SameLine(); // Place the next widget on the same line 

            //------------------------------COM Port Drop Down-------------------------------|
            if (serialManObj != nullptr && serialManObj->isConnected() == false) {
                // Button to trigger port scan and open popup
                if (ImGui::Button("Open COM Ports")) {
                    serialManObj->commPortNames.clear();
                    serialManObj->listPorts(&serialManObj->commPortNames);
                    ui.open_popup_port = true;
                    ImGui::OpenPopup("SerialPortPopup");
                }

                // The popup itself
                if (ImGui::BeginPopup("SerialPortPopup")) {
                    ImGui::Text("Select a COM port:");
                    ImGui::Separator();
                    for (const auto& port : serialManObj->commPortNames) {
                        bool isSelected = (serialManObj->getCommPort() == port);
                        if (ImGui::Selectable(port.c_str(), isSelected)) {
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


            //-----------------------------Comm Baud Entry Text-----------------------------|
            ImGui::Text("Baud Rate:");
            strncpy(ui.input_buffer_Baud, serialManObj->getBaudRate().c_str(), IM_ARRAYSIZE(ui.input_buffer_Baud) - 1);
            ui.input_buffer_Baud[IM_ARRAYSIZE(ui.input_buffer_Baud) - 1] = '\0';
            ImGui::SameLine(); // Place the next widget on the same line
            ImGui::SetNextItemWidth(100);  // Set width of the input field
            if (ImGui::InputText("##Comm_Baud_Entry", ui.input_buffer_Baud, IM_ARRAYSIZE(ui.input_buffer_Baud))) {
                serialManObj->setBaudRate(ui.input_buffer_Baud);
            }
            //-----------------------------Comm Baud Entry Text-----------------------------|

            ImGui::SameLine(); // Place the next widget on the same line

            //------------------------------BAUD Port Drop Down-------------------------------|
            if (serialManObj != nullptr && serialManObj->isConnected() == false) {
                if (ImGui::Button("Open BAUD Rates")) {
                    serialManObj->commBaudNames.clear();
                    serialManObj->listBaudRates(&serialManObj->commBaudNames);
                    ui.open_popup_baud = true;
                    ImGui::OpenPopup("SerialBaudPopup");
                }

                if (ImGui::BeginPopup("SerialBaudPopup")) {
                    ImGui::Text("Select a BAUD rate:");
                    ImGui::Separator();

                    for (const auto& baud : serialManObj->commBaudNames) {
                        bool isSelected = (serialManObj->getBaudRate() == baud);

                        if (ImGui::Selectable(baud.c_str(), isSelected)) {
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

            //-----------------------------Connect Button-----------------------------|
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
                }
                else {
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

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Terminal Config")) {
            // Put content for Tab 2 here
            //------------------------------Scroll Back Entry-------------------------------|
            snprintf(ui.input_buffer_scrollback_len, IM_ARRAYSIZE(ui.input_buffer_scrollback_len), "%zu", term_out.display_buff_num_chars);
            ImGui::Text("Char Buffer Size:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);  // Set width of the input field
            ImGui::InputText("##Scroll_Back_Entry", ui.input_buffer_scrollback_len, IM_ARRAYSIZE(ui.input_buffer_scrollback_len));
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                size_t new_value = strtoul(ui.input_buffer_scrollback_len, nullptr, 10);
                if (new_value > 0)
                {
                    term_out.display_buff_num_chars = new_value;
                }
            }
            //------------------------------Scroll Back Entry-------------------------------|

            //-----------------------------Clear Serial Buffer--------------------------|
            if (ImGui::Button("Clear Serial Buffer")) {
                _Term_rxBufferQueue.clear();
                _Term_rxBufferQueue.shrink_to_fit();
            }
            //-----------------------------Clear Serial Buffer--------------------------|

            ImGui::SameLine(); // Place the next widget on the same line

            //-----------------------------Clear Terminal Text--------------------------|
            if (ImGui::Button("Clear Terminal")) {
                term_out.clearDisplayText();
            }
            //-----------------------------Clear Terminal Text--------------------------|


            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Send")) {
            // Put content for Tab 3 here
            ImGui::InputTextMultiline("##terminal_input", ui.inputBuffer, IM_ARRAYSIZE(ui.inputBuffer),
                ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 5),
                ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine);
            //-----------------------------Send Button--------------------------|
            if (ImGui::Button("Send") || (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_Enter, false))) {
                if (this->handle_send_button() == 0) {
                    //so something agfter clicking send
                }
            }
            //-----------------------------Send Button--------------------------|

            //-----------------------------Clear Button--------------------------|
            if (ImGui::Button("Clear") || (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_Enter, false))) {
                ui.inputBuffer[0] = '\0'; //used to make the char buff 'empty'
            }
            //-----------------------------Clear Button--------------------------|

            if (ImGui::Checkbox("EOL_+CR", &controls.tx_eol_cr))
            {
                //something else of needed
            }
            if (ImGui::Checkbox("EOL_+LF", &controls.tx_eol_lf))
            {
                //something else of needed
            }


            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }




    ImGui::NewLine();



    //ImGui::EndChild();
    ImGui::End();

	return 0;
}

