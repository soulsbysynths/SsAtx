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
	, drawMode_(drawMode)
	, Z_ORDER(zOrder)
	, COLUMNS_(RECT_.width >> font->SIZE_BIT_SHIFT.width)
	, ROWS_(RECT_.height / font->SIZE_BIT_SHIFT.height)

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
	graphics::Rect r;
	r.x = RECT_.width / 2;
	r.y = RECT_.height / 2;
	r.width = RECT_.width - r.x;
	r.height = RECT_.height - r.y;
	paint(r);
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
	                  CONSTRAIN_SIZE_);
	
	int c = rect.x >> FONT_->SIZE_BIT_SHIFT.width;
	int r = rect.y >> FONT_->SIZE_BIT_SHIFT.height;
	int columns = rect.width >> FONT_->SIZE_BIT_SHIFT.width;
	int rows = rect.height >> FONT_->SIZE_BIT_SHIFT.height;
	
	for (int j = 0; j < rows; j++)
	{
		for (int i = 0; i < columns; i++)
		{
			graphics.drawCharacter({ i << FONT_->SIZE_BIT_SHIFT.width, j << FONT_->SIZE_BIT_SHIFT.height }, 
			                       FONT_, 
			                       charMap_[c][r], 
			                       drawMode_);
			c++;
		}
		r++;
		c = 0;
	}

	graphics.drawRect({ 0, 0, graphics.getRectPtr()->width, graphics.getRectPtr()->height }, DM_GREY);
	paintLabel_(this, &graphics);	
}

graphics::Rect Label::initRect(graphics::Rect* rect)
{
	using namespace graphics;
	Graphics::constrainRectGraphics(rect, CONSTRAIN_SIZE_);
	Graphics::constrainRect(rect, CONSTRAIN_SIZE_, FONT_->SIZE_BIT_SHIFT);
	return *rect;
}