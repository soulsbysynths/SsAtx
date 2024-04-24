#include "Slider.h"

using namespace graphics;

const Font* Slider::FONT_ = &atx8x16Tiny;

Slider::Slider(const uint8_t id,
               const Size* clipSize, 
               const Rect* rect,
               const int minValue,
               const int maxValue,
               void(*paintControl)(Control*, Graphics*),
               int value,
               const uint8_t zOrder)
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
	Label::setText(parseValue(value));
	
	const int BAR_W = getRectPtr()->size.width - 2;
	
	int oldFillW = (value_ - MIN_VALUE_) * BAR_W / (MAX_VALUE_ - MIN_VALUE_);
	int newFillW = (value - MIN_VALUE_) * BAR_W / (MAX_VALUE_ - MIN_VALUE_);
	int changeW = newFillW - oldFillW;
	
	if (changeW < 0)
	{
		changeW *= -1;
		oldFillW = newFillW;
	}
	
	Rect changed = 
	{ 
	{
		(oldFillW + 1),
		0
	},		
	{
		changeW,
		getRectPtr()->size.height
	}
	};
	
	addChanged(&changed);
	
	value_ = value;
}

std::string Slider::parseValue(int value)
{
	if (MIN_VALUE_ < 0 && value > 0)
	{
		return "+" + std::to_string(value) ;
	}
	else
	{
		return std::to_string(value) ;
	}
}

void Slider::paintGraphics(Graphics* graphics) 
{
	int fillX = (value_ - MIN_VALUE_) * (getRectPtr()->size.width - 2) / (MAX_VALUE_ - MIN_VALUE_);
	Rect fillRect = 
	{ 
	{ 
		1, 
		2 
	},
	{
		fillX,
		getRectPtr()->size.height - 4
	}
	};
	
	graphics::clip(&fillRect, getChangedPtr());
	
	if (graphics::isContained(getChangedPtr(), &fillRect))
	{
		graphics->fillRect(fillRect, CO_GREY, DM_MASK);
	}
	
	fillRect = 
	{ 
	{ 
		fillX, 
		2 
	},
	{
		getRectPtr()->size.width - fillX - 1,
		getRectPtr()->size.height - 4
	}
	};
	
	graphics::clip(&fillRect, getChangedPtr());
	
	if (graphics::isContained(getChangedPtr(), &fillRect))
	{
		graphics->drawRect(fillRect, CO_BLACK, DM_MASK);
	}
	
	if (isAllChanged())
	{
		Rect sliderRect = getControlRect();
		graphics->drawRoundRect(sliderRect,4);	
	}
}