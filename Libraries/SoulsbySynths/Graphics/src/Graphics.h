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
		         const Size* constrainSize, 
		         const Colour initialiseColour = CO_BLACK);
	
		~Graphics(void);
		
		inline static void constrainRectGraphics(Rect* rect, const Size* constrainSize)
		{
			constrainRect(rect, constrainSize, &QUANTISE_BIT_SHIFT_);	
		}
		
		inline static Rect constrainRectGraphics(const Rect* rect, const Size* constrainSize)
		{
			Rect r = *rect;
			constrainRect(&r, constrainSize, &QUANTISE_BIT_SHIFT_);	
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
		
		void drawLine(Line line, 
		              Colour colour = CO_WHITE, 
		              DrawMode drawMode = DM_OR_MASK);
		void drawRect(Rect rect, 
		              Colour colour = CO_WHITE, 
		              DrawMode drawMode = DM_OR_MASK);
		void fillRect(Rect rect, 
		              Colour colour = CO_WHITE, 
		              DrawMode drawMode = DM_OR_MASK);
		void drawCharacter(Point location, 
		                   const Font* font, 
		                   char character, 
		                   Colour colour = CO_WHITE, 
		                   DrawMode drawMode = DM_OR_MASK);
		void drawCircle(Point location,
		                int radius,
		                CircleQuarterFlags quarters,
		                Colour colour = CO_WHITE,
		                DrawMode drawMode = DM_OR_MASK);
		void fillCircle(Point location,
		                int radius,
		                CircleQuarterFlags quarters,
		                int delta = 0,
		                Colour color = CO_WHITE,
		                DrawMode drawMode = DM_OR_MASK);
		void drawRoundRect(Rect rect,
		                   int radius,
		                   Colour colour = CO_WHITE,
		                   DrawMode drawMode = DM_OR_MASK);
		void fillRoundRect(Rect rect,
		                   int radius,
		                   Colour colour = CO_WHITE,
		                   DrawMode drawMode = DM_OR_MASK);
		static void constrainRect(Rect* rect, 
		                          const Size* constrainSize, 
		                          const Size* quantiseBitShift);
		static Rect initRect(const Rect* rect, 
		                     const Size* constrainSize, 
		                     const Font* font);
		static void enlargeGrid(Grid* grid, 
		                        const GridLocation addLocation);
		static void enlargeRect(Rect* rect, 
		                        const Point* addPoint);
		static void enlargeRect(Rect* rect, 
		                        const Rect* addRect);
		static void enlargeRect(Rect* rect, 
		                        const Grid* addGrid, 
		                        const Size* gridSizeBitShift);
		static bool isInnerRectContainedInOuterRect(const Rect* outerRect, 
		                                            const Rect* innerRect);
		static const Rect getRoundRectCircleQuarterRect(const Rect* rect, 
		                                                int radius, 
		                                                CircleQuarter quarter);
		static const Rect getRoundRectSideRect(const Rect* rect, 
		                                       int radius, 
		                                       Side side);
		
		private:
		
		inline const size_t getBufferSize() const
		{
			return (RECT_.size.width >> QUANTISE_BIT_SHIFT_.width) * (RECT_.size.height >> QUANTISE_BIT_SHIFT_.height);
		}
				
		inline void drawPixel(Point* point, 
		                      Colour colour = CO_WHITE,
		                      DrawMode drawMode = DM_OR_MASK)
		{	
			drawBuffer(point->x + (point->y >> QUANTISE_BIT_SHIFT_.height) * getRectPtr()->size.width, 
			           (1 << (point->y & 0x07)), 
			           colour,
			           drawMode);
		}
		
		inline void drawPixel(Point point, 
		                      Colour colour = CO_WHITE,
		                      DrawMode drawMode = DM_OR_MASK)
		{	
			drawPixel(&point, colour, drawMode);
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
		
		static void drawArray(const uint8_t* inBegin, 
		                      const uint8_t* inEnd, 
		                      uint8_t* outBegin,
		                      const uint8_t* maskBegin,
		                      const uint8_t* maskEnd,
		                      const Colour colour = CO_WHITE,
		                      const DrawMode drawMode = DM_OR_MASK,
		                      int patternIndex = 0);
		static void drawArray(const uint8_t* inBegin, 
		                      const uint8_t* inEnd, 
		                      uint8_t* outBegin,
		                      const uint8_t mask,
		                      const Colour colour = CO_WHITE,
		                      const DrawMode drawMode = DM_OR_MASK,
		                      int patternIndex = 0);
		void drawHorizontalLine(Point startPoint,
		                        int width, 
		                        Colour colour = CO_WHITE, 
		                        DrawMode drawMode = DM_OR_MASK);
		void drawVerticalLine(Point startPoint,
		                      int height, 
		                      Colour colour = CO_WHITE, 
		                      DrawMode drawMode = DM_OR_MASK);
		void drawBuffer(int startIndex, 
		                const size_t size, 
		                const uint8_t mask, 
		                const Colour colour = CO_WHITE,
		                DrawMode drawMode = DM_OR_MASK);
		void drawBuffer(int startIndex, 
		                const size_t size, 
		                const uint8_t* mask, 
		                const size_t maskSize,
		                const Colour colour = CO_WHITE,
		                DrawMode drawMode = DM_OR_MASK);
		
		uint8_t* buffer_ = NULL;
		const Rect RECT_;
		static const Size QUANTISE_BIT_SHIFT_; // Bit shift to store 8 pixel values (on/off) in a byte
	};
}
