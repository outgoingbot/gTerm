#include "serialManager.h"



// Constructor: Initializes and starts the background thread
serialManager::serialManager(){
	//serialComm = comm; //I dont think I need to copy this, its being done in the Initialization List above

	//TODO: this needs to move into Connect button.

	//TODO: I NEED TO REMOVE ALOT OF STUFF FROM CONSTRUCTORS! THERE NEEDS TO BE MORE GETTERS AND SETTERS

#ifdef IS_WINDOWS
	vComPort = new RS232Comm("\\\\.\\COM12"); // Windows serial port
#elif defined(IS_LINUX)
	serialPort = new LinuxSerialComm("/dev/ttyS0"); // Linux serial port
#endif

	//create the nonstop thread to keep reading the seria port
	readThread = new std::thread(&serialManager::readLoop, this); ///retrieve buffer
	running = true;
}

serialManager::~serialManager() {
	stop();
}

// Push new data into the buffer
void serialManager::pushData(const char* data, size_t length) {
	std::lock_guard<std::mutex> lock(bufferMutex);
	bufferQueue.insert(bufferQueue.end(), data, data + length);
}


// Retrieve a specified amount of data from the buffer
std::deque<char> serialManager::getData(size_t length) {
	std::lock_guard<std::mutex> lock(bufferMutex);
	std::deque<char> result;

	if (bufferQueue.size() < length) {
		length = bufferQueue.size();
	}

	result.insert(result.end(), bufferQueue.begin(), bufferQueue.begin() + length);
	bufferQueue.erase(bufferQueue.begin(), bufferQueue.begin() + length);

	return result;
}

// Check if there is data in the buffer
bool serialManager::hasData() {
	std::lock_guard<std::mutex> lock(bufferMutex);
	return !bufferQueue.empty();
}

// Stop the background thread
void serialManager::stop() {
	running = false;
	if (readThread->joinable()) {
		readThread->join();
	}
}

// Thread function that continuously reads data from the serial port
void serialManager::readLoop() {
	while (running) {
		char buffer[512]; // Temporary buffer for incoming data
		int bytesRead = 0;

		vComPort->ReadData(buffer, sizeof(buffer), &bytesRead);

		if (bytesRead > 0) {
			pushData(buffer, bytesRead);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Prevents CPU overuse (getting ~12% CPU)
	}
}


void serialManager::copyToCharArray(char* outBuffer, size_t bufferSize) {
	if (!outBuffer || bufferSize == 0) return; // Handle invalid input

	std::lock_guard<std::mutex> lock(bufferMutex);

	size_t numToCopy = std::min(bufferSize, bufferQueue.size()); // Copy up to bufferSize elements

	for (size_t i = 0; i < numToCopy; ++i) {
		outBuffer[i] = bufferQueue.front(); // Copy from deque to char array
		bufferQueue.pop_front(); // Remove from deque
	}

	if (numToCopy < bufferSize) {
		outBuffer[numToCopy] = '\0'; // Null-terminate to make it a valid C-string (optional)
	}
}




void serialManager::listBaudRates(std::deque<std::string>* queue) {

}


void serialManager::listPorts(std::deque<std::string>* queue) {
	vComPort->ListComPorts(queue);
}


void serialManager::connect(std::string port, std::string baud) {
	//if (!(_rs232comm->IsConnected())) { //prevent trying to connect twice	
	//	//Build Comm port string and char array
	//	std::string str = "\\\\.\\" + std::string(port);
	//	char commPort[32];
	//	strcpy_s(commPort, str.c_str());

	//	//Build Baud Rate DWORD for DCB
	//	DWORD commBaud_DWORD = std::stoul(std::string(baud));

	//	DCB serialParams; //I can add more serial parameters here instead of in the RS232 class
	//	serialParams.BaudRate = commBaud_DWORD;
	//	//serialParams.ByteSize = 8;


	//	//rxBuffer = (char*)calloc(COMM_BUFF_RX_SIZE, sizeof(char));
	//	// 
	////Need to start a thread here
	//	_rs232comm->Connect((const char*)commPort, serialParams);
	//	if (_rs232comm->IsConnected()) { // Opens thread, need to kill in deconstructor
	//		serial_thread = new std::thread(&serial::start_com_thread, this); ///retrieve buffer
	//	}
	//}

}


void serialManager::disconnect() {

}



bool serialManager::isConnected() {

	//return _rs232comm->IsConnected();
	return false;
}


//int serial::popCharFromRXQue(char* key) {
//	if (!_register.rx_char_que.empty()) {
//		char key = _register.rx_char_que.front();
//		_register.rx_char_que.pop_front();
//		return SERIAL_OK;
//	}
//	return SERIAL_ERROR;
//}
//
//
//int serial::pushCharToTXQue(char* key){
//	_register.tx_char_que.push_back(*key);
//	return 0;
//}
//
//
//int serial::pushCharToRXQue(char* key) {
//	_register.rx_char_que.push_back(*key);
//	return 0;
//}
