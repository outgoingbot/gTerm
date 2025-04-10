#include "RS232Comm.h"


RS232Comm::RS232Comm() {	
	//We're not yet connected
	this->connected = false;
	//Initialize RS232 variables
	hSerial = INVALID_HANDLE_VALUE;
	errors = 0;
	ZeroMemory(&status, sizeof(COMSTAT));
	ZeroMemory(&dcbSerialParams, sizeof(DCB));
	dcbSerialParams.DCBlength = sizeof(DCB);  // Required before calling SetCommState


	//Intialize the Virtual serial port Parameters
	vSerialParams.port = "\\\\.\\COM12";
	vSerialParams.baud = "57600";
}


RS232Comm::~RS232Comm() {
	//Check if we are connected before trying to disconnect
	if (this->connected) {
		//We're no longer connected
		this->connected = false;

		//Todo: free the buffers
		
		//Close the serial handler
		if (hSerial != INVALID_HANDLE_VALUE) {
			CloseHandle(hSerial);
		}
	}
}


void RS232Comm::ReadData(char* buffer, unsigned int nbChar, int* returnVal) {
	if (!buffer || !returnVal) return;

	*returnVal = 0;
	DWORD bytesRead = 0;
	ClearCommError(hSerial, &errors, &status);

	if (status.cbInQue > 0) {
		DWORD toRead = (status.cbInQue > nbChar) ? nbChar : status.cbInQue;
		if (ReadFile(hSerial, buffer, toRead, &bytesRead, NULL)) {
			*returnVal = static_cast<int>(bytesRead);
		}
		else {
			*returnVal = -1; // Indicate failure
		}
	}
}



bool RS232Comm::WriteData(const char *buffer, unsigned int nbChar)
{
	DWORD bytesSend;

	//Try to write the buffer on the Serial port
	if (!WriteFile(this->hSerial, (void *)buffer, nbChar, &bytesSend, 0)) {
		//In case it don't work get comm error and return false
		ClearCommError(this->hSerial, &this->errors, &this->status);
		return false;
	} else {
		return true;
	}
}


bool RS232Comm::ListBaudRates(std::deque<std::string>* BaudRateNames) {
	for (auto i = 0; i < SERIAL_NUM_BAUDS; i++) BaudRateNames->push_back(baudlist[i]);
	return 0;
}


bool RS232Comm::ListComPorts(std::deque<std::string>* ComPortNames) { //added function to find the present serial 
	char lpTargetPath[5000]; // buffer to store the path of the COMPORTS
	bool gotPort = false; // in case the port is not found
	for (int i = 0; i < 255; i++) { // checking ports from COM0 to COM255
		std::string str = "COM" + std::to_string(i); // converting to COM0, COM1, COM2
		DWORD test = QueryDosDevice(str.c_str(), lpTargetPath, 5000);

		// Test the return value and error if any
		if (test != 0) { //QueryDosDevice returns zero if it didn't find an object
			std::string windowsFormatComPort = "\\\\.\\" + str;
			ComPortNames->push_back(windowsFormatComPort);
			std::cout << str << ": " << lpTargetPath << std::endl;
			gotPort = true;
		}

		//Dont like this style
		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER){
		}
	}
	std::cout << "Leaving RS232COMM\r\n" << std::endl;
	return gotPort;
}





bool RS232Comm::connect() {
	//set serialParams

	char portName[64];
	vSerialParams.port.copy(portName, vSerialParams.port.size());
	portName[vSerialParams.port.size()] = '\0';  // null-terminate


	//Build Baud Rate DWORD for DCB
	//std::string baud = serialParams->baud;
	//DWORD commBaud_DWORD = std::stoul(std::string(baud));

	//TODO: copy the serialParams struct members into the DCB struct members
	dcbSerialParams.BaudRate = (DWORD) std::stoul(vSerialParams.baud);
	//dcbSerialParams.BaudRate = 57600;//CBR_115200;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	//Setting the DTR to Control_Enable ensures that the MCU is properly reset upon establishing a connection
	dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

	//now actually connect
	return this->Connect(portName, dcbSerialParams);


	//return true;
}



bool RS232Comm::Connect(const char* portName, DCB dcbSerialParams) {
	if (this->connected == false) {
		//Try to connect to the given port throuh CreateFile
		this->hSerial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		//this->hSerial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		//Check if the connection was successfull
		if (this->hSerial == INVALID_HANDLE_VALUE) {
			DWORD error = GetLastError();
			//If not success full display an Error
			if (GetLastError() == ERROR_FILE_NOT_FOUND) {

				//Print Error if neccessary
				printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);
				return false;
			}else{
				printf("ERROR!!! CreateFileA failed with error code: %lu\n", error);
				return false;
			}
		}else{
			//If connected we try to set the comm parameters
			//DCB dcbSerialParams = { 0 };

			//Try to get the current
			if (!GetCommState(this->hSerial, &dcbSerialParams)) {
				//If impossible, show an error
				printf("failed to get current serial parameters!");
				return false;
			}else{
				//DCB Paramaters must be configured be setting the Comm State
				//Set the parameters and check for their proper application
				if (!SetCommState(hSerial, &dcbSerialParams)) {
					printf("ALERT: Could not set Serial Port parameters");
					return false;
				}else{
					//If everything went fine we're connected
					this->connected = true;
					this->_killThreadFlag = false;
					//Flush any remaining characters in the buffers 
					PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
					//We wait as the arduino board will be reseting
					Sleep(MCU_WAIT_TIME);
				}

				//now set the timeouts ( we control the timeout overselves using WaitForXXX()
				COMMTIMEOUTS timeouts;

				timeouts.ReadIntervalTimeout = MAXDWORD;
				timeouts.ReadTotalTimeoutMultiplier = 0;
				timeouts.ReadTotalTimeoutConstant = 0;
				timeouts.WriteTotalTimeoutMultiplier = 0;
				timeouts.WriteTotalTimeoutConstant = 0;

				if (!SetCommTimeouts(hSerial, &timeouts)) {
					printf("ALERT: Could not set Serial Port parameters");
					return false;
				}

			}
		}
	}
	//Simply return the connection status
	return this->connected;
}



bool RS232Comm::disconnect() {

	return this->Disconnect();
}


bool RS232Comm::Disconnect(){
	if (this->connected == true) {
		//ToDo: RS232Comm Object still exists. has not been deconstructed. possible mem leak?
		this->_killThreadFlag = true;
		this->connected = false;
		CloseHandle(this->hSerial);
	}
	return this->connected;
}


bool RS232Comm::IsConnected() {
	//Simply return the connection status
	return this->connected;
}
