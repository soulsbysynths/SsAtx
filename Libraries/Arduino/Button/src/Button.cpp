#include "Button.h"

using namespace graphics;

Button::Button(const uint8_t id,
               const Font* font, 
               const Size* constrainSize, 
               const Rect* rect,
               std::string text, 
               void(*paintControl)(Control*, Graphics*),
               const bool pressed,
               const bool selected,
               const uint8_t zOrder)
	: Label(id, 
	        font, 
	        constrainSize, 
	        rect, 
	        text, 
	        paintControl, 
	        SA_CENTRE, 
	        SA_CENTRE, 
	        false, 
	        CO_BLACK,
	        CO_WHITE,
	        getPressedDrawMode(),
	        zOrder)
	, pressed_(pressed)
	, selected_(selected)
{
}

void Button::paintGraphics(Graphics* graphics) 
{
	// Only paint if painting the whole button.
	if (!isAllChanged())
	{
		return;
	}
	
	Label::setDrawMode(getPressedDrawMode());
	
	Rect buttonRect = getControlRect();
	
	if (pressed_)
	{	
		graphics->fillRoundRect(buttonRect, RADIUS_);		
		Label::paintGraphics(graphics);
		return;		
	}

	Label::paintGraphics(graphics);
	
	Colour colour = CO_WHITE;
	
	for (int i = 0; i < 2; i++)
	{
		for (int s = SI_TOP; s < SI_MAX; s++)
		{
			Rect sideRect = getRoundRectSideRect(&buttonRect, RADIUS_, (Side)s);
			graphics->drawLine( {
				                   sideRect.location,
			                   {
				                   sideRect.location.x + sideRect.size.width,
				                   sideRect.location.y + sideRect.size.height
			                   }
			                   },
			                   colour);
		}
	
		for (int c = CQ_TOPLEFT; c < CQ_MAX; c++)
		{
			const Rect circleQuarterRect = getRoundRectCircleQuarterRect(&buttonRect, RADIUS_, (CircleQuarter)c);
			graphics->drawCircle(circleQuarterRect.location, 
			                     RADIUS_, 
			                     (CircleQuarterFlags)(1 << c),
			                     colour);
		}
		
		if (selected_)
		{
			buttonRect.location.x++;
			buttonRect.size.width -= 2;
			buttonRect.location.y++;
			buttonRect.size.height -= 2;
			colour = CO_GREY;
		}
		else
		{
			return;
		}
	}	
}