#pragma once
#include <Graphics.h>
#include <string>
#include <vector>
#include <Font.h>

class Label
{
	public:
	Label(const uint8_t id,
	      const Font* font, 
	      graphics::Size constrainSize, 
	      graphics::Rect rect,
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
		return { RECT_.x, RECT_.y };
	}
	
	inline const graphics::Size getSize() const
	{
		return { RECT_.width, RECT_.height };
	}
	
	void setText(std::string text);
	inline void paint()
	{
		paint({ 0, 0, RECT_.width, RECT_.height });
	}
	
	void paint(graphics::Rect rect);
	const uint8_t ID;
	const uint8_t Z_ORDER;
	
	protected:
	
	
	private:
	graphics::Rect initRect(graphics::Rect* rect);
	void drawBorder(graphics::Graphics* g, graphics::Point location, int column, int row);
	void(*paintLabel_)(Label*, graphics::Graphics*) = NULL;
	const Font* FONT_ = NULL;
	const graphics::Size CONSTRAIN_SIZE_;
	const graphics::Rect RECT_;
	char** charMap_ = NULL;
	const int COLUMNS_ = 0;
	const int ROWS_ = 0;
	bool border_ = false;
	graphics::StringAlignment alignment_ = graphics::SA_NEAR;
	graphics::StringAlignment lineAlignment_ = graphics::SA_NEAR;
	graphics::Colour backColour_ = graphics::CO_BLACK;
	graphics::Colour foreColour_ = graphics::CO_WHITE;
	graphics::DrawMode drawMode_ = graphics::DM_OR_MASK;
	
};