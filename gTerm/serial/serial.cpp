#include "serial.h"



serial::serial() {
	_rs232comm = new RS232Comm(_rx_char_buff, _tx_char_buff, buff_size);
}


serial::~serial() {

}


void serial::listBaudRates(std::deque<std::string>* queue) {

}


void serial::listPorts(std::deque<std::string>* queue) {

}


void serial::connect(std::string port, std::string baud) {
	if (!(_rs232comm->IsConnected())) { //prevent trying to connect twice	
		//Build Comm port string and char array
		std::string str = "\\\\.\\" + std::string(port);
		char commPort[32];
		strcpy_s(commPort, str.c_str());

		//Build Baud Rate DWORD for DCB
		DWORD commBaud_DWORD = std::stoul(std::string(baud));

		DCB serialParams; //I can add more serial parameters here instead of in the RS232 class
		serialParams.BaudRate = commBaud_DWORD;
		//serialParams.ByteSize = 8;


		//rxBuffer = (char*)calloc(COMM_BUFF_RX_SIZE, sizeof(char));
		// 
	//Need to start a thread here
		_rs232comm->Connect((const char*)commPort, serialParams);
		if (_rs232comm->IsConnected()) { // Opens thread, need to kill in deconstructor
			serial_thread = new std::thread(&serial::start_com_thread, this); ///retrieve buffer
		}
	}

}


void serial::disconnect() {

}



bool serial::isConnected() {

	return _rs232comm->IsConnected();
}


int serial::popCharFromRXQue(char* key) {
	if (!_register.rx_char_que.empty()) {
		char key = _register.rx_char_que.front();
		_register.rx_char_que.pop_front();
		return SERIAL_OK;
	}
	return SERIAL_ERROR;
}


int serial::pushCharToTXQue(char* key){
	_register.tx_char_que.push_back(*key);
	return 0;
}


int serial::pushCharToRXQue(char* key) {
	_register.rx_char_que.push_back(*key);
	return 0;
}




int serial::start_com_thread() {
	static unsigned int bytes_to_read = 512;
	static int retVal = 0;

	_rs232comm->ReadData(_rx_char_buff, bytes_to_read, &retVal);
	for (int cnt = 0; cnt < retVal; cnt++) {
		pushCharToRXQue((char*)(_rx_char_buff + cnt));
	}
	
	return 0;
}