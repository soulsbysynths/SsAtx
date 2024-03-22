#ifndef _CANVAS_H_
#define _CANVAS_H_
#include <Graphics.h>
#include <string.h>

namespace graphics
{
	typedef struct Point
	{
		int x;
		int y;
	} Point;
	
	typedef struct Size
	{
		int width;
		int height;
	} Size;
	
	typedef struct Rect
	{
		int x;
		int y;
		int width;
		int height;
	} Rect;

	enum DrawMode
	{
		DM_WHITE  = 0,
		DM_BLACK,
		DM_INVERT
	};
	
	class Canvas
	{
		public:
		Canvas(Rect rect, 
		       Size outputSize, 
		       uint8_t initialiseValue = 0x00, 
		       Rect quantizeRectBitShift = { 0, 3, 0, 3 })
			: RECT_(constrainRect(rect, outputSize, quantizeRectBitShift))
		{
			buffer_ = new uint8_t[getBufferSize()];
			memset(buffer_, initialiseValue, getBufferSize());
		}
	
		~Canvas(void)
		{
			delete[] buffer_;
		}
		
		inline void setBufferData(const int index, const uint8_t value)
		{
			if (index < 0 || index >= getBufferSize())
			{
				return;
			}
			
			buffer_[index] = value;
		}
		
		uint8_t* getBufferPtr()
		{ 
			return buffer_;
		}
		
		const uint8_t* getBufferPtr() const
		{
			return buffer_;
		}
		
		const size_t getBufferSize() const
		{
			return (RECT_.width * RECT_.height) >> 3;
		}
	
		const Rect* getRectPtr() const
		{
			return &RECT_;
		}
	
		const int getWidth() const
		{
			return RECT_.width;
		}
		
		const int getHeight() const
		{
			return RECT_.height;
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
		
		private:
		inline Rect constrainRect(Rect rect, Size outputSize, Rect quantizeRectBitShift)
		{
			rect.x >>= quantizeRectBitShift.x;
			rect.x <<= quantizeRectBitShift.x;
			rect.y >>= quantizeRectBitShift.y;
			rect.y <<= quantizeRectBitShift.y;
			rect.width >>= quantizeRectBitShift.width;
			rect.width <<= quantizeRectBitShift.width;
			rect.height >>= quantizeRectBitShift.height;
			rect.height <<= quantizeRectBitShift.height;
			
			if (rect.width == 0)
			{
				rect.width = 1 << quantizeRectBitShift.width;
			}
			
			if (rect.height == 0)
			{
				rect.height = 1 << quantizeRectBitShift.height;
			}
			
			if (rect.x + rect.width > outputSize.width)
			{
				rect.x = outputSize.width - rect.width;
			}
		
			if (rect.y + rect.height > outputSize.height)
			{
				rect.y = outputSize.height - rect.height;
			}
			
			return rect;
		}
		uint8_t* buffer_ = NULL;
		const Rect RECT_;
	};	 
}



#endif // _CANVAS_H_