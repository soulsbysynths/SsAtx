//LedCircular.cpp  Hardware 16 LED array
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

#include "LedCircular.h"

// default constructor
LedCircular::LedCircular() : LedCircular(16)
{

} //LedCircular

LedCircular::LedCircular(uint_fast8_t arraySize)
{
	flashCnt_ = new uint8_t [arraySize]();
	flashOnOffTicks_c_ = new uint8_t [arraySize]();
	flashTick_ = new uint16_t [arraySize]();
	segFlashIndex_ = new uint8_t [arraySize]();
	arraySize_ = arraySize;
	for (uint_fast8_t i=0;i<arraySize_;++i)
	{
		segFlashIndex_[i] = FLASH_OFF; // i;  ****WAS i , prob for Ade32 need to revisit why this was. stops normal usage from working
	}
}

LedCircular::LedCircular(uint8_t index, uint_fast8_t arraySize, LedCircularBase * base) : LedCircular(arraySize)
{
	base_ = base;
	index_ = index;
}

// default destructor
LedCircular::~LedCircular()
{
	delete[] flashCnt_;
	delete[] flashOnOffTicks_c_;
	delete[] flashTick_;
	delete[] segFlashIndex_;
} //~LedCircular

uint16_t LedCircular::getOutput()
{
	uint16_t output = 0;
	for (uint_fast8_t i=0;i<arraySize_;++i)
	{
		if (segFlashIndex_[i]==FLASH_OFF)
		{
			bitWrite(output,i,(bitRead(invert_c_,i) ^ bitRead(led_c_,i)));
		}
		else
		{
			bitWrite(output,i,(bitRead(invert_c_,i) ^ bitRead(flash_c_,segFlashIndex_[i])));
		}
	}
	return output;
}

void LedCircular::select(uint_fast8_t value)
{
	led_c_ =  1 << value;
}

void LedCircular::fill(uint_fast8_t value)
{
	led_c_ = (1 << (value + 1)) - 1;
}

void LedCircular::setSegment(uint_fast8_t segment, bool led_on)
{
	bitWrite(led_c_,segment,led_on);
}
bool LedCircular::getSegment(uint_fast8_t segment)
{
	return (bool)bitRead(led_c_,segment);
}
void LedCircular::setFlashOffTicks(uint_fast8_t index, uint_fast8_t offticks)
{
	flashOnOffTicks_c_[index] = compressFourBit(flashOnOffTicks_c_[index],offticks,0);
}
uint_fast8_t  LedCircular::getFlashOffTicks(uint_fast8_t index)
{
	return uncompressFourBit(flashOnOffTicks_c_[index],0);
}
void LedCircular::setFlashOnTicks(uint_fast8_t index, uint_fast8_t onticks)
{
	flashOnOffTicks_c_[index] = compressFourBit(flashOnOffTicks_c_[index],onticks,1);
}
uint_fast8_t  LedCircular::getFlashOnticks(uint_fast8_t index)
{
	return uncompressFourBit(flashOnOffTicks_c_[index],1);
}
void LedCircular::flash(uint_fast8_t index, uint8_t flashes, uint_fast8_t onTicks, uint_fast8_t offTicks, bool startOn)
{
	flashCnt_[index] = flashes;
	flashTick_[index] = 0;
	setFlashOnTicks(index,onTicks);
	setFlashOffTicks(index,offTicks);
	setSegment(index,startOn);
}
void LedCircular::flashStop(uint_fast8_t index)
{
	flashCnt_[index] = 0;
	setSegment(index,false);
}
void LedCircular::flashStopSome(uint_fast16_t state)
{
	for (uint_fast8_t i=0;i<16;++i)
	{
		if (bitRead(state,i)==false)
		{
			flashStop(i);
		}
	}
}
void LedCircular::flashStopAll()
{
	memset(flashCnt_,0,arraySize_);//sizeof(flashCnt_));
	led_c_ = 0;
}
void LedCircular::flashOffAll()
{
	for (uint_fast8_t i=0;i<arraySize_;++i)
	{
		segFlashIndex_[i] = FLASH_OFF;
	}
}
void LedCircular::refreshFlash(uint_fast8_t tick_inc)
{
	bool state;
	uint_fast8_t onticks = 0;
	uint_fast8_t offticks = 0;
	
	for(uint_fast8_t j=0;j<arraySize_;++j)
	{
		if(flashCnt_[j]>0)
		{
			flashTick_[j] += tick_inc;
			state = getSegment(j);
			onticks = getFlashOnticks(j);
			offticks = getFlashOffTicks(j);
			if((state==true && flashTick_[j]>=onticks) || (state==false && flashTick_[j]>=offticks))
			{
				if(state==true)
				{
					flashTick_[j] -= onticks;
				}
				else
				{
					flashTick_[j] -= offticks;
				}
				if (onticks<15)
				{
					setSegment(j,!state);
				}
				if(flashCnt_[j]<255)
				{
					flashCnt_[j]--;
				}
			}
		}
	}
}
void LedCircular::refreshFlashLayered(uint_fast8_t tick_inc)
{
	bool state;
	uint_fast8_t onticks = 0;
	uint_fast8_t offticks = 0;
	
	for(uint_fast8_t i=0;i<arraySize_;++i)
	{
		if(flashCnt_[i]>0)
		{
			flashTick_[i] += tick_inc;
			state = bitRead(flash_c_,i);
			onticks = getFlashOnticks(i);
			offticks = getFlashOffTicks(i);
			if((state==true && flashTick_[i]>=onticks) || (state==false && flashTick_[i]>=offticks))
			{
				if(state==true)
				{
					flashTick_[i] -= onticks;
					if (onticks<15)
					{
						state = false;
					}
				}
				else
				{
					flashTick_[i] -= offticks;
					if (offticks<15)
					{
						state = true;
					}
				}
				bitWrite(flash_c_,i,state);
				if(flashCnt_[i]<255)
				{
					flashCnt_[i]--;
				}
				if (flashCnt_[i]==0)
				{
					for (uint_fast8_t j=0;j<arraySize_;++j)
					{
						if (segFlashIndex_[j]==i)
						{
							segFlashIndex_[j] = FLASH_OFF;
						}
					}
					if (base_!=NULL)
					{
						base_->ledcFlashFinished(index_,i);
					}
				}
			}
		}
	}
}