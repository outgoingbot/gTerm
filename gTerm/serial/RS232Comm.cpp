#include "RS232Comm.h"


RS232Comm::RS232Comm(char* rx_buff, char* tx_buff, unsigned int buff_size)
{	
	if (buff_size <= 0) buff_size = DEFAULT_BUFF_SIZE;
	char* _rx_char_buffer = (char*)calloc(buff_size, sizeof(char));
	char* _tx_char_buffer = (char*)calloc(buff_size, sizeof(char));

	//memset(_rx_char_buffer, '\0', sizeof(char) * buff_size);
	//memset(_tx_char_buffer, '\0', sizeof(char) * buff_size);

	//copy the value in the rs232 pointer to the value in serial pointer
	if (_rx_char_buffer != nullptr && _tx_char_buffer != nullptr) {
		rx_buff = _rx_char_buffer;
		tx_buff = _tx_char_buffer;
	}
	//We're not yet connected
	this->connected = false;
}


RS232Comm::~RS232Comm()
{
	//Check if we are connected before trying to disconnect
	if (this->connected)
	{
		//We're no longer connected
		this->connected = false;

		//Todo: free the buffers
		
		//Close the serial handler
		CloseHandle(this->hSerial);
	}
}


void RS232Comm::ReadData(char* buffer, unsigned int nbChar, int* returnVal){
		static char c;
		unsigned int toRead = 0;//Number of bytes we ask to read
		DWORD bytesRead = 0; //Number of bytes we'll have read
		
		//Use the ClearCommError function to get status info on the Serial port
		ClearCommError(this->hSerial, &this->errors, &this->status);
		
		//Check if there is something to read
		if (this->status.cbInQue > 0)
		{
			//queueSize = this->status.cbInQue;
			//If there is we check if there is enough data to read the required number
			//of characters, if not we'll read only the available characters to prevent
			//locking of the application.
			toRead = (this->status.cbInQue > nbChar) ? nbChar : this->status.cbInQue;
			
			/*if (this->status.cbInQue > nbChar)
				toRead = nbChar;
			else
				toRead = this->status.cbInQue;*/

			//Try to read the require number of chars, and return the number of read bytes on success
			if (ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL))
			{
			////push all chars in the llbuffer to the deque
			//	for (uint8_t i = 0; i < bytesRead; i++) rxBuffer->push(*(buffer + i));

			//	if (this->rxBuffer->qSizeUsed() > 0) { //if i got at least 1 byte
			//		c = this->rxBuffer->pop();
			//	}

				*returnVal = (int)bytesRead;
			}


		}
}


bool RS232Comm::WriteData(const char *buffer, unsigned int nbChar)
{
	DWORD bytesSend;

	//Try to write the buffer on the Serial port
	if (!WriteFile(this->hSerial, (void *)buffer, nbChar, &bytesSend, 0))
	{
		//In case it don't work get comm error and return false
		ClearCommError(this->hSerial, &this->errors, &this->status);

		return false;
	}
	else
		return true;
}


bool RS232Comm::ListBaudRates(std::deque<std::string>* BaudRateNames) {
	for (auto i = 0; i < SERIAL_NUM_BAUDS; i++) BaudRateNames->push_back(baudlist[i]);
	return 0;
}


bool RS232Comm::ListComPorts(std::deque<std::string>* ComPortNames) //added function to find the present serial 
{
	char lpTargetPath[5000]; // buffer to store the path of the COMPORTS
	bool gotPort = false; // in case the port is not found

	for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
	{
		std::string str = "COM" + std::to_string(i); // converting to COM0, COM1, COM2
		DWORD test = QueryDosDevice(str.c_str(), lpTargetPath, 5000);

		// Test the return value and error if any
		if (test != 0) //QueryDosDevice returns zero if it didn't find an object
		{
			ComPortNames->push_back(str);
			std::cout << str << ": " << lpTargetPath << std::endl;
			gotPort = true;
		}

		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
		}
	}

	return gotPort;
}


bool RS232Comm::Connect(const char* portName, DCB dcbSerialParams)
{
	if (this->connected == false) {
		//Try to connect to the given port throuh CreateFile
		this->hSerial = CreateFile(portName,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		//Check if the connection was successfull
		if (this->hSerial == INVALID_HANDLE_VALUE)
		{
			//If not success full display an Error
			if (GetLastError() == ERROR_FILE_NOT_FOUND) {

				//Print Error if neccessary
				printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);
				return false;
			}
			else
			{
				printf("ERROR!!!");
				return false;
			}
		}
		else
		{
			//If connected we try to set the comm parameters
			//DCB dcbSerialParams = { 0 };

			//Try to get the current
			if (!GetCommState(this->hSerial, &dcbSerialParams))
			{
				//If impossible, show an error
				printf("failed to get current serial parameters!");
				return false;
			}
			else
			{
				//Define serial connection parameters for the arduino board
				//dcbSerialParams.BaudRate = 2000000;//CBR_115200;
				dcbSerialParams.ByteSize = 8;
				dcbSerialParams.StopBits = ONESTOPBIT;
				dcbSerialParams.Parity = NOPARITY;
				//Setting the DTR to Control_Enable ensures that the Arduino is properly
				//reset upon establishing a connection
				dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

				//Set the parameters and check for their proper application
				if (!SetCommState(hSerial, &dcbSerialParams))
				{
					printf("ALERT: Could not set Serial Port parameters");
					return false;
				}
				else
				{
					//If everything went fine we're connected
					this->connected = true;
					this->_killThreadFlag = false;
					//Flush any remaining characters in the buffers 
					PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
					//We wait as the arduino board will be reseting
					Sleep(ARDUINO_WAIT_TIME);
				}

				//now set the timeouts ( we control the timeout overselves using WaitForXXX()
				COMMTIMEOUTS timeouts;

				timeouts.ReadIntervalTimeout = MAXDWORD;
				timeouts.ReadTotalTimeoutMultiplier = 0;
				timeouts.ReadTotalTimeoutConstant = 0;
				timeouts.WriteTotalTimeoutMultiplier = 0;
				timeouts.WriteTotalTimeoutConstant = 0;

				if (!SetCommTimeouts(hSerial, &timeouts))
				{
					printf("ALERT: Could not set Serial Port parameters");
					return false;
				}

			}
		}
	}
	//Simply return the connection status
	return this->connected;
}

bool RS232Comm::Disconnect()
{
	if (this->connected == true) {
		//ToDo: RS232Comm Object still exists. has not been deconstructed. possible mem leak?
		this->_killThreadFlag = true;
		this->connected = false;
		CloseHandle(this->hSerial);
	}
	return this->connected;
}


bool RS232Comm::IsConnected()
{
	//Simply return the connection status
	return this->connected;
}
