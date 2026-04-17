#pragma once

#include <deque>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <queue>
#include "virtualComm.h"

//Choose what Serial Driver Header to #include. ifdef Variables defined in CMakeLists
#ifdef IS_WINDOWS
#include "RS232Comm.h"
#elif defined(IS_LINUX)
#include "LinuxSerialComm.h"
#endif

#define MAX_CHAR_COUNT 80000 //Max number if chars in the deque object

//DEBUG to prevent me having to select the port and baud during development
//Set to zero to remove default Comm port and baud settings
#define DEBUG_COMM_DEFAULTS 1
#define DEBUG_TO_TERMINAL 0

#define SERIAL_OK 0
#define SERIAL_ERROR -1
#define SERIAL_WARNING -2

class serialManager {

public:
	serialManager();
	~serialManager();

	//TODO: Should use this struct for the public members
	//typedef struct registers {
	//	bool _connected = false;
	//	//The are the queues that are indirectly accessed outside thie class from above.
	//	std::deque<char> rx_char_que;
	//	std::deque<char> tx_char_que;
	//	std::deque<std::string> com_port_names_que;
	//	std::deque<std::string> com_baud_rate_que;
	//};

	bool connect();
	bool disconnect();

	bool isConnected();

	void listPorts(std::deque<std::string>* queue);
	void listBaudRates(std::deque<std::string>* queue);

	void setCommPort(const std::string& port);
	std::string getCommPort() const;
	
	void setBaudRate(const std::string& baud);
	std::string getBaudRate() const;
	
	std::deque<std::string> commPortNames;
	std::deque<std::string> commBaudNames;


	void pushToRxQueue(const char* data, size_t length);
	size_t getNewDataFromRxQueue(std::deque<char>& destination);

	//setTxBuffer queue
	void pushToTxQueue(const std::string& data);
	
	void stopThread();

	void debug_getKernelcharCount(size_t* len);

private:
	
	virtualComm* _vComPort; //the virtualComport class is local to serialManager class.

	std::thread* serial_IO_Thread = nullptr;
	std::atomic<bool> threadIsRunning;

	std::deque<char> rxQueue; //this is the master rx buffer to be used by all other classes
	std::mutex rxMutex;

	std::queue<std::string> txQueue;// tx serial data queue
	std::mutex txMutex;

	void serial_IO_Loop();

	size_t deubug_kernel_num_chars_copied;	
};