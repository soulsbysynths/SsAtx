#include "Graphics.h"

using namespace graphics;

const struct graphics::Size Graphics::QUANTISE_BIT_SHIFT_ = { 0, 3 };

Graphics::Graphics(const Rect rect, 
                   const Size* constrainSize, 
                   const Colour initialiseColour /* CO_BLACK*/)
	: RECT_(constrainRectGraphics(&rect, constrainSize))
{
	buffer_ = new uint8_t[getBufferSize()];
	uint8_t* begin = buffer_;
	uint8_t* end = &buffer_[getBufferSize()];
	for (; begin != end; ++begin)
	{
		*begin = getColourPattern(initialiseColour, (int)begin);
	}
}

Graphics::~Graphics(void)
{
	delete[] buffer_;
}

void Graphics::drawLine(Point startPoint, 
                        Point endPoint, 
                        Colour colour, /* CO_WHITE*/
                        DrawMode drawMode /* = DM_OR_MASK */)
{
	if (startPoint.x == endPoint.x) 
	{
		if (startPoint.y > endPoint.y)
		{
			std::swap(startPoint.y, endPoint.y);
		}
		
		drawVerticalLine(startPoint, endPoint.y - startPoint.y, colour, drawMode);
	}
	else if (startPoint.y == endPoint.y) 
	{
		if (startPoint.x > endPoint.x)
		{
			std::swap(startPoint.x, endPoint.x);
		}
		
		drawHorizontalLine(startPoint, endPoint.x - startPoint.x, colour, drawMode);
	}
	else
	{
		int steep = abs(endPoint.y - startPoint.y) > abs(endPoint.x - startPoint.x);
		if (steep) 
		{
			std::swap(startPoint.x, startPoint.y);
			std::swap(endPoint.x, endPoint.y);
		}

		if (startPoint.x > endPoint.x) 
		{
			std::swap(startPoint.x, endPoint.x);
			std::swap(startPoint.y, endPoint.y);
		}

		int dx, dy;
		dx = endPoint.x - startPoint.x;
		dy = abs(endPoint.y - startPoint.y);

		int err = dx / 2;
		int ystep;

		if (startPoint.y < endPoint.y) 
		{
			ystep = 1;
		}
		else 
		{
			ystep = -1;
		}

		for (; startPoint.x <= endPoint.x; startPoint.x++)
		{
			if (steep) 
			{
				Point swapped = { startPoint.y, startPoint.x };
				drawPixel(&swapped, colour, drawMode);
			}
			else 
			{
				drawPixel(&startPoint, colour, drawMode);
			}
			err -= dy;
			if (err < 0) 
			{
				startPoint.y += ystep;
				err += dx;
			}
		}
	}
}


void Graphics::drawHorizontalLine(Point startPoint,
                                  int width, 
                                  Colour colour, /* CO_WHITE*/
                                  DrawMode drawMode /* = DM_OR_MASK */)
{	
	if (startPoint.x < 0)
	{
		// Clip left
		width += startPoint.x;
		startPoint.x = 0;
	}
		
	if ((startPoint.x + width) > RECT_.size.width)
	{
		// Clip right
		width = (RECT_.size.width - startPoint.x - 1);
	}
		
	if (startPoint.x >= RECT_.size.width || width <= 0 || startPoint.y < 0 || startPoint.y >= RECT_.size.height)
	{
		return;
	}
	
	int index = (startPoint.y >> QUANTISE_BIT_SHIFT_.height) * RECT_.size.width + startPoint.x;
	uint8_t mask = 1 << (startPoint.y & 0x07);
	
	drawBuffer(index, width, mask, colour, drawMode);
}


void Graphics::drawVerticalLine(Point startPoint,
                                int height, 
                                Colour colour, /* CO_WHITE*/
                                DrawMode drawMode /* = DM_OR_MASK */)
{
	if (startPoint.y < 0)
	{
		// Clip top
		height += startPoint.y;
		startPoint.y = 0;
	}
		
	if ((startPoint.y + height) > RECT_.size.height)
	{
		// Clip bottom
		height = (RECT_.size.height - startPoint.y - 1);
	}
		
	if (startPoint.y >= RECT_.size.height || height <= 0 || startPoint.x < 0 || startPoint.x >= RECT_.size.width)
	{
		return;
	}
		// Proceed only if height is now positive
	  // this display doesn't need ints for coordinates,
	  // use local byte registers for faster juggling
	int index = (startPoint.y >> QUANTISE_BIT_SHIFT_.height) * RECT_.size.width + startPoint.x;
	
	// do the first partial byte, if necessary - this requires some masking
	uint8_t mod = (startPoint.y & 7);
	if (mod)
	{
		// mask off the high n bits we want to set
		mod = 8 - mod;
		// note - lookup table results in a nearly 10% performance
		// improvement in fill* functions
		// uint8_t mask = ~(0xFF >> mod);
		static const uint8_t PREMASK[8] = {
			0x00,
			0x80,
			0xC0,
			0xE0,
			0xF0,
			0xF8,
			0xFC,
			0xFE
		};
		
		uint8_t mask = PREMASK[mod];
		// adjust the mask if we're not going to reach the end of this byte
		if (height < mod)
		{
			mask &= (0XFF >> (mod - height));
		}
		
		drawBuffer(index, mask, colour, drawMode);
		index += RECT_.size.width;
	}

	if (height >= mod)
	{
		// More to go?
		height -= mod;
		// Write solid bytes while we can - effectively 8 rows at a time
		if (height >= 8)
		{
			do
			{
				drawBuffer(index, 0xFF, colour, drawMode);
				index += RECT_.size.width; // Advance pointer 8 rows
				height -= 8; // Subtract 8 rows from height
			}
			while (height >= 8);
		}

		if (height)
		{
			// Do the final partial byte, if necessary
			mod = height & 0x07;
			// this time we want to mask the low bits of the byte,
			// vs the high bits we did above
			// uint8_t mask = (1 << mod) - 1;
			// note - lookup table results in a nearly 10% performance
			// improvement in fill* functions
			static const uint8_t POSTMASK[8] = { 
				0x00,
				0x01,
				0x03,
				0x07,
				0x0F,
				0x1F,
				0x3F,
				0x7F 
			};
					
			uint8_t mask = POSTMASK[mod];
			drawBuffer(index, mask, colour, drawMode);	
		}
	}
}

