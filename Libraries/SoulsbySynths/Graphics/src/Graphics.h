#pragma once
#include <GraphicsTypes.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h> 
#include <algorithm>
#include <functional>
#include <Font.h>

namespace graphics
{	
	class Graphics
	{
		public:
		Graphics(const Rect rect, 
		         const Size constrainSize, 
		const Colour initialiseColour = CO_BLACK);
	
		~Graphics(void);
		
		inline static void constrainRectGraphics(Rect* rect, const Size constrainSize)
		{
			constrainRect(rect, constrainSize, { 0, GRAPHICS_BIT_SHIFT });	
		}
		
		inline static Rect constrainRectGraphics(const Rect* rect, const Size constrainSize)
		{
			Rect r = *rect;
			constrainRect(&r, constrainSize, { 0, GRAPHICS_BIT_SHIFT });	
			return r;
		}
		
		inline const Rect* getRectPtr() const
		{
			return &RECT_;
		}
		
		inline uint8_t* getBufferPtr()
		{ 
			return buffer_;
		}
		
		inline const uint8_t* getBufferPtr() const
		{
			return buffer_;
		}
		
		void drawLine(Point startPoint, 
		              Point endPoint, 
		              Colour colour = CO_WHITE, 
		              DrawMode drawMode = DM_OR_MASK);
		void drawRect(Rect rect, 
		              Colour colour = CO_WHITE, 
		              DrawMode drawMode = DM_OR_MASK);
		void drawCharacter(Point location, 
		                   const Font* font, 
		                   char character, 
		                   Colour colour = CO_WHITE, 
		                   DrawMode drawMode = DM_OR_MASK);
		static void constrainRect(Rect* rect, 
		                          const Size constrainSize, 
		                          const Size quantiseBitShift);
		
		private:
		
		inline const size_t getBufferSize() const
		{
			return (RECT_.width * RECT_.height) >> GRAPHICS_BIT_SHIFT;
		}
		
		inline void drawBuffer(int startIndex, 
		                       const size_t size, 
		                       const uint8_t mask, 
		                       const Colour colour = CO_WHITE,
		                       DrawMode drawMode = DM_OR_MASK)
		{	
			if (startIndex < 0 || (startIndex + size) > getBufferSize())
			{
				return;
			}
			
			drawArray(&buffer_[startIndex], 
			          &buffer_[startIndex + size], 
			          &buffer_[startIndex], 
			          mask, 
			          colour,
			          drawMode, 
			          startIndex);
		}
		
		inline void drawBuffer(int startIndex, 
		                       const size_t size, 
		                       const uint8_t* mask, 
		                       const size_t maskSize,
		                       const Colour colour = CO_WHITE,
		                       DrawMode drawMode = DM_OR_MASK)
		{	
			if (startIndex < 0 || (startIndex + size) > getBufferSize())
			{
				return;
			}
			
			drawArray(&buffer_[startIndex], 
			          &buffer_[startIndex + size], 
			          &buffer_[startIndex], 
			          mask, 
			          &mask[maskSize], 
			          colour, 
			          drawMode, 
			          startIndex);
		}

		inline static void drawArray(const uint8_t* inBegin, 
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
		
		inline static void drawArray(const uint8_t* inBegin, 
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
		
		inline void drawPixel(Point* point, 
		                      Colour colour = CO_WHITE,
		                      DrawMode drawMode = DM_OR_MASK)
		{	
			drawBuffer(point->x + (point->y >> GRAPHICS_BIT_SHIFT) * getRectPtr()->width, 
			           (1 << (point->y & 0x07)), 
			           colour,
			           drawMode);
		}
		
		inline void drawBuffer(int index, 
		                       const uint8_t mask, 
		                       const Colour colour = CO_WHITE, 
		                       DrawMode drawMode = DM_OR_MASK)
		{
			if (index < 0 || index >= getBufferSize())
			{
				return;
			}
			
			buffer_[index] = drawByte(buffer_[index], mask, colour, drawMode, index);
		}
		
		inline static uint8_t drawByte(const uint8_t inByte, 
		                               const uint8_t mask, 
		                               const Colour colour, 
		                               const DrawMode drawMode, 
		                               int patternIndex = 0)
		{
			return maskByte(inByte, 
			                maskByte(getColourPattern(colour, patternIndex), 
			                         mask, 
			                         graphics::DM_AND_MASK), 
			                drawMode);
		}
		
		inline static uint8_t maskByte(const uint8_t byte, 
		                               const uint8_t mask, 
		                               const DrawMode drawMode)
		{
			switch (drawMode)
			{
				case DM_MASK:
					return mask;
				case DM_NOT_MASK:
					return ~mask;
				case DM_AND_MASK:
					return byte & mask;
				case DM_AND_NOT_MASK:
					return byte & ~mask;
				case DM_OR_MASK:
					return byte | mask;
				case DM_OR_NOT_MASK:
					return byte | ~mask;
				case DM_XOR_MASK:
					return byte ^ mask;
				case DM_XOR_NOT_MASK:
					return byte ^ ~mask;
			}
		}
		
		inline static const uint8_t getColourPattern(const Colour colour, const int patternIndex = 0)
		{
			switch (colour)
			{
				case CO_WHITE:
					return 0xFF;
				case CO_LIGHTGREY:
					return (patternIndex & 0x01) ? 0x77 : 0xDD;
				case CO_GREY:
					return (patternIndex & 0x01) ? 0xAA : 0x55;
				case CO_DARKGREY:
					return (patternIndex & 0x01) ? 0x88 : 0x22;
				case CO_BLACK:
					return 0x00;
			}
		}
		
		void drawHorizontalLine(int x, 
		                        int y, 
		                        int width, 
		                        Colour colour = CO_WHITE, 
		                        DrawMode drawMode = DM_OR_MASK);
		
		void drawVerticalLine(int x, 
		                      int y, 
		                      int height, 
		                      Colour colour = CO_WHITE, 
		                      DrawMode drawMode = DM_OR_MASK);
		
		uint8_t* buffer_ = NULL;
		const Rect RECT_;
		static const uint8_t GRAPHICS_BIT_SHIFT = 3; // Bit shift to store 8 pixel values (on/off) in a byte
	};
}
