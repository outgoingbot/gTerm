#pragma once

//Need to figure out a way to build using the correct serial driver
//I think Rs232 includes <windows>. maybe RS232 is cross platform? that easier to change?

#include <deque>
#include <thread>
#include <mutex>
#include <atomic>
#include "virtualComm.h"
#include <algorithm>

//Choose what Serial Driver Header to #include. ifdef Variables defined in CMakeLists
#ifdef IS_WINDOWS
	#include "RS232Comm.h"
#elif defined(IS_LINUX)
	#include "LinuxSerialComm.h"
#endif


#define SERIAL_OK 0
#define SERIAL_ERROR -1
#define SERIAL_WARNING -2


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
	std::string selectedPort;



	serialManager();
	~serialManager();

	void listBaudRates(std::deque<std::string>* queue);
	void listPorts(std::deque<std::string>* queue);

	void setComPort(std::string* string);
	void settBaudRate(std::string* string);

	std::string getCommPort();
	std::string getCBaudRate();
	
	bool connect();
	bool disconnect();
	
	bool hasData();
	void pushData(const char* data, size_t length);

	void copyData(std::deque<char> *rxBufferQueue_public);

	bool isConnected();

	void stopThread();

private:
	std::deque<char> rxBufferQueue;
	//std::deque<char> txBufferQueue;

	std::mutex bufferMutex;
	std::thread* readThread = nullptr;
	std::atomic<bool> threadIsRunning;

	virtualComm* _vComPort; //the virtualComport class is local to serialManager class.

	// Internal function for the background reading thread
	void readLoop();
};