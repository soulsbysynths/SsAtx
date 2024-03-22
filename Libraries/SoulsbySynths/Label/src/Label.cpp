#include "Label.h"
Label::Label(graphics::Rect rect, 
             graphics::Size outputSize,
             std::string text, 
             const Font* font, 
             graphics::StringAlignment alignment,
             graphics::StringAlignment lineAlignment,
             bool border, 
             graphics::DrawMode drawMode, 
             void(*paint)(Label*),
             graphics::Rect quantizeRectBitShift)
	: text_(text)
	, font_(font)
	, border_(border)
	, alignment_(alignment)
	, lineAlignment_(lineAlignment)
	, drawMode_(drawMode)
	, paint_(paint)
{
	using namespace graphics;
	
	canvas_ = new Canvas(rect, outputSize, 0x00, quantizeRectBitShift);
}

Label::~Label(void) 
{
	delete canvas_;
}

void Label::paint()
{
	
}