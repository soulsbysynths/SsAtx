/*
* Oled.cpp
*
* Created: 12/01/2018 17:27:01
* Author: info
*/


#include "Oled.h"

// default constructor
Oled::Oled(uint8_t index, OledBase* base, uint8_t width, uint8_t height)
{
	index_ = index;
	base_ = base;
	width_ = width;
	height_ = height;
	columns_ = width_ / FONT_WIDTH;
	rows_ = height_ / FONT_HEIGHT;

	charBuffer_ = new uint8_t *[columns_];
	for (uint_fast8_t i=0;i<columns_;++i)
	{
		charBuffer_[i] = new uint8_t[rows_];
		memset(charBuffer_[i],0,rows_);
	}
	invertStart_ = new uint8_t[rows_];
	memset(invertStart_,0,sizeof(uint8_t) * rows_);
	invertEnd_ = new uint8_t[rows_];
	memset(invertEnd_,0,sizeof(uint8_t) * rows_);
	wideMap_ = new uint16_t[rows_];
	memset(wideMap_,0,sizeof(uint16_t) * rows_);
	changedMap_ = new uint16_t[rows_];
	memset(changedMap_,0,sizeof(uint16_t) * rows_);
} //Oled

// default destructor
Oled::~Oled()
{
	if(base_!=NULL)
	{
		delete base_;
	}
	// free the allocated memory
	for(uint_fast8_t i=0;i<columns_;++i)
	{
		if (charBuffer_[i]!=NULL)
		{
			delete [] charBuffer_[i];
		}
	}
	if (charBuffer_!=NULL)
	{
		delete [] charBuffer_;
	}
	if (invertStart_!=NULL)
	{
		delete [] invertStart_;
	}
	if (invertEnd_!=NULL)
	{
		delete [] invertEnd_;
	}
	if (wideMap_!=NULL)
	{
		delete [] wideMap_;
	}
	if (changedMap_!=NULL)
	{
		delete [] changedMap_;
	}
} //~Oled

