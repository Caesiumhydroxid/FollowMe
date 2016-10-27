#include"stdafx.h"
#include<serial\serial.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "SerialThread.h"

SerialThread::SerialThread(std::string description)
{
	assignRightArduinoPort(description);
	serDevice.setBaudrate(115200);
	serDevice.setFlowcontrol(serial::flowcontrol_none);
	serDevice.setStopbits(serial::stopbits_one);
	//serDevice.open();
}


void SerialThread::operator()()
{
	while (running)
	{
		if (!queue.empty())
		{
			serDevice.write(queue);
		}
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(10ms);
	}
}

void SerialThread::assignRightArduinoPort(std::string description)
{
	auto ports = serial::list_ports();
	for (auto iter = ports.begin(); iter != ports.end(); iter++)
	{
		if (iter->description.find(description) != std::string::npos)
		{
			this->serDevice.setPort(iter->port);
			break;
		}
	}
}

void SerialThread::sendInQueue(uint8_t x)
{
	std::lock_guard<std::mutex> lock(this->queueMutex);
	this->queue.push_back(x);
}

void SerialThread::sendInQueue(const uint8_t* data, size_t size)
{
	std::lock_guard<std::mutex> lock(this->queueMutex);
	for (int i = 0; i < size; i++)
	{
		this->queue.push_back(data[i]);
	}
}
