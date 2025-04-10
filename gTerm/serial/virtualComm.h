// SerialComm.h - Abstract Base Class for Serial Communication (Cross-Platform)
#ifndef VIRTUALCOMM_H
#define VIRTUALCOMM_H

#include <string>
#include <deque>


class virtualComm {

public:

    //Define the serial parameters that will be used for both RS232Comm and LinuxSerialComm  
    typedef struct serialParameters {
        std::string port;
        std::string baud;
        //TODO: add other params like: 8,N,1
    } serialParameters;

    //the Primary serial Parameters are maintained by the virtual class
    serialParameters vSerialParams;

    virtual ~virtualComm() = default;

    // Pure virtual function for reading data
    virtual void ReadData(char* buffer, unsigned int nbChar, int* returnVal) = 0;

    // Pure virtual function for reading data
    virtual bool ListComPorts(std::deque<std::string>* ComPortNames) = 0;

    virtual bool connect() = 0;

    //Check if we are actually connected
    virtual bool IsConnected() = 0;


    //virtual bool setPort(serialParams* serialParams);

    //virtual bool setBaud(serialParams* serialParams);
};

#endif // VIRTUALCOMM_H
