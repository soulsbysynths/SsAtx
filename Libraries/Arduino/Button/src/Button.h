#pragma once
#include <Graphics.h>
#include <Control.h>
#include <string>
#include <algorithm>
#include <vector>
#include <Font.h>
#include <Label.h>

namespace graphics
{
	class Button : public Label
	{
		public:
		Button(const uint8_t id,
		       const Font* font, 
		       const Size* constrainSize, 
		       const Rect* rect,
		       std::string text, 
		       void(*paintControl)(Control*, Graphics*),
		       const bool pressed = false,
		       const bool selected = true,
		       const uint8_t zOrder = 0);
	
		~Button(void)
		{
		}
		
		using Control::paint;
		void paint(Rect* rect);
	
		protected:
		void paintGraphics(Rect* rect, Graphics* graphics) override;
		
		private:
	
		inline const DrawMode getPressedDrawMode() const
		{
			return pressed_ ? DM_NOT_MASK : DM_MASK;
		}
	
		inline const Colour getSelectedColour() const
		{
			return selected_ ? CO_WHITE : CO_GREY;
		}
	
		static const int RADIUS_ = 4;
		bool pressed_ = false;
		bool selected_ = false;
	};	 
}
