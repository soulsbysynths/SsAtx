#pragma once
#include <Graphics.h>
#include <stdint.h>

class Display
{
	public:
	const uint8_t ID = 0;
	const graphics::Size SIZE = { 0, 0 };
	Display(const uint8_t id, 
	        const graphics::Size size)
		: ID(id)
		, SIZE(size)
	{
	}
	~Display(void);
	virtual void writeGraphics(const graphics::Graphics* graphics) = 0;
};