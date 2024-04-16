#ifndef _GRAPHICSMATHS_H_
#define _GRAPHICSMATHS_H_
#include <stdint.h>
#include <GraphicsTypes.h>

namespace graphics
{
	inline static void clip(Rect* rect, 
	                        const Size* clipSize, 
	                        const Size* quantiseBitShift)
	{
		// Crop out of bounds left.
		if (rect->location.x < 0)
		{
			rect->size.width += rect->location.x;
			rect->location.x = 0;
		}
	
		// Crop out of bounds top.
		if (rect->location.y < 0)
		{
			rect->size.height += rect->location.y;
			rect->location.y = 0;
		}

		// Quantise	location
		Point originalLocation = rect->location;
	
		rect->location.x >>= quantiseBitShift->width;
		rect->location.x <<= quantiseBitShift->width;
		rect->location.y >>= quantiseBitShift->height;
		rect->location.y <<= quantiseBitShift->height;
	
		// Adjust size for new location, then quantise size
		rect->size.width += (originalLocation.x - rect->location.x);
		rect->size.height += (originalLocation.y - rect->location.y);
	
		rect->size.width >>= quantiseBitShift->width;
		rect->size.width <<= quantiseBitShift->width;
		rect->size.height >>= quantiseBitShift->height;
		rect->size.height <<= quantiseBitShift->height;
	
		// Force width >= quantise width
		if (rect->size.width < (1 << quantiseBitShift->width))
		{
			rect->size.width = (1 << quantiseBitShift->width);
		}
	
		// Force height >= quantise height
		if (rect->size.height < (1 << quantiseBitShift->height))
		{
			rect->size.height = (1 << quantiseBitShift->height);
		}
	
		// Crop out of bounds right.
		if (rect->location.x + rect->size.width > clipSize->width)
		{
			rect->location.x = clipSize->width - rect->size.width;
		}
	
		// Crop out of bounds bottom.
		if (rect->location.y + rect->size.height > clipSize->height)
		{
			rect->location.y = clipSize->height - rect->size.height;
		}

	}
		
	static void enlarge(Grid* grid, 
	                    const GridLocation addLocation)
	{
		if (grid->size.columns == 0 && grid->size.rows == 0)
		{
			// Must be first cell
			grid->location = addLocation;
			grid->size = { 1, 1 };
			return;
		}

		GridLocation endLocation = 
		{
			grid->location.column + grid->size.columns - 1, 
			grid->location.row + grid->size.rows - 1
		};
		
		if (addLocation.column < grid->location.column)
		{
			grid->location.column = addLocation.column;
		}
		else if (addLocation.column > endLocation.column)
		{
			endLocation.column = addLocation.column;
		}
	
		grid->size.columns = endLocation.column - grid->location.column + 1;
	
		if (addLocation.row < grid->location.row)
		{
			grid->location.row = addLocation.row;
		}
		else if (addLocation.row > endLocation.row)
		{
			endLocation.row = addLocation.row;
		}
	
		grid->size.rows = endLocation.row - grid->location.row + 1;
	}
	
	static void enlarge(Rect* rect, 
	                    const Point* addPoint)
	{
		if (rect->size.width == 0 && rect->size.height == 0)
		{
			// Must be first point
			rect->location = *addPoint;
			rect->size = { 1, 1 };
			return;
		}

		Point endPoint = 
		{
			rect->location.x + rect->size.width - 1, 
			rect->location.y + rect->size.height - 1
		};
		
		if (addPoint->x < rect->location.x)
		{
			rect->location.x = addPoint->x;
		}
		else if (addPoint->x > endPoint.x)
		{
			endPoint.x = addPoint->x;
		}
	
		rect->size.width = endPoint.x - rect->location.x + 1;
	
		if (addPoint->y < rect->location.y)
		{
			rect->location.y = addPoint->y;
		}
		else if (addPoint->y > endPoint.y)
		{
			endPoint.y = addPoint->y;
		}
	
		rect->size.height = endPoint.y - rect->location.y + 1;
	}
	
