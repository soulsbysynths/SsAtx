/*
 * SdCardBase.h
 *
 * Created: 04/06/2018 17:54:42
 *  Author: info
 */ 


#ifndef ATXCSDCARDBASE_H_
#define ATXCSDCARDBASE_H_

#include <stdint.h>

class AtxCSdCardBase
{
	public:
	virtual void sdFunctionChanged(uint8_t func) = 0;
	virtual void sdFunctionActivated(uint8_t func) = 0;
	
	virtual void sdPrintString(const char * string, const char icon = 0) = 0;
	virtual void sdPrintFail(const char * msg) = 0;
	virtual void sdHighlightChar(uint8_t charPos) = 0;
	virtual void sdRefreshStatusBar(uint8_t barFill) = 0;
	//virtual void sdLoadSetting(uint8_t param, uint16_t value) = 0;
	//virtual void sdLoadPatch(uint8_t cardNum, uint8_t param, uint16_t value) = 0;
};

#endif /* ATXCSDCARDBASE_H_ */