#pragma once
#include <stdint.h>
#include "MidiMsg.h"
#include "SysexMsg.h"

class AtxCHardwareBase
{
public:
	//virtual void hardwareSwitchChanged(uint8_t sw, uint8_t newValue) = 0;  //unconvinced I'll need this one
	virtual void hardwareSwitchHeld(uint8_t sw) = 0;
	virtual void hardwareSwitchClicked(uint8_t sw) = 0;
	virtual void hardwareSwitchDoubleClicked(uint8_t sw) = 0;
	virtual void hardwareSwitchChanged(uint8_t sw, bool way) = 0;
	virtual void hardwareCtrlValueChanged(uint8_t ctrl, uint8_t newValue) = 0;
	virtual void hardwareRotaryEncoderValueChanged(uint8_t rot, int8_t newValue, bool direction) = 0;
	virtual void hardwareExtMidiRxd(midi::MidiSource src, midi::MidiMsg* msg) = 0;
	virtual void hardwareI2cMidiRxd(uint8_t card, midi::MidiMsg* msg) = 0;
	virtual void hardwareI2cSysexRxd(uint8_t card, midi::SysexMsg* sysex) = 0;
	virtual void hardwareInitCard(uint8_t card) = 0;
	virtual void hardwareExtClk() = 0;
};