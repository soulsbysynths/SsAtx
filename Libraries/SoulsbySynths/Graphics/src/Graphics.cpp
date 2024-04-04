#include "Graphics.h"

using namespace graphics;

Graphics::Graphics(const Rect rect, 
                   const Size constrainSize, 
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
		
		drawVerticalLine(startPoint.x, startPoint.y, endPoint.y - startPoint.y, colour, drawMode);
	}
	else if (startPoint.y == endPoint.y) 
	{
		if (startPoint.x > endPoint.x)
		{
			std::swap(startPoint.x, endPoint.x);
		}
		
		drawHorizontalLine(startPoint.x, startPoint.y, endPoint.x - startPoint.x);
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


void Graphics::drawHorizontalLine(int x, 
                                  int y, 
                                  int width, 
                                  Colour colour, /* CO_WHITE*/
                                  DrawMode drawMode /* = DM_OR_MASK */)
{	
	if (x < 0)
	{
		// Clip left
		width += x;
		x = 0;
	}
		
	if ((x + width) > RECT_.width)
	{
		// Clip right
		width = (RECT_.width - x - 1);
	}
		
	if (x >= RECT_.width || width <= 0 || y < 0 || y >= RECT_.height)
	{
		return;
	}
	
	int index = (y >> GRAPHICS_BIT_SHIFT) * RECT_.width + x;
	uint8_t mask = 1 << (y & 0x07);
	
	drawBuffer(index, width, mask, colour, drawMode);
}


void Graphics::drawVerticalLine(int x, 
                                int y, 
                                int height, 
                                Colour colour, /* CO_WHITE*/
                                DrawMode drawMode /* = DM_OR_MASK */)
{
	if (y < 0)
	{
		// Clip top
		height += y;
		y = 0;
	}
		
	if ((y + height) > RECT_.height)
	{
		// Clip bottom
		height = (RECT_.height - y - 1);
	}
		
	if (y >= RECT_.height || height <= 0 || x < 0 || x >= RECT_.width)
	{
		return;
	}
		// Proceed only if height is now positive
	  // this display doesn't need ints for coordinates,
	  // use local byte registers for faster juggling
	int index = (y >> GRAPHICS_BIT_SHIFT) * RECT_.width + x;
	
	// do the first partial byte, if necessary - this requires some masking
	uint8_t mod = (y & 7);
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
		index += RECT_.width;
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
				index += RECT_.width; // Advance pointer 8 rows
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
	
	if (location.x < 0 || location.x > (RECT_.width - FONT_WIDTH) || location.y < 0 || location.y > (RECT_.height - FONT_HEIGHT))
	{
		return;   
	}
	   
	const uint8_t* CHAR_BITMAP = font->getCharPtr(character);
	int row = location.y >> GRAPHICS_BIT_SHIFT;
	int startIndex = row * RECT_.width + location.x;
	int charIndex = 0;
	while (charIndex < font->getCharSize())
	{
		drawBuffer(startIndex,
		           FONT_WIDTH,
		           &CHAR_BITMAP[charIndex],
		           FONT_WIDTH,
		           colour,
		           drawMode);
		startIndex += RECT_.width;
		charIndex += FONT_WIDTH;
	}
}


void Graphics::drawRect(Rect rect, 
                        Colour colour,
                        DrawMode drawMode /* = DM_OR_MASK */)
{
	drawHorizontalLine(rect.x,
	                   rect.y,
	                   rect.width,
	                   colour,
	                   drawMode);
	
	drawHorizontalLine(rect.x,
	                   rect.y + rect.height - 1,
	                   rect.width,
	                   colour,
	                   drawMode);
	
	drawVerticalLine(rect.x,
	                 rect.y,
	                 rect.height,
	                 colour,
	                 drawMode);
	
	drawVerticalLine(rect.x + rect.width - 1,
	                 rect.y,
	                 rect.height,
	                 colour,
	                 drawMode);
}

void Graphics::constrainRect(Rect* rect, const Size constrainSize, const Size quantiseBitShift)
{
	// Crop out of bounds left.
	if (rect->x < 0)
	{
		rect->width += rect->x;
		rect->x = 0;
	}
	
	// Crop out of bounds top.
	if (rect->y < 0)
	{
		rect->height += rect->y;
		rect->y = 0;
	}

	// Quantise	location
	Point originalLocation = { rect->x, rect->y };
	
	rect->x >>= quantiseBitShift.width;
	rect->x <<= quantiseBitShift.width;
	rect->y >>= quantiseBitShift.height;
	rect->y <<= quantiseBitShift.height;
	
	// Adjust size for new location, then quantise size
	rect->width += (originalLocation.x - rect->x);
	rect->height += (originalLocation.y - rect->y);
	
	rect->width >>= quantiseBitShift.width;
	rect->width <<= quantiseBitShift.width;
	rect->height >>= quantiseBitShift.height;
	rect->height <<= quantiseBitShift.height;
	
	// Force width >= quantise width
	if (rect->width < (1 << quantiseBitShift.width))
	{
		rect->width = (1 << quantiseBitShift.width);
	}
	
	// Force height >= quantise height
	if (rect->height < (1 << quantiseBitShift.height))
	{
		rect->height = (1 << quantiseBitShift.height);
	}
	
	// Crop out of bounds right.
	if (rect->x + rect->width > constrainSize.width)
	{
		rect->x = constrainSize.width - rect->width;
	}
	
	// Crop out of bounds bottom.
	if (rect->y + rect->height > constrainSize.height)
	{
		rect->y = constrainSize.height - rect->height;
	}

}