#include "serialManager.h"



// Constructor: Initializes and starts the background thread
serialManager::serialManager(){

	//create the virtual Com Port Class that serialManager will interact with
#ifdef IS_WINDOWS
	_vComPort = new RS232Comm(); // Windows serial port
#elif defined(IS_LINUX)
	_vComPort = new LinuxSerialComm(); // Linux serial port
#endif

	selectedPort = "NONE";
}


serialManager::~serialManager() {
	this->stopThread();
	delete _vComPort;
}


// Check if there is data in the buffer
bool serialManager::hasData() {
	std::lock_guard<std::mutex> lock(bufferMutex);
	return !rxBufferQueue.empty();
}


// Stop the background thread
void serialManager::stopThread() {
	running = false;
	if (readThread->joinable()) {
		readThread->join();
	}
}


// Thread function that continuously reads data from the serial port. the serial manager dequeu<char> is ready
void serialManager::readLoop() {
	//DEBUG_TO_TERMINAL
	while (running) {
		char buffer[512]; // Temporary buffer for RX serial data
		int bytesRead = 0;

		//get the chars from the windows comm handler file. put them in a temportary char buufer.
		_vComPort->ReadData(buffer, sizeof(buffer), &bytesRead);

		//move the char array elements to the deque<char> array.
		if (bytesRead > 0) {
			pushData(buffer, bytesRead);
			#if DEBUG_TO_TERMINAL
				//DEBUG the serial data to the console.
				for (unsigned int i = 0; i < bytesRead; ++i) {
					std::cout << buffer[i];
				}
			#endif			
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Prevents CPU overuse (getting ~12% CPU)
	}
}


//Todo: move this inside of readLoop. no point for this to be its own function
// Push new data into the deque<char> array
void serialManager::pushData(const char* data, size_t length) {
	#define MAX_CHAR_COUNT 1000 //Max Scroll back is 1000 lines
	
	std::lock_guard<std::mutex> lock(bufferMutex);
	this->rxBufferQueue.insert(this->rxBufferQueue.end(), data, data + length);

	const size_t maxSize = 10000;
	if (rxBufferQueue.size() > maxSize) {
		std::cout << "ERROR: MAX_LINE_COUNT Limit" << std::endl;
		size_t toRemove = rxBufferQueue.size() - maxSize;
		rxBufferQueue.erase(rxBufferQueue.begin(), rxBufferQueue.begin() + toRemove);
	}

}



void serialManager::listBaudRates(std::deque<std::string>* queue) {

}


void serialManager::listPorts(std::deque<std::string>* queue) {
	_vComPort->ListComPorts(queue);
}


void serialManager::setComPort(std::string* string) {

	_vComPort->vSerialParams.port = *string;
}

void serialManager::settBaudRate(std::string* string) {
	_vComPort->vSerialParams.baud = *string;

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


bool serialManager::disconnect() {
	this->stopThread();
	return _vComPort->disconnect();
}


bool serialManager::isConnected() {
	return _vComPort->IsConnected();
}
