// LinuxSerialComm.cpp - Linux Serial Port Implementation
#include "LinuxSerialComm.h"

LinuxSerialComm::LinuxSerialComm() {

    // Optional: zero out tty here if you want
    memset(&tty, 0, sizeof tty);

    //this is dumb but just saving here anyways
    vSerialParams.port = "/dev/ttyUSB0";
    vSerialParams.baud = "57600";
}

LinuxSerialComm::~LinuxSerialComm() {
    if (serialPort != -1) {
        close(serialPort);
    }
}


void LinuxSerialComm::ReadData(char* buffer, unsigned int nbChar, int* returnVal) {
    if (!buffer || !returnVal || serialPort == -1) {
        if (returnVal) *returnVal = -1;
        return;
    }

    *returnVal = 0;

    // Set up the file descriptor set
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(serialPort, &readfds);

    // Set timeout to 100 ms
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;  // 100ms

    int sel = select(serialPort + 1, &readfds, NULL, NULL, &timeout);
    if (sel > 0) {
        // Data is available
        ssize_t bytesRead = read(serialPort, buffer, nbChar);
        if (bytesRead >= 0) {
            *returnVal = static_cast<int>(bytesRead);
        }
        else {
            perror("read");
            *returnVal = -1;
        }
    }
    else if (sel == 0) {
        // Timeout, no data
        *returnVal = 0;
    }
    else {
        // Error in select
        perror("select");
        *returnVal = -1;
    }
}


//Check if we are actually connected
bool LinuxSerialComm::IsConnected() {
    return this->connected;
}


bool LinuxSerialComm::connect() {
    vSerialParams.port = "/dev/ttyUSB0";
    vSerialParams.baud = "57600";

    char portName[64];
    strncpy(portName, vSerialParams.port.c_str(), sizeof(portName) - 1);
    portName[sizeof(portName) - 1] = '\0';

    std::cout << "connecting to: " << portName << std::endl;

    serialPort = open(portName, O_RDWR | O_NOCTTY);
    //serialPort = open(portName, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (serialPort == -1) {
        std::cout << "ERROR: Could not open port: " << portName << std::endl;
        return false;
    }

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
    else {
        std::cout << "ERROR: Bad Baud: " << baud << std::endl;
        return false;
    }

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
    //Adding a Timout
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 1; //100ms Timeout

    std::cout << "VMIN: " << (int)tty.c_cc[VMIN] << ", VTIME: " << (int)tty.c_cc[VTIME] << std::endl;

    if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
        std::cout << "Failed to set linux com attributes" << std::endl;
        close(serialPort);
        serialPort = -1;
        return false;
    }
    std::cout << "SUCCESS: Linux Serial Connected " << std::endl;
    this->connected = true;
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

    return false;
}