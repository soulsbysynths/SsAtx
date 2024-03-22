#include "Ssd1306Group.h"

Ssd1306Group::Ssd1306Group(void(*initialisePins)(void), 
                           void(*writeVcc)(uint8_t), 
                           void(*writeReset)(uint8_t))
	: initialisePins_(initialisePins)
	, writeVcc_(writeVcc)
	, writeReset_(writeReset)
{
}

void Ssd1306Group::initialise()
{
	initialisePins_();
	writeVcc_(LOW);
	writeReset_(HIGH);
	delay(1);
	writeReset_(LOW);
	delay(10);
	writeReset_(HIGH);
	
	for (const auto& oled : oleds_)	
	{	
		oled->initialise();
	}
	
	delay(1);
	writeVcc_(HIGH);
	delay(1);
	
	for (const auto &oled : oleds_)
	{	
		oled->setDisplayOn(true);
	}
	
	delay(100);
	
	for (const auto& oled : oleds_)	
	{	
		oled->clearDisplay();
	}
}

void Ssd1306Group::add(const uint8_t id, 
                       const graphics::Size size, 
                       SPIClass *spi,
                       void(*writeSelect)(Ssd1306* ssd, uint8_t),
                       void(*writeDc)(Ssd1306* ssd, uint8_t),
                       const Ssd1306::Settings* settings)
{
	oleds_.push_back(new Ssd1306(id, size, spi, writeSelect, writeDc, settings));
}
