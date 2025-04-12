#pragma once

//Need to figure out a way to build using the correct serial driver
//I think Rs232 includes <windows>. maybe RS232 is cross platform? that easier to change?

#include <deque>
#include <thread>
#include <mutex>
#include <atomic>
#include "virtualComm.h"
#include <algorithm>


//TODO: Should this be moved into the virtualComm class?
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
	//typedef struct registers {
	//	bool _connected = false;
	//	//The are the queues that are indirectly accessed outside thie class from above.
	//	std::deque<char> rx_char_que;
	//	std::deque<char> tx_char_que;
	//	std::deque<std::string> com_port_names_que;
	//	std::deque<std::string> com_baud_rate_que;
	//};
	//


	serialManager();

	~serialManager();

	void pushData(const char* data, size_t length);
	std::deque<char> getData(size_t length);
	bool hasData();
	void stopThread();



	void listBaudRates(std::deque<std::string>* queue);
	void listPorts(std::deque<std::string>* queue);

	void setComPort(std::string* string);
	void settBaudRate(std::string* string);

	std::string getCommPort();
	std::string getCBaudRate();
	
	bool connect();
	bool disconnect();
	
	bool isConnected();

	//int popCharFromRXQue(char* key);
	//int pushCharToRXQue(char* key);

	//int pushCharToTXQue(char* key);

	//void copyToCharArray(char* outBuffer, size_t bufferSize);

	std::deque<std::string> commPortNames;
	std::string selectedPort;

	std::deque<char> rxBufferQueue;

private:
	//these are used just for access. I can access the RS232Comm class char buffer
	//char* _rx_char_buff = nullptr;
	//char* _tx_char_buff = nullptr;
	//unsigned int buff_size = 1024;

	//registers _register;

	//std::thread* serial_thread = nullptr;

	//new Test
	
	std::mutex bufferMutex;
	std::thread* readThread = nullptr;
	std::atomic<bool> running;

	virtualComm* _vComPort; //the virtualComport class is local to serialManager class.

	// Internal function for the background reading thread
	void readLoop();

};