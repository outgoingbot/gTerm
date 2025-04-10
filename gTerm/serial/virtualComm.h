// SerialComm.h - Abstract Base Class for Serial Communication (Cross-Platform)
#ifndef VIRTUALCOMM_H
#define VIRTUALCOMM_H

class virtualComm {
public:
    virtual ~virtualComm() = default;

    // Pure virtual function for reading data
    virtual void ReadData(char* buffer, unsigned int nbChar, int* returnVal) = 0;

    // Pure virtual function for reading data
    virtual bool ListComPorts(std::deque<std::string>* ComPortNames) = 0;
};

#endif // VIRTUALCOMM_H
