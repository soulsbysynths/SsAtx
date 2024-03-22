/* 
* OledBuffer.h
*
* Created: 23/08/2021 21:29:16
* Author: info
*/


#ifndef __OLEDBUFFER_H__
#define __OLEDBUFFER_H__

#include "Oled.h"
#include "Atx.h"

class OledBuffer
{
//variables
public:
	enum ShapeExponential : uint8_t
	{
		SEX_OFF = 0,
		SEX_EXP,
		SEX_INVEXP,
		SEX_STEEPEXP,
		SEX_STEEPINVEXP
	};
protected:
private:
	static const uint8_t OLED_BUFFER_WIDTH = 48;
	uint8_t pixelX_ = 0;
	uint8_t pixelY_ = 0;
	uint16_t buffer_[OLED_BUFFER_WIDTH] = {0};
	bool readyToPrint_ = 0;
	Oled* oled_ = NULL;
	uint8_t printRow_ = 0;
//functions
public:
	OledBuffer();
	~OledBuffer();
	const uint16_t* getBufferPtr() const { return  buffer_; }
	uint16_t* getBufferPtr() { return buffer_; }	
	void setOledPtr(Oled* ptr){oled_ = ptr;}
	const Oled* getOledPtr() const { return  oled_; }
	Oled* getOledPtr() { return oled_; }
	void setPrintRow(uint8_t newValue){printRow_ = newValue;}
	uint8_t getPrintRow(){return printRow_;}
	void setReadyToPrint(bool newValue){readyToPrint_ = newValue;}
	bool getReadyToPrint(){return readyToPrint_;}
	void drawPixelBuffer(uint_fast8_t x, uint_fast8_t y){drawPixelBuffer(x,y,false);}
	void drawPixelBuffer(uint_fast8_t x, uint_fast8_t y, bool wide){drawPixelBuffer(x,y,wide,SEX_OFF);}
	void drawPixelBuffer(uint_fast8_t x, uint_fast8_t y, bool wide, ShapeExponential shpExp);
	void drawLineBuffer(uint_fast8_t x, uint_fast8_t y){drawLineBuffer(x,y,false);}
	void drawLineBuffer(uint_fast8_t x, uint_fast8_t y, bool wide){drawLineBuffer(x,y,wide,SEX_OFF);}
	void drawLineBuffer(uint_fast8_t x, uint_fast8_t y, bool wide, ShapeExponential shpExp);
	void drawArcBuffer(uint_fast8_t x, uint_fast8_t y, uint8_t quadrant, bool wide);
	void clearBuffer(){memset(buffer_,0,sizeof(buffer_));}
	void clearRow(){oled_->clearRow(printRow_);}
	void printBuffer();
protected:
private:
	OledBuffer( const OledBuffer &c );
	OledBuffer& operator=( const OledBuffer &c );
	uint_fast8_t calcExpCoord(uint_fast8_t coord, ShapeExponential shpExp);
}; //OledBuffer

#endif //__OLEDBUFFER_H__
