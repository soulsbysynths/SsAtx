#include "Slider.h"

using namespace graphics;

const Font* FONT_ = &atx8x16Tiny;

Slider::Slider(const uint8_t id,
               const Size* clipSize, 
               const Rect* rect,
               const int minValue,
               const int maxValue,
               void(*paintControl)(Control*, Graphics*),
               int value,
               const uint8_t zOrder) try
	: Label(id, 
	        FONT_, 
	        clipSize, 
			rect,
	        parseValue(value), 
	        paintControl, 
	        SA_NEAR, 
	        SA_CENTRE, 
	        false, 
	        CO_BLACK,
	        CO_WHITE,
	        DM_XOR_MASK,
	        zOrder)
	, MIN_VALUE_(minValue)
	, MAX_VALUE_(maxValue)

{	
	setValue(value);
}

void Slider::setValue(int value)
{
}

std::string Slider::parseValue(int value)
{
	if (MIN_VALUE_ < 0 && value)
	{
		return "+" + std::to_string(value);
	}
	else
	{
		return std::to_string(value);
	}
}

void Slider::paint(Rect* rect)
{
	// Make sure rect conforms to size constraints
	graphics::clip(rect, &RECT_.size, FONT_->getSizeBitShiftPtr()); //constrain paint rect to label rect
	
	Graphics graphics( { 
	                  { 
		                  RECT_.location.x + rect->location.x, 
		                  RECT_.location.y + rect->location.y
	                  }, 
		                  rect->size
	                  }, 
	                  CLIP_SIZE_);

	paintGraphics(rect, &graphics);
	paintControl_(this, &graphics);	
}

void Slider::paintGraphics(Rect* rect, Graphics* graphics) 
{
	Rect buttonRect = 
	{ 
		{ 0, 0 },
		RECT_.size 
	};
	
	
	
	Label::setDrawMode(getPressedDrawMode());
	
	if (pressed_)
	{
		// Only paint if painting the whole button.
		if (!isEqual(&buttonRect, rect))
		{
			return;
		}
		
		graphics->fillRoundRect(buttonRect, RADIUS_);		
		Label::paintGraphics(rect, graphics);
		return;		
	}

	Label::paintGraphics(rect, graphics);
	
	Colour colour = CO_WHITE;
	
	for (int i = 0; i < 2; i++)
	{
		for (int s = SI_TOP; s < SI_MAX; s++)
		{
			Rect sideRect = getRoundRectSideRect(&buttonRect, RADIUS_, (Side)s);
			if (isContained(rect, &sideRect))
			{
				graphics->drawLine( {
					                   sideRect.location,
				                   {
					                   sideRect.location.x + sideRect.size.width,
					                   sideRect.location.y + sideRect.size.height
				                   }
				                   },
				                   colour);
			}
		}
	
		for (int c = CQ_TOPLEFT; c < CQ_MAX; c++)
		{
			const Rect circleQuarterRect = getRoundRectCircleQuarterRect(&buttonRect, RADIUS_, (CircleQuarter)c);
			if (isContained(rect, &circleQuarterRect))
			{
				graphics->drawCircle(circleQuarterRect.location, 
				                     RADIUS_, 
				                     (CircleQuarterFlags)(1 << c),
				                     colour);
			}
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