void Graphics::drawCharacter(Point location,
                             const Font* font, 
                             char character, 
                             Colour colour, /* = CO_WHITE */
                             DrawMode drawMode /* = DM_OR_MASK */)
{
	const int FONT_WIDTH = font->getWidth();
	const int FONT_HEIGHT = font->getHeight();
	
	uint8_t* blankChar;
	const uint8_t* CHAR_BITMAP = font->getCharPtr(character);
	if (CHAR_BITMAP == NULL)
	{
		blankChar = new uint8_t[font->getCharSize()];
		std::fill(blankChar, blankChar + font->getCharSize(), 0);
		CHAR_BITMAP = blankChar;
	}
	
	if (location.x < 0 || location.x > (RECT_.size.width - FONT_WIDTH) || location.y < 0 || location.y > (RECT_.size.height - FONT_HEIGHT))
	{
		return;   
	}
	   
	int row = location.y >> QUANTISE_BIT_SHIFT_.height;
	int startIndex = row * RECT_.size.width + location.x;
	int charIndex = 0;
	while (charIndex < font->getCharSize())
	{
		drawBuffer(startIndex,
		           FONT_WIDTH,
		           &CHAR_BITMAP[charIndex],
		           FONT_WIDTH,
		           colour,
		           drawMode);
		startIndex += RECT_.size.width;
		charIndex += FONT_WIDTH;
	}
	
	if (font->getCharPtr(character) == NULL)
	{
		delete[] blankChar;
	}
}


void Graphics::drawRect(Rect rect, 
                        Colour colour,
                        DrawMode drawMode /* = DM_OR_MASK */)
{
	drawHorizontalLine(rect.location,
	                   rect.size.width,
	                   colour,
	                   drawMode);
	
	drawHorizontalLine( {
		                   rect.location.x,
		                   rect.location.y + rect.size.height - 1
	                   },
	                   rect.size.width,
	                   colour,
	                   drawMode);
	
	drawVerticalLine(rect.location,
	                 rect.size.height,
	                 colour,
	                 drawMode);
	
	drawVerticalLine( {
		                 rect.location.x + rect.size.width - 1,
		                 rect.location.y
	                 },
	                 rect.size.height,
	                 colour,
	                 drawMode);
}

void Graphics::constrainRect(Rect* rect, const Size* constrainSize, const Size* quantiseBitShift)
{
	// Crop out of bounds left.
	if (rect->location.x < 0)
	{
		rect->size.width += rect->location.x;
		rect->location.x = 0;
	}
	
	// Crop out of bounds top.
	if (rect->location.y < 0)
	{
		rect->size.height += rect->location.y;
		rect->location.y = 0;
	}

	// Quantise	location
	Point originalLocation = rect->location;
	
	rect->location.x >>= quantiseBitShift->width;
	rect->location.x <<= quantiseBitShift->width;
	rect->location.y >>= quantiseBitShift->height;
	rect->location.y <<= quantiseBitShift->height;
	
	// Adjust size for new location, then quantise size
	rect->size.width += (originalLocation.x - rect->location.x);
	rect->size.height += (originalLocation.y - rect->location.y);
	
	rect->size.width >>= quantiseBitShift->width;
	rect->size.width <<= quantiseBitShift->width;
	rect->size.height >>= quantiseBitShift->height;
	rect->size.height <<= quantiseBitShift->height;
	
	// Force width >= quantise width
	if (rect->size.width < (1 << quantiseBitShift->width))
	{
		rect->size.width = (1 << quantiseBitShift->width);
	}
	
	// Force height >= quantise height
	if (rect->size.height < (1 << quantiseBitShift->height))
	{
		rect->size.height = (1 << quantiseBitShift->height);
	}
	
	// Crop out of bounds right.
	if (rect->location.x + rect->size.width > constrainSize->width)
	{
		rect->location.x = constrainSize->width - rect->size.width;
	}
	
	// Crop out of bounds bottom.
	if (rect->location.y + rect->size.height > constrainSize->height)
	{
		rect->location.y = constrainSize->height - rect->size.height;
	}

}