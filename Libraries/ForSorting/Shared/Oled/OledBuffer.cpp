/*
* OledBuffer.cpp
*
* Created: 23/08/2021 21:29:16
* Author: info
*/


#include "OledBuffer.h"

// default constructor
OledBuffer::OledBuffer()
{
} //OledBuffer

// default destructor
OledBuffer::~OledBuffer()
{
} //~OledBuffer

void OledBuffer::drawPixelBuffer(uint_fast8_t x, uint_fast8_t y, bool wide, ShapeExponential shpExp)
{
	uint_fast8_t off;
	pixelX_ = x;
	pixelY_ = y;

	bitSet(buffer_[x],y);
	if (wide)
	{
		bitSet(buffer_[x+1],y);
	}
}

void OledBuffer::drawLineBuffer(uint_fast8_t x, uint_fast8_t y, bool wide, ShapeExponential shpExp)
{
	int_fast8_t x0 = pixelX_, y0 = pixelY_, x1 = x, y1 = y;
	pixelX_ = x;
	pixelY_ = y;

	int_fast8_t dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int_fast8_t dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int_fast8_t err = (dx>dy ? dx : -dy)/2, e2;

	for(;;)
	{
		drawPixelBuffer(x0,y0,wide);
		if (x0==x1 && y0==y1) break;
		e2 = err;
		if (e2 >-dx)
		{
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy)
		{
			err += dx;
			y0 += sy;
		}
	}
}
void OledBuffer::drawArcBuffer(uint_fast8_t x, uint_fast8_t y, uint8_t quadrant, bool wide)
{
	
	int_fast8_t x0 = pixelX_, y0 = pixelY_, x1 = x, y1 = y;
	pixelX_ = x;
	pixelY_ = y;
	
	int_fast8_t a = abs(x1 - x0) << 1, b = abs(y1 - y0) << 1, b1 = b & 1; /* values of diameter */
	int_fast16_t dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a; /* error increment */
	int_fast16_t err = dx + dy + b1 * a * a, e2; /* error of 1.step */

	if (x0 > x1)
	{
		x0 = x1;
		x1 += (a >> 1);
	}
	/* if called with swapped points */
	if (y0 > y1) y0 = y1; /* .. exchange them */
	switch(quadrant)
	{
		case 0:
		x0 -= (a>>1);
		y0 -= (a>>1);	
		break;
		case 1:
		x1 += (a>>1);
		y0 -= (b>>1);
		break;
		case 2:
		x1 += (a>>1);
		break;
		case 3:
		x0 -= (a>>1);
		y1 -= (b>>1);
		break;
	}
	y0 += (b + 1) >> 1; /// 2; /* starting pixel */
	y1 = y0 - b1;
	a *= (a << 3); //8 * a;
	b1 = ((b * b) << 3); //8 * b * b;

	do
	{
		switch(quadrant)
		{
			case 0:
			drawPixelBuffer(x1, y0, wide); /*   I. Quadrant */
			break;
			case 1:
			drawPixelBuffer(x0, y0, wide); /*  II. Quadrant */
			break;
			case 2:
			drawPixelBuffer(x0, y1, wide); /* III. Quadrant */
			break;
			case 3:
			drawPixelBuffer(x1, y1, wide); /*  IV. Quadrant */
			break;
		}

		e2 = (err << 1); //2 * err;
		if (e2 >= dx)
		{
			x0++;
			x1--;
			err += dx += b1;
		}
		/* x step */
		if (e2 <= dy)
		{
			y0++;
			y1--;
			err += dy += a;
		}
		/* y step */
	} while (x0 <= x1);

	while (y0 - y1 < b)
	{
		/* too early stop of flat ellipses a=1 */
		y0++;
		if(quadrant<=1) drawPixelBuffer(x0 - 1, y0, wide); /* -> complete tip of ellipse */
		y1--;
		if(quadrant>=2) drawPixelBuffer(x0 - 1, y1, wide);
	}
}

uint_fast8_t OledBuffer::calcExpCoord(uint_fast8_t coord, ShapeExponential shpExp)
{
	switch(shpExp)
	{
		//not totally sure why inv is flipped, but that's what looks right!
		case SEX_OFF:
		return coord;
		break;
		case SEX_EXP:
		return convertInvExponential4Bit(coord);
		break;
		case SEX_INVEXP:
		return convertExponential4Bit(coord);
		break;
		case SEX_STEEPEXP:
		return convertInvSteepExponential4Bit(coord);
		break;
		case SEX_STEEPINVEXP:
		return convertSteepExponential4Bit(coord);
		break;
	}
}
void OledBuffer::printBuffer()
{
	if (oled_!=NULL && readyToPrint_==true)
	{
		oled_->printRow(printRow_,OLED_BUFFER_WIDTH,buffer_);
		//oled_[o]->setChangedMap(oledBufferRow_,0xFFFF);   //argh if I can work this bug out, I can print to any row
		readyToPrint_ = false;
	}
}
