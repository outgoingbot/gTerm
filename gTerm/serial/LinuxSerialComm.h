// LinuxSerialComm.h - Linux Implementation of SerialComm
#ifndef LINUXSERIALCOMM_H
#define LINUXSERIALCOMM_H

#include "virtualComm.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <deque>

class LinuxSerialComm : public virtualComm {
private:
    int serialPort = -1;
    struct termios tty;

public:
    LinuxSerialComm();
    ~LinuxSerialComm();
    void ReadData(char* buffer, unsigned int nbChar, int* returnVal) override;
    bool ListBaudRates(std::deque<std::string>* BaudRateNames);

    //Check if we are actually connected
    bool IsConnected();

    bool connect();

    bool disconnect();

    //Connection status
    bool connected = false;

};

#endif // LINUXSERIALCOMM_H
