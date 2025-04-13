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

//void LinuxSerialComm::ReadData(char* buffer, unsigned int nbChar, int* returnVal) {
//    if (!buffer || !returnVal || serialPort == -1) return;
//
//    *returnVal = read(serialPort, buffer, nbChar);
//    if (*returnVal == -1) {
//        *returnVal = -1; // Indicate failure
//    }
//}

void LinuxSerialComm::ReadData(char* buffer, unsigned int nbChar, int* returnVal) {
    if (!buffer || !returnVal || serialPort == -1) return;

    ssize_t bytesRead = read(serialPort, buffer, nbChar);
    *returnVal = static_cast<int>(bytesRead); // cast ssize_t to int

    // Optional: you could check for errno or log if bytesRead == -1
}



//Check if we are actually connected
bool LinuxSerialComm::IsConnected() {

    return this->connected;
}

bool LinuxSerialComm::connect() {
    vSerialParams.port = "/dev/ttyS0";
    vSerialParams.baud = "57600";

    char portName[64];
    strncpy(portName, vSerialParams.port.c_str(), sizeof(portName) - 1);
    portName[sizeof(portName) - 1] = '\0';

    std::cout << "connecting to: " << portName << std::endl;

    serialPort = open(portName, O_RDWR | O_NOCTTY);
    if (serialPort == -1) return false;

    if (tcgetattr(serialPort, &tty) != 0) {
        close(serialPort);
        serialPort = -1;
        return false;
    }

    int baud = std::stoi(vSerialParams.baud);
    speed_t speed;
    if (baud == 9600)   speed = B9600;
    else if (baud == 19200)  speed = B19200;
    else if (baud == 38400)  speed = B38400;
    else if (baud == 57600)  speed = B57600;
    else if (baud == 115200) speed = B115200;
    else return false;

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    tty.c_lflag = 0;
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
    this->connected = false;

    if (serialPort != -1) {
        close(serialPort);
        serialPort = -1;
        return true; // Successfully disconnected
    }
    return false; // Already disconnected
}



bool LinuxSerialComm::ListComPorts(std::deque<std::string>* ComPortNames) {

}