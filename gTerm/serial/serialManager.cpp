#include "serialManager.h"


// Constructor: Initializes and starts the background thread
serialManager::serialManager(){

	//create the virtual Com Port Class that serialManager will interact with
#ifdef IS_WINDOWS
	_vComPort = new RS232Comm(); // Windows serial port
#elif defined(IS_LINUX)
	_vComPort = new LinuxSerialComm(); // Linux serial port
#endif
	//TODO: This variable should only be used for presentation
	selectedPort = "SELECT_PORT";
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


void serialManager::stopThread() {
	threadIsRunning = false;

	if (readThread) {
		if (std::this_thread::get_id() == readThread->get_id()) {
			std::cerr << "ERROR: Attempted to join thread from itself!" << std::endl;
			return;
		}

		if (readThread->joinable()) {
			readThread->join();
		}
		
		std::cout << "readThread Joined" << std::endl;
		delete readThread;
		readThread = nullptr;
	}
}


// Thread function that continuously reads data from the serial port. the serial manager dequeu<char> is ready
void serialManager::readLoop() {
#define DEBUG_TO_TERMINAL 1
	while (threadIsRunning) {
		char buffer[512];
		int bytesRead = 0;
		//Scrape everything off the top of the kernel serial file
		_vComPort->ReadData(buffer, sizeof(buffer), &bytesRead);
		//Push the char buffer elements onto a rxDeque Object acting as a char queue
		if (bytesRead > 0) {
			pushData(buffer, bytesRead);

		#if DEBUG_TO_TERMINAL
			for (int i = 0; i < bytesRead; ++i)	std::cout << buffer[i];
		#endif
		}
		else if (bytesRead == -1) {
			perror("Serial read failed");
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	std::cout << "Serial thread exiting cleanly.\n";
}


// Push new data into the deque<char> array
void serialManager::pushData(const char* data, size_t length) {
	#define MAX_CHAR_COUNT 10000 //Max number if chars in the deque object
	
	std::lock_guard<std::mutex> lock(bufferMutex);
	this->rxBufferQueue.insert(this->rxBufferQueue.end(), data, data + length);

	const size_t maxSize = MAX_CHAR_COUNT;
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
		threadIsRunning = true;
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
