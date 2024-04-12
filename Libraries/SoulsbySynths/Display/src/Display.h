#pragma once
#include <Graphics.h>
#include <stdint.h>

class Display
{
	public:
	Display(const uint8_t id, 
	        const graphics::Size size)
		: ID_(id)
		, SIZE_(size)
	{
	}
	
	~Display(void)
	{
	}
	
	inline const uint8_t getId() const
	{
		return ID_;
	}
	
	inline const int getWidth() const
	{
		return SIZE_.width;
	}
	
	inline const int getHeight() const
	{
		return SIZE_.height;
	}
	
	inline const graphics::Size* getSizePtr() const
	{
		return &SIZE_;
	}
	
	virtual void writeGraphics(const graphics::Graphics* graphics) = 0;
	
	protected:
	static const uint8_t CLEAR_DATA_ = 0x00;
	const uint8_t ID_ = 0;
	const graphics::Size SIZE_ = { 0, 0 };
};