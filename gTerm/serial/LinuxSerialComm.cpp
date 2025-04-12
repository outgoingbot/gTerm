// LinuxSerialComm.cpp - Linux Serial Port Implementation
#include "LinuxSerialComm.h"

LinuxSerialComm::LinuxSerialComm() {

    // Optional: zero out tty here if you want
    memset(&tty, 0, sizeof tty);

    //this is dumb but just saving here anyways
    vSerialParams.port = "/dev/ttyS0";
    vSerialParams.baud = "57600";
}

LinuxSerialComm::~LinuxSerialComm() {
    if (serialPort != -1) {
        close(serialPort);
    }
}

void LinuxSerialComm::ReadData(char* buffer, unsigned int nbChar, int* returnVal) {
    if (!buffer || !returnVal || serialPort == -1) return;

    *returnVal = read(serialPort, buffer, nbChar);
    if (*returnVal == -1) {
        *returnVal = -1; // Indicate failure
    }
}

//Check if we are actually connected
bool LinuxSerialComm::IsConnected() {

    return this->connected;
}


bool LinuxSerialComm::connect() {
    
    serialPort = open(vSerialParams.port.c_str(), O_RDWR | O_NOCTTY);
    if (serialPort == -1) {
        return false; // Failed to open port
    }

    if (tcgetattr(serialPort, &tty) != 0) {
        close(serialPort);
        serialPort = -1;
        return false;
    }

    // Configure settings
    cfsetospeed(&tty, B57600);
    cfsetispeed(&tty, B57600);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    tty.c_lflag = 0; // No canonical mode, no echo
    tty.c_oflag = 0;
    tty.c_iflag = 0;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
        close(serialPort);
        serialPort = -1;
        return false;
    }

    return true;
}


bool LinuxSerialComm::disconnect() {
    this->connect = false;

    if (serialPort != -1) {
        close(serialPort);
        serialPort = -1;
        return true; // Successfully disconnected
    }
    return false; // Already disconnected
}



bool LinuxSerialComm::ListBaudRates(std::deque<std::string>* BaudRateNames) {

}