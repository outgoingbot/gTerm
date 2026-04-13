#pragma once

#include <deque>
#include <thread>
#include <mutex>
#include <atomic>
#include "virtualComm.h"
#include <algorithm>

//DEBUG to prevent me having to select the port and baud during development
//Set to zero to remove default Comm port and baud settings
#define DEBUG_COMM_DEFAULTS 1

#define DEBUG_TO_TERMINAL 0


//Choose what Serial Driver Header to #include. ifdef Variables defined in CMakeLists
#ifdef IS_WINDOWS
	#include "RS232Comm.h"
#elif defined(IS_LINUX)
	#include "LinuxSerialComm.h"
#endif


#define SERIAL_OK 0
#define SERIAL_ERROR -1
#define SERIAL_WARNING -2

#define MAX_CHAR_COUNT 40000 //Max number if chars in the deque object

class serialManager {

public:

	//TODO: Should use this struct for the public members
	//typedef struct registers {
	//	bool _connected = false;
	//	//The are the queues that are indirectly accessed outside thie class from above.
	//	std::deque<char> rx_char_que;
	//	std::deque<char> tx_char_que;
	//	std::deque<std::string> com_port_names_que;
	//	std::deque<std::string> com_baud_rate_que;
	//};

	std::deque<std::string> commPortNames;
	std::deque<std::string> commBaudNames;

	serialManager();
	~serialManager();

	void listBaudRates(std::deque<std::string>* queue);
	void listPorts(std::deque<std::string>* queue);

	std::string getCommPort() const;
	void setCommPort(const std::string& port);
	std::string getBaudRate() const;
	void setBaudRate(const std::string& baud);
	
	
	bool connect();
	bool disconnect();
	
	void pushData(const char* data, size_t length);

	size_t getNewData(std::deque<char>& destination);

	void debug_getKernelcharCount(size_t* len);

	bool isConnected();

	void stopThread();

private:
	size_t deubug_kernel_num_chars_copied;
	std::deque<char> rxBufferQueue; //this is the master rx buffer to be used by all other classes
	//std::deque<char> txBufferQueue;

	std::mutex bufferMutex;
	std::thread* readThread = nullptr;
	std::atomic<bool> threadIsRunning;

	virtualComm* _vComPort; //the virtualComport class is local to serialManager class.

	// Internal function for the background reading thread
	void readLoop();
};