	static void enlarge(Rect* rect, 
	                    const Rect* addRect)
	{
		const Point startPoint = addRect->location;
		const Point endPoint = 
		{ 
			addRect->location.x + addRect->size.width,
			addRect->location.y + addRect->size.height
		};
		enlarge(rect, &startPoint);
		enlarge(rect, &endPoint);
	}
		
	static void enlarge(Rect* rect, 
	                    const Grid* addGrid, 
	                    const Size* gridSizeBitShift)
	{
		const Point startPoint = 
		{ 
			addGrid->location.column << gridSizeBitShift->width,
			addGrid->location.row << gridSizeBitShift->height
		};
	
		const Point endPoint = 
		{ 
			(addGrid->location.column + addGrid->size.columns) << gridSizeBitShift->width,
			(addGrid->location.row + addGrid->size.rows) << gridSizeBitShift->height
		};
	
		enlarge(rect, &startPoint);
		enlarge(rect, &endPoint);
	}
	
	inline static bool isContained(const Rect* outerRect, 
	                               const Rect* innerRect)
	{
		if (innerRect->location.x >= outerRect->location.x &&
		   innerRect->location.y >= outerRect->location.y &&
		   (innerRect->location.x + innerRect->size.width) <= (outerRect->location.x + outerRect->size.width) && 
		   (innerRect->location.y + innerRect->size.height <= (outerRect->location.y + outerRect->size.height)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	inline static bool isEqual(const Rect* rect1, 
	                               const Rect* rect2)
	{
		if (rect1->location.x == rect2->location.x &&
		   rect1->location.y == rect2->location.y &&
		   rect1->size.width == rect2->size.width &&  
		   rect1->size.height == rect2->size.height)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	static const Rect getRoundRectCircleQuarterRect(const Rect* rect, 
	                                                int radius, 
	                                                CircleQuarter quarter)
	{
		switch (quarter)
		{
			case CQ_TOPLEFT:
				return 
				{
				{
					rect->location.x + radius, 
					rect->location.y + radius
				},
				{
					-radius,
					-radius
				}
				};
			case CQ_TOPRIGHT:
				return 
				{
				{
					rect->location.x + rect->size.width - radius - 1, 
					rect->location.y + radius
				},
				{
					radius,
					-radius
				}
				};
			case CQ_BOTTOMRIGHT:
				return 
				{
				{
					rect->location.x + rect->size.width - radius - 1, 
					rect->location.y + rect->size.height - radius - 1
				}, 
				{ 
					radius,
					radius
				}
				};
			default:
				return 
				{
				{
					rect->location.x + radius, 
					rect->location.y + rect->size.height - radius - 1
				},
				{
					-radius,
					radius
				}
				};
			
		}
	}
	
	static const Rect getRoundRectSideRect(const Rect* rect, 
	                                       int radius, 
	                                       Side side)
	{
		switch (side)
		{
			case SI_TOP:
				return
				{ 
				{
					rect->location.x + radius, 
					rect->location.y
				}, 
				{
					rect->size.width - 2 * radius,
					0
				}
				};
			case SI_RIGHT:
				return
				{ 
				{
					rect->location.x + rect->size.width - 1, 
					rect->location.y + radius
				}, 
				{
					0,
					rect->size.height - 2 * radius
				}
				};
			case SI_BOTTOM:
				return
				{ 
				{
					rect->location.x + radius, 
					rect->location.y + rect->size.height - 1
				},
				{
					rect->size.width - 2 * radius,
					0
				}
				};
			default:
				return
				{ 
				{
					rect->location.x, 
					rect->location.y + radius
				}, 
				{
					0,
					rect->size.height - 2 * radius
				}
				};
		}
	}
}
#endif // _GRAPHICSMATHS_H_