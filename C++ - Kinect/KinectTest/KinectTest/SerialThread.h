#pragma once
#include<serial\serial.h>
#include<vector>
#include<mutex>
class SerialThread
{
	serial::Serial serDevice;

	void assignRightArduinoPort(std::string description);
	std::vector<uint8_t> queue;
	std::mutex queueMutex;
public:
	void operator()();
	bool running = true;
	SerialThread(std::string description);
	void sendInQueue(uint8_t x);
	void sendInQueue(const uint8_t* x,size_t size);
};