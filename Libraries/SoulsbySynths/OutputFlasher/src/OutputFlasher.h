#pragma once
#include <Arduino.h>

class OutputFlasher 
{
public:
	void initialise(uint8_t outputPin, uint16_t flashTicks);
	void poll();
private:
	uint8_t outputState_ = LOW;
	uint8_t outputPin_ = 0;
	uint16_t flashTicks_ = 1000;
	uint32_t lastTick_ = 0;
};