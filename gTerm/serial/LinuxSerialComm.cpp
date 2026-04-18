#include "LinuxSerialComm.h"

LinuxSerialComm::LinuxSerialComm() {
    memset(&tty2, 0, sizeof(tty2));
}

LinuxSerialComm::~LinuxSerialComm() {
    if (serialPort != -1) {
        close(serialPort);
        serialPort = -1;
    }
}

void LinuxSerialComm::ReadData(char* buffer, unsigned int nbChar, int* returnVal) {
    if (!buffer || !returnVal || serialPort == -1) {
        if (returnVal) *returnVal = -1;
        return;
    }
    *returnVal = 0;
    ssize_t bytesRead = read(serialPort, buffer, nbChar);
    if (bytesRead > 0) {
        *returnVal = static_cast<int>(bytesRead);
    }
    else if (bytesRead == 0) {
        *returnVal = 0;
    }
    else {
        LOG_ERROR_ERRNO("read");
        *returnVal = -1;
    }
}

bool LinuxSerialComm::WriteData(const char* buffer, unsigned int nbChar)
{
    if (!buffer || nbChar == 0 || serialPort == -1 || !connected) {
        return false;
    }
    ssize_t bytesWritten = write(serialPort, buffer, nbChar);
    if (bytesWritten < 0) {
        LOG_ERROR_ERRNO("write");
        return false;
    }
    // tcdrain replacement using ioctl (works with termios2)
    if (ioctl(serialPort, TCSBRK, 1) != 0) {
        LOG_ERROR_ERRNO("ioctl TCSBRK");
    }
    return (static_cast<unsigned int>(bytesWritten) == nbChar);
}

bool LinuxSerialComm::IsConnected() {
    return this->connected;
}

bool LinuxSerialComm::connect() {
    if (vSerialParams.port.empty()) {
        LOG_ERROR("Port name is empty");
        return false;
    }

    char portName[64];
    strncpy(portName, vSerialParams.port.c_str(), sizeof(portName) - 1);
    portName[sizeof(portName) - 1] = '\0';

    LOG_INFO("Connecting to: " << portName);

    serialPort = open(portName, O_RDWR | O_NOCTTY);
    if (serialPort == -1) {
        LOG_ERROR_SYS("Could not open port: " << portName);
        return false;
    }

    // Get current settings using termios2
    if (ioctl(serialPort, TCGETS2, &tty2) != 0) {
        LOG_ERROR_SYS("TCGETS2 failed");
        close(serialPort);
        serialPort = -1;
        return false;
    }

    int baud = 0;
    try {
        baud = std::stoi(vSerialParams.baud);
    }
    catch (...) {
        LOG_ERROR("Invalid baud rate string: " << vSerialParams.baud);
        close(serialPort);
        serialPort = -1;
        return false;
    }

    if (baud <= 0) {
        LOG_ERROR("Bad baud rate: " << baud);
        close(serialPort);
        serialPort = -1;
        return false;
    }

    // Custom baud (any speed)
    tty2.c_cflag &= ~CBAUD;
    tty2.c_cflag |= BOTHER;

    tty2.c_ispeed = baud;
    tty2.c_ospeed = baud;

    // Raw 8N1 settings
    tty2.c_cflag |= (CLOCAL | CREAD);
    tty2.c_cflag &= ~PARENB;
    tty2.c_cflag &= ~CSTOPB;
    tty2.c_cflag &= ~CSIZE;
    tty2.c_cflag |= CS8;

    tty2.c_lflag = 0;
    tty2.c_oflag = 0;
    tty2.c_iflag = 0;

    tty2.c_cc[VMIN] = 0; // 0 = return as soon as data is available or timeout expires
    tty2.c_cc[VTIME] = 1; // VTIME is in tenths of a second -> 1 = 100ms

    LOG_INFO("Setting baud rate: " << baud << " | VMIN: " << (int)tty2.c_cc[VMIN] << ", VTIME: " << (int)tty2.c_cc[VTIME]);

    if (ioctl(serialPort, TCSETS2, &tty2) != 0) {
        LOG_ERROR_SYS("TCSETS2 failed for baud " << baud);
        close(serialPort);
        serialPort = -1;
        return false;
    }

    //test this. to flush any garbage on connect
    //ioctl(serialPort, TCFLSH, TCIOFLUSH);

    LOG_SUCCESS("Linux Serial Connected at " << baud << " baud");
    this->connected = true;
    return true;
}

bool LinuxSerialComm::disconnect() {
    this->connected = false;
    if (serialPort != -1) {
        close(serialPort);
        serialPort = -1;
        return true;
    }
    return false;
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
                if (filename.rfind(prefix, 0) == 0) {
                    std::string fullPath = entry.path();
                    int fd = open(fullPath.c_str(), O_RDWR | O_NOCTTY);
                    if (fd != -1) {
                        struct termios2 tio2;
                        if (ioctl(fd, TCGETS2, &tio2) == 0) {
                            ComPortNames->push_back(fullPath);
                            LOG_INFO("Available: " << fullPath);
                            gotPort = true;
                        }
                        else {
                            LOG_ERROR_SYS("Invalid serial device: " << fullPath);
                        }
                        close(fd);
                    }
                    else {
                        LOG_ERROR_SYS("Cannot open: " << fullPath);
                    }
                    break;
                }
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        LOG_ERROR_SYS("Error listing serial ports: " << e.what());
        return false;
    }

    LOG_INFO("Leaving LinuxSerialComm::ListComPorts");
    return gotPort;
}