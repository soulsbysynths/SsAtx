/* 
* Oled.h
*
* Created: 12/01/2018 17:27:01
* Author: info
*/


#ifndef __OLED_H__
#define __OLED_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "OledBase.h"
#include "OledProgmem.h"
#include "Atx.h"
#include "Helpers.h"

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

#define  OLED_WIDE_W 96

#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif

//#define SWAP(a, b) { int_fast8_t t = a; a = b; b = t; }

class Oled
{
//variables
public:
	static const uint8_t FONT_HEIGHT = 16;
	static const uint8_t FONT_HALF_HEIGHT = FONT_HEIGHT >> 1;
	static const uint8_t FONT_WIDTH_BS = 3;
	static const uint8_t FONT_WIDTH = 1 << FONT_WIDTH_BS;
	static const uint8_t FONT_WIDTH_MASK = FONT_WIDTH-1;
	static const uint8_t FONT_LOGO = 128;
	static const char FONT_FOLDER = 16;
	static const char FONT_VOLUME = 29;
	static const char FONT_BACK = 17;
	static const char FONT_SDCARD = 18;
	static const char FONT_NEWFILE = 19;
	static const char FONT_NEWFOLDER = 20;
	static const char FONT_OK = 21;
	static const char FONT_PS = 22;
	static const char FONT_PS_SEL_OFF = 5;
	static const uint8_t FONT_LOOP = 174;
	static const uint8_t FONT_AR = 167;
	static const char FONT_6x8_CLEAR = 20;
	static const char FONT_DUMMY_GFX = 0xFF;  // a dummy char to show that when printing gfx buffer, all chars have changed
protected:
private:
	static const uint8_t OFFSET_6X8_ROW = 6;  //might want to change this if you ever need 6x8 on any other row
	OledBase* base_ = NULL;
	uint8_t index_ = 0;
	uint8_t width_ = 0;
	uint8_t height_ = 0;
	uint8_t columns_ = 0;
	uint8_t rows_ = 0;
	uint8_t pixelX_ = 0;
	uint8_t pixelY_ = 0;
	uint8_t thermometerRow_ = atx::UNUSED;
	bool thermometerLine_ = false;
	uint8_t* invertStart_ = NULL;
	uint8_t* invertEnd_ = NULL;
	uint8_t** charBuffer_ = NULL;
	uint8_t char6x8Buffer_[8][2] = {{0}};
	uint16_t* wideMap_ = NULL;
	uint16_t* changedMap_ = NULL;
	bool changed6x8_ = false;
	char* overlay_ = NULL;
	uint8_t overlayRow_ = 0;
//functions
public:
	void init(uint8_t extVccCmd);
	uint8_t getIndex(){return index_;}
	uint8_t getWidth(){return width_;}
	uint8_t getRows(){return rows_;}
	uint8_t getColumns(){return columns_;}
	void setInvertRange(uint_fast8_t row, uint8_t start, uint8_t end);
	void clearInvertMapRow(uint_fast8_t row);
	void clearInvertMap();
	void setThermometerRow(uint8_t newRow);
	uint8_t getThermometerRow(){return thermometerRow_;}
	void setThermometerLine(bool newLine);
	bool getThermometerLine(){return thermometerLine_;}
	void setChangedMap(uint_fast8_t row, uint16_t newMap){changedMap_[row] = newMap;}
	//void setOverlay(const char* newOverlay);
	//void clearOverlay();
	//void setOverlayRow(uint8_t newRow){overlayRow_ = newRow;}
	void txDisplayOnOff(bool on);
	void clearDisplay();
	void clearRow(uint_fast8_t row);
	void refresh();
	void printRow(uint_fast8_t row, uint_fast8_t width, uint16_t * gfx);
	void printPixel(uint_fast8_t x, uint_fast8_t row, uint_fast8_t y, bool wide);
	void printLine(uint_fast8_t x, uint_fast8_t row, uint_fast8_t y, bool wide);
	void printDigit6x8Buffer(uint_fast8_t col, uint_fast8_t row, uint_fast8_t digit);
	//void printThreeDigit(uint_fast8_t col, uint_fast8_t row, uint_fast16_t number, bool wide);
	//void printInt(uint_fast8_t col, uint_fast8_t row, int_fast32_t num, bool wide, bool padBlank, uint8_t leadingZeros);
	//void printFloat(uint_fast8_t col, uint_fast8_t row, float num, bool wide, bool padBlank, uint8_t decimalPlaces);
	//void printString(uint_fast8_t col, uint_fast8_t row, const char * str, bool wide, bool padBlank);
	void printCharBuffer(uint_fast8_t col, uint_fast8_t row, uint8_t charnum, bool wide);
	void printIntBuffer(uint_fast8_t col, uint_fast8_t row, int_fast32_t num, bool wide, bool padBlank, uint8_t leadingZeros);
	void printFloatBuffer(uint_fast8_t col, uint_fast8_t row, float num, bool wide, bool padBlank, uint8_t decimalPlaces);
	void printStringBuffer(uint_fast8_t col, uint_fast8_t row, const char * str, bool wide, bool padBlank);
	Oled() {}
	Oled(uint8_t index, OledBase* base, uint8_t width, uint8_t height);
	~Oled();
protected:
private:
	//Oled( const Oled &c );
	//Oled& operator=( const Oled &c );
	void printChar(uint_fast8_t col, uint_fast8_t row, uint8_t charnum, bool wide);
	void printDigit6x8(uint_fast8_t col, uint_fast8_t row, uint_fast8_t digit);
}; //Oled

#endif //__OLED_H__
