#include "Label.h"

using namespace graphics;

Label::Label(const uint8_t id,
             const Font* font, 
             const Size* constrainSize, 
             const Rect* rect,
             std::string text, 
             void(*paintControl)(Control*, Graphics*),
             StringAlignment alignment,
             StringAlignment lineAlignment,
             bool border, 
             Colour backColour,
             Colour foreColour,
             DrawMode drawMode,
             const uint8_t zOrder)
	: Control(id, 
	          constrainSize, 
	          Graphics::initRect(rect, constrainSize, font), 
	          paintControl, 
	          zOrder)
	, FONT_(font)
	, alignment_(alignment)
	, lineAlignment_(lineAlignment)
	, border_(border)
	, backColour_(backColour)
	, foreColour_(foreColour)
	, drawMode_(drawMode)
	, GRID_SIZE_( {
	             RECT_.size.width >> font->getSizeBitShiftWidth(), 
	             RECT_.size.height >> font->getSizeBitShiftHeight()
	             })

{
	for (int r = 0; r < GRID_SIZE_.rows; r++)
	{
		text_.push_back(std::string(GRID_SIZE_.columns, '\a')); //fill with an unused char to force update when setting text.
	}
	
	setText(text);
}

void Label::setText(std::string text)
{
	size_t pos = 0;
	std::vector<std::string> newText;
	Grid paintGrid = {};
	const char LF = '\n';

	while ((pos = text.find(LF)) != std::string::npos)
	{
		newText.push_back(text.substr(0, pos));
		text.erase(0, pos + sizeof(LF));
	}
	newText.push_back(text);
	
	const int NEW_GRID_ROWS = newText.size();
	int startR;
	switch (lineAlignment_)
	{
		case SA_NEAR:
			startR = 0;
			break;
		case SA_CENTRE:
			startR = (GRID_SIZE_.rows - NEW_GRID_ROWS) / 2;
			break;
		case SA_FAR:
			startR = GRID_SIZE_.rows - NEW_GRID_ROWS;
			break;
	}
	
	int r = 0;
	// Pad empty rows until reach start row
	while (r < startR)
	{
		if (r >= 0 && r < GRID_SIZE_.rows)
		{
			int c = 0;
			while (c < GRID_SIZE_.columns)
			{
				if (text_[r][c] != '\0')
				{
					text_[r][c] = '\0';
					Graphics::enlargeGrid(&paintGrid, { c, r });
				}
				
				c++;
			}
		}
		
		r++;
	}
	
	//draw new rows
	for (int newR = 0; newR < NEW_GRID_ROWS; newR++)	
	{
		if (r >= 0 && r < GRID_SIZE_.rows)
		{
			const int NEW_GRID_COLUMNS = newText[newR].length();
			int startC;
			switch (alignment_)
			{
				case SA_NEAR:
					startC = 0;
					break;
				case SA_CENTRE:
					startC = (GRID_SIZE_.columns - NEW_GRID_COLUMNS) / 2;
					break;
				case SA_FAR:
					startC = GRID_SIZE_.columns - NEW_GRID_COLUMNS;
					break;
			}
		
			int c = 0;
			// Pad empty columns until reach start column
			while (c < startC)
			{
				if (c >= 0 && c < GRID_SIZE_.columns)
				{	
					if (text_[r][c] != '\0')
					{
						text_[r][c] = '\0';
						Graphics::enlargeGrid(&paintGrid, { c, r });
					}
				}
				
				c++;
			}

			// Draw new columns
			for (int newC = 0; newC < NEW_GRID_COLUMNS; newC++)
			{
				if (c >= 0 && c < GRID_SIZE_.columns)
				{			
					if (text_[r][c] != newText[newR][newC])
					{
						text_[r][c] = newText[newR][newC];
						Graphics::enlargeGrid(&paintGrid, { c, r });
					}
				}
				
				c++;
			}
			
			// Pad empty columns to end of row
			while (c < GRID_SIZE_.columns)
			{
				if (text_[r][c] != '\0')
				{
					text_[r][c] = '\0';
					Graphics::enlargeGrid(&paintGrid, { c, r });
				}
				
				c++;
			}
		}
		
		r++;
	}
	
	// Pad empty row to end of rect
	while (r < GRID_SIZE_.rows)
	{
		int c = 0;
		while (c < GRID_SIZE_.columns)
		{
			if (text_[r][c] != '\0')
			{
				text_[r][c] = '\0';
				Graphics::enlargeGrid(&paintGrid, { c, r });
			}
			
			c++;
		}
		
		r++;
	}
	
	Graphics::enlargeRect(&paintRect_, &paintGrid, FONT_->getSizeBitShiftPtr());
}



void Label::paintGraphics(Rect* rect, Graphics* graphics)
{
	uint8_t r = rect->location.y >> FONT_->getSizeBitShiftHeight();
	const GridSize GRID_SIZE = 
	{ 
		rect->size.width >> FONT_->getSizeBitShiftWidth(),
		rect->size.height >> FONT_->getSizeBitShiftHeight()
	};
	
	for (uint8_t j = 0; j < GRID_SIZE.rows; j++)
	{
		uint8_t c = rect->location.x >> FONT_->getSizeBitShiftWidth();
		for (uint8_t i = 0; i < GRID_SIZE.columns; i++)
		{
			Point location = 
			{
				i << FONT_->getSizeBitShiftWidth(),
				j << FONT_->getSizeBitShiftHeight()
			};
			
			graphics->drawCharacter(location,
			                        FONT_, 
			                        text_[r][c], 
			                        foreColour_,
			                        drawMode_);
			if (border_)
			{
				drawBorder(graphics, location, c, r);
			}
			
			c++;
		}
		r++;
	}

}

void Label::paint(Rect* rect)
{
	// Make sure rect conforms to size constraints
	Graphics::constrainRect(rect, &RECT_.size, FONT_->getSizeBitShiftPtr()); //constrain paint rect to label rect
	Graphics graphics( { 
	                  {
		                  RECT_.location.x + rect->location.x, 
		                  RECT_.location.y + rect->location.y
	                  }, 
		                  rect->size
	                  }, 
	                  CONSTRAIN_SIZE_,
	                  backColour_);
	
	paintGraphics(rect, &graphics);
	paintControl_(this, &graphics);	
}

void Label::drawBorder(Graphics* g, Point location, int column, int row)
{
	static const Colour colour = CO_WHITE;
	static const DrawMode drawMode = DM_OR_MASK;
	
	if (column == 0)
	{
		g->drawLine( {
			            location,
		            {
			            location.x,
			            location.y + FONT_->getHeight()
		            }
		            },
		            colour, 
		            drawMode);
	}
				
	if (column == (GRID_SIZE_.columns - 1))
	{
		g->drawLine( {
		            {
			            location.x + FONT_->getWidth() - 1,
			            location.y
		            },
		            {
			            location.x + FONT_->getWidth() - 1,
			            location.y + FONT_->getHeight()
		            }
		            },
		            colour,
		            drawMode);
	}
				
	if (row == 0)
	{
		g->drawLine( {
			            location,
		            {
			            location.x + FONT_->getWidth(),
			            location.y
		            }
		            },
		            colour,
		            drawMode);
	}
				
	if (row == (GRID_SIZE_.rows - 1))
	{
		g->drawLine( {
		            {
			            location.x,
			            location.y + FONT_->getHeight() - 1
		            },
		            {
			            location.x + FONT_->getWidth(),
			            location.y + FONT_->getHeight() - 1
		            }
		            },
		            colour,
		            drawMode);
	}
}

