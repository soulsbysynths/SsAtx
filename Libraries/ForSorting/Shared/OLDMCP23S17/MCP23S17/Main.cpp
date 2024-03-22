#include <atmel_start.h>
#include "MCP23S17.h"

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	MCP23S17 mcp(GPIO(GPIO_PORTA, 20), 0);
	mcp.begin();
	for (uint16_t i = 0; i < 16; i++)
	{
		mcp.pinMode(i, atx::OUTPUT);
	}
	
	bool o = 0;
	uint8_t p = 0;
	
	/* Replace with your application code */
	while (1) {
		mcp.digitalWrite(p, o);
		p++;
		p &= 0x0F;
		if (p == 0)
		{
			o = !o;
		}
	}
}
