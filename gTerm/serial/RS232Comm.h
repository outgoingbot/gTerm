#ifndef RS232COMM_H_INCLUDED
#define RS232COMM_H_INCLUDED
/*
This is low level so mostly char buffers


*/


#define ARDUINO_WAIT_TIME 1

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include <deque>
#include "SerialComm.h"

#define SERIAL_NUM_BAUDS 5
#define DEFAULT_BUFF_SIZE 64

class RS232Comm : public SerialComm {
private:
	//Serial comm handler
	HANDLE hSerial;
	
	//Keep track of last error
	DWORD errors;
	
	//Get various information about the connection
	COMSTAT status;

	//Connection status
	bool connected = false;

	
	std::string baudlist[SERIAL_NUM_BAUDS] = { "9600","57600","115200","1000000", "2000000" };
	
	bool _killThreadFlag = false;

	//char* _rx_char_buffer = nullptr;
	//char* _tx_char_buffer = nullptr;

public:
	//these are the ram locations that low level tx/rx commands access



	//Initialize Serial communication with the given COM port
	RS232Comm(const char* portName);
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

	void killThread() { _killThreadFlag = true; }

	
	//get the availve com ports on the windows machine
	bool ListComPorts(std::deque<std::string> *ComPortNames);

	bool ListBaudRates(std::deque<std::string>* BaudRateNames);

	bool Connect(const char* portName, DCB dcbSerialParams = { 0 });

	bool Disconnect();

	//Check if we are actually connected
	bool IsConnected();
	
};

#endif // RS232COMM_H_INCLUDED