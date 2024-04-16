#pragma once
#include <GraphicsDrawing.h>
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
		
		inline static void clip(Rect* rect, const Size* constrainSize)
		{
			graphics::clip(rect, constrainSize, &QUANTISE_BIT_SHIFT_);	
		}
		
		inline static Rect clip(const Rect* rect, const Size* constrainSize)
		{
			Rect r = *rect;
			graphics::clip(&r, constrainSize, &QUANTISE_BIT_SHIFT_);	
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
		static Rect constructRect(const Rect* rect, 
		                     const Size* constrainSize, 
		                     const Font* font);
		
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
