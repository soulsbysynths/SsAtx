#include "Graphics.h"

using namespace graphics;

void Graphics::drawLine(Canvas* canvas, 
                        Point startPoint, 
                        Point endPoint, 
                        DrawMode drawMode /* = DM_WHITE */)
{
	if (startPoint.x == endPoint.x) 
	{
		if (startPoint.y > endPoint.y)
		{
			std::swap(startPoint.y, endPoint.y);
		}

		if (startPoint.y < 0 || endPoint.y >= canvas->getRectPtr()->height)
		{
			return;
		}
		
		drawVerticalLine(canvas, startPoint.x, startPoint.y, endPoint.y - startPoint.y, drawMode);
	}
	else if (startPoint.y == endPoint.y) 
	{
		if (startPoint.x > endPoint.x)
		{
			std::swap(startPoint.x, endPoint.x);
		}
		
		if (startPoint.y < 0 || endPoint.y >= canvas->getRectPtr()->height)
		{
			return;
		}
		
		drawHorizontalLine(canvas, startPoint.x, startPoint.y, endPoint.x - startPoint.x);
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
				drawPixel(canvas, &swapped, drawMode);
			}
			else 
			{
				drawPixel(canvas, &startPoint, drawMode);
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


void Graphics::drawHorizontalLine(Canvas* canvas, 
                                  int x, 
                                  int y, 
                                  int width, 
                                  DrawMode drawMode /* = DM_WHITE */)
{
	
	const int CANV_WIDTH = canvas->getWidth();
	const int CANV_HEIGHT = canvas->getHeight();
	
	if (x < 0)
	{
		// Clip left
		width += x;
		x = 0;
	}
		
	if ((x + width) >= CANV_WIDTH)
	{
		// Clip right
		width = (CANV_WIDTH - x - 1);
	}
		
	if (x >= CANV_WIDTH || width <= 0 || y < 0 || y >= CANV_HEIGHT)
	{
		return;
	}
	
	int index = (y >> 3) * CANV_WIDTH + x;
	uint8_t mask = 1 << (y & 0x07);
	
	canvas->maskBuffer(index, width, mask, drawMode);
}


void Graphics::drawVerticalLine(Canvas* canvas, 
                                int x, 
                                int y, 
                                int height, 
                                DrawMode drawMode /* = DM_WHITE */)
{
	const int CANV_WIDTH = canvas->getWidth();
	const int CANV_HEIGHT = canvas->getHeight();
	
	if (y < 0)
	{
		// Clip top
		height += y;
		y = 0;
	}
		
	if ((y + height) >= CANV_HEIGHT)
	{
		// Clip bottom
		height = (CANV_HEIGHT - y - 1);
	}
		
	if (y >= CANV_HEIGHT || height <= 0 || x < 0 || x >= CANV_WIDTH)
	{
		return;
	}
		// Proceed only if height is now positive
	  // this display doesn't need ints for coordinates,
	  // use local byte registers for faster juggling
	int index = (y >> 3) * CANV_WIDTH + x;
	
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
		
		canvas->maskBuffer(index, mask, drawMode);
		index += CANV_WIDTH;
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
				canvas->maskBuffer(index, 0xFF, drawMode);
				index += CANV_WIDTH; // Advance pointer 8 rows
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
			canvas->maskBuffer(index, mask, drawMode);	
		}
	}
}

void Graphics::drawPixel(Canvas* canvas,
                         Point* point, 
                         DrawMode drawMode)
{	
	canvas->maskBuffer(point->x + (point->y >> 3) * canvas->getRectPtr()->width, (1 << (point->y & 0x07)), drawMode);
}


void Graphics::drawCharacter(Canvas* canvas,
                             Point location,
                             const Font* font, 
                             char character, 
                             DrawMode drawMode /* = DM_WHITE */)
{

	const int CANV_WIDTH = canvas->getWidth();
	const int CANV_HEIGHT = canvas->getHeight();
	const int FONT_WIDTH = font->getWidth();
	const int FONT_HEIGHT = font->getHeight();
	
	if (location.x < 0 || location.x > (CANV_WIDTH - FONT_WIDTH) || location.y < 0 || location.y > (CANV_HEIGHT - FONT_HEIGHT))
	{
		return;   
	}
	   
	const uint8_t* CHAR_BITMAP = font->getCharPtr(character);
	
	int row = location.y >> 3;
	uint8_t* CANV_BUFFER = canvas->getBufferPtr() + (row * CANV_WIDTH + location.x);
	int charIndex = 0;
	while (charIndex < font->getCharSize())
	{
		std::copy(&CHAR_BITMAP[charIndex], &CHAR_BITMAP[charIndex + FONT_WIDTH], CANV_BUFFER);
		CANV_BUFFER += CANV_WIDTH;
		charIndex += FONT_WIDTH;
	}
}


void Graphics::drawRect(Canvas* canvas,
                        Rect rect, 
                        DrawMode drawMode /* = DM_WHITE */)
{
	rect.height--;
	rect.width--;
	drawHorizontalLine(canvas,
	                   rect.x,
	                   rect.y,
	                   rect.width,
	                   drawMode);
	
	drawHorizontalLine(canvas,
	                   rect.x,
	                   rect.y + rect.height,
	                   rect.width,
	                   drawMode);
	
	drawVerticalLine(canvas,
	                 rect.x,
	                 rect.y,
	                 rect.height,
	                 drawMode);
	
	drawVerticalLine(canvas,
	                 rect.x + rect.width,
	                 rect.y,
	                 rect.height,
	                 drawMode);
}
