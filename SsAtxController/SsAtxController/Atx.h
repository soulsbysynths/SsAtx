#pragma once
namespace atx
{
	typedef enum {
		LOW     = 0,
		HIGH    = 1,
		CHANGE  = 2,
		FALLING = 3,
		RISING  = 4,
	} PinStatus;

	typedef enum {
		INPUT            = 0x0,
		OUTPUT           = 0x1,
		INPUT_PULLUP     = 0x2,
		INPUT_PULLDOWN   = 0x3,
		OUTPUT_OPENDRAIN = 0x4,
	} PinMode;

	typedef enum {
		LSBFIRST = 0,
		MSBFIRST = 1,
	} BitOrder; 
	
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitToggle(value, bit) ((value) ^= (1UL << (bit)))
#define bitWrite(value, bit, bitvalue) ((bitvalue) ? bitSet((value), (bit)) : bitClear((value), (bit)))

#ifndef bit
#define bit(b) (1UL << (b))
#endif
	
}