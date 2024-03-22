#include "OutputFlasher.h"


void OutputFlasher::initialise(uint8_t outputPin, uint16_t flashTicks)
{
	outputPin_ = outputPin;
	flashTicks_ = flashTicks;
	pinMode(outputPin, OUTPUT);
	digitalWrite(outputPin_, outputState_);
}

void OutputFlasher::poll()
{
	uint32_t tick = millis();
	if (tick - lastTick_ >= flashTicks_)
	{
		lastTick_ += flashTicks_;
		outputState_ = 1 - outputState_;
		digitalWrite(outputPin_, outputState_);
	}
}
