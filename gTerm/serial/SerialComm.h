// SerialComm.h - Abstract Base Class for Serial Communication (Cross-Platform)
#ifndef SERIALCOMM_H
#define SERIALCOMM_H

class SerialComm {
public:
    virtual ~SerialComm() = default;

    // Pure virtual function for reading data
    virtual void ReadData(char* buffer, unsigned int nbChar, int* returnVal) = 0;

    // Pure virtual function for reading data
    virtual bool ListComPorts(std::deque<std::string>* ComPortNames) = 0;
};

#endif // SERIALCOMM_H
