//AnalogueControl.h  Hardware analogue control (ie potentiometer)
//Copyright (C) 2015  Paul Soulsby info@soulsbysynths.com
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __ANALOGUECONTROL_H__
#define __ANALOGUECONTROL_H__

#include <stdlib.h>
#include <stdint.h>
#include "AnalogueControlBase.h"

class AnalogueControl
{
//variables
public:
protected:
private:
	AnalogueControlBase* base_ = NULL;
	static const uint8_t CTRL_MOVE_THRESHOLD = 4;
	static const uint16_t CTRL_MOVE_TIMEOUT = 500;
	bool moving_ = false;
	uint8_t valueLast_ = 0;
	uint16_t moveTick_ = 0;
	uint8_t value_ = 0;
	uint8_t outputValue_ = 0;
	bool latching_ = false;
	bool latched_ = true;
	bool locked_ = false;
	uint8_t latchValMin_ = 0;
	uint8_t latchValMax_ = 255;
	uint8_t index_ = 0;
//functions
public:
	AnalogueControl() {}
	AnalogueControl(uint8_t index, uint8_t initValue, AnalogueControlBase* base);
	AnalogueControl(uint8_t initValue);
	~AnalogueControl();
	void setValue(uint8_t newValue){value_ = newValue;}
	uint8_t getValue(){return value_;}
	uint8_t getOutputValue(){return outputValue_;}  //takes into account for locked
	void setMoving(bool newValue){moving_ = newValue;}
	bool getMoving(){return moving_;}
	void setLatching(bool newValue);
	bool getLatching(){return latching_;}
	bool getLatched(){return latched_;}
	void setLatched(bool newValue){latched_ = newValue;}
	void resetLatch(uint8_t latchValue){resetLatch(latchValue,latchValue);}
	void resetLatch(uint8_t latchValueMin, uint8_t latchValueMax);
	void setLocked(bool newValue){locked_ = newValue;}
	bool getLocked(){return locked_;}
	bool hasChanged(uint8_t ticksPassed);  //deprecated
	void poll(uint8_t ticksPassed);
protected:
private:
	
	//AnalogueControl( const AnalogueControl &c );
	//AnalogueControl& operator=( const AnalogueControl &c );

}; //AnalogueControl

#endif //__ANALOGUECONTROL_H__
