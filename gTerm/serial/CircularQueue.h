#pragma once
#include "CircularQueue.h"
#include <stdlib.h>
#include <string.h>

class CircularQueue {

public:
	CircularQueue(unsigned int size); // Constructor
	~CircularQueue();

	bool push(char key); // Pushes a byte into the buffer
	char pop(); // Uses a byte from the buffer (unsigned char because 1 byte)
	int peek(char* argBuffer, int sz); // Fills an arg buffer with contents of queue without popping, returns len
	int zeroBuffer(); //memset the buffer to all zeros

	unsigned short qSize(); // returns the size of the queue
	unsigned short qSizeUsed(); // returns the number of elements in the queue
	unsigned short qSizeAvailable(); // returns the number of free space in the queue
	char* buffer; // This will hold however many bytes the macro defines
private:
	
	unsigned short queueSize = 0; //number of elements in the queue
	unsigned short bufferSize;
	int short head = 0;
	int short tail = 0; // Indexes of head and tail respectively. Used to keep track of where in queue the program is.

};

