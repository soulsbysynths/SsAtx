//Oled.h  Oled base class for Atx
//Copyright (C) 2018  Paul Soulsby info@soulsbysynths.com
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

#ifndef OLEDBASE_H_
#define OLEDBASE_H_
#include <stdint.h>

class OledBase
{
	public:
	virtual void oledTxCommand(uint8_t index, uint8_t command) = 0;
	virtual void oledTxData(uint8_t index, const uint8_t * data, uint_fast16_t size, bool wide) = 0;
	virtual void oledTxZeroData(uint8_t index, uint_fast16_t size) = 0;
};



#endif /* OLEDBASE_H_ */