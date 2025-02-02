#pragma once

//Need to figure out a way to build using the correct serial driver
//I think Rs232 includes <windows>. maybe RS232 is cross platform? that easier to change?

//#ifdef WIN
#include "RS232Comm.h"
//#else
//	#include "Linux_serial"
//#endif

//#include "CircularQueue.h"
#include <deque>
#include <thread>

#define SERIAL_OK 0
#define SERIAL_ERROR -1
#define SERIAL_WARNING -2


class serial {

public:
	typedef struct registers {
		bool _connected = false;
		//The are the queues that are indirectly accessed outside thie class from above.
		std::deque<char> rx_char_que;
		std::deque<char> tx_char_que;
		std::deque<std::string> com_port_names_que;
		std::deque<std::string> com_baud_rate_que;
	};
	


	serial(void);
	~serial();

	void listBaudRates(std::deque<std::string>* queue);
	void listPorts(std::deque<std::string>* queue);

	
	void connect(std::string port, std::string baud);
	void disconnect();
	
	bool isConnected();

	int popCharFromRXQue(char* key);
	int pushCharToRXQue(char* key);

	int pushCharToTXQue(char* key);

	//need threaded function that constantly copies the chars from the RS232 class into the
	//serial class <deque>
	int start_com_thread();

private:
	//these are used just for access. I can access the RS232Comm class char buffer
	char* _rx_char_buff = nullptr;
	char* _tx_char_buff = nullptr;
	unsigned int buff_size = 1024;

	RS232Comm* _rs232comm;
	registers _register;

	std::thread* serial_thread = nullptr;


};