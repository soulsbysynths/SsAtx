#ifndef _GRAPHICSTYPES_H_
#define _GRAPHICSTYPES_H_
#include <stdint.h>

namespace graphics
{
	typedef struct Point
	{
		int x;
		int y;
	} Point;
	
	typedef struct Size
	{
		int width;
		int height;
	} Size;
	
	typedef struct Rect
	{
		int x;
		int y;
		int width;
		int height;
	} Rect;
	
	enum StringAlignment : uint8_t
	{
		SA_NEAR   = 0,
		SA_CENTRE,
		SA_FAR
	};

	enum DrawMode
	{
		DM_WHITE  = 0,
		DM_BLACK,
		DM_INVERT
	};
}
#endif // _GRAPHICSTYPES_H_