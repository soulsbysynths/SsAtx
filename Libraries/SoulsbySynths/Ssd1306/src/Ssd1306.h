#ifndef _SSD1306_H_
#define _SSD1306_H_

#include <Adafruit_GFX.h>
#include <Graphics.h>
#include <stdint.h>

class Ssd1306
{
	public:
	
	enum VComLevel : uint8_t
	{
		VCOM_65 = 0x00,
		VCOM_77 = 0x20,
		VCOM_83 = 0x30
	};
	
	typedef struct Settings
	{
		uint8_t displayStartLine;
		uint8_t contrastLevel;
		bool segmentRemap;
		bool invert;
		uint8_t multiplexRatio;
		bool comScanRemapped;
		uint8_t displayOffset;
		uint8_t columnOffset;
		uint8_t divideRatio;
		uint8_t oscFreq;
		uint8_t prechargePeriod;
		bool comPinsAltConfig;
		bool comPinsLRRemap;
		VComLevel vComLevel;
		bool chargePumpEnabled;
	} Ssd1306Settings;
	
	Ssd1306(const uint8_t id, 
	        const graphics::Size size, 
	        SPIClass *spi,
	        void(*writeSelect)(Ssd1306*, uint8_t),
	        void(*writeDc)(Ssd1306*, uint8_t),
	        const Settings* settings);
	~Ssd1306(void);
	void initialise();
	void setDisplayOn(bool way);
	void setContrast(uint8_t value);
	inline uint8_t getContrast()
	{
		return contrast_;
	}	
	void clearDisplay();
	void writeGraphics(const graphics::Graphics* graphics);
	const uint8_t ID = 0;
	const graphics::Size SIZE = { 0, 0 };
	private:
	enum MemoryMode : uint8_t
	{
		MM_HORIZONTAL = 0x00,
		MM_VERTICAL,
		MM_PAGE
	};
	
	void setDisplayStartLine(uint8_t displayStartLine);
	void setMemoryMode(MemoryMode memoryMode);
	void setSegmentRemap(bool col127MappedToSeg0);
	void setInvert(bool inverted);
	void setMultiplexRatio(uint8_t multiplexRatio);
	void setComScanDirection(bool remapped);
	void setDisplayOffset(uint8_t displayOffset);
	void setOscDivision(uint8_t divideRatio, uint8_t oscFreq);
	void setPrechargePeriod(uint8_t prechargePeriod);
	void setComPins(bool altPinConfig, bool leftRightRemap);
	void setVComLevel(VComLevel vcomLevel);
	void setChargePump(bool enabled);
	void setColumnAddress(uint8_t startAddress, uint8_t endAddress);
	void setPageAddress(uint8_t startAddress, uint8_t endAddress);
	void setEntireDisplayOn(bool on);
	void setActivateScroll(bool activate);
		
	void writeCommand(uint8_t command);
	void writeData(uint8_t data, uint16_t repeat);
	void writeData(const uint8_t* data, size_t size);
	
	static const uint8_t SSD1306_MEMORYMODE_ = 0x20;
	static const uint8_t SSD1306_COLUMNADDR_ = 0x21;
	static const uint8_t SSD1306_PAGEADDR_ = 0x22;
	static const uint8_t SSD1306_DEACTIVATE_SCROLL_ = 0x2E;
	static const uint8_t SSD1306_ACTIVATE_SCROLL_ = 0x2F;
	static const uint8_t SSD1306_SETSTARTLINE_ = 0x40;
	static const uint8_t SSD1306_SETCONTRAST_ = 0x81;
	static const uint8_t SSD1306_CHARGEPUMP_ = 0x8D;
	static const uint8_t SSD1306_SEGREMAP_ = 0xA0;
	static const uint8_t SSD1306_DISPLAYALLON_RESUME_ = 0xA4;
	static const uint8_t SSD1306_DISPLAYALLON_ = 0xA5;
	static const uint8_t SSD1306_NORMALDISPLAY_ = 0xA6;
	static const uint8_t SSD1306_INVERTDISPLAY_ = 0xA7;
	static const uint8_t SSD1306_SETMULTIPLEX_ = 0xA8;
	static const uint8_t SSD1306_DISPLAYOFF_ = 0xAE;
	static const uint8_t SSD1306_DISPLAYON_ = 0xAF;
	static const uint8_t SSD1306_SETPAGESTART_ = 0xB0;
	static const uint8_t SSD1306_COMSCANINC_ = 0xC0;
	static const uint8_t SSD1306_COMSCANDEC_ = 0xC8;
	static const uint8_t SSD1306_SETDISPLAYOFFSET_ = 0xD3;
	static const uint8_t SSD1306_SETDISPLAYCLOCKDIV_ = 0xD5;
	static const uint8_t SSD1306_SETPRECHARGE_ = 0xD9;
	static const uint8_t SSD1306_SETCOMPINS_ = 0xDA;
	static const uint8_t SSD1306_SETVCOMDETECT_ = 0xDB;

	const Settings* SETTINGS_ = NULL;
	void(*writeSelect_)(Ssd1306*, uint8_t) = NULL;
	void(*writeDc_)(Ssd1306*, uint8_t) = NULL;
	SPIClass* spi_ = NULL;
	uint8_t contrast_ = 0;
};
#endif // _SSD1306_H_