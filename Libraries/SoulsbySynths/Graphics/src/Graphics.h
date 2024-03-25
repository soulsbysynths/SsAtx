#pragma once
#include <GraphicsTypes.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h> 
#include <algorithm>
#include <Font.h>

namespace graphics
{	
	class Graphics
	{
		public:
		Graphics(Rect rect, 
		         Size outputSize, 
		         uint8_t initialiseValue = 0x00, 
		         Rect quantizeRectBitShift = { 0, 3, 0, 3 });
	
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
			return (RECT_.width * RECT_.height) >> 3;
		}
			
		inline void maskBuffer(int index, size_t size, const uint8_t mask, DrawMode drawMode)
		{
			if ((index + size) >= getBufferSize())
			{
				size = getBufferSize() - index;
			}
			
			while (size)
			{
				switch (drawMode)
				{
					case DM_WHITE:
						buffer_[index] |= mask;
						break;
					case DM_BLACK:
						buffer_[index] &= ~mask;
						break;
					case DM_INVERT:
						buffer_[index] ^= mask;
						break;
				}
				
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
				case DM_BLACK:
					buffer_[index] &= ~mask;
					break;
				case DM_INVERT:
					buffer_[index] ^= mask;
					break;
			}
		}
		
		Rect constrainRect(Rect rect, Size outputSize, Rect quantizeRectBitShift);
		void drawHorizontalLine(int x, int y, int width, DrawMode drawMode = DM_WHITE);
		void drawVerticalLine(int x, int y, int height, DrawMode drawMode = DM_WHITE);
		void drawPixel(Point* point, DrawMode drawMode);
		
		uint8_t* buffer_ = NULL;
		const Rect RECT_;
	};
}
