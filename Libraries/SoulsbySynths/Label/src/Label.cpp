#include "Label.h"
Label::Label(const uint8_t id,
             const Font* font, 
             graphics::Size constrainSize, 
             graphics::Rect rect,
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
	, RECT_(initRect(&rect))
	, paintLabel_(paintLabel)
	, alignment_(alignment)
	, lineAlignment_(lineAlignment)
	, border_(border)
	, backColour_(backColour)
	, foreColour_(foreColour)
	, drawMode_(drawMode)
	, Z_ORDER(zOrder)
	, COLUMNS_(RECT_.width >> font->SIZE_BIT_SHIFT.width)
	, ROWS_(RECT_.height >> font->SIZE_BIT_SHIFT.height)

{
	charMap_ = new char*[COLUMNS_];
	for (int i = 0; i < COLUMNS_; i++)
	{
		charMap_[i] = new char[ROWS_];
	}
	
	for (int j = 0; j < ROWS_; j++)
	{
		for (int i = 0; i < COLUMNS_; i++)
		{
			charMap_[i][j] = 'a' + (j * COLUMNS_) + i;
		}
	}
	
	paint();
	//		graphics::Rect r;
	//		r.x = RECT_.width / 2;
	//		r.y = RECT_.height / 2;
	//		r.width = RECT_.width - r.x;
	//		r.height = RECT_.height - r.y;
	//		paint(r);
}

Label::~Label(void) 
{
	for (int i = 0; i < COLUMNS_; i++)
	{
		delete[] charMap_[i];
	}
	
	delete[] charMap_;
}

void Label::setText(std::string text)
{
	using namespace graphics;
	
	char newCharMap[COLUMNS_][ROWS_];
	memset(&newCharMap, ' ', sizeof(newCharMap));
	
	size_t pos;
	std::string line;
	std::vector<std::string> textSplit;
	int lines = 0;
	const std::string LF = "\n";

	while ((pos = text.find(LF)) != std::string::npos)
	{
		textSplit.push_back(text.substr(0, pos));
		text.erase(0, pos + LF.length());
		lines++;
	}
	
	int y;
	switch (lineAlignment_)
	{
		case SA_NEAR:
			y = 0;
			break;
		case SA_CENTRE:
			y = (ROWS_ - lines) / 2;
			break;
		case SA_FAR:
			y = ROWS_ - lines;
			break;
	}
	
}

void Label::paint(graphics::Rect rect)
{
	using namespace graphics;
	
	// Make sure rect conforms to size constraints
	Graphics::constrainRect(&rect, { RECT_.width, RECT_.height }, FONT_->SIZE_BIT_SHIFT); //constrain paint rect to label rect
	
	Graphics graphics(
	                  { 
		                  RECT_.x + rect.x, 
		                  RECT_.y + rect.y, 
		                  rect.width,
		                  rect.height
	                  }, 
	                  CONSTRAIN_SIZE_,
	                  backColour_);
	
	uint8_t r = rect.y >> FONT_->SIZE_BIT_SHIFT.height;
	const uint8_t COLUMNS = rect.width >> FONT_->SIZE_BIT_SHIFT.width;
	const uint8_t ROWS = rect.height >> FONT_->SIZE_BIT_SHIFT.height;
	
	for (uint8_t j = 0; j < ROWS; j++)
	{
		uint8_t c = rect.x >> FONT_->SIZE_BIT_SHIFT.width;
		for (uint8_t i = 0; i < COLUMNS; i++)
		{
			Point location = 
			{
				i << FONT_->SIZE_BIT_SHIFT.width,
				j << FONT_->SIZE_BIT_SHIFT.height
			};
			graphics.drawCharacter(location,
			                       FONT_, 
			                       charMap_[c][r], 
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
		            CO_WHITE, 
		            DM_OR_MASK);
	}
				
	if (column == (COLUMNS_ - 1))
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
		            CO_WHITE,
		            DM_OR_MASK);
	}
				
	if (row == 0)
	{
		g->drawLine(location,
		            {
			            location.x + FONT_->getWidth(),
			            location.y
		            },
		            CO_WHITE,
		            DM_OR_MASK);
	}
				
	if (row == (ROWS_ - 1))
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
		            CO_WHITE,
		            DM_OR_MASK);
	}
}

graphics::Rect Label::initRect(graphics::Rect* rect)
{
	using namespace graphics;
	Graphics::constrainRectGraphics(rect, CONSTRAIN_SIZE_);
	Graphics::constrainRect(rect, CONSTRAIN_SIZE_, FONT_->SIZE_BIT_SHIFT);
	return *rect;
}