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
#include "Atx.h"
#include "AtxCcSet.h"
#include "AtxCcEnum.h"
#include "AtxCPrintDispProgmem.h"
#include "AtxCProgmem.h"
#include "AtxSynthZoneMap.h"
#include "Midi.h"
#include "SysexMsg.h"

typedef struct PrintDispInfo
{
	uint8_t row;
	uint8_t bits; 
	uint8_t tag;
	uint8_t value;
	uint16_t scValue;
	uint8_t zone;
} PrintDispInfo;

class AtxCPrintDisp
{
	//variables
	public:
	static atx::Firmware channelFirmware[atx::MAX_CARDS];
	protected:
	private:
	static OledBuffer* buffer_;
	static Oled* xtrOled_;
	
	//static atx::AtxCcList ccList_[atx::MAX_ZONES];
	//functions
	public:
	static void printDisp(Oled* oled, atx::AtxCcSet* ccSet, uint_fast8_t cc);
	static void printXtrWave(midi::SysexMsg* msg);
	static void printXtrArp(midi::SysexMsg* msg);
	static midi::MidiSysexType getSysexTypeFromXtr(atx::DispExtra xtr);
	//static atx::AtxCcList& getCcList(uint_fast8_t zone){return AtxCPrintDisp::ccList_[zone];}
	static OledBuffer*& getOledBuffer(){return AtxCPrintDisp::buffer_;}
	static Oled*& getOledPtr(){return AtxCPrintDisp::xtrOled_;}
	protected:
	private:
	AtxCPrintDisp(){}
	~AtxCPrintDisp(){}
	AtxCPrintDisp( const AtxCPrintDisp &c );
	AtxCPrintDisp& operator=( const AtxCPrintDisp &c );
	static void convMidiNoteString(uint8_t note, char * str);
	static void clearThermom(Oled* oled, uint_fast8_t row);
	static void clearInvert(Oled* oled, uint_fast8_t row);
	static void printFmtInt(Oled* oled, PrintDispInfo* info);
	static void printFmtOnOff(Oled* oled, PrintDispInfo* info);
	static void printFmtSigned(Oled* oled, PrintDispInfo* info);
	static void printFmt6BitRange(Oled* oled, PrintDispInfo* info);
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
	static void printXtrEnv(PrintDispInfo* info, atx::AtxCcSet* ccSet);
	static void printXtrFilter(PrintDispInfo* info, atx::AtxCcSet* ccSet);
	static void printXtrZoneLayout();
	static void printXtrOdyIcon(PrintDispInfo* info, atx::AtxCcSet* ccSet);
}; //AtxCPrintDisp

#endif //__ATXCPRINTDISP_H__
