// SerialComm.h - Abstract Base Class for Serial Communication (Cross-Platform)
#ifndef VIRTUALCOMM_H
#define VIRTUALCOMM_H

#include <string>
#include <deque>
#include <cstdint>

class virtualComm {

public:

    //Define the serial parameters that will be used for both RS232Comm and LinuxSerialComm  
    struct serialParameters {
        std::string port;
        std::string baud;
        uint8_t byteSize;
        //TODO: add other params like: 8,N,1
    } vSerialParams;

    static inline const std::string baudlist[] = { "4800","9600","57600","115200","921600","1000000", "2000000" };

    
    virtual ~virtualComm() = default;

    virtual bool connect() = 0;
    virtual bool disconnect() = 0;
    virtual bool IsConnected() = 0;

    // Pure virtual function for reading data
    virtual void ReadData(char* buffer, unsigned int nbChar, int* returnVal) = 0;
    virtual bool WriteData(const char* buffer, unsigned int nbChar) = 0;

    // Pure virtual function for Listing Comm Ports
    virtual bool ListComPorts(std::deque<std::string>* ComPortNames) = 0;

    //function for Listing baud rates    
    bool ListBaudRates(std::deque<std::string>* BaudRateNames) {
        if (!BaudRateNames) return false;

        for (const auto& baud : baudlist) {
            BaudRateNames->push_back(baud);
        }
        return true;
    }

};

#endif // VIRTUALCOMM_H
