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
		Point location;
		Size size;
	} Rect;
	
	typedef struct Line
	{
		Point start;
		Point end;
	} Line;
	
	typedef struct GridLocation
	{
		int column;
		int row;
	} GridLocation;
	
	typedef struct GridSize
	{
		int columns;
		int rows;
	} GridSize;
	
	typedef struct Grid
	{
		GridLocation location;
		GridSize size;
	} Grid;
	
	typedef struct BoundingBox
	{
		int top;
		int right;
		int bottom;
		int left;
	} BoundingBox;
	
	enum Side : uint8_t
	{
		SI_TOP    = 0,
		SI_RIGHT,
		SI_BOTTOM,
		SI_LEFT,
		SI_MAX
	};
	
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
	
	enum CircleQuarter : uint8_t
	{
		CQ_TOPLEFT     = 0,
		CQ_TOPRIGHT,    
		CQ_BOTTOMRIGHT, 
		CQ_BOTTOMLEFT,
		CQ_MAX
	};
	
	enum CircleQuarterFlags : uint8_t
	{
		CQF_TOPLEFT     = 1 << CQ_TOPLEFT,
		CQF_TOPRIGHT    = 1 << CQ_TOPRIGHT,
		CQF_BOTTOMRIGHT = 1 << CQ_BOTTOMRIGHT,
		CQF_BOTTOMLEFT  = 1 << CQ_BOTTOMLEFT
	};
	
}
#endif // _GRAPHICSTYPES_H_