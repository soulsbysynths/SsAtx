/*
* AtxCHardware.h
*
* Created: 11/01/2018 01:07:02
* Author: info
*/


#ifndef __ATXCHARDWARE_H__
#define __ATXCHARDWARE_H__

#include <stdint.h>
#include "AtxCHardwareBase.h"
#include <MCP23S17.h>
#include "Atx.h"

class AtxCHardware
{
public:
	static const uint8_t CTRLS = 6;
	enum RotGroup : uint8_t
	{
		RG_GLOBAL = 0,
		RG_FUNC,
		RG_VAL,
		RG_BANK
	};
	static AtxCHardware& getInstance()
	{
		static AtxCHardware instance; // Guaranteed to be destroyed.
		return instance;  // Instantiated on first use.
	}
	void construct(AtxCHardwareBase* base);
	uint8_t initialize();
	void dummy();
private:
	static const uint8_t ENCODERS = 3;
	static const uint8_t EXPANDERS = 4;
	static const uint8_t OLEDS = 8;
	static const uint8_t SWITCHES = 4;
	MCP23S17* expander_[EXPANDERS];
	AtxCHardwareBase* base_;
	AtxCHardware() {}
	AtxCHardware(AtxCHardwareBase* base);
	~AtxCHardware();
	AtxCHardware(const AtxCHardware &c);
	AtxCHardware& operator=(const AtxCHardware &c);
};

#endif //__ATXCHARDWARE_H__