//LedCircular.h  Hardware 16 LED array
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

#ifndef __LEDCIRCULAR_H__
#define __LEDCIRCULAR_H__

#include "Helpers.h"
#include "LedCircularBase.h"
#include <string.h>
#include <stdint.h>

#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif

class LedCircular
{
//variables
public:
	static const uint8_t FLASH_OFF = 0xFF;
protected:
private:
	uint16_t led_c_ = 0;
	uint8_t arraySize_ = 0;	//needed because sizeof doesn't work without it
	uint8_t * flashCnt_;
	uint8_t * flashOnOffTicks_c_;   //compresses on and off ticks into 4 bit
	uint16_t * flashTick_;
	uint8_t *  segFlashIndex_;
	uint16_t flash_c_ = 0;
	LedCircularBase* base_ = NULL;
	uint8_t index_ = 0;
	uint16_t invert_c_ = 0;
//functions
public:
	LedCircular();
	LedCircular(uint_fast8_t arraySize);
	LedCircular(uint8_t index, uint_fast8_t arraySize, LedCircularBase * base);
	~LedCircular();
	void select(uint_fast8_t value);
	void fill(uint_fast8_t value);
	void setState(uint16_t leds){led_c_ = leds;}
	uint16_t getState(){return led_c_;}
	void setInvertState(uint16_t invert){invert_c_ = invert;}
	uint16_t getInvertState(){return invert_c_;}
	uint16_t getOutput();
	void setSegment(uint_fast8_t segment, bool led_on);
	bool getSegment(uint_fast8_t segment);
	void setSegmentFlashIndex(uint_fast8_t segment, uint_fast8_t index){segFlashIndex_[segment] = index;}
	void setInvert(uint_fast8_t segment,bool newValue){bitWrite(invert_c_,segment,newValue);}
	bool getInvert(uint_fast8_t segment){return bitRead(invert_c_,segment);}
	void flash(uint_fast8_t index, uint8_t flashes, uint_fast8_t onTicks, uint_fast8_t offTicks, bool startOn);
	void flashStop(uint_fast8_t index);
	void flashStopSome(uint_fast16_t state);
	void flashStopAll();
	void flashOffAll();
	bool getFlashing(uint_fast8_t index){return (bool)flashCnt_[index];}
	void setFlashOffTicks(uint_fast8_t index, uint_fast8_t offticks);
	void setFlashOnTicks(uint_fast8_t index, uint_fast8_t onticks);
	void refreshFlash(uint_fast8_t tick_inc);
	void refreshFlashLayered(uint_fast8_t tick_inc);
protected:
private:
	LedCircular( const LedCircular &c );
	LedCircular& operator=( const LedCircular &c );
	uint_fast8_t  getFlashOnticks(uint_fast8_t index);
	uint_fast8_t  getFlashOffTicks(uint_fast8_t index);
}; //LEDCIRCULAR

#endif //__LEDCIRCULAR_H__
