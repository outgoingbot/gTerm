// LinuxSerialComm.h - Linux Implementation of SerialComm
#ifndef LINUXSERIALCOMM_H
#define LINUXSERIALCOMM_H

#include "SerialComm.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>

class LinuxSerialComm : public SerialComm {
private:
    int serialPort;

public:
    LinuxSerialComm(const char* portName);
    ~LinuxSerialComm();
    void ReadData(char* buffer, unsigned int nbChar, int* returnVal) override;
};

#endif // LINUXSERIALCOMM_H
