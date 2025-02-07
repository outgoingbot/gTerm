// LinuxSerialComm.cpp - Linux Serial Port Implementation
#include "LinuxSerialComm.h"

LinuxSerialComm::LinuxSerialComm(const char* portName) {
    serialPort = open(portName, O_RDWR | O_NOCTTY);

    if (serialPort == -1) {
        return; // Failed to open port
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(serialPort, &tty) != 0) {
        close(serialPort);
        serialPort = -1;
        return;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~PARENB; // No parity
    tty.c_cflag &= ~CSTOPB; // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8; // 8 data bits

    tcsetattr(serialPort, TCSANOW, &tty);
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
