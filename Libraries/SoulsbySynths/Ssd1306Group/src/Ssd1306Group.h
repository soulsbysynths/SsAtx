#ifndef _SSD1306GROUP_H_
#define _SSD1306GROUP_H_

#include <Arduino.h>
#include <SPI.h>
#include <vector>
#include <Ssd1306.h>

class Ssd1306Group
{
	public:
	Ssd1306Group(void(*initialisePins)(void),
	             void(*writeVcc)(uint8_t),
	             void(*writeReset)(uint8_t));
	
	void add(const uint8_t id, 
	         const graphics::Size size, 
	         SPIClass *spi,
	         void(*writeSelect)(Ssd1306* ssd, uint8_t),
	         void(*writeDc)(Ssd1306* ssd, uint8_t),
	         const Ssd1306::Settings* settings);
	void initialise();
	const Ssd1306* getOledPtr(uint8_t index) const { return oleds_[index]; }
	Ssd1306* getOledPtr(uint8_t index) { return oleds_[index]; }
	private:
	void(*initialisePins_)(void) = NULL;
	void(*writeVcc_)(uint8_t) = NULL;
	void(*writeReset_)(uint8_t) = NULL;
	std::vector<Ssd1306*> oleds_;
};

#endif // _SSD1306GROUP_H_