#pragma once

#include <stdint.h>
#include "AtxCHardware.h"

class AtxC : public AtxCHardwareBase
{
private:
	static AtxC * instance_;
	AtxCHardware& hardware_;
public:
	AtxC();
	~AtxC();
	void initialize();
	void pollFast();
	void poll(uint8_t ticksPassed);
	//void pollSequencer();
	//void setGlobalBank(GlobalBank newBank);
	void printFreeMem();
	
	//**************Hardware***************************
	void hardwareSwitchHeld(uint8_t sw) {}
	void hardwareSwitchClicked(uint8_t sw) {}
	void hardwareSwitchDoubleClicked(uint8_t sw) {}
	void hardwareSwitchChanged(uint8_t sw, bool way) {}
	void hardwareCtrlValueChanged(uint8_t ctrl, uint8_t newValue) {}
	void hardwareRotaryEncoderValueChanged(uint8_t rot, int8_t newValue, bool direction) {}
	void hardwareExtMidiRxd(midi::MidiSource src, midi::MidiMsg * msg) {}//{dispatchMidi(src, msg); }
	void hardwareI2cMidiRxd(uint8_t card, midi::MidiMsg * msg) {}
	void hardwareI2cSysexRxd(uint8_t card, midi::SysexMsg* sysex) {}
	void hardwareInitCard(uint8_t card) {}
	void hardwareExtClk() {}
	
};

