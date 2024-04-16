#ifndef _GRAPHICSDRAWING_H_
#define _GRAPHICSDRAWING_H_
#include  <GraphicsTypes.h>
#include <GraphicsMaths.h>

namespace
{
	inline static const uint8_t getColourData(const graphics::Colour colour, const int patternIndex = 0)
	{
		switch (colour)
		{
			case graphics::CO_WHITE:
				return 0xFF;
			case graphics::CO_LIGHTGREY:
				return (patternIndex & 0x01) ? 0x77 : 0xDD;
			case graphics::CO_GREY:
				return (patternIndex & 0x01) ? 0xAA : 0x55;
			case graphics::CO_DARKGREY:
				return (patternIndex & 0x01) ? 0x88 : 0x22;
			case graphics::CO_BLACK:
				return 0x00;
		}
	}	
	
	inline static uint8_t maskByte(const uint8_t byte, 
	                               const uint8_t mask, 
	                               const graphics::DrawMode drawMode)
	{
		switch (drawMode)
		{
			case graphics::DM_MASK:
				return mask;
			case graphics::DM_NOT_MASK:
				return ~mask;
			case graphics::DM_AND_MASK:
				return byte & mask;
			case graphics::DM_AND_NOT_MASK:
				return byte & ~mask;
			case graphics::DM_OR_MASK:
				return byte | mask;
			case graphics::DM_OR_NOT_MASK:
				return byte | ~mask;
			case graphics::DM_XOR_MASK:
				return byte ^ mask;
			case graphics::DM_XOR_NOT_MASK:
				return byte ^ ~mask;
		}
	}
}

namespace graphics
{

	
	inline static uint8_t drawByte(const uint8_t inByte, 
	                               const uint8_t mask, 
	                               const Colour colour, 
	                               const DrawMode drawMode, 
	                               int patternIndex = 0)
	{
		return maskByte(inByte, 
		                maskByte(getColourData(colour, patternIndex), 
		                         mask, 
		                         graphics::DM_AND_MASK), 
		                drawMode);
	}
	
	static void drawArray(const uint8_t* inBegin, 
	                      const uint8_t* inEnd, 
	                      uint8_t* outBegin,
	                      const uint8_t* maskBegin,
	                      const uint8_t* maskEnd,
	                      const Colour colour = CO_WHITE,
	                      const DrawMode drawMode = DM_OR_MASK,
	                      int patternIndex = 0)
	{		
		while (inBegin != inEnd && maskBegin != maskEnd)
		{
			*outBegin = drawByte(*inBegin, *maskBegin, colour, drawMode, patternIndex);
			++outBegin; 
			++inBegin;
			++maskBegin;
			++patternIndex;
		}
	}
	
	static void drawArray(const uint8_t* inBegin, 
	                      const uint8_t* inEnd, 
	                      uint8_t* outBegin,
	                      const uint8_t mask,
	                      const Colour colour = CO_WHITE,
	                      const DrawMode drawMode = DM_OR_MASK,
	                      int patternIndex = 0)
	{		
		while (inBegin != inEnd)
		{
			*outBegin = drawByte(*inBegin, mask, colour, drawMode, patternIndex);
			++outBegin; 
			++inBegin;
			++patternIndex;
		}
	}
	

	
}



#endif // _GRAPHICSDRAWING_H_