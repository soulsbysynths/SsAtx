#pragma once
#include <Graphics.h>
#include <stdint.h>
#include <stddef.h>

class Control
{
	public:
	Control(const uint8_t id,
	        const graphics::Size* constrainSize, 
	        const graphics::Rect rect,
	        void(*paintControl)(Control*, graphics::Graphics*),
	        const uint8_t zOrder = 0)
		: ID_(id)
		, CONSTRAIN_SIZE_(constrainSize)
		, RECT_(rect)
		, paintControl_(paintControl)
		, Z_ORDER_(zOrder)
	{
	}
	
	~Control(void)
	{
	}
	
	inline void paint()
	{
		graphics::Rect paintRect = 
		{ 
			{ 0, 0 },
			RECT_.size		
		};
		
		paint(&paintRect);
	}
	
	inline const uint8_t getId() const 
	{
		return ID_;
	}
	
	virtual void paint(graphics::Rect* rect) = 0;
	
	protected:
	void(*paintControl_)(Control*, graphics::Graphics*) = NULL;
	const graphics::Size* CONSTRAIN_SIZE_;
	const graphics::Rect RECT_;
	const uint8_t ID_;
	const uint8_t Z_ORDER_;
};