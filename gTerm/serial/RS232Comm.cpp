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
	vSerialParams.baud = "115200";
	vSerialParams.byteSize = 8;
}


RS232Comm::~RS232Comm() {
	if (this->connected) {
		this->connected = false;
		if (hSerial != INVALID_HANDLE_VALUE) { //Close the serial handler
			CloseHandle(hSerial);
		}
	}
	LOG_INFO("RS232Comm deleted");
}


bool RS232Comm::connect() {
	//set serialParams
	char portName[64];
	vSerialParams.port.copy(portName, vSerialParams.port.size());
	portName[vSerialParams.port.size()] = '\0';  // null-terminate

	//Copy the serialParams struct members into the DCB struct members
	dcbSerialParams.BaudRate = (DWORD)std::stoul(vSerialParams.baud);
	dcbSerialParams.ByteSize = (BYTE)vSerialParams.byteSize;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE; //DTR to pulse reset on common MCU Boards

	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
	timeouts.ReadTotalTimeoutConstant = 100;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;

	//open the Comm Port using the dcbSerialParams
	return this->openPort(portName, timeouts, dcbSerialParams);
}



bool RS232Comm::openPort(const char* portName, COMMTIMEOUTS timeouts, DCB dcbSerialParams) {
	if (this->connected) {
		return true; // Already connected
	}
	
	this->hSerial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (this->hSerial == INVALID_HANDLE_VALUE) {
		LOG_ERROR_WIN32("RS232comm CreateFileA failed on port " << portName);
		return false;
	}

	// Get current serial port settings
	DCB currentParams = { 0 };
	if (!GetCommState(this->hSerial, &currentParams)) {
		LOG_ERROR_WIN32("RS232comm Failed to get current serial parameters");
		CloseHandle(this->hSerial);
		this->hSerial = INVALID_HANDLE_VALUE;
		return false;
	}

	// Apply the new settings from the caller
	if (!SetCommState(this->hSerial, &dcbSerialParams)) {
		LOG_ERROR_WIN32("RS232comm Could not set Serial Port parameters");
		CloseHandle(this->hSerial);
		this->hSerial = INVALID_HANDLE_VALUE;
		return false;
	}

	// Successfully connected
	this->connected = true;

	// Clear buffers
	PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

	// Wait for MCU reset
	Sleep(MCU_WAIT_TIME);

	//TODO: Move timouts into connect and pass into openPort
	// Set timeouts
	//COMMTIMEOUTS timeouts;
	//timeouts.ReadIntervalTimeout = MAXDWORD;
	//timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
	//timeouts.ReadTotalTimeoutConstant = 100;
	//timeouts.WriteTotalTimeoutMultiplier = 0;
	//timeouts.WriteTotalTimeoutConstant = 0;

	if (!SetCommTimeouts(this->hSerial, &timeouts)) {
		LOG_ERROR_WIN32("RS232comm Could not set Serial Port timeouts");
		CloseHandle(this->hSerial);
		this->hSerial = INVALID_HANDLE_VALUE;
		this->connected = false;
		return false;
	}

	return true;
}


bool RS232Comm::disconnect() {
	return this->closePort();
}


bool RS232Comm::closePort() {
	if (this->connected == true) {
		this->connected = false;
		CloseHandle(this->hSerial);
	}
	return this->connected;
}


//Simply return the connection status
bool RS232Comm::IsConnected() {
	return this->connected;
}


bool RS232Comm::ListComPorts(std::deque<std::string>* ComPortNames) { //added function to find the present serial 
	char lpTargetPath[5000]; // buffer to store the path of the COMPORTS
	bool gotPort = false; // in case the port is not found
	LOG_INFO("gTerm RS232Comm Building List Ports");
	for (int i = 0; i < 255; i++) { // checking ports from COM0 to COM255
		std::string str = "COM" + std::to_string(i); // converting to COM0, COM1, COM2
		SetLastError(0); // Clear any previous error before calling QueryDosDevice
		DWORD test = QueryDosDevice(str.c_str(), lpTargetPath, 5000);

		// Test the return value and error if any
		if (test != 0) { //QueryDosDevice returns zero if it didn't find an object
			std::string windowsFormatComPort = "\\\\.\\" + str;
			ComPortNames->push_back(windowsFormatComPort);
			LOG_INFO(str << ": " << lpTargetPath);
			gotPort = true;
		} else {
			// QueryDosDevice failed - check why
			DWORD error = GetLastError();

			if (error == ERROR_INSUFFICIENT_BUFFER) {
				LOG_WARN("RS232Comm QueryDosDevice buffer too small for COM" << i);
			}
			else if (error != ERROR_FILE_NOT_FOUND && error != ERROR_SUCCESS) {
				// Only log unexpected errors (most COM ports simply don't exist -> ERROR_FILE_NOT_FOUND)
				LOG_ERROR_WIN32("RS232Comm QueryDosDevice failed for " << str);
			}
			// else: normal case - port does not exist, do nothing
		}
	}
	return gotPort;
}


//trys to read the requested number of bytes (chars) into char* buffer. Returns number of bytes actually read.
void RS232Comm::ReadData(char* buffer, unsigned int nbChar, int* returnVal) {
	if (!buffer || !returnVal) return;
	if (!nbChar) return;

	*returnVal = 0;
	DWORD bytesRead = 0;
	//Let the Timeouts be blocking here. The timeouts set on openPort prevent the serial_IO_Thread from using 100% CPU!
	if (ReadFile(hSerial, buffer, nbChar, &bytesRead, NULL)) {
		*returnVal = static_cast<int>(bytesRead);
	}
	else {
		*returnVal = -1; // Indicate failure
	}
}


bool RS232Comm::WriteData(const char* buffer, unsigned int nbChar) {
	DWORD bytesSend;
	//Try to write the buffer on the Serial port
	if (!WriteFile(this->hSerial, (void*)buffer, nbChar, &bytesSend, 0)) {
		//In case it don't work get comm error and return false
		ClearCommError(this->hSerial, &this->errors, &this->status);
		return false;
	}
	else {
		return true;
	}
}
