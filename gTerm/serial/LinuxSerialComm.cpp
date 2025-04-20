// LinuxSerialComm.cpp - Linux Serial Port Implementation
#include "LinuxSerialComm.h"

LinuxSerialComm::LinuxSerialComm() {

    // Optional: zero out tty here if you want
    memset(&tty, 0, sizeof tty);
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
    //vSerialParams.port = "/dev/ttyUSB0";
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
    if (!ComPortNames) return false;

    namespace fs = std::filesystem;
    const std::string devDir = "/dev";
    const std::vector<std::string> patterns = { "ttyS", "ttyUSB", "ttyACM" };
    bool gotPort = false;

    try {
        for (const auto& entry : fs::directory_iterator(devDir)) {
            std::string filename = entry.path().filename();
            for (const auto& prefix : patterns) {
                if (filename.rfind(prefix, 0) == 0) { // starts with
                    std::string fullPath = entry.path();

                    // Try to open the device
                    int fd = open(fullPath.c_str(), O_RDWR | O_NOCTTY);
                    if (fd != -1) {
                        struct termios tio;
                        if (tcgetattr(fd, &tio) == 0) {
                            // tcgetattr success — it's a real serial device
                            ComPortNames->push_back(fullPath);
                            std::cout << "Available: " << fullPath << std::endl;
                            gotPort = true;
                        }
                        else {
                            std::cout << "Invalid serial device: " << fullPath << std::endl;
                        }
                        close(fd);
                    }
                    else {
                        std::cout << "Cannot open: " << fullPath << std::endl;
                    }

                    break; // Already matched a pattern, no need to check others
                }
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error listing serial ports: " << e.what() << std::endl;
        return false;
    }

    std::cout << "Leaving LinuxSerialComm::ListComPorts\n" << std::endl;
    return gotPort;
}