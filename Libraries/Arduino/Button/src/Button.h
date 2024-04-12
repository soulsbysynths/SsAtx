#pragma once
#include <Graphics.h>
#include <Control.h>
#include <string>
#include <algorithm>
#include <vector>
#include <Font.h>
#include <Label.h>

class Button : public Label
{
	public:
	Button(const uint8_t id,
	      const Font* font, 
	      const graphics::Size* constrainSize, 
	      const graphics::Rect* rect,
	      std::string text, 
	      void(*paintControl)(Control*, graphics::Graphics*),
	      const bool pressed = false,
	      const bool selected = true,
	      const uint8_t zOrder = 0);
	
	~Button(void)
	{
	}
	
	void paint(graphics::Rect* rect);
	
	protected:
	
	private:
	
	inline const graphics::DrawMode getPressedDrawMode() const
	{
		return pressed_ ? graphics::DM_NOT_MASK : graphics::DM_MASK;
	}
	
	inline const graphics::Colour getSelectedColour() const
	{
		return selected_ ? graphics::CO_WHITE : graphics::CO_GREY;
	}
	
	bool pressed_ = false;
	bool selected_ = false;
};