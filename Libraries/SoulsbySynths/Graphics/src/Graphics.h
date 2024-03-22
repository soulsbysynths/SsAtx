#pragma once
#include <stdint.h>
#include <stdlib.h> 
#include <algorithm>
#include "Font.h"
#include <Canvas.h>

namespace graphics
{	
	enum StringAlignment : uint8_t
	{
		SA_NEAR = 0,
		SA_CENTRE,
		SA_FAR
	};
	
	class Graphics
	{
		public:
		static void drawLine(Canvas* canvas, Point startPoint, Point endPoint, DrawMode drawMode = DM_WHITE);
		static void drawRect(Canvas*, Rect rect, DrawMode drawMode = DM_WHITE);
		static void drawCharacter(Canvas* canvas, Point location, const Font* font, char character, DrawMode drawMode = DM_WHITE);
		private:
		static void drawHorizontalLine(Canvas* canvas, int x, int y, int width, DrawMode drawMode = DM_WHITE);
		static void drawVerticalLine(Canvas* canvas, int x, int y, int height, DrawMode drawMode = DM_WHITE);
		static void drawPixel(Canvas* canvas, Point* point, DrawMode drawMode);
	};
}
