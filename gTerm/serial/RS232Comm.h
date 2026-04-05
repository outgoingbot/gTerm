#ifndef RS232COMM_H_INCLUDED
#define RS232COMM_H_INCLUDED
/*
This is low level so mostly char buffers


*/


#define MCU_WAIT_TIME 1
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include <deque>
#include "virtualComm.h"

//#define SERIAL_NUM_BAUDS 5
//#define DEFAULT_BUFF_SIZE 64

class RS232Comm : public virtualComm {
private:

	//Serial comm handler
	HANDLE hSerial;
	
	//Keep track of last error
	DWORD errors;
	
	//Get various information about the connection
	COMSTAT status;

	//Connection status
	bool connected = false;

	//Connection Parameters
	DCB dcbSerialParams;
	
	//OVERLAPPED m_overlapped = { 0 };
	//HANDLE     m_hEvent = NULL;      // event used by WaitCommEvent

public:
	//these are the ram locations that low level tx/rx commands access

	//Initialize Serial communication
	RS232Comm();
	//Close the connection
	~RS232Comm();

	//Read data in a buffer, if nbChar is greater than the
	//maximum number of bytes available, it will return only the
	//bytes available. The function return -1 when nothing could
	//be read, the number of bytes actually read.
	void ReadData(char* buffer, unsigned int nbChar, int* returnVal) override;

	//Writes data from a buffer through the Serial connection
	//return true on success.
	bool WriteData(const char *buffer, unsigned int nbChar);

	//void killThread() { _killThreadFlag = true; }
		
	//get the availve com ports on the windows machine
	bool ListComPorts(std::deque<std::string> *ComPortNames);

	bool Connect(const char* portName, DCB dcbSerialParams = { 0 });

	bool Disconnect();

	//Check if we are actually connected
	bool IsConnected();

	bool connect();

	bool disconnect();

	//event driven comm port
	//void SetupEventDrivenRead();
	//bool WaitForData(int timeoutMs = -1) override;   // -1 means infinite
};

#endif // RS232COMM_H_INCLUDED