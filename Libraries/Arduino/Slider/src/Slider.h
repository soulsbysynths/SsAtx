#pragma once
#include <string.h>
#include <Graphics.h>
#include <Font.h>
#include <Atx8x16Tiny.h>
#include <Label.h>

namespace graphics
{
	class Slider : private Label
	{
		public:
		Slider(const uint8_t id,
		       const Size* constrainSize, 
		       const Rect* rect,
		       const int minValue,
		       const int maxValue,
		       void(*paintControl)(Control*, Graphics*),
		       int value = 0,
		       const uint8_t zOrder = 0);
	
		~Slider(void)
		{
		}
		
		using Control::paint;
		void paint(Rect* rect) override;
		void setValue(int value);
		
		protected:
		void paintGraphics(Rect* rect, Graphics* graphics) override;
		
		private:
		inline const Rect clip(const Rect* rect)
		{
			return { 
				rect->location,
			{
				rect->size.width,
				FONT_->getHeight() 
			}
			};
		}
		
		std::string parseValue(int value);
		const int MIN_VALUE_;
		const int MAX_VALUE_;
		static const Font* FONT_;
		int value_;
	};
}