void Oled::init(uint8_t extVccCmd)
{
	// Init sequence
	base_->oledTxCommand(index_, SSD1306_DISPLAYOFF);                    // 0xAE
	base_->oledTxCommand(index_, SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
	base_->oledTxCommand(index_, 0x80);                                  // the suggested ratio 0x80

	base_->oledTxCommand(index_, SSD1306_SETMULTIPLEX);                  // 0xA8
	base_->oledTxCommand(index_, height_ - 1);

	base_->oledTxCommand(index_, SSD1306_SETDISPLAYOFFSET);              // 0xD3
	base_->oledTxCommand(index_, 0x0);                                   // no offset
	base_->oledTxCommand(index_, SSD1306_SETSTARTLINE | 0x0);            // line #0
	base_->oledTxCommand(index_, SSD1306_CHARGEPUMP);                    // 0x8D
	base_->oledTxCommand(index_, 0x10); //ext vcc
	base_->oledTxCommand(index_, SSD1306_MEMORYMODE);                    // 0x20
	base_->oledTxCommand(index_, 0x00);                                  // 0x0 act like ks0108
	base_->oledTxCommand(index_, SSD1306_SEGREMAP | 0x1);
	base_->oledTxCommand(index_, SSD1306_COMSCANDEC);                    // 0xC8

	base_->oledTxCommand(index_, SSD1306_SETCOMPINS);                    // 0xDA
	base_->oledTxCommand(index_, 0x12);

	base_->oledTxCommand(index_, SSD1306_SETCONTRAST);                   // 0x81
	base_->oledTxCommand(index_, 0x80);

	base_->oledTxCommand(index_, SSD1306_SETPRECHARGE);                  // 0xd9
	base_->oledTxCommand(index_, extVccCmd); //ext vcc

	base_->oledTxCommand(index_, SSD1306_SETVCOMDETECT);                 // 0xDB
	base_->oledTxCommand(index_, 0x40);
	base_->oledTxCommand(index_, SSD1306_DISPLAYALLON_RESUME);           // 0xA4
	base_->oledTxCommand(index_, SSD1306_NORMALDISPLAY);                 // 0xA6
	//ssd1306_command(SSD1306_INVERTDISPLAY);
	
	base_->oledTxCommand(index_, SSD1306_DEACTIVATE_SCROLL);
}

void Oled::txDisplayOnOff(bool on)
{
	if(on)
	{
		base_->oledTxCommand(index_, SSD1306_DISPLAYON);
	}
	else
	{
		base_->oledTxCommand(index_, SSD1306_DISPLAYOFF);
	}
}

void Oled::clearRow(uint_fast8_t row)
{
	for (uint_fast8_t i=0;i<columns_;++i)
	{
		if (charBuffer_[i][row]!=' ')
		{
			charBuffer_[i][row] = ' ';
			bitSet(changedMap_[row],i);
		}
		if (bitRead(wideMap_[row],i)!=false)
		{
			bitWrite(wideMap_[row],i,false);
			bitSet(changedMap_[row],i);
		}
	}
	//if(thermometerRow_==row)
	//{
	//thermometerRow_ = atx::UNUSED;  //no this doesn't work for some reason
	//}
}

void Oled::clearDisplay()
{
	base_->oledTxCommand(index_,SSD1306_COLUMNADDR);
	base_->oledTxCommand(index_,0);   // Column start address (0 = reset)
	base_->oledTxCommand(index_,127); // Column ed address (127 = reset)
	
	base_->oledTxCommand(index_,SSD1306_PAGEADDR);
	base_->oledTxCommand(index_,0); // Page start address (0 = reset)
	base_->oledTxCommand(index_,7); // Page end address (7 = reset)

	base_->oledTxZeroData(index_,1024);
	for (uint_fast8_t i=0;i<columns_;++i)
	{
		memset(charBuffer_[i],' ',rows_);
	}
	//for (uint_fast8_t i=0;i<(columns_>>1);++i)
	//{
	//memset(invertMap_[i],0,rows_);
	//}
	memset(invertStart_,0,sizeof(uint8_t) * rows_);
	memset(invertEnd_,0,sizeof(uint8_t) * rows_);
	memset(wideMap_,0,sizeof(uint16_t) * rows_);
	memset(changedMap_,0xFF,sizeof(uint16_t) * rows_);
	thermometerRow_ = atx::UNUSED;
}

void Oled::printRow(uint_fast8_t row, uint_fast8_t width, uint16_t * gfx)
{
	uint_fast8_t off;
	for (uint_fast8_t i=0;i<columns_;++i)
	{
		charBuffer_[i][row] = FONT_DUMMY_GFX;
	}
	if(width_==OLED_WIDE_W)
	{
		off = 16;
	}
	else
	{
		off = 40;
	}
	base_->oledTxCommand(index_,SSD1306_COLUMNADDR);
	base_->oledTxCommand(index_,off);   // Column start address (0 = reset)
	base_->oledTxCommand(index_,off + width_ - 1);

	base_->oledTxCommand(index_,SSD1306_PAGEADDR);
	base_->oledTxCommand(index_,(row << 1)); // Page start address (0 = reset)
	base_->oledTxCommand(index_,(row << 1)+1); // Page end address
	uint8_t data[width];
	for (uint8_t y=0;y<2;++y)
	{
		for (uint8_t x = 0;x<width;++x)
		{
			data[x] = (gfx[x] >> (y<<3)) & 0xFF;
		}
		base_->oledTxData(index_, data, width, false);
	}

}
void Oled::printPixel(uint_fast8_t x, uint_fast8_t row, uint_fast8_t y, bool wide)
{
	uint_fast8_t off;
	pixelX_ = x;
	pixelY_ = y;
	uint8_t line = y>>3;
	uint8_t data[1] = {0};
	bitSet(data[0],(y & 0x07));

	if(width_==OLED_WIDE_W)
	{
		off = 16;
	}
	else
	{
		off = 40;
	}
	base_->oledTxCommand(index_,SSD1306_COLUMNADDR);
	base_->oledTxCommand(index_,x+off);   // Column start address (0 = reset)
	if (wide)
	{
		base_->oledTxCommand(index_,x+off+1);
	}
	else
	{
		base_->oledTxCommand(index_,x+off);
	}
	
	base_->oledTxCommand(index_,SSD1306_PAGEADDR);
	base_->oledTxCommand(index_,(row << 1)+line); // Page start address (0 = reset)
	base_->oledTxCommand(index_,(row << 1)+line); // Page end address
	base_->oledTxData(index_, data, 1, wide);
}

void Oled::printLine(uint_fast8_t x, uint_fast8_t row, uint_fast8_t y, bool wide)
{
	int_fast8_t x0 = pixelX_, y0 = pixelY_, x1 = x, y1 = y;
	pixelX_ = x;
	pixelY_ = y;
	
	int_fast8_t dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int_fast8_t dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int_fast8_t err = (dx>dy ? dx : -dy)/2, e2;
	
	for(;;)
	{
		printPixel(x0,row,y0,wide);
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

void Oled::printChar(uint_fast8_t col, uint_fast8_t row, uint8_t charnum, bool wide) {
	using namespace atx;
	uint_fast8_t off;
	if(width_==OLED_WIDE_W)
	{
		off = 2;
	}
	else
	{
		off = 5;
	}
	base_->oledTxCommand(index_,SSD1306_COLUMNADDR);
	base_->oledTxCommand(index_,(col+off) << 3);   // Column start address (0 = reset)
	if (wide)
	{
		base_->oledTxCommand(index_,((col+off) << 3) + 15);
	}
	else
	{
		base_->oledTxCommand(index_,((col+off) << 3) + 7);
	}
	
	base_->oledTxCommand(index_,SSD1306_PAGEADDR);
	base_->oledTxCommand(index_,(row << 1)); // Page start address (0 = reset)
	base_->oledTxCommand(index_,(row << 1) + 1); // Page end address
	
	if(invertStart_[row]!=UNUSED && invertEnd_[row]!=UNUSED)
	{
		uint8_t x = col << FONT_WIDTH_BS;
		uint8_t f[FONT_HEIGHT];
		if (thermometerRow_==row && charnum>=43 && charnum<=57)
		{
			charnum += 109;  //tiny num
		}

		for (uint_fast8_t i=0;i<FONT_HALF_HEIGHT;++i)
		{
			if((x>=invertStart_[row] && x<invertEnd_[row]) || (x>=invertEnd_[row] && x<invertStart_[row]))
			{
				f[i] = ~FONT_8X16[charnum][i];
				f[i+FONT_HALF_HEIGHT] = ~FONT_8X16[charnum][i+FONT_HALF_HEIGHT];
			}
			else
			{
				f[i] = FONT_8X16[charnum][i];
				f[i+FONT_HALF_HEIGHT] = FONT_8X16[charnum][i+FONT_HALF_HEIGHT];
			}
			x++;
		}
		if (thermometerRow_==row)
		{
			if (thermometerLine_ && col==(columns_>>1))
			{
				f[0] = ~f[0];
				f[8] = ~f[8];
			}
			uint8_t therm;
			if (col==0)
			{
				therm = 0;
			}
			else if(col==(columns_-1))
			{
				therm = 2;
			}
			else
			{
				therm = 1;
			}
			for (uint_fast8_t i = FONT_HEIGHT ; i-- > 0 ; )
			{
				f[i] |= FONT_THERMOMETER[therm][i];
				f[i] &= FONT_THERMOMETER_MASK[therm][i];
			}
		}
		
		base_->oledTxData(index_, f, FONT_HEIGHT, wide);
	}
	else
	{
		base_->oledTxData(index_, FONT_8X16[charnum], FONT_HEIGHT, wide);
	}
}

void Oled::printDigit6x8Buffer(uint_fast8_t col, uint_fast8_t row, uint_fast8_t digit)
{
	char6x8Buffer_[col][row] = digit;
	changed6x8_ = true;
}

void Oled::printDigit6x8(uint_fast8_t col, uint_fast8_t row, uint_fast8_t digit)
{
	uint_fast8_t off;

	row += OFFSET_6X8_ROW;
	off = 7;

	base_->oledTxCommand(index_,SSD1306_COLUMNADDR);
	base_->oledTxCommand(index_,((col+off) * 6) - 2);   // Column start address (0 = reset)
	base_->oledTxCommand(index_,((col+off) * 6) + 3);
	
	base_->oledTxCommand(index_,SSD1306_PAGEADDR);
	base_->oledTxCommand(index_,row); // Page start address (0 = reset)
	base_->oledTxCommand(index_,row); // Page end address

	base_->oledTxData(index_, FONT_6x8_DIGIT[digit], 6, false);
}

void Oled::printIntBuffer(uint_fast8_t col, uint_fast8_t row, int_fast32_t num, bool wide, bool padBlank, uint8_t leadingZeros)
{
	size_t maxLen = columns_ - col; //null term char
	if (wide)
	{
		maxLen >>= 1;
	}
	maxLen++;
	char strTest[maxLen];
	char fmt[] = "%00d";
	int realSize = 0;
	fmt[2] = leadingZeros + 48;
	realSize = snprintf(strTest,maxLen,fmt,num);
	if(realSize>0 && realSize<maxLen)
	{
		realSize++; //output doesn't include null term
		char str[realSize];
		strncpy(str,strTest,sizeof(str));
		printStringBuffer(col,row,str, wide,padBlank);
	}
}

void Oled::printFloatBuffer(uint_fast8_t col, uint_fast8_t row, float num, bool wide, bool padBlank, uint8_t decimalPlaces)
{
	size_t maxLen = columns_ - col; //null term char
	if (wide)
	{
		maxLen >>= 1;
	}
	maxLen++;
	char strTest[maxLen];
	char fmt[] = "%.0f";
	int realSize = 0;
	fmt[2] = decimalPlaces + 48;
	realSize = snprintf(strTest,maxLen,fmt,num);
	if(realSize>0 && realSize<maxLen)
	{
		realSize++; //output doesn't include null term
		char str[realSize];
		strncpy(str,strTest,sizeof(str));
		printStringBuffer(col,row,str, wide,padBlank);
	}
}

void Oled::printStringBuffer(uint_fast8_t col, uint_fast8_t row, const char * str, bool wide, bool padBlank)
{
	uint_fast8_t  l = strlen(str);
	uint_fast8_t c = col;

	for(uint_fast8_t i=0;i<l;++i)
	{
		printCharBuffer(c, row, (uint8_t)str[i],wide);
		if(wide)
		{
			c += 2;
		}
		else
		{
			c++;
		}
	}
	if(padBlank)
	{
		for(uint_fast8_t i=c;i<columns_;i++)
		{
			printCharBuffer(i, row, ' ',false);
		}
	}
}

void Oled::printCharBuffer(uint_fast8_t col, uint_fast8_t row, uint8_t charnum, bool wide)
{
	if (col>=columns_ || row>=rows_)
	{
		return;
	}
	if ((charBuffer_[col][row]!=charnum) || (bitRead(wideMap_[row],col)!=wide))
	{
		charBuffer_[col][row] = charnum;
		bitWrite(wideMap_[row],col,wide);
		bitSet(changedMap_[row],col);
	}
}

//void Oled::setOverlay(const char* newOverlay)
//{
//if (overlay_==NULL)
//{
//overlay_ = (char *)malloc(columns_);
//}
//strncpy(overlay_,newOverlay,columns_);
//changedMap_[overlayRow_] = 0xFFFF;
//}
//
//void Oled::clearOverlay()
//{
//free(overlay_);
//overlay_ = NULL;
//changedMap_[overlayRow_] = 0xFFFF;
//}

void Oled::refresh()
{
	for (uint_fast8_t row=0;row<rows_;++row)
	{
		if(changedMap_[row]>0)
		{
			for (uint_fast8_t col=0;col<columns_;++col)
			{
				if (bitRead(changedMap_[row],col))
				{
					bitClear(changedMap_[row],col);
					//if(overlay_!=NULL && row==overlayRow_ && *(overlay_+col)!=' ')
					////if (arFrame_>0 && row==AR_ROW && AR_ANIM[arFrame_][col]!=atx::UNUSED)
					//{
					//printChar(col,row,*(overlay_+col),false);
					//}
					//else
					//{
					if (bitRead(wideMap_[row],col))
					{
						printChar(col,row,charBuffer_[col][row],true);
						if (col<(columns_-1))
						{
							bitSet(changedMap_[row],col+1);
							//charBuffer_[col+1][row] = charBuffer_[col][row];
						}
					}
					else
					{
						printChar(col,row,charBuffer_[col][row],false);
					}
					//}
				}
				if (bitRead(wideMap_[row],col))
				{
					col++;
				}
			}
		}
	}
	if(changed6x8_)
	{
		for(uint_fast8_t r=0;r<2;++r)
		{
			for(uint_fast8_t c=0;c<8;++c)
			{
				printDigit6x8(c,r,char6x8Buffer_[c][r]);
			}
		}
		changed6x8_ = false;
	}
}


void Oled::setInvertRange(uint_fast8_t row, uint8_t start, uint8_t end)
{
	uint8_t rangeValue[2][2] = {{invertStart_[row],invertEnd_[row]},{start,end}};  //[old = 0,new = 1][start value = 0,end value = 1]

	invertStart_[row] = rangeValue[1][0];
	invertEnd_[row] = rangeValue[1][1];
	for (uint_fast8_t i=0;i<columns_;++i)
	{
		uint8_t rangeMask[2][3] = {{0}};   //[old = 0, new = 1][start = 0, end = 1, result = 2]
		for(uint_fast8_t j=0;j<2;++j)  //old new value
		{
			for(uint_fast8_t k=0;k<2;++k)  //start / end value
			{
				if(rangeValue[j][k]>8)
				{
					rangeMask[j][k] = 0xFF;
					rangeValue[j][k] -= 8;
				}
				else
				{
					rangeMask[j][k] = (1U << rangeValue[j][k]) - 1U;
					rangeValue[j][k] = 0;
				}
			}
			rangeMask[j][2] = rangeMask[j][0] ^ rangeMask[j][1];
		}
		uint8_t changedByte = rangeMask[0][2] ^ rangeMask[1][2];
		if(changedByte>0) 
		{
			bitSet(changedMap_[row],i);
		}
	}
}
void Oled::clearInvertMapRow(uint_fast8_t row)
{
	using namespace atx;
	if(invertStart_[row]!=UNUSED || invertEnd_[row]!=UNUSED)
	{
		invertStart_[row] = UNUSED;
		invertEnd_[row] = UNUSED;
		changedMap_[row] = 0xFFFF;		
	}
}

void Oled::clearInvertMap()
{
	for (uint_fast8_t i=0;i<rows_;++i)
	{
		clearInvertMapRow(i);
	}
}

void Oled::setThermometerLine(bool newLine)
{
	if (newLine!=thermometerLine_)
	{
		bitSet(changedMap_[thermometerRow_],(columns_>>1));
		thermometerLine_ = newLine;
	}
}
void Oled::setThermometerRow(uint8_t newRow)
{
	if (newRow!=thermometerRow_)
	{
		if (thermometerRow_!=atx::UNUSED)
		{
			changedMap_[thermometerRow_] = 0xFFFF;
		}
		if (newRow!=atx::UNUSED)
		{
			changedMap_[newRow] = 0xFFFF;
		}
		thermometerRow_ = newRow;
	}
}

//void Oled::printInt(uint_fast8_t col, uint_fast8_t row, int_fast32_t num, bool wide, bool padBlank, uint8_t leadingZeros)
//{
//size_t maxLen = columns_ - col; //null term char
//if (wide)
//{
//maxLen >>= 1;
//}
//maxLen++;
//char strTest[maxLen];
//char fmt[] = "%00d";
//int realSize = 0;
//fmt[2] = leadingZeros + 48;
//realSize = snprintf(strTest,maxLen,fmt,num);
//if(realSize>0 && realSize<maxLen)
//{
//realSize++; //output doesn't include null term
//char str[realSize];
//strncpy(str,strTest,sizeof(str));
//printString(col,row,str, wide,padBlank);
//}
//}
//
//void Oled::printFloat(uint_fast8_t col, uint_fast8_t row, float num, bool wide, bool padBlank, uint8_t decimalPlaces)
//{
//size_t maxLen = columns_ - col; //null term char
//if (wide)
//{
//maxLen >>= 1;
//}
//maxLen++;
//char strTest[maxLen];
//char fmt[] = "%.0f";
//int realSize = 0;
//fmt[2] = decimalPlaces + 48;
//realSize = snprintf(strTest,maxLen,fmt,num);
//if(realSize>0 && realSize<maxLen)
//{
//realSize++; //output doesn't include null term
//char str[realSize];
//strncpy(str,strTest,sizeof(str));
//printString(col,row,str, wide,padBlank);
//}
//}
//
//void Oled::printString(uint_fast8_t col, uint_fast8_t row, const char * str, bool wide, bool padBlank)
//{
//uint_fast8_t  l = strlen(str);
//uint_fast8_t c = col;
//
//for(uint_fast8_t i=0;i<l;++i)
//{
//printChar(c, row, (uint8_t)str[i],wide);
//if(wide)
//{
//c += 2;
//}
//else
//{
//c++;
//}
//}
//if(padBlank)
//{
//for(uint_fast8_t i=c;i<columns_;++i)
//{
//printChar(i, row, 32,false);
//}
//}
//}