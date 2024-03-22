//SsHelpers.h  Useful functions for Soulsby Synths classes.   NEEDS TIDYING.  NUMEROUS OSCITRON EXPERIMENTS IN HERE.
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

#ifndef __HELPERS_H__
#define __HELPERS_H__

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO) || defined(ARDUINO_AVR_A_STAR_328PB)
#include <avr/pgmspace.h>
#endif

#include <math.h>
#include <stdlib.h>

//static const char EXP_CONVERT[128] PROGMEM = {0,1,1,2,2,3,4,4,5,5,6,7,7,8,9,9,10,11,11,12,13,13,14,15,15,16,17,18,18,19,20,20,21,22,23,23,24,25,26,27,27,28,29,30,31,31,32,33,34,35,36,37,37,38,39,40,41,42,43,44,45,46,47,47,48,49,50,51,52,53,54,55,56,57,58,59,61,62,63,64,65,66,67,68,69,70,72,73,74,75,76,77,79,80,81,82,83,85,86,87,89,90,91,92,94,95,96,98,99,100,102,103,105,106,107,109,110,112,113,115,116,118,119,121,122,124,125,127};
//// TOO STEEP! static const char EXP_CONVERT[128] PROGMEM = {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,7,7,7,7,8,8,8,9,9,10,10,10,11,11,12,12,13,13,14,15,15,16,16,17,18,19,19,20,21,22,23,24,25,26,27,28,29,30,31,32,34,35,36,38,39,41,43,44,46,48,50,52,54,56,58,61,63,65,68,71,73,76,79,82,86,89,93,96,100,104,108,112,117,121,127};
////static const char EXP_CONVERT_BIPOLAR[256] PROGMEM = {-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-124,-124,-124,-124,-124,-124,-124,-124,-124,-124,-124,-124,-123,-123,-123,-123,-123,-123,-123,-123,-123,-122,-122,-122,-122,-122,-122,-122,-122,-121,-121,-121,-121,-121,-121,-120,-120,-120,-120,-120,-120,-119,-119,-119,-119,-119,-118,-118,-118,-118,-118,-117,-117,-117,-117,-116,-116,-116,-116,-115,-115,-115,-114,-114,-114,-114,-113,-113,-113,-112,-112,-112,-111,-111,-110,-110,-110,-109,-109,-108,-108,-107,-107,-107,-106,-106,-105,-105,-104,-104,-103,-103,-102,-101,-101,-100,-100,-99,-98,-98,-97,-96,-96,-95,-94,-93,-93,-92,-91,-90,-89,-89,-88,-87,-86,-85,-84,-83,-82,-81,-80,-79,-78,-77,-76,-75,-73,-72,-71,-70,-69,-67,-66,-65,-63,-62,-60,-59,-57,-56,-54,-52,-51,-49,-47,-46,-44,-42,-40,-38,-36,-34,-32,-30,-28,-26,-23,-21,-19,-16,-14,-11,-9,-6,-4,-1,2,5,8,11,14,17,20,23,27,30,34,37,41,44,48,52,56,60,64,68,73,77,82,86,91,96,101,106,111,116,122,127};
//
//static const unsigned char COMPRESS_FOUR_BIT_MASK[2] PROGMEM = {0x0F,0xF0};
//static const unsigned char COMPRESS_TWO_BIT_MASK[4] PROGMEM = {0x03,0x0C,0x30,0xC0};
//
//inline
//char convertExponential(char input)
//{
//return (char)pgm_read_byte(&(EXP_CONVERT[(unsigned char)input]));
//}
//inline
//char convertInvExponential(char input)
//{
//return 127-(char)pgm_read_byte(&(EXP_CONVERT[127-(unsigned char)input]));
//}
//inline
//char convertExponentialBipolar(char input)
//{
//return (char)pgm_read_byte(&(EXP_CONVERT_BIPOLAR[(int)input+128]));
//}
//inline
//unsigned char shapeExponential(char input, float shapeAmt, unsigned char multiplier)
//{
////zero gain = multiplier
//float f, g;
//f = (float)input / 127;
//g = round(exp(f * shapeAmt) * (float)multiplier - 1);
//return (unsigned char)g;
//}
//inline
//unsigned char shapeExponential(int input, float shapeAmt, unsigned char multiplier)
//{
////zero gain = multiplier
//float f, g;
//f = (float)input / 4095;
//g = round(exp(f * shapeAmt) * (float)multiplier - 1);
//return (unsigned char)g;
//}
//inline
//unsigned char compressFourBit(unsigned char input, unsigned char newValue, unsigned char pos)
//{
//unsigned char mask = pgm_read_byte(&(COMPRESS_FOUR_BIT_MASK[pos]));
//unsigned char shift = 4 * pos;
//return (input & ~mask) | ((newValue<<shift) & mask);
//}
//inline
//unsigned char uncompressFourBit(unsigned char input, unsigned char pos)
//{
//unsigned char mask = pgm_read_byte(&(COMPRESS_FOUR_BIT_MASK[pos]));
//unsigned char shift = 4 * pos;
//unsigned char out = input & mask;
//return out >> shift;
//}
//inline
//unsigned char compressTwoBit(unsigned char input, unsigned char newValue, unsigned char pos)
//{
//unsigned char mask = pgm_read_byte(&(COMPRESS_TWO_BIT_MASK[pos]));
//unsigned char shift = 2 * pos;
//return (input & ~mask) | ((newValue<<shift) & mask);
//}
//inline
//unsigned char uncompressTwoBit(unsigned char input, unsigned char pos)
//{
//unsigned char mask = pgm_read_byte(&(COMPRESS_TWO_BIT_MASK[pos]));
//unsigned char shift = 2 * pos;
//unsigned char out = input & mask;
//return out >> shift;
//}
//inline
//char constrainChar(int input)
//{
//if(input>127)
//{
//return 127;
//}
//else if (input<-128)
//{
//return -128;
//}
//else
//{
//return (char)input;
//}
//}
//inline
//unsigned char constrainUChar(int input)
//{
//if(input>255)
//{
//return 255;
//}
//else if (input<0)
//{
//return 0;
//}
//else
//{
//return (unsigned char)input;
//}
//}
//inline
//int constrainInt(long input)
//{
//if(input>32767)
//{
//return 32767;
//}
//else if (input<-32768)
//{
//return -32768;
//}
//else
//{
//return (int)input;
//}
//}
//inline
//long map(long x, long in_min, long in_max, long out_min, long out_max)
//{
//return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
//}
//inline
//char randomValue(char min, char max)
//{
//return (char) (((rand()&0xFF) * ((int)max-(int)min))>>8) + (int)min;
//}

