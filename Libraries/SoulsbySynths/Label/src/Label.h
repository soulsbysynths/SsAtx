#pragma once
#include <Graphics.h>
#include <Control.h>
#include <string>
#include <algorithm>
#include <vector>
#include <Font.h>

class Label : public Control
{
	public:
	Label(const uint8_t id,
	      const Font* font, 
	      const graphics::Size* constrainSize, 
	      const graphics::Rect* rect,
	      std::string text, 
	      void(*paintControl)(Control*, graphics::Graphics*),
	      graphics::StringAlignment alignment = graphics::SA_NEAR,
	      graphics::StringAlignment lineAlignment = graphics::SA_NEAR,
	      bool border = false,
	      graphics::Colour backColour = graphics::CO_BLACK,
	      graphics::Colour foreColour = graphics::CO_WHITE,
	      graphics::DrawMode drawMode = graphics::DM_OR_MASK,
	      const uint8_t zOrder = 0);
	
	~Label(void)
	{
	}
		
	void setText(std::string text);
	void paint(graphics::Rect* rect);
	
	protected:
	void paint(graphics::Rect* rect, graphics::Graphics* graphics);
	const Font* FONT_ = NULL;
	
	private:
	void paint(graphics::Grid* grid);
	void drawBorder(graphics::Graphics* g, graphics::Point location, int column, int row);
	const graphics::GridSize GRID_SIZE_;
	bool border_ = false;
	std::vector<std::string> text_;
	graphics::StringAlignment alignment_ = graphics::SA_NEAR;
	graphics::StringAlignment lineAlignment_ = graphics::SA_NEAR;
	graphics::Colour backColour_ = graphics::CO_BLACK;
	graphics::Colour foreColour_ = graphics::CO_WHITE;
	graphics::DrawMode drawMode_ = graphics::DM_OR_MASK;
	
};