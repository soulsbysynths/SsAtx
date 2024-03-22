/*
* AtxCPrintDisp.h
*
* Created: 23/08/2021 21:18:00
* Author: info
*/


#ifndef __ATXCPRINTDISP_H__
#define __ATXCPRINTDISP_H__

#include "Oled.h"
#include "OledBuffer.h"
#include "AtxCcSet.h"
#include "AtxCcEnum.h"
#include "AtxC"

typedef struct PrintDispInfo
{
	uint8_t row;
	uint8_t bits; 
	uint8_t tag;
	uint16_t scValue;
} PrintDispInfo;

class AtxCPrintDisp
{
	//variables
	public:
	static OledBuffer* buffer;
	protected:
	private:

	//functions
	public:
	static void printDisp(Oled* oled, atx::AtxCcSet* ccSet, uint_fast8_t cc);
	protected:
	private:
	AtxCPrintDisp(){}
	~AtxCPrintDisp(){}
	AtxCPrintDisp( const AtxCPrintDisp &c );
	AtxCPrintDisp& operator=( const AtxCPrintDisp &c );
	static void convMidiNoteString(uint8_t note, char * str);
	static void printCcOneLine(Oled* oled, atx::AtxCcSet* ccSet, uint_fast8_t cc);
	static void clearThermom(Oled* oled, uint_fast8_t row);
	static void clearInvert(Oled* oled, uint_fast8_t row);
	static void printFmtInt(Oled* oled, PrintDispInfo* info);
	static void printFmtOnOff(Oled* oled, PrintDispInfo* info);
	static void printFmtSigned(Oled* oled, PrintDispInfo* info);
	static void printFmtDrumPattern(Oled* oled, PrintDispInfo* info, uint_fast8_t cc);
	static void printFmtSignedFloat(Oled* oled, PrintDispInfo* info);
	static void printFmtMidiNote(Oled* oled, PrintDispInfo* info);
	static void printFmtDiv(Oled* oled, PrintDispInfo* info);
	static void printFmtPpqn(Oled* oled, PrintDispInfo* info);
	static void printFmtIntPlain(Oled* oled, PrintDispInfo* info);
	static void printFmt2Bit(Oled* oled, PrintDispInfo* info);
	static void printFmtBinary(Oled* oled, PrintDispInfo* info);
	static void printFmtOffsetInt(Oled* oled, PrintDispInfo* info);
	static void printFmtFirmware(Oled* oled, PrintDispInfo* info);
	static void printFmtCcList(Oled* oled, PrintDispInfo* info, atx::AtxCcSet* ccSet);
	static void printFmtTag(Oled* oled, PrintDispInfo* info);
	static void printXtrWave(Oled* oled, PrintDispInfo* info);
	static void printXtrArp(Oled* oled, PrintDispInfo* info);
	static void printXtrEnv(Oled* oled, PrintDispInfo* info, atx::AtxCcSet* ccSet);
	static void printXtrFilter(Oled* oled, PrintDispInfo* info, atx::AtxCcSet* ccSet);
	static void printXtrZoneLayout(Oled* oled);
	static void printXtrOdyIcon(Oled* oled, PrintDispInfo* info, atx::AtxCcSet* ccSet);
}; //AtxCPrintDisp

#endif //__ATXCPRINTDISP_H__
