#pragma once
#include <Canvas.h>
#include <Graphics.h>
#include <string>
#include <Font.h>

class Label
{
	public:
	Label(graphics::Rect rect, 
	      graphics::Size outputSize,
	      std::string text, 
	      const Font* font, 
	      graphics::StringAlignment alignment = graphics::SA_NEAR,
	      graphics::StringAlignment lineAlignment = graphics::SA_NEAR,
	      bool border = false, 
	      graphics::DrawMode drawMode = graphics::DM_WHITE, 
	      void(*paint)(Label*) = NULL,
	      graphics::Rect quantizeRectBitShift = { 0, 3, 0, 3 });
	
	~Label(void);
	
	inline int getWidth()
	{
		return canvas_->getWidth();
	}
	
	inline int getHeight()
	{
		return canvas_->getHeight();
	}
	
	inline int setText(std::string text)
	{
		text_ = text;
	}
	
	inline std::string getText()
	{
		return text_;
	}
	
	private:
	void paint();
	std::string text_ = "";
	const Font* font_ = NULL;
	void(*paint_)(Label*) = NULL;
	graphics::Canvas* canvas_ = NULL;
	bool border_ = false;
	graphics::StringAlignment alignment_ = graphics::SA_NEAR;
	graphics::StringAlignment lineAlignment_ = graphics::SA_NEAR;
	graphics::DrawMode drawMode_ = graphics::DM_WHITE;
};