#pragma once
#include "Ssd1306.h"

const Ssd1306::Settings QG9632TSWEG01_SETTINGS = //WIDE OLED
{
	// displayStartLine;
	0x40,
	// contrastLevel;
	0x80,
	// segmentRemap;
	true,
	// invert;
	false,
	// multiplexRatio;
	0x1F,
	// comScanRemapped;
	true,
	// displayOffset;
	0x00,
	// columnOffset - see data sheet for this value
	0x10,
	// divideRatio;
	0x00,
	// oscFreq
	0x08,
	// prechargePeriod;
	0x1F,
	// comPinsAltConfig;
	true,
	// comPinsLRRemap;
	false,
	// vComLevel;
	Ssd1306::VCOM_77,
	// chargePumpEnabled;		
	false 
};

const Ssd1306::Settings CG4864TSWEG04_SETTINGS = //TALL OLED
{
	// displayStartLine;
	0x40,
	// contrastLevel;
	0x80,
	// segmentRemap;
	true,
	// invert;
	false,
	// multiplexRatio;
	0x3F,
	// comScanRemapped;
	true,
	// displayOffset;
	0x00,
	// columnOffset - see data sheet for this value
	0x28,
	// divideRatio;
	0x00,
	// oscFreq
	0x08,
	// prechargePeriod;
	0xF1,
	// comPinsAltConfig;
	true,
	// comPinsLRRemap;
	false,
	// vComLevel;
	Ssd1306::VCOM_77,
	// chargePumpEnabled;		
	false 
};