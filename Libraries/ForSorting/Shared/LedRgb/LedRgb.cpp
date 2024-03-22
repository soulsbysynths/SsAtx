//LedRgb.cpp  Hardware RGB LED
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

#include "LedRgb.h"

// default constructor
LedRgb::LedRgb()
{
} //Ledswitch

// default destructor
LedRgb::~LedRgb()
{
} //~Ledswitch

void LedRgb::setFlashOnCol(LedRgbColour oncol)
{
	flashOnOff_c_ = compressFourBit(flashOnOff_c_,(uint8_t)oncol,1);
}
LedRgb::LedRgbColour LedRgb::getFlashOnCol()
{
	return (LedRgbColour)uncompressFourBit(flashOnOff_c_, 1);
}
void LedRgb::setFlashOffCol(LedRgbColour offcol)
{
	flashOnOff_c_ = compressFourBit(flashOnOff_c_,(uint8_t)offcol,0);
}
LedRgb::LedRgbColour LedRgb::getFlashOffCol()
{
	return (LedRgbColour)uncompressFourBit(flashOnOff_c_,0);
}
void LedRgb::flash(uint8_t flashes, uint_fast8_t  onTicks, uint_fast8_t  offTicks, LedRgbColour onCol, LedRgbColour offCol,bool startOn)
{
	flashCnt_ = flashes;
	flashTick_ = 0;
	setFlashOnTicks(onTicks);
	setFlashOffTicks(offTicks);
	setFlashOnCol(onCol);
	setFlashOffCol(offCol);
	if(startOn==true)
	{
		output_ = onCol;
	}
	else
	{
		output_ = offCol;
	}
}

void LedRgb::flashStop()
{
	flashCnt_ = 0;
	output_ = led_;
}
void LedRgb::setFlashOffTicks(uint_fast8_t  offticks)
{
	flashOnOffTicks_c_ = compressFourBit(flashOnOffTicks_c_,offticks,0);
}
uint_fast8_t  LedRgb::getFlashOffTicks()
{
	return uncompressFourBit(flashOnOffTicks_c_,0);
}
void LedRgb::setFlashOnTicks(uint_fast8_t  onticks)
{
	flashOnOffTicks_c_ = compressFourBit(flashOnOffTicks_c_,onticks,1);
}
uint_fast8_t  LedRgb::getFlashOnticks()
{
	return uncompressFourBit(flashOnOffTicks_c_,1);
}
void LedRgb::refreshFlash(uint_fast8_t  tick_inc)
{
	if(flashCnt_>0)
	{
		flashTick_ += tick_inc;
		LedRgbColour col = getOutput();
		LedRgbColour col_on = getFlashOnCol();
		LedRgbColour col_off = getFlashOffCol();
		uint_fast8_t  onticks = getFlashOnticks();
		uint_fast8_t  offticks = getFlashOffTicks();
		if((col==col_on && flashTick_>=onticks) || (col==col_off && flashTick_>=offticks))
		{
			if(col==col_on)
			{
				flashTick_ -= onticks;
				output_ = col_off;
			}
			else
			{
				flashTick_ -= offticks;
				output_ = col_on;
			}
			if (flashCnt_<255)
			{
				flashCnt_--;
			}
			if(flashCnt_==0)
			{
				output_ = led_;
			}
		}
	}	
}