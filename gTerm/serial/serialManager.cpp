#include "serialManager.h"



// Constructor: Initializes and starts the background thread
serialManager::serialManager(){

	//create the virtual Com Port Class that serialManager will interact with
#ifdef IS_WINDOWS
	_vComPort = new RS232Comm(); // Windows serial port
#elif defined(IS_LINUX)
	_vComPort = new LinuxSerialComm("/dev/ttyS0"); // Linux serial port
#endif

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

		_vComPort->ReadData(buffer, sizeof(buffer), &bytesRead);

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
	_vComPort->ListComPorts(queue);
}


void serialManager::setComPort(std::string* string) {


}

void serialManager::settBaudRate(std::string* string) {

}

std::string serialManager::getCommPort() {
	return _vComPort->vSerialParams.port; //this in turn could be a getter
}


std::string serialManager::getCBaudRate() {
	return _vComPort->vSerialParams.baud; //this in turn could be a getter function
}


bool serialManager::connect() {
	//Start the RS232 or Linux Serial port (hosted by vComPort virtual class)
	if (_vComPort->connect()) {
		//create the nonstop thread to keep reading the seria port if connect retruns true
		readThread = new std::thread(&serialManager::readLoop, this); ///retrieve buffer
		running = true;
		return true;
	}
	else {
		return false;
	}
}


void serialManager::disconnect() {

}



bool serialManager::isConnected() {

	///TODO: return a proper value here

	//return _rs232comm->IsConnected();
	return _vComPort->IsConnected();
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
