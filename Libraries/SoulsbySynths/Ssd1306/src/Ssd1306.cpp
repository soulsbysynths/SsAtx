
#include "Ssd1306.h"

Ssd1306::Ssd1306(const uint8_t id, 
                 const graphics::Size size, 
                 SPIClass* spi,
                 void(*writeSelect)(Ssd1306*, uint8_t),
                 void(*writeDc)(Ssd1306*, uint8_t),
                 const Settings* settings)
	: Display(id, size) 
	, spi_(spi)
	, writeSelect_(writeSelect)
	, writeDc_(writeDc)
	, SETTINGS_(settings)
{
}

Ssd1306::~Ssd1306(void) 
{

}

void Ssd1306::initialise()
{
	setDisplayOn(false);
	setOscDivision(SETTINGS_->divideRatio, SETTINGS_->oscFreq);
	setMultiplexRatio(SETTINGS_->multiplexRatio);
	setDisplayOffset(SETTINGS_->displayOffset);
	setDisplayStartLine(SETTINGS_->displayStartLine);
	setChargePump(SETTINGS_->chargePumpEnabled);
	setMemoryMode(Ssd1306::MM_HORIZONTAL);
	setSegmentRemap(SETTINGS_->segmentRemap);
	setComScanDirection(SETTINGS_->comScanRemapped);
	setComPins(SETTINGS_->comPinsAltConfig, SETTINGS_->comPinsLRRemap);
	setPrechargePeriod(SETTINGS_->prechargePeriod);
	setVComLevel(SETTINGS_->vComLevel);
	setEntireDisplayOn(true);
	setInvert(SETTINGS_->invert);
	setContrast(SETTINGS_->contrastLevel);	
	setActivateScroll(false);
}

void Ssd1306::setDisplayOn(bool way)
{
	writeCommand(way ? SSD1306_DISPLAYON_ : SSD1306_DISPLAYOFF_);
}

void Ssd1306::setContrast(uint8_t value)
{
	writeCommand(SSD1306_SETCONTRAST_);
	writeCommand(value);
}

void Ssd1306::clearDisplay()
{
	writeCommand(SSD1306_COLUMNADDR_);
	writeCommand(0); // Column start address (0 = reset)
	writeCommand(127); // Column ed address (127 = reset)
	
	writeCommand(SSD1306_PAGEADDR_);
	writeCommand(0); // Page start address (0 = reset)
	writeCommand(7); // Page end address (7 = reset)
	
	writeData(0x00, (uint16_t)1024);
	
}

void Ssd1306::writeGraphics(const graphics::Graphics* graphics)
{
	using namespace graphics;
	
	const Rect* rect = graphics->getRectPtr();
		
	uint8_t x = rect->x + SETTINGS_->columnOffset;
	uint8_t y = rect->y >> 3;
	uint8_t w = rect->width;
	uint8_t h = rect->height >> 3;
	
	if (w == 0 || h == 0)
	{
		return;
	}
	
	writeCommand(SSD1306_COLUMNADDR_);
	writeCommand(x & 0x7F); 
	writeCommand((x + w  - 1) & 0x7F); 
	
	writeCommand(SSD1306_PAGEADDR_);
	writeCommand(y & 0x07); 
	writeCommand((y + h - 1) & 0x07);
	
	uint16_t size = (uint16_t)w * h;
	writeData(graphics->getBufferPtr(), size);
}

void Ssd1306::writeCommand(uint8_t command)
{
	writeSelect_(this, LOW);
	writeDc_(this, HIGH);
	writeSelect_(this, HIGH);
	spi_->transfer(command);
	writeSelect_(this, LOW);
}

void Ssd1306::writeData(const uint8_t* data, size_t size)
{
	writeSelect_(this, LOW);
	writeDc_(this, LOW);
	writeSelect_(this, HIGH);
	spi_->transfer((void*)data, size);
	writeSelect_(this, LOW);
}

void Ssd1306::writeData(uint8_t data, uint16_t repeat)
{
	writeSelect_(this, LOW);
	writeDc_(this, LOW);
	writeSelect_(this, HIGH);
	for (uint16_t i = 0; i < repeat; i++)
	{
		spi_->transfer(data);
	}
	
	writeSelect_(this, LOW);
}

void Ssd1306::setDisplayStartLine(uint8_t value)
{
	writeCommand(SSD1306_SETSTARTLINE_ | (value & 0x3F));
}

void Ssd1306::setMemoryMode(MemoryMode value)
{
	writeCommand(SSD1306_MEMORYMODE_);
	writeCommand(value);
}

void Ssd1306::setSegmentRemap(bool col127MappedToSeg0)
{
	writeCommand(SSD1306_SEGREMAP_ | (col127MappedToSeg0 ? 0x01 : 0x00));
}

void Ssd1306::setInvert(bool inverted)
{
	writeCommand(inverted ? SSD1306_INVERTDISPLAY_ : SSD1306_NORMALDISPLAY_);
}

void Ssd1306::setMultiplexRatio(uint8_t value)
{
	writeCommand(SSD1306_SETMULTIPLEX_);
	writeCommand(value & 0x3F);
}

void Ssd1306::setComScanDirection(bool remapped)
{
	writeCommand(remapped ? SSD1306_COMSCANDEC_ : SSD1306_COMSCANINC_);
}

void Ssd1306::setDisplayOffset(uint8_t value)
{
	writeCommand(SSD1306_SETDISPLAYOFFSET_);
	writeCommand(value & 0x3F);
}

void Ssd1306::setOscDivision(uint8_t divideRatio, uint8_t oscFreq)
{
	writeCommand(SSD1306_SETDISPLAYCLOCKDIV_);
	writeCommand((divideRatio & 0x0F) | (oscFreq << 4));
}
void Ssd1306::setPrechargePeriod(uint8_t value)
{
	writeCommand(SSD1306_SETPRECHARGE_);
	writeCommand(value);
}

void Ssd1306::setComPins(bool altPinConfig, bool leftRightRemap)
{
	writeCommand(SSD1306_SETCOMPINS_);
	writeCommand(0x02 | (altPinConfig ? 0x10 : 0x00) | (leftRightRemap ? 0x20 : 0x00));
}

void Ssd1306::setVComLevel(VComLevel value)
{
	writeCommand(SSD1306_SETVCOMDETECT_);
	writeCommand(0x40);
}

void Ssd1306::setChargePump(bool enabled)
{
	writeCommand(SSD1306_CHARGEPUMP_);
	writeCommand(0x10 | (enabled ? 0x40 : 0x00));
	
}

void Ssd1306::setColumnAddress(uint8_t startAddress, 
                               uint8_t endAddress)
{
	writeCommand(SSD1306_COLUMNADDR_);
	writeCommand(startAddress & 0x7F);
	writeCommand(endAddress & 0x7F);
}

void Ssd1306::setPageAddress(uint8_t startAddress, 
                             uint8_t endAddress)
{
	writeCommand(SSD1306_PAGEADDR_);
	writeCommand(startAddress & 0x7F);
	writeCommand(endAddress & 0x7F);
}

void Ssd1306::setEntireDisplayOn(bool resume)
{
	writeCommand(resume ? SSD1306_DISPLAYALLON_RESUME_ : SSD1306_DISPLAYALLON_);
}

void Ssd1306::setActivateScroll(bool activate)
{
	writeCommand(activate ? SSD1306_ACTIVATE_SCROLL_ : SSD1306_DEACTIVATE_SCROLL_);
}