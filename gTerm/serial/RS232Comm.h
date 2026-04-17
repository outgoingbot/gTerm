#ifndef RS232COMM_H_INCLUDED
#define RS232COMM_H_INCLUDED


#define MCU_WAIT_TIME 1 //wait time for MCU to boot after DTR pulse
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <deque>
#include "virtualComm.h"


class RS232Comm : public virtualComm {
private:

	//Serial comm handler
	HANDLE hSerial;
	
	//Keep track of last error
	DWORD errors;
	
	//Get various information about the connection
	COMSTAT status;

	//Connection Parameters
	DCB dcbSerialParams;
	
public:

	//Initialize Serial communication
	RS232Comm();
	//Close the connection
	~RS232Comm();

	bool connect();
	bool openPort(const char* portName, DCB dcbSerialParams = { 0 });

	bool disconnect();
	bool closePort();

	//return connected flag
	bool IsConnected();

	//get the availve com ports on the windows machine
	bool ListComPorts(std::deque<std::string>* ComPortNames);

	//Read data in a buffer, if nbChar is greater than the
	//maximum number of bytes available, it will return only the
	//bytes available. The function return 0 when nothing could
	//be read, the number of bytes actually read, and -1 on error
	void ReadData(char* buffer, unsigned int nbChar, int* returnVal) override;

	//Writes data from a buffer through the Serial connection
	//return true on success.
	bool WriteData(const char *buffer, unsigned int nbChar);

	
	//Connection status
	bool connected = false;
};

#endif // RS232COMM_H_INCLUDED