//#elif defined(ARDUINO_SAMD_ZERO)

#include <math.h>
#include <stdlib.h>
#include <stdint.h>

static const char EXP_STEEP_CONVERT[128]
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO) || defined(ARDUINO_AVR_A_STAR_328PB)
PROGMEM
#endif
 = {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,7,7,7,7,8,8,8,9,9,10,10,10,11,11,12,12,13,13,14,15,15,16,16,17,18,19,19,20,21,22,23,24,25,26,27,28,29,30,31,32,34,35,36,38,39,41,43,44,46,48,50,52,54,56,58,61,63,65,68,71,73,76,79,82,86,89,93,96,100,104,108,112,117,121,127};

static const int8_t EXP_CONVERT[128]
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO) || defined(ARDUINO_AVR_A_STAR_328PB)
PROGMEM
#endif
= {0,1,1,2,2,3,4,4,5,5,6,7,7,8,9,9,10,11,11,12,13,13,14,15,15,16,17,18,18,19,20,20,21,22,23,23,24,25,26,27,27,28,29,30,31,31,32,33,34,35,36,37,37,38,39,40,41,42,43,44,45,46,47,47,48,49,50,51,52,53,54,55,56,57,58,59,61,62,63,64,65,66,67,68,69,70,72,73,74,75,76,77,79,80,81,82,83,85,86,87,89,90,91,92,94,95,96,98,99,100,102,103,105,106,107,109,110,112,113,115,116,118,119,121,122,124,125,127};
//static const int8_t EXP_CONVERT_BIPOLAR[256]
//#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO)
//PROGMEM
//#endif
//= {-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-125,-124,-124,-124,-124,-124,-124,-124,-124,-124,-124,-124,-124,-123,-123,-123,-123,-123,-123,-123,-123,-123,-122,-122,-122,-122,-122,-122,-122,-122,-121,-121,-121,-121,-121,-121,-120,-120,-120,-120,-120,-120,-119,-119,-119,-119,-119,-118,-118,-118,-118,-118,-117,-117,-117,-117,-116,-116,-116,-116,-115,-115,-115,-114,-114,-114,-114,-113,-113,-113,-112,-112,-112,-111,-111,-110,-110,-110,-109,-109,-108,-108,-107,-107,-107,-106,-106,-105,-105,-104,-104,-103,-103,-102,-101,-101,-100,-100,-99,-98,-98,-97,-96,-96,-95,-94,-93,-93,-92,-91,-90,-89,-89,-88,-87,-86,-85,-84,-83,-82,-81,-80,-79,-78,-77,-76,-75,-73,-72,-71,-70,-69,-67,-66,-65,-63,-62,-60,-59,-57,-56,-54,-52,-51,-49,-47,-46,-44,-42,-40,-38,-36,-34,-32,-30,-28,-26,-23,-21,-19,-16,-14,-11,-9,-6,-4,-1,2,5,8,11,14,17,20,23,27,30,34,37,41,44,48,52,56,60,64,68,73,77,82,86,91,96,101,106,111,116,122,127};

