#pragma once
#include <Graphics.h>
#include <string>
#include <algorithm>
#include <vector>
#include <Font.h>

class Label
{
	public:
	Label(const uint8_t id,
	      const Font* font, 
	      const graphics::Size* constrainSize, 
	      const graphics::Rect* rect,
	      std::string text, 
	      void(*paintLabel)(Label*, graphics::Graphics*),
	      graphics::StringAlignment alignment = graphics::SA_NEAR,
	      graphics::StringAlignment lineAlignment = graphics::SA_NEAR,
	      bool border = false,
	      graphics::Colour backColour = graphics::CO_BLACK,
	      graphics::Colour foreColour = graphics::CO_WHITE,
	      graphics::DrawMode drawMode = graphics::DM_OR_MASK,
	      const uint8_t zOrder = 0);
	
	~Label(void);
	
	inline const graphics::Point getLocation() const
	{
		return RECT_.location;
	}
	
	inline const graphics::Size getSize() const
	{
		return RECT_.size;
	}
	
	void setText(std::string text);
	inline void paint()
	{
		paint({ { 0, 0 }, RECT_.size });
	}
	
	void paint(graphics::Rect rect);
	const uint8_t ID;
	const uint8_t Z_ORDER;
	
	protected:
	
	private:
	inline void paint(int column, int row)
	{
		paint({
			      column << FONT_->SIZE_BIT_SHIFT.width,
			      row << FONT_->SIZE_BIT_SHIFT.height,
			      FONT_->getWidth(),
			      FONT_->getHeight()
		      });
	}
	
	graphics::Rect initRect(const graphics::Rect* rect);
	void drawBorder(graphics::Graphics* g, graphics::Point location, int column, int row);
	void(*paintLabel_)(Label*, graphics::Graphics*) = NULL;
	const Font* FONT_ = NULL;
	const graphics::Size* CONSTRAIN_SIZE_;
	const graphics::Rect RECT_;
	const graphics::GridSize GRID_SIZE_;
	bool border_ = false;
	std::vector<std::string> text_;
	graphics::StringAlignment alignment_ = graphics::SA_NEAR;
	graphics::StringAlignment lineAlignment_ = graphics::SA_NEAR;
	graphics::Colour backColour_ = graphics::CO_BLACK;
	graphics::Colour foreColour_ = graphics::CO_WHITE;
	graphics::DrawMode drawMode_ = graphics::DM_OR_MASK;
	
};