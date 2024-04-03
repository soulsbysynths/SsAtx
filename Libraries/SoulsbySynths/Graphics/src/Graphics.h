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
		         uint8_t initialiseValue = 0x00);
	
		~Graphics(void);
		
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
		
		void drawLine(Point startPoint, Point endPoint, DrawMode drawMode = DM_WHITE);
		void drawRect(Rect rect, DrawMode drawMode = DM_WHITE);
		void drawCharacter(Point location, const Font* font, char character, DrawMode drawMode = DM_WHITE);
		static void constrainRect(Rect* rect, const Size constrainSize, const Size quantiseBitShift);
		static void constrainRectGraphics(Rect* rect, const Size constrainSize)
		{
			constrainRect(rect, constrainSize, { 0, GRAPHICS_BIT_SHIFT });	
		}
		
		static Rect constrainRectGraphics(const Rect* rect, const Size constrainSize)
		{
			Rect r = *rect;
			constrainRect(&r, constrainSize, { 0, GRAPHICS_BIT_SHIFT });	
			return r;
		}
		
		private:
		inline void setBufferData(const int index, const uint8_t value)
		{
			if (index < 0 || index >= getBufferSize())
			{
				return;
			}
			
			buffer_[index] = value;
		}
		
		inline const size_t getBufferSize() const
		{
			return (RECT_.width * RECT_.height) >> GRAPHICS_BIT_SHIFT;
		}
			
		inline void maskBuffer(int index, size_t size, const uint8_t mask, DrawMode drawMode)
		{		
			while (size)
			{
				maskBuffer(index, mask, drawMode);
				index++;
				size--;
			}
		}
		
		inline void maskBuffer(int index, const uint8_t mask, DrawMode drawMode)
		{
			if (index < 0 || index >= getBufferSize())
			{
				return;
			}

			switch (drawMode)
			{
				case DM_WHITE:
					buffer_[index] |= mask;
					break;
				case DM_GREY:
					buffer_[index] |= (mask & ((index & 0x01) ? 0xAA : 0x55));
					break;
				case DM_BLACK:
					buffer_[index] &= ~mask;
					break;
				case DM_INVERT:
					buffer_[index] ^= mask;
					break;
			}
		}
		
		void drawHorizontalLine(int x, int y, int width, DrawMode drawMode = DM_WHITE);
		void drawVerticalLine(int x, int y, int height, DrawMode drawMode = DM_WHITE);
		void drawPixel(Point* point, DrawMode drawMode);
		
		uint8_t* buffer_ = NULL;
		const Rect RECT_;
		static const uint8_t GRAPHICS_BIT_SHIFT = 3; // Bit shift to store 8 pixel values (on/off) in a byte
	};
}
