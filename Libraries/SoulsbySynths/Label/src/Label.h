#pragma once
#include <Graphics.h>
#include <string>
#include <Font.h>

class Label
{
	public:
	Label(graphics::Rect rect, 
	      std::string text, 
	      const Font* font, 
	      graphics::StringAlignment alignment = graphics::SA_NEAR,
	      graphics::StringAlignment lineAlignment = graphics::SA_NEAR,
	      bool border = false, 
	      graphics::DrawMode drawMode = graphics::DM_WHITE, 
	      void(*paint)(Label*, graphics::Graphics*) = NULL);
	
	~Label(void);
	
	inline int getWidth()
	{
		return rect_.width;
	}
	
	inline int getHeight()
	{
		return rect_.height;
	}
	
	void setText(std::string text);
	
	inline std::string getText()
	{
		return text_;
	}
	
	private:
	void paint();
	graphics::Rect rect_;
	std::string text_ = "";
	const Font* font_ = NULL;
	void(*paint_)(Label*, graphics::Graphics*) = NULL;
	bool border_ = false;
	graphics::StringAlignment alignment_ = graphics::SA_NEAR;
	graphics::StringAlignment lineAlignment_ = graphics::SA_NEAR;
	graphics::DrawMode drawMode_ = graphics::DM_WHITE;
};