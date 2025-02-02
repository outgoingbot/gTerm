/*
 *  Created on: Nov 8, 2022
 *      Author: MZ
 */

#include <CircularQueue.h>

CircularQueue::CircularQueue(unsigned int size) {
	// Init
	buffer = (char *)calloc(size, sizeof(char));
	bufferSize = size;
	head = 0;
	tail = 0; // set head and tail elements
	queueSize = 0;

}

CircularQueue::~CircularQueue() {
	free(buffer);
}

bool CircularQueue::push(char key) {
	// Inserts an element at the end of the buffer.
	// The tail index is increased. If the tail index exceeds
	// the allocated buffer, it loops back to 0, maintaining
	// the circular queue property.

	if (queueSize >= bufferSize) {
		return false; // Prevent buffer from overflowing
	}

	buffer[tail] = key; // inserts the byte at the end of the buffer
	tail++; // increase the tail

	if (tail >= bufferSize) { // Reset tail if bufferSize
		tail = 0;
	}
	queueSize++;
	if (queueSize >= bufferSize) queueSize = bufferSize; // Prevents bufferSize from getting stuck at a higher value than the buffer size
	return true;

}

char CircularQueue::pop() {
	// Removes the element from the head of the queue
	// and returns it. The head index is then increased.
	// If the head index exceeds BUFF_SIZE, it is looped
	// around back to 0.
	if (queueSize > 0) {
		queueSize--;
		char key = buffer[head]; // puts the element at head of array into key
		head++;
		if (head >= bufferSize) head = 0;

		return key;
	}
	else {
		return 0; // If the queue is empty, return 0.
	}

}


int CircularQueue::peek(char* argBuffer, int sz) {
	//  "pops" a string into the buffer without removing it from the queue
	if (sz > queueSize) sz = queueSize;
	for (int i = 0; i < sz; i++) {
		if (head + i < bufferSize) argBuffer[i] = buffer[head + i];
		else argBuffer[i] = buffer[i];
	}
	return queueSize; // Return data popped
}


unsigned short CircularQueue::qSize() {
	return bufferSize;
}

unsigned short CircularQueue::qSizeUsed() {
	return queueSize;
}

unsigned short CircularQueue::qSizeAvailable() {
	return bufferSize - queueSize;
}

int CircularQueue::zeroBuffer() {
	memset(buffer, 0, bufferSize);
	head = 0; //reset the queue params
	tail = 0;
	queueSize = 0;
	return 0;
}

