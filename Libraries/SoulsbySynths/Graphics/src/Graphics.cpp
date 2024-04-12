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

void Graphics::drawLine(Line line, 
                        Colour colour, /* CO_WHITE*/
                        DrawMode drawMode /* = DM_OR_MASK */)
{
	if (line.start.x == line.end.x) 
	{
		if (line.start.y > line.end.y)
		{
			std::swap(line.start.y, line.end.y);
		}
		
		drawVerticalLine(line.start, line.end.y - line.start.y, colour, drawMode);
	}
	else if (line.start.y == line.end.y) 
	{
		if (line.start.x > line.end.x)
		{
			std::swap(line.start.x, line.end.x);
		}
		
		drawHorizontalLine(line.start, line.end.x - line.start.x, colour, drawMode);
	}
	else
	{
		int steep = abs(line.end.y - line.start.y) > abs(line.end.x - line.start.x);
		if (steep) 
		{
			std::swap(line.start.x, line.start.y);
			std::swap(line.end.x, line.end.y);
		}

		if (line.start.x > line.end.x) 
		{
			std::swap(line.start.x, line.end.x);
			std::swap(line.start.y, line.end.y);
		}

		int dx, dy;
		dx = line.end.x - line.start.x;
		dy = abs(line.end.y - line.start.y);

		int err = dx / 2;
		int ystep;

		if (line.start.y < line.end.y) 
		{
			ystep = 1;
		}
		else 
		{
			ystep = -1;
		}

		for (; line.start.x <= line.end.x; line.start.x++)
		{
			if (steep) 
			{
				Point swapped = { line.start.y, line.start.x };
				drawPixel(&swapped, colour, drawMode);
			}
			else 
			{
				drawPixel(&line.start, colour, drawMode);
			}
			err -= dy;
			if (err < 0) 
			{
				line.start.y += ystep;
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
		
	if (startPoint.x >= RECT_.size.width || 
	    width <= 0 || 
	    startPoint.y < 0 || 
	    startPoint.y >= RECT_.size.height)
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
		
	if (startPoint.y >= RECT_.size.height || 
	    height <= 0 || 
	    startPoint.x < 0 || 
	    startPoint.x >= RECT_.size.width)
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
		static const uint8_t PREMASK[8] = 
		{
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
			static const uint8_t POSTMASK[8] = 
			{ 
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
	
	if (location.x < 0 || 
	    location.x > (RECT_.size.width - FONT_WIDTH) || 
	    location.y < 0 || 
	    location.y > (RECT_.size.height - FONT_HEIGHT))
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

void Graphics::drawCircle(Point location,
                          int radius,
                          CircleQuarter quarters,
                          Colour colour,
                          DrawMode drawMode /* = DM_OR_MASK */)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (quarters & CQ_BOTTOMRIGHT)
		{
			drawPixel( { 
				          location.x + x, 
				          location.y + y 
			          }, 
			          colour, 
			          drawMode);
			drawPixel( {
				          location.x + y, 
				          location.y + x
			          },
			          colour, 
			          drawMode);
		}
		
		if (quarters & CQ_TOPRIGHT)
		{
			drawPixel( {
				          location.x + x, 
				          location.y - y
			          },
			          colour,
			          drawMode);
			drawPixel( {
				          location.x + y, 
				          location.y - x
			          },
			          colour,
			          drawMode);
		}
		
		if (quarters & CQ_BOTTOMLEFT)
		{
			drawPixel( {
				          location.x - y, 
				          location.y + x 
			          },
			          colour,
			          drawMode);
			drawPixel( {
				          location.x - x, 
				          location.y + y 
			          },
			          colour,
			          drawMode);
		}
		
		if (quarters & CQ_TOPLEFT)
		{
			drawPixel( {
				          location.x - y, 
				          location.y - x 
			          }, 
			          colour,
			          drawMode);
			drawPixel( {
				          location.x - x, 
				          location.y - y
			          },
			          colour,
			          drawMode);
		}
	}
}

void Graphics::drawRoundRect(Rect rect,
                             int radius,
                             Colour colour,
                             DrawMode drawMode /* = DM_OR_MASK */)
{
	int16_t maxRadius = ((rect.size.width < rect.size.height) ? rect.size.width : rect.size.height) / 2; // 1/2 minor axis
	if (radius > maxRadius)
	{
		radius = maxRadius;
	}

	drawHorizontalLine( {
		                   rect.location.x + radius, 
		                   rect.location.y
	                   }, 
	                   rect.size.width - 2 * radius, 
	                   colour,
	                   drawMode); // Top
	drawHorizontalLine( {
		                   rect.location.x + radius, 
		                   rect.location.y + rect.size.height - 1
	                   },
	                   rect.size.width - 2 * radius, 
	                   colour,
	                   drawMode); // Bottom
	drawVerticalLine( {
		                 rect.location.x, 
		                 rect.location.y + radius
	                 }, 
	                 rect.size.height - 2 * radius, 
	                 colour,
	                 drawMode); // Left
	drawVerticalLine( {
		                 rect.location.x + rect.size.width - 1, 
		                 rect.location.y + radius
	                 }, 
	                 rect.size.height - 2 * radius, 
	                 colour,
	                 drawMode); // Right
	drawCircle( {
		           rect.location.x + radius, 
		           rect.location.y + radius
	           },
	           radius,
	           CQ_TOPLEFT,
	           colour,
	           drawMode);
	drawCircle( {
		           rect.location.x + rect.size.width - radius - 1, 
		           rect.location.y + radius
	           },
	           radius, 
	           CQ_TOPRIGHT, 
	           colour,
	           drawMode);
	drawCircle( {
		           rect.location.x + rect.size.width - radius - 1, 
		           rect.location.y + rect.size.height - radius - 1
	           }, 
	           radius, 
	           CQ_BOTTOMRIGHT, 
	           colour,
	           drawMode);
	drawCircle( {
		           rect.location.x + radius, 
		           rect.location.y + rect.size.height - radius - 1
	           },
	           radius, 
	           CQ_BOTTOMLEFT,
	           colour,
	           drawMode);
}

void Graphics::drawBuffer(int startIndex, 
                          const size_t size, 
                          const uint8_t mask, 
                          const Colour colour,
                          DrawMode drawMode)
{	
	if (startIndex < 0 || (startIndex + size) > getBufferSize())
	{
		return;
	}
			
	drawArray(&buffer_[startIndex], 
	          &buffer_[startIndex + size], 
	          &buffer_[startIndex], 
	          mask, 
	          colour,
	          drawMode, 
	          startIndex);
}
		
void Graphics::drawBuffer(int startIndex, 
                          const size_t size, 
                          const uint8_t* mask, 
                          const size_t maskSize,
                          const Colour colour,
                          DrawMode drawMode)
{	
	if (startIndex < 0 || (startIndex + size) > getBufferSize())
	{
		return;
	}
			
	drawArray(&buffer_[startIndex], 
	          &buffer_[startIndex + size], 
	          &buffer_[startIndex], 
	          mask, 
	          &mask[maskSize], 
	          colour, 
	          drawMode, 
	          startIndex);
}

void Graphics::drawArray(const uint8_t* inBegin, 
                         const uint8_t* inEnd, 
                         uint8_t* outBegin,
                         const uint8_t* maskBegin,
                         const uint8_t* maskEnd,
                         const Colour colour,
                         const DrawMode drawMode,
                         int patternIndex)
{		
	while (inBegin != inEnd && maskBegin != maskEnd)
	{
		*outBegin = drawByte(*inBegin, *maskBegin, colour, drawMode, patternIndex);
		++outBegin; 
		++inBegin;
		++maskBegin;
		++patternIndex;
	}
}
		
void Graphics::drawArray(const uint8_t* inBegin, 
                         const uint8_t* inEnd, 
                         uint8_t* outBegin,
                         const uint8_t mask,
                         const Colour colour,
                         const DrawMode drawMode,
                         int patternIndex)
{		
	while (inBegin != inEnd)
	{
		*outBegin = drawByte(*inBegin, mask, colour, drawMode, patternIndex);
		++outBegin; 
		++inBegin;
		++patternIndex;
	}
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

Rect Graphics::initRect(const Rect* rect, const Size* constrainSize, const Font* font)
{
	Rect constrainedRect = Graphics::constrainRectGraphics(rect, constrainSize);
	Graphics::constrainRect(&constrainedRect, constrainSize, font->getSizeBitShiftPtr());
	return constrainedRect;
}

void Graphics::enlargeGridToIncludeLocation(Grid* grid, const GridLocation location)
{
	if (grid->size.columns == 0 && grid->size.rows == 0)
	{
		// Must be first cell
		grid->location = location;
		grid->size = { 1, 1 };
		return;
	}

	GridLocation endLocation = 
	{
		grid->location.column + grid->size.columns - 1, 
		grid->location.row + grid->size.rows - 1
	};
		
	if (location.column < grid->location.column)
	{
		grid->location.column = location.column;
	}
	else if (location.column > endLocation.column)
	{
		endLocation.column = location.column;
	}
	
	grid->size.columns = endLocation.column - grid->location.column + 1;
	
	if (location.row < grid->location.row)
	{
		grid->location.row = location.row;
	}
	else if (location.row > endLocation.row)
	{
		endLocation.row = location.row;
	}
	
	grid->size.rows = endLocation.row - grid->location.row + 1;
}