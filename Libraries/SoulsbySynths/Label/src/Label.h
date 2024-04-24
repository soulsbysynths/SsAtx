#pragma once
#include <Graphics.h>
#include <Control.h>
#include <string>
#include <algorithm>
#include <vector>
#include <Font.h>

namespace graphics
{
	class Label : public Control
	{
		public:
		Label()
		{
		}
		
		Label(const uint8_t id,
		      const Font* font, 
		      const Size* clipSize, 
		      const Rect* rect,
		      std::string text, 
		      void(*paintControl)(Control*, Graphics*),
		      StringAlignment alignment = SA_NEAR,
		      StringAlignment lineAlignment = SA_NEAR,
		      bool border = false,
		      Colour backColour = CO_BLACK,
		      Colour foreColour = CO_WHITE,
		      DrawMode drawMode = DM_OR_MASK,
		      const uint8_t zOrder = 0);
	
		~Label(void)
		{
		}
		
		void setText(std::string text);
		void setDrawMode(DrawMode drawMode);
		using Control::paintAll;
	
		protected:
		void paintGraphics(Graphics* graphics) override;
		const Font* FONT_ = NULL;
		const GridSize GRID_SIZE_ = { };
		
		private:
		void drawBorder(Graphics* g, Point location, int column, int row);
		bool border_ = false;
		std::vector<std::string> text_;
		StringAlignment alignment_ = SA_NEAR;
		StringAlignment lineAlignment_ = SA_NEAR;
		Colour backColour_ = CO_BLACK;
		Colour foreColour_ = CO_WHITE;
		DrawMode drawMode_ = DM_OR_MASK;
	
	};
}
