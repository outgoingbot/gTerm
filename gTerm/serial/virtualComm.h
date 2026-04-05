// SerialComm.h - Abstract Base Class for Serial Communication (Cross-Platform)
#ifndef VIRTUALCOMM_H
#define VIRTUALCOMM_H

#include <string>
#include <deque>
#include <cstdint>


class virtualComm {

public:

    //Define the serial parameters that will be used for both RS232Comm and LinuxSerialComm  
    typedef struct serialParameters {
        std::string port;
        std::string baud;
        uint8_t byteSize;
        //TODO: add other params like: 8,N,1
    } serialParameters;

    static inline const std::string baudlist[] = { "9600","57600","115200","1000000", "2000000" };

    //the Primary serial Parameters are maintained by the virtual class
    serialParameters vSerialParams;

    virtual ~virtualComm() = default;

    // Pure virtual function for reading data
    virtual void ReadData(char* buffer, unsigned int nbChar, int* returnVal) = 0;

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

    virtual bool connect() = 0;

    virtual bool disconnect() = 0;

    //Check if we are actually connected
    virtual bool IsConnected() = 0;

    
    // Event-driven support - returns true if new data is available
        // timeoutMs = -1 means wait forever
    //virtual bool WaitForData(int timeoutMs = -1) = 0;
    //Event driver rs232com
    //virtual void SetupEventDrivenRead() = 0;

    //virtual bool setPort(serialParams* serialParams);

    //virtual bool setBaud(serialParams* serialParams);
};

#endif // VIRTUALCOMM_H
