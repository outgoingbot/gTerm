#include "serialManager.h"

// Constructor: Initializes and starts the background thread
serialManager::serialManager(){
	deubug_kernel_num_chars_copied = 0;
	//create the virtual Com Port Class that serialManager will interact with
#ifdef IS_WINDOWS
	_vComPort = new RS232Comm(); // Windows serial port
#elif defined(IS_LINUX)
	_vComPort = new LinuxSerialComm(); // Linux serial port
#endif

	//TODO: used for debug during deveopment Hard COded
#if DEBUG_COMM_DEFAULTS
	setCommPort(R"(\\.\COM11)"); //R() is raw string
	setBaudRate("115200");
#endif
}


serialManager::~serialManager() {
	this->stopThread();
	delete _vComPort;
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

void serialManager::listBaudRates(std::deque<std::string>* queue) {
	_vComPort->ListBaudRates(queue);
}


void serialManager::listPorts(std::deque<std::string>* queue) {
	_vComPort->ListComPorts(queue);
}


void serialManager::setCommPort(const std::string& port) {
	if (_vComPort) {
		_vComPort->vSerialParams.port = port;
	}
}

void serialManager::setBaudRate(const std::string& baud) {
	if (_vComPort) {
		_vComPort->vSerialParams.baud = baud;
	}
}

std::string serialManager::getCommPort() const {
	return _vComPort ? _vComPort->vSerialParams.port : "";
}


std::string serialManager::getBaudRate() const {
	return _vComPort ? _vComPort->vSerialParams.baud : "";
}



// Thread function that continuously reads data from the serial port.
void serialManager::readLoop() {
	while (threadIsRunning) {
		//copy the kernel comm buffer into this buffer
		char buffer[8196]; //ideally this would be small if our thread is fast, but that uses lotsa cpu :(
		int bytesRead = 0;
		//Scrape everything off the top of the kernel serial file.
		_vComPort->ReadData(buffer, sizeof(buffer), &bytesRead);
		if (bytesRead > 0) {
			//push the kernel char buffer into the serialManager Dequeue<std::char>
			pushData(buffer, bytesRead); //Serial manager method to copy the local buffer into the deque
			//DEBUG Keep track of the char copied from the kernel driver
			deubug_kernel_num_chars_copied = bytesRead;
#if DEBUG_TO_TERMINAL
			//std::cout is slowwwww
			for (int i = 0; i < bytesRead; ++i)	std::cout << buffer[i]; //also print serial data to open debug cmd console
#endif
		}
		else if (bytesRead == -1) {
			perror("Serial read failed");
			//TODO: Do something here to handle error?
		}
	}

	std::cout << "Serial thread exiting cleanly.\n";
}



// Push new char data into the deque<char> array. we grabbed it from the kernal buffer and shove it into our deque<char> vector
void serialManager::pushData(const char* data, size_t length) {
	//here is where we pump the char array into the deque<char> at the end()
	std::lock_guard<std::mutex> lock(bufferMutex);
	this->rxBufferQueue.insert(this->rxBufferQueue.end(), data, data + length);

	const size_t maxSize = MAX_CHAR_COUNT;
	if (rxBufferQueue.size() > maxSize) {
		//std::cout << "ERROR: MAX_LINE_COUNT Limit" << std::endl;
		size_t toRemove = rxBufferQueue.size() - maxSize;
		rxBufferQueue.erase(rxBufferQueue.begin(), rxBufferQueue.begin() + toRemove);
	}
}



size_t serialManager::getNewData(std::deque<char>& destination){
	std::lock_guard<std::mutex> lock(bufferMutex);

	size_t charsAdded = 0;

	if (!rxBufferQueue.empty()){
		//get the size of the internal queue
		charsAdded = rxBufferQueue.size();
		// Append only the new data
		destination.insert(destination.end(),rxBufferQueue.begin(),rxBufferQueue.end());
		// Clear internal queue
		rxBufferQueue.clear();
	}

	// Enforce maximum size on the PUBLIC queue
	const size_t maxSize = MAX_CHAR_COUNT;
	if (destination.size() > maxSize){
		size_t toRemove = destination.size() - maxSize;
		destination.erase(destination.begin(), destination.begin() + toRemove);
	}
	return charsAdded;
}



void serialManager::stopThread() {
	threadIsRunning = false;
	deubug_kernel_num_chars_copied = 0;
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



void serialManager::debug_getKernelcharCount(size_t* len) {
	std::lock_guard<std::mutex> lock(bufferMutex);
	*len = deubug_kernel_num_chars_copied;  // Assignment operator does a full copy
}

