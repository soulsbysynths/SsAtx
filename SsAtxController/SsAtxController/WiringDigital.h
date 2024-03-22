#pragma once
#include <stdint.h>
#include <hpl_gpio.h>
#include <hal_gpio.h>
#include <Atx.h>

namespace wiring_digital
{
#ifndef LOW
#define LOW 0x00
#endif
#ifndef HIGH
#define HIGH 0x01
#endif
	
	inline void pinMode(uint8_t pin, atx::PinMode pinMode)
	{
		switch (pinMode)
		{
		case atx::INPUT:
			gpio_set_pin_direction(pin, GPIO_DIRECTION_IN);
			gpio_set_pin_pull_mode(pin, GPIO_PULL_OFF);
			break;
		case atx::OUTPUT:
			gpio_set_pin_direction(pin, GPIO_DIRECTION_OUT);
			gpio_set_pin_pull_mode(pin, GPIO_PULL_OFF);
			break;
		case atx::INPUT_PULLUP:
			gpio_set_pin_direction(pin, GPIO_DIRECTION_IN);
			gpio_set_pin_pull_mode(pin, GPIO_PULL_UP);
			break;
		case atx::INPUT_PULLDOWN:
			gpio_set_pin_direction(pin, GPIO_DIRECTION_IN);
			gpio_set_pin_pull_mode(pin, GPIO_PULL_DOWN);
			break;
		default:
			break;
		}
		
	}
	
	inline void digitalWrite(uint8_t pin, uint8_t val)
	{
		gpio_set_pin_level(pin, (bool)val);
	}
	
}