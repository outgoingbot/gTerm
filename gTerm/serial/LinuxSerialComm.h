#ifndef LINUXSERIALCOMM_H
#define LINUXSERIALCOMM_H

#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <deque>
#include <vector>
#include <iostream>
#include <sys/ioctl.h>
#include <asm/termbits.h>

#include "virtualComm.h"
#include "../logger.h"

class LinuxSerialComm : public virtualComm {
private:
    int serialPort = -1;
    struct termios2 tty2;

public:
    LinuxSerialComm();
    ~LinuxSerialComm();

    void ReadData(char* buffer, unsigned int nbChar, int* returnVal) override;
    bool WriteData(const char* buffer, unsigned int nbChar) override;

    bool ListComPorts(std::deque<std::string>* ComPortNames);
    bool IsConnected();
    bool connect();
    bool disconnect();

    bool connected = false;
};

#endif // LINUXSERIALCOMM_H