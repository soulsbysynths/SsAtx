//AnalogueControlBase.h  analogue control base class
//Copyright (C) 2017  Paul Soulsby info@soulsbysynths.com
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


#ifndef ANALOGUECONTROLBASE_H_
#define ANALOGUECONTROLBASE_H_

#include <stdint.h>

class AnalogueControlBase
{
	public:
	virtual void actrlValueChanged(uint8_t index, uint8_t newValue) = 0;
	virtual void actrlMovingChanged(uint8_t index, bool newValue) = 0;
	virtual void actrlLatchedChanged(uint8_t index, bool newValue) = 0;
};



#endif /* ANALOGUECONTROLBASE_H_ */