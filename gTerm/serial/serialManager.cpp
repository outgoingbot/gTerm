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
	//TODO: This variable should only be used for presentation
	selectedPort = "SELECT_PORT";
	selectedBaud = "SELECT_BAUD";

	//TODO: used for debug during deveopment Hard COded
#if DEBUG_COMM_DEFAULTS
	selectedPort = "\\\\.\\COM11";
	selectedBaud = "115200";
#endif
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
#define SLEEP_THREAD_MS 5
#define DEBUG_TO_TERMINAL 1
	while (threadIsRunning) {
		//we should receive 1000 bytes in 5ms at 2,000,000 bps baud. keep this in mid when selecting a sleep time for the thread
		char buffer[8196]; //copy the kernel comm buffer into this buffer
		int bytesRead = 0;
		//Scrape everything off the top of the kernel serial file. this is our usable rx buff in SLEEP_THREAD_MS ms intervals
		//think of this as collecting serial data from a data register
		_vComPort->ReadData(buffer, sizeof(buffer), &bytesRead);

		//Push the char buffer elements onto a rxDeque Object acting as a char queue
		if (bytesRead > 0) {
			//DEBUG Keep track of the char copied from the kernel driver
			deubug_kernel_num_chars_copied = bytesRead;
			//push the kernel char buffer into the serialManager Dequeue<std::char>
			pushData(buffer, bytesRead); //Serial manager method to copy the local buffer into the deque

		#if DEBUG_TO_TERMINAL
			for (int i = 0; i < bytesRead; ++i)	std::cout << buffer[i]; //also print serial data to open debug cmd console
		#endif
		}
		else if (bytesRead == -1) {
			perror("Serial read failed");
		}

		//slow down the thread to reduce cpu usage. may be a better way to do this, base sleep time on baud rate.
		// Time = Total Bits / Baud Rate
		// Total Bits = (Start Bit + Stop Bit + 8 Bits in a byte) * number of bytes //for 8N1
		//for example if baud is 115200 then we can expect a byte at 1/(115200/10) (1 start + 8 + 1 stop bit [8N1]) = 86.8 uS / byte
		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_THREAD_MS));
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


//Copies the deque<char> to another. I want to keep the mutex only in serialManager
//This is required to keep the rxBufferQueue thread safe!
void serialManager::copyData(std::deque<char>* rxBufferQueue_public) {
    std::lock_guard<std::mutex> lock(bufferMutex);
    *rxBufferQueue_public = rxBufferQueue;  // Assignment operator does a full copy
}

void serialManager::debug_getKernelcharCount(size_t* len) {
	std::lock_guard<std::mutex> lock(bufferMutex);
	*len = deubug_kernel_num_chars_copied;  // Assignment operator does a full copy
}



void serialManager::listBaudRates(std::deque<std::string>* queue) {
	_vComPort->ListBaudRates(queue);
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


std::string serialManager::getBaudRate() {
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
