#include "AtxC.h"
// default constructor
AtxC::AtxC()
	: hardware_(AtxCHardware::getInstance())
{
	hardware_.construct(this);
} //AtxC

// default destructor
AtxC::~AtxC()
{
} //~AtxC

void AtxC::initialize()
{
	hardware_.initialize();
}

void AtxC::pollFast()
{
	
}

void AtxC::poll(uint8_t ticksPassed)
{
	hardware_.dummy();
}

void AtxC::printFreeMem()
{
	
}