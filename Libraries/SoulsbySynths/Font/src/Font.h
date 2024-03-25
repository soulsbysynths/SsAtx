#pragma once
#include <stdint.h>
#include <stddef.h>
#include <GraphicsTypes.h>

class Font
{
	public:
	Font(const uint8_t** charData, graphics::Size size, char startChar, uint8_t charCount)
		: CHAR_DATA(charData)
		, SIZE(size)
		, START_CHAR(startChar)
		, CHAR_COUNT(charCount)
	{
	}
	
	const char START_CHAR;
	const uint8_t CHAR_COUNT;
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
		
		return CHAR_DATA[character - START_CHAR];
	}
	
	inline const int getCharSize() const
	{
		return (SIZE.width * SIZE.height) >> 3;
	}
	
	inline const int getWidth() const
	{
		return SIZE.width;
	}
	
	inline const int getHeight() const
	{
		return SIZE.height;
	}
	
	private:
	const uint8_t** CHAR_DATA;
	const graphics::Size SIZE;
};