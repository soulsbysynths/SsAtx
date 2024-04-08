#include "Label.h"
Label::Label(const uint8_t id,
             const Font* font, 
             const graphics::Size* constrainSize, 
             const graphics::Rect* rect,
             std::string text, 
             void(*paintLabel)(Label*, graphics::Graphics*),
             graphics::StringAlignment alignment,
             graphics::StringAlignment lineAlignment,
             bool border, 
             graphics::Colour backColour,
             graphics::Colour foreColour,
             graphics::DrawMode drawMode,
             const uint8_t zOrder)
	: ID(id)
	, FONT_(font)
	, CONSTRAIN_SIZE_(constrainSize)
	, RECT_(initRect(rect))
	, paintLabel_(paintLabel)
	, alignment_(alignment)
	, lineAlignment_(lineAlignment)
	, border_(border)
	, backColour_(backColour)
	, foreColour_(foreColour)
	, drawMode_(drawMode)
	, Z_ORDER(zOrder)
	, GRID_SIZE_({
	             RECT_.size.width >> font->SIZE_BIT_SHIFT.width, 
	             RECT_.size.height >> font->SIZE_BIT_SHIFT.height
	             })

{
	//	charMap_ = new char*[GRID_SIZE_.columns];
	//	for (int i = 0; i < GRID_SIZE_.columns; i++)
	//	{
	//		charMap_[i] = new char[GRID_SIZE_.rows];
	//	}
	
	for (int r = 0; r < GRID_SIZE_.rows; r++)
	{
		text_.push_back(std::string(GRID_SIZE_.columns, '\a')); //fill with an unused char to force update when setting text.
	}
	
	
	setText(text);
	
	//	for (int j = 0; j < GRID_SIZE_.rows; j++)
	//	{
	//		for (int i = 0; i < GRID_SIZE_.columns; i++)
	//		{
	//			charMap_[i][j] = 'a' + (j * GRID_SIZE_.columns) + i;
	//		}
	//	}
	//	
		//paint();
		//		graphics::Rect r;
		//		r.x = RECT_.width / 2;
		//		r.y = RECT_.height / 2;
		//		r.width = RECT_.width - r.x;
		//		r.height = RECT_.height - r.y;
		//		paint(r);
}

Label::~Label(void) 
{
	//	for (int i = 0; i < GRID_SIZE_.columns; i++)
	//	{
	//		delete[] charMap_[i];
	//	}
	//	
	//	delete[] charMap_;
}

void Label::setText(std::string text)
{
	using namespace graphics;
	
	size_t pos = 0;
	std::vector<std::string> newText;
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
					paint(c, r);
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
						paint(c, r);
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
						paint(c, r);
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
					paint(c, r);
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
				paint(c, r);
			}
			
			c++;
		}
		
		r++;
	}
}

void Label::paint(graphics::Rect rect)
{
	using namespace graphics;
	
	// Make sure rect conforms to size constraints
	
	Graphics::constrainRect(&rect, &RECT_.size, &FONT_->SIZE_BIT_SHIFT); //constrain paint rect to label rect
	
	Graphics graphics( {
	                  {
		                  RECT_.location.x + rect.location.x, 
		                  RECT_.location.y + rect.location.y
	                  }, 
		                  rect.size
	                  }, 
	                  CONSTRAIN_SIZE_,
	                  backColour_);
	
	uint8_t r = rect.location.y >> FONT_->SIZE_BIT_SHIFT.height;
	const GridSize GRID_SIZE = { 
		rect.size.width >> FONT_->SIZE_BIT_SHIFT.width,
		rect.size.height >> FONT_->SIZE_BIT_SHIFT.height
	};
	
	for (uint8_t j = 0; j < GRID_SIZE.rows; j++)
	{
		uint8_t c = rect.location.x >> FONT_->SIZE_BIT_SHIFT.width;
		for (uint8_t i = 0; i < GRID_SIZE.columns; i++)
		{
			Point location = 
			{
				i << FONT_->SIZE_BIT_SHIFT.width,
				j << FONT_->SIZE_BIT_SHIFT.height
			};
			graphics.drawCharacter(location,
			                       FONT_, 
			                       text_[r][c], 
			                       foreColour_,
			                       drawMode_);
			if (border_)
			{
				drawBorder(&graphics, location, c, r);
			}
			c++;
		}
		r++;
	}

	//graphics.drawRect({ 0, 0, graphics.getRectPtr()->width, graphics.getRectPtr()->height }, CO_DARKGREY, DM_OR_MASK);
	paintLabel_(this, &graphics);	
}

void Label::drawBorder(graphics::Graphics* g, graphics::Point location, int column, int row)
{
	using namespace graphics;
	
	if (column == 0)
	{
		g->drawLine(location,
		            {
			            location.x,
			            location.y + FONT_->getHeight()
		            },
		            CO_GREY, 
		            DM_OR_MASK);
	}
				
	if (column == (GRID_SIZE_.columns - 1))
	{
		g->drawLine(
		            {
			            location.x + FONT_->getWidth() - 1,
			            location.y
		            },
		            {
			            location.x + FONT_->getWidth() - 1,
			            location.y + FONT_->getHeight()
		            },
		            CO_GREY,
		            DM_OR_MASK);
	}
				
	if (row == 0)
	{
		g->drawLine(location,
		            {
			            location.x + FONT_->getWidth(),
			            location.y
		            },
		            CO_GREY,
		            DM_OR_MASK);
	}
				
	if (row == (GRID_SIZE_.rows - 1))
	{
		g->drawLine(
		            {
			            location.x,
			            location.y + FONT_->getHeight() - 1
		            },
		            {
			            location.x + FONT_->getWidth(),
			            location.y + FONT_->getHeight() - 1
		            },
		            CO_GREY,
		            DM_OR_MASK);
	}
}

graphics::Rect Label::initRect(const graphics::Rect* rect)
{
	using namespace graphics;
	Rect constrainedRect = Graphics::constrainRectGraphics(rect, CONSTRAIN_SIZE_);
	Graphics::constrainRect(&constrainedRect, CONSTRAIN_SIZE_, &FONT_->SIZE_BIT_SHIFT);
	return constrainedRect;
}