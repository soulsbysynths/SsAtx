#pragma once
#include <Graphics.h>
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
	
		void setValue(int value);
	
		private:
		const int MIN_VALUE_;
		const int MAX_VALUE_;
		int value_;
	};
}
