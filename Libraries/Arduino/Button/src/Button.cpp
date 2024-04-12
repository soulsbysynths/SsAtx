#include "Button.h"

Button::Button(const uint8_t id,
               const Font* font, 
               const graphics::Size* constrainSize, 
               const graphics::Rect* rect,
               std::string text, 
               void(*paintControl)(Control*, graphics::Graphics*),
               const bool pressed,
               const bool selected,
               const uint8_t zOrder)
	: Label(id, 
	        font, 
	        constrainSize, 
	        rect, 
	        text, 
	        paintControl, 
	        graphics::SA_CENTRE, 
	        graphics::SA_CENTRE, 
	        false, 
	        graphics::CO_BLACK,
	        graphics::CO_WHITE,
	        getPressedDrawMode(),
	        zOrder)
	, pressed_(pressed)
	, selected_(selected)
{
}

void Button::paint(graphics::Rect* rect)
{
	using namespace graphics;
	// Make sure rect conforms to size constraints
	
	Graphics::constrainRect(rect, &RECT_.size, FONT_->getSizeBitShiftPtr()); //constrain paint rect to label rect
	
	Graphics graphics( 
	                  {
	                  {
		                  RECT_.location.x + rect->location.x, 
		                  RECT_.location.y + rect->location.y
	                  }, 
		                  rect->size
	                  }, 
	                  CONSTRAIN_SIZE_,
	                  CO_BLACK);
	
	Label::paint(rect, &graphics);

	Line line;
	line.start.x = 0;
	line.start.y = 2;
	line.end.x = 0;
	line.end.y = RECT_.size.height - 3;
	

	//graphics.drawRect({ 0, 0, graphics.getRectPtr()->width, graphics.getRectPtr()->height }, CO_DARKGREY, DM_OR_MASK);
	paintControl_(this, &graphics);	
	
}