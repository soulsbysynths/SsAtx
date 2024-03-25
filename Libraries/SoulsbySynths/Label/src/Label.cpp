#include "Label.h"
Label::Label(graphics::Rect rect,
             std::string text, 
             const Font* font, 
             graphics::StringAlignment alignment,
             graphics::StringAlignment lineAlignment,
             bool border, 
             graphics::DrawMode drawMode, 
             void(*paint)(Label*, graphics::Graphics*))
	: text_(text)
	, font_(font)
	, border_(border)
	, alignment_(alignment)
	, lineAlignment_(lineAlignment)
	, drawMode_(drawMode)
	, paint_(paint)
{
	using namespace graphics;
}

Label::~Label(void) 
{

}

void Label::paint()
{
	
}