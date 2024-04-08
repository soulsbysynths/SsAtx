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
	
	typedef struct GridSize
	{
		int columns;
		int rows;
	} GridSize;
	
	typedef struct Rect
	{
		Point location;
		Size size;
	} Rect;
	
	enum StringAlignment : uint8_t
	{
		SA_NEAR   = 0,
		SA_CENTRE,
		SA_FAR
	};

	enum Colour : uint8_t
	{
		CO_WHITE     = 0,
		CO_LIGHTGREY,
		CO_GREY,
		CO_DARKGREY,
		CO_BLACK
	};
	
	enum DrawMode : uint8_t
	{
		DM_MASK         = 0,
		DM_NOT_MASK,
		DM_AND_MASK,
		DM_AND_NOT_MASK,
		DM_OR_MASK,
		DM_OR_NOT_MASK,
		DM_XOR_MASK,
		DM_XOR_NOT_MASK
	};
}
#endif // _GRAPHICSTYPES_H_