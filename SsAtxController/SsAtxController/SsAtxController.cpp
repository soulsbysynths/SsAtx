#include <atmel_start.h>
#include "AtxC.h"
#include "MidiMsg.h"

int main(void)
{
	AtxC controller;
	
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	controller.initialize();
	
	bool o = 0;
	int p = GPIO(GPIO_PORTA, 27);
	
	gpio_set_pin_direction(p, GPIO_DIRECTION_OUT);
	
	midi::MidiMsg msg;
	msg.setChannel(1);
	gpio_set_pin_level(p, msg.getCommand());
	
	/* Replace with your application code */
	while (1) {
		gpio_set_pin_level(p, o);
		o = 1 - o;		
		controller.poll(0);
	}
}
