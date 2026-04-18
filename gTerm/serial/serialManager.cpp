#include "serialManager.h"

serialManager::serialManager(){
	//create the virtual Com Port Class that serialManager will interact with
#ifdef IS_WINDOWS
	_vComPort = new RS232Comm(); // Windows serial port
#elif defined(IS_LINUX)
	_vComPort = new LinuxSerialComm(); // Linux serial port
#endif

	//TODO: used for debug during deveopment Hard COded
#if DEBUG_COMM_DEFAULTS
	setCommPort(R"(\\.\COM12)"); //R() is raw string
	setBaudRate("115200");
#endif

	deubug_kernel_num_chars_copied = 0;
}


serialManager::~serialManager() {
	this->stopThread();
	delete _vComPort;
}


bool serialManager::connect() {
	//Start the RS232 or Linux Serial port (hosted by vComPort virtual class)
	if (_vComPort->connect()) {
		//create the nonstop thread to keep reading the seria port if connect retruns true
		LOG_INFO("gTerm serialManager Comm : " << getCommPort());
		LOG_INFO("gTerm serialManager Baud : " << getBaudRate());
		LOG_SUCCESS("gTerm seriManager Thread Started");
		serial_IO_Thread = new std::thread(&serialManager::serial_IO_Loop, this); ///retrieve buffer
		threadIsRunning = true;
		return true;
	}
	else {
		LOG_ERROR("gTerm seriManager Thread Failed To Start");
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


void serialManager::listPorts(std::deque<std::string>* queue) {
	_vComPort->ListComPorts(queue);
}


void serialManager::listBaudRates(std::deque<std::string>* queue) {
	_vComPort->ListBaudRates(queue);
}


void serialManager::setCommPort(const std::string& port) {
	if (_vComPort) {
		_vComPort->vSerialParams.port = port;
	}
}


std::string serialManager::getCommPort() const {
	return _vComPort ? _vComPort->vSerialParams.port : "";
}


void serialManager::setBaudRate(const std::string& baud) {
	if (_vComPort) {
		_vComPort->vSerialParams.baud = baud;
	}
}


std::string serialManager::getBaudRate() const {
	return _vComPort ? _vComPort->vSerialParams.baud : "";
}


// Push new char data into the deque<char> array. we grabbed it from the kernal buffer and shove it into our deque<char> vector
void serialManager::pushToRxQueue(const char* data, size_t length) {
	//here is where we pump the char array into the deque<char> at the end()
	std::lock_guard<std::mutex> lock(rxMutex);
	this->rxQueue.insert(this->rxQueue.end(), data, data + length);

	const size_t maxSize = MAX_CHAR_COUNT;
	if (rxQueue.size() > maxSize) {
		size_t toRemove = rxQueue.size() - maxSize;
		rxQueue.erase(rxQueue.begin(), rxQueue.begin() + toRemove);
	}
}


size_t serialManager::getNewDataFromRxQueue(std::deque<char>& destination) {
	std::lock_guard<std::mutex> lock(rxMutex);

	size_t charsAdded = 0;

	if (!rxQueue.empty()) {
		//get the size of the internal queue
		charsAdded = rxQueue.size();
		// Append only the new data
		destination.insert(destination.end(), rxQueue.begin(), rxQueue.end());
		// Clear internal queue
		rxQueue.clear();
	}

	// Enforce maximum size on the PUBLIC queue
	const size_t maxSize = MAX_CHAR_COUNT;
	if (destination.size() > maxSize) {
		size_t toRemove = destination.size() - maxSize;
		destination.erase(destination.begin(), destination.begin() + toRemove);
	}
	return charsAdded;
}


void serialManager::pushToTxQueue(const std::string& data) {
	if (data.empty()) return;
	std::lock_guard<std::mutex> lock(txMutex);
	txQueue.push(data);
}


#if 1
// Thread function that continuously reads data from the serial port.
void serialManager::serial_IO_Loop() {
	while (threadIsRunning) {
		// drain and send queued data
		std::string toSend;
		{ //why is there a curly brace here?
			std::lock_guard<std::mutex> lock(txMutex);
			if (!txQueue.empty()) {
				toSend = std::move(txQueue.front());
				txQueue.pop();
			}
		}
		//Consider using "chunking here to prevent very large transmits from blocking the ReadData() for too long"
		if (!toSend.empty()) {
			_vComPort->WriteData(toSend.c_str(), static_cast<unsigned int>(toSend.size()));
		}

		//copy the kernel comm buffer into this buffer
		char buffer[8196]; //ideally this would be small if our thread is fast, but that uses lotsa cpu :(
		int bytesRead = 0;
		//Scrape everything off the top of the kernel serial file.
		_vComPort->ReadData(buffer, sizeof(buffer), &bytesRead);
		if (bytesRead > 0) {
			//push the kernel char buffer into the serialManager Dequeue<std::char>
			pushToRxQueue(buffer, bytesRead); //Serial manager method to copy the local buffer into the deque
			//DEBUG Keep track of the char copied from the kernel driver
			deubug_kernel_num_chars_copied = bytesRead;
#if DEBUG_TO_TERMINAL
			//std::cout is slowwwww
			for (int i = 0; i < bytesRead; ++i)	std::cout << buffer[i]; //also print serial data to open debug cmd console
#endif
		}
		else if (bytesRead == -1) {
			LOG_ERROR_ERRNO("Serial read failed");
			//TODO: Do something here to handle error?
		}
	}
	LOG_INFO("gTerm serialManager serial_IO_Thread Thread Exiting Cleanly");
}
#else
//may test txQueue "Chunking here"

#endif




void serialManager::stopThread() {
	threadIsRunning = false;
	deubug_kernel_num_chars_copied = 0;
	if (serial_IO_Thread) {
		if (std::this_thread::get_id() == serial_IO_Thread->get_id()) {
			LOG_ERROR("gTerm serialManager Attempted to join thread from itself!");
			return;
		}

		if (serial_IO_Thread->joinable()) {
			serial_IO_Thread->join();
		}
		LOG_SUCCESS("gTerm seriManager serial_IO_Thread Joined");
		delete serial_IO_Thread;
		serial_IO_Thread = nullptr;
	}
}



void serialManager::debug_getKernelcharCount(size_t* len) {
	std::lock_guard<std::mutex> lock(rxMutex);
	*len = deubug_kernel_num_chars_copied;  // Assignment operator does a full copy
}

