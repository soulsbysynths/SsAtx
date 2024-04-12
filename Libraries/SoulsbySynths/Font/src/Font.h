#pragma once
#include <stdint.h>
#include <stddef.h>
#include <GraphicsTypes.h>

class Font
{
	public:
	Font(const uint8_t** charData, graphics::Size sizeBitShift, char startChar, uint8_t charCount)
		: CHAR_DATA_(charData)
		, SIZE_BIT_SHIFT_(sizeBitShift)
		, START_CHAR_(startChar)
		, CHAR_COUNT_(charCount)
	{
	}
	
	inline const char getStartChar() const
	{
		return START_CHAR_;
	}
	
	inline const graphics::Size* getSizeBitShiftPtr() const
	{
		return &SIZE_BIT_SHIFT_;
	}
	
	inline const int getSizeBitShiftWidth() const
	{
		return SIZE_BIT_SHIFT_.width;
	}
	
	inline const int getSizeBitShiftHeight() const
	{
		return SIZE_BIT_SHIFT_.height;
	}
	
	inline const char getEndChar() const
	{
		return START_CHAR_ + CHAR_COUNT_ - 1;
	}
	
	inline const uint8_t* getCharPtr(char character) const
	{
		if ((character - START_CHAR_) > CHAR_COUNT_)
		{
			return NULL;
		}
		
		return CHAR_DATA_[character - START_CHAR_];
	}
	
	inline const int getCharSize() const
	{
		return (getWidth() * getHeight()) >> 3;
	}
	
	inline const int getWidth() const
	{
		return (1 << SIZE_BIT_SHIFT_.width);
	}
	
	inline const int getHeight() const
	{
		return (1 << SIZE_BIT_SHIFT_.height);
	}
	
	private:
	const uint8_t** CHAR_DATA_;
	const char START_CHAR_;
	const uint8_t CHAR_COUNT_;
	const graphics::Size SIZE_BIT_SHIFT_;
};