/*
* AtxCHardware.cpp
*
* Created: 11/01/2018 01:07:01
* Author: info
*/

#include "AtxCHardware.h"
// default constructor
AtxCHardware::AtxCHardware(AtxCHardwareBase* base)
{
	//midiUsbHost = new USBH_MIDI (&usbH);
	construct(base);
} //AtxCHardware

// default destructor
AtxCHardware::~AtxCHardware()
{
	if (base_ != NULL)
	{
		delete base_;
	}
} //~AtxCHardware


void AtxCHardware::construct(AtxCHardwareBase* base)
{
	base_ = base;
	
	for (uint8_t i = 0; i < EXPANDERS; ++i)
	{
		expander_[i] = new MCP23S17(GPIO(GPIO_PORTA,20), i); 
	}
	
}

uint8_t AtxCHardware::initialize()
{
	using namespace atx;
	
	for (uint8_t i = 0; i < EXPANDERS; ++i)
	{
		expander_[i]->begin();
	}
	
	for (uint8_t i = 0; i < 3; ++i)
	{
		for (uint8_t j = 0; j < 16; ++j)
		{
			expander_[i]->pinMode(j, OUTPUT);
			
		}
		expander_[i]->writePort(0xFFFF);
	}
	for (uint8_t i = 0; i < 8; ++i)
	{
		expander_[RG_BANK]->pinMode(i, OUTPUT);
	}
	for (uint8_t i = 8; i < 12; ++i)
	{
		expander_[RG_BANK]->pinMode(i, INPUT_PULLUP);
	}
	for (uint8_t i = 12; i < 16; ++i)
	{
		expander_[RG_BANK]->pinMode(i, OUTPUT);
	}
	uint8_t sw = expander_[RG_BANK]->readPort(1);
	
	expander_[RG_BANK]->digitalWrite(0, 1);
	
	return 0;
}

void AtxCHardware::dummy()
{
	static bool t = false;
	expander_[RG_BANK]->digitalWrite(0, t);
	t = !t;
}