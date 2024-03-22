//LedRgb.cpp  Hardware RGB LED control
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

#ifndef __LEDRGB_H__
#define __LEDRGB_H__

#include "Helpers.h"

class LedRgb
{
//variables
public:
	enum LedRgbColour : uint8_t
	{
		OFF,
		RED,
		GREEN,
		YELLOW,
		BLUE,
		MAGENTA,
		CYAN,
		WHITE
	};
protected:
private:
	static const uint8_t TICK_SCALE = 16;
	LedRgbColour led_ = OFF;
	LedRgbColour output_ = OFF;
	uint8_t flashOnOff_c_ = 0;
	uint8_t flashCnt_ = 0;
	uint8_t flashOnOffTicks_c_ = 0;  //compresses on and off ticks into 4 bit
	uint8_t flashTick_ = 0;
//functions
public:
	LedRgb();
	~LedRgb();
	void setColour(LedRgbColour colour){led_ = colour;output_ = colour;}
	LedRgbColour getColour(){return led_;}
	LedRgbColour getOutput(){return output_;}
	bool getFlashing(){return (bool)flashCnt_;}
	void flash(uint8_t flashes, uint_fast8_t  onTicks, uint_fast8_t  offTicks, LedRgbColour onCol, LedRgbColour offCol, bool startOn);
	void flash(uint8_t flashes, uint_fast8_t  onTicks, uint_fast8_t  offTicks, LedRgbColour onCol){flash(flashes,onTicks,offTicks,onCol,led_,true);}
	void flashStop();
	void refreshFlash(uint_fast8_t  tick_inc);
protected:
private:
	LedRgb( const LedRgb &c );
	LedRgb& operator=( const LedRgb &c );
	void setFlashOnTicks(uint_fast8_t  onticks);
	uint_fast8_t  getFlashOnticks();
	void setFlashOffTicks(uint_fast8_t  offticks);
	uint_fast8_t  getFlashOffTicks();
	void setFlashOnCol(LedRgbColour oncol);
	LedRgbColour getFlashOnCol();
	void setFlashOffCol(LedRgbColour offcol);
	LedRgbColour getFlashOffCol();
}; //LedRgb

#endif //__LEDRGB_H__
