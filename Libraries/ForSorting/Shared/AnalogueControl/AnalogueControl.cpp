//AnalogueControl.cpp  Hardware analogue control (ie potentiometer)
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

#include "AnalogueControl.h"

// default constructor
AnalogueControl::AnalogueControl(uint8_t initValue)
{
	value_ = initValue;
	valueLast_ = initValue;
	outputValue_ = initValue;
} //AnalogueControl

AnalogueControl::AnalogueControl(uint8_t index, uint8_t initValue, AnalogueControlBase* base)
{
	base_ = base;
	index_ = index;
	latching_ = false;
	latched_ = false;
	latchValMin_ = 0;
	latchValMax_ = 0;
	value_ = initValue;
	valueLast_ = initValue;
	outputValue_ = initValue;
}

// default destructor
AnalogueControl::~AnalogueControl()
{
	if (base_ != NULL)
	{
		delete base_;
	}
} //~AnalogueControl


void AnalogueControl::setLatching(bool newValue)
{
	bool newLatched;
	latching_ = newValue;
	if (latching_==false)
	{
		newLatched = true;
	}
	//else if (value_>=latchValMin_ && value_<=latchValMax_)
	else if (outputValue_>=latchValMin_ && outputValue_<=latchValMax_)
	{
		newLatched = true;
	}
	else
	{
		newLatched = false;
	}
	if (newLatched!=latched_)
	{
		latched_ = newLatched;
		//if(latching_==true) 
		//{
			base_->actrlLatchedChanged(index_,latched_);
		//}
	}
}

void AnalogueControl::resetLatch(uint8_t latchValueMin, uint8_t latchValueMax)
{
	latchValMin_ = latchValueMin;
	latchValMax_ = latchValueMax;
	setLatching(true);
}

bool AnalogueControl::hasChanged(uint8_t ticksPassed)
{
	bool changed = false;

	int16_t diff = abs((int16_t)value_ - valueLast_);
	if(diff>CTRL_MOVE_THRESHOLD && moving_==false)
	{
		moving_ = true;
		moveTick_ = 0;
	}
	else if (moving_==true)
	{
		moveTick_ += ticksPassed;
		if(moveTick_>=CTRL_MOVE_TIMEOUT)
		{
			moving_ = false;
		}
	}
	
	if(moving_==true)
	{
		if(value_!=valueLast_)
		{
			valueLast_ = value_;
			changed = true;
		}
	}
	return changed;
}

void AnalogueControl::poll(uint8_t ticksPassed)
{
	int16_t diff = abs((int16_t)value_ - valueLast_);
	if(diff>CTRL_MOVE_THRESHOLD && moving_==false)
	{
		moving_ = true;
		moveTick_ = 0;
		base_->actrlMovingChanged(index_,true);
	}
	else if (moving_==true)
	{
		moveTick_ += ticksPassed;
		if(moveTick_>=CTRL_MOVE_TIMEOUT)
		{
			moving_ = false;
			base_->actrlMovingChanged(index_,false);
		}
	}
	
	if(moving_==true)
	{
		if(value_!=valueLast_)
		{
			if (locked_==false && latching_ && latched_==false && ((value_>=latchValMin_ && valueLast_<=latchValMin_) || (value_<=latchValMax_ && valueLast_>=latchValMax_)))
			{
				latched_ = true;
				base_->actrlLatchedChanged(index_,latched_);
			}		
			valueLast_ = value_;
			if (locked_==false)
			{
				outputValue_ = value_;
				base_->actrlValueChanged(index_,value_);
			}
			
		}
	}
}

//void AnalogueControl::poll(uint8_t ticksPassed)
//{
//
	//int16_t diff = abs((int16_t)value_ - valueLast_);
	//if(diff>CTRL_MOVE_THRESHOLD && moving_==false)
	//{
		//moving_ = true;
		//moveTick_ = 0;
		//base_->actrlMovingChanged(index_,true);
	//}
	//else if (moving_==true)
	//{
		//moveTick_ += ticksPassed;
		//if(moveTick_>=CTRL_MOVE_TIMEOUT)
		//{
			//moving_ = false;
			//base_->actrlMovingChanged(index_,false);
		//}
	//}
	//
	//if(moving_==true)
	//{
		//if(value_!=valueLast_)
		//{
			//valueLast_ = value_;
			//base_->actrlValueChanged(index_,value_);
		//}
	//}
//}