static const uint8_t COMPRESS_FOUR_BIT_MASK[2] = {0x0F,0xF0};
static const uint8_t COMPRESS_TWO_BIT_MASK[4] = {0x03,0x0C,0x30,0xC0};

inline
int8_t convertExponential(int8_t input)
{
	#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO) || defined(ARDUINO_AVR_A_STAR_328PB)
	return (int8_t)pgm_read_byte(&(EXP_CONVERT[(uint8_t)input]));
	#elif defined(ARDUINO_SAMD_ZERO)
	return (int8_t)EXP_CONVERT[(uint8_t)input];
	#endif
}
inline
int8_t convertInvExponential(int8_t input)
{
	#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO) || defined(ARDUINO_AVR_A_STAR_328PB)
	return 127-(int8_t)pgm_read_byte(&(EXP_CONVERT[127-(uint8_t)input]));
	#elif defined(ARDUINO_SAMD_ZERO)
	return 127-(int8_t)EXP_CONVERT[127-(uint8_t)input];
	#endif
}
inline
int8_t convertSteepExponential(int8_t input)
{
	#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO) || defined(ARDUINO_AVR_A_STAR_328PB)
	return (int8_t)pgm_read_byte(&(EXP_STEEP_CONVERT[(uint8_t)input]));
	#elif defined(ARDUINO_SAMD_ZERO)
	return (int8_t)EXP_STEEP_CONVERT[(uint8_t)input];
	#endif
}
inline
int8_t convertInvSteepExponential(int8_t input)
{
	#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO) || defined(ARDUINO_AVR_A_STAR_328PB)
	return 127-(int8_t)pgm_read_byte(&(EXP_STEEP_CONVERT[127-(uint8_t)input]));
	#elif defined(ARDUINO_SAMD_ZERO)
	return 127-(int8_t)EXP_STEEP_CONVERT[127-(uint8_t)input];
	#endif
}
inline
int8_t convertExponential4Bit(int8_t input)
{
	#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO) || defined(ARDUINO_AVR_A_STAR_328PB)
	return (int8_t)pgm_read_byte(&(EXP_CONVERT[input << 3])) >> 3;
	#elif defined(ARDUINO_SAMD_ZERO)
	return (int8_t)EXP_CONVERT[input << 3] >> 3;
	#endif
}
inline
int8_t convertInvExponential4Bit(int8_t input)
{
	#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO) || defined(ARDUINO_AVR_A_STAR_328PB)
	return 15-(pgm_read_byte(&(EXP_CONVERT[(15-input) << 3])) >> 3);
	#elif defined(ARDUINO_SAMD_ZERO)
	return 15-(EXP_CONVERT[(15-input) << 3] >> 3);
	#endif
}
inline
int8_t convertSteepExponential4Bit(int8_t input)
{
	#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO) || defined(ARDUINO_AVR_A_STAR_328PB)
	return (int8_t)pgm_read_byte(&(EXP_STEEP_CONVERT[input << 3])) >> 3;
	#elif defined(ARDUINO_SAMD_ZERO)
	return (int8_t)EXP_STEEP_CONVERT[input << 3] >> 3;
	#endif
}
inline
int8_t convertInvSteepExponential4Bit(int8_t input)
{
	#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO) || defined(ARDUINO_AVR_A_STAR_328PB)
	return 15-(pgm_read_byte(&(EXP_STEEP_CONVERT[(15-input) << 3])) >> 3);
	#elif defined(ARDUINO_SAMD_ZERO)
	return 15-(EXP_STEEP_CONVERT[(15-input) << 3] >> 3);
	#endif
}
//inline
//int8_t convertExponentialBipolar(int8_t input)
//{
//return (int8_t)EXP_CONVERT_BIPOLAR[(int16_t)input+128];
//}
inline
uint8_t shapeExponential(int8_t input, float shapeAmt, uint8_t multiplier)
{
	//zero gain = multiplier
	float f, g;
	f = (float)input / 127;
	g = round(exp(f * shapeAmt) * (float)multiplier - 1);
	return (uint8_t)g;
}
//inline
//uint8_t shapeExponential(int16_t input, float shapeAmt, uint8_t multiplier)
//{
	////zero gain = multiplier
	//float f, g;
	//f = (float)input / 4095;
	//g = round(exp(f * shapeAmt) * (float)multiplier - 1);
	//return (uint8_t)g;
//}
inline
uint8_t compressFourBit(uint8_t input, uint8_t newValue, uint8_t pos)
{
	uint8_t mask = COMPRESS_FOUR_BIT_MASK[pos];
	return (input & ~mask) | ((newValue<<(pos << 2)) & mask);
}
inline
uint8_t uncompressFourBit(uint8_t input, uint8_t pos)
{
	uint8_t mask = COMPRESS_FOUR_BIT_MASK[pos];
	uint8_t out = input & mask;
	return out >> (pos << 2);
}
inline
uint8_t compressTwoBit(uint8_t input, uint8_t newValue, uint8_t pos)
{
	uint8_t mask = COMPRESS_TWO_BIT_MASK[pos];
	return (input & ~mask) | ((newValue<<(pos << 1)) & mask);
}
inline
uint8_t uncompressTwoBit(uint8_t input, uint8_t pos)
{
	uint8_t mask = COMPRESS_TWO_BIT_MASK[pos];
	uint8_t out = input & mask;
	return out >> (pos << 1);
}
inline
int8_t constrainChar(int16_t input)
{
	if(input>INT8_MAX)
	{
		return INT8_MAX;
	}
	else if (input<INT8_MIN)
	{
		return INT8_MIN;
	}
	else
	{
		return (int8_t)input;
	}
}
inline
uint8_t constrainUChar(int16_t input)
{
	if(input>UINT8_MAX)
	{
		return UINT8_MAX;
	}
	else if (input<0)
	{
		return 0;
	}
	else
	{
		return (uint8_t)input;
	}
}
inline
int16_t constrainInt(int32_t input)
{
	if(input>INT16_MAX)
	{
		return INT16_MAX;
	}
	else if (input<INT16_MIN)
	{
		return INT16_MIN;
	}
	else
	{
		return (int16_t)input;
	}
}
inline
uint16_t constrainUInt(int32_t input)
{
	if(input>UINT16_MAX)
	{
		return UINT16_MAX;
	}
	else if (input<0)
	{
		return 0;
	}
	else
	{
		return (uint16_t)input;
	}
}
inline
uint16_t constrainUnsignedUser(uint8_t bits, uint16_t input)
{
	uint16_t mask = (1 << bits) - 1;
	if (input > mask)
	{
		return mask;
	} 
	else
	{
		return input;
	}
}
inline
int32_t map(int32_t x, int32_t inMin, int32_t inMax, int32_t outMin, int32_t outMax)
{
	return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}
inline
int8_t randomValue(int8_t min, int8_t max)
{
	return (int8_t) (((rand()&0xFF) * ((int16_t)max-(int16_t)min))>>8) + (int16_t)min;
}
inline
int8_t rightshiftChar(int8_t input, uint8_t shift)
{
	return (input < 0) ? -((-input) >> shift) : input >> shift;
}
inline
int16_t rightshiftInt(int16_t input, uint8_t shift)
{
	return (input < 0) ? -((-input) >> shift) : input >> shift;
}
inline
int32_t rightshiftLong(int32_t input, uint8_t shift)
{
	return (input < 0) ? -((-input) >> shift) : input >> shift;
}
inline
uint16_t swapBits(uint16_t input, uint8_t b1, uint8_t b2)
{
	//http://graphics.stanford.edu/~seander/bithacks.html#SwappingBitsXOR
	uint16_t x = ((input >> b1) ^ (input >> b2)) & ((1U << 1) - 1); // XOR temporary
	return input ^ ((x << b1) | (x << b2));
}
inline
uint8_t reverseBits(uint8_t input)
{
	uint32_t b = input;
	b = ((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16; 
	return (uint8_t)b;
}
inline
uint8_t bitCount(uint8_t b)
{
	b = b - ((b >> 1) & 0x55);
	b = (b & 0x33) + ((b >> 2) & 0x33);
	return (((b + (b >> 4)) & 0x0F) * 0x01);
}
//#endif
#endif //__HELPERS_H__

