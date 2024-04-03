#pragma once
#include <stdint.h>
#include <stddef.h>
#include <GraphicsTypes.h>

class Font
{
	public:
	Font(const uint8_t** charData, graphics::Size sizeBitShift, char startChar, uint8_t charCount)
		: CHAR_DATA_(charData)
		, SIZE_BIT_SHIFT(sizeBitShift)
		, START_CHAR(startChar)
		, CHAR_COUNT(charCount)
	{
	}
	
	const char START_CHAR;
	const uint8_t CHAR_COUNT;
	const graphics::Size SIZE_BIT_SHIFT;
	inline const char getEndChar() const
	{
		return START_CHAR + CHAR_COUNT - 1;
	}
	
	inline const uint8_t* getCharPtr(char character) const
	{
		if ((character - START_CHAR) > CHAR_COUNT)
		{
			return NULL;
		}
		
		return CHAR_DATA_[character - START_CHAR];
	}
	
	inline const int getCharSize() const
	{
		return (getWidth() * getHeight()) >> 3;
	}
	
	inline const int getWidth() const
	{
		return (1 << SIZE_BIT_SHIFT.width);
	}
	
	inline const int getHeight() const
	{
		return (1 << SIZE_BIT_SHIFT.height);
	}
	
	private:
	const uint8_t** CHAR_DATA_;
};