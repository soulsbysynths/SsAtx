/*
* AtxCEditor.h
*
* Created: 19/01/2018 15:54:23
* Author: info
*/


#ifndef __ATXCEDITOR_H__
#define __ATXCEDITOR_H__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "AtxCEditorBase.h"
#include "Atx.h"
#include "MidiMsg.h"
#include "AtxCcSet.h"


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

class AtxCEditor : public atx::AtxCcSet
{
	//variables
	public:
	enum AwaitActivateMode : uint8_t
	{
		AAM_OFF = 0,
		AAM_ON,
		AAM_YES,
		AAM_NO
	};
	static const uint8_t EDIT_CHANNELS = 17;
	static const uint8_t SETTINGS_CHANNEL = 16;
	static const uint8_t BANKS = 4;
	static const uint8_t BANKS_MASK = BANKS-1;
	static const uint8_t FUNCS = 16;
	static const uint8_t FUNCS_MASK = FUNCS-1;
	static const uint8_t ZONES = 16;  //8 synth zones and theoretical 8 global zones
	static const uint8_t ZONES_MASK = FUNCS-1;
	static const uint8_t CTRLS = 6;
	protected:
	private:
	AtxCEditorBase* base_;
	uint8_t channel_ = 0;
	uint8_t func_[EDIT_CHANNELS][BANKS] = {{0}};
	uint8_t zone_[EDIT_CHANNELS] = {0};
	uint8_t	bank_[EDIT_CHANNELS] = {0};
	AwaitActivateMode awaitActivate_ = AAM_OFF;
	//functions
	public:
	static AtxCEditor& getInstance()
	{
		static AtxCEditor instance; // Guaranteed to be destroyed.
		return instance;  // Instantiated on first use.
	}
	//void construct(AtxCEditorBase* base, AtxCard* cardPtrs[]);
	void construct(AtxCEditorBase* base);
	void setChannel(uint8_t newValue);
	uint8_t getChannel(){return channel_;}
	void setBank(uint8_t newValue);
	uint8_t getBank(){return bank_[channel_];}
	void nextBank();
	void setFunc(uint8_t newValue);
	uint8_t getFunc(){return func_[channel_][bank_[channel_]];}
	void nextFunc(bool way);
	void setEditorFromCc(uint8_t channel, uint8_t cc);
	void setZone(uint8_t channel, uint8_t newValue);
	void setZone(uint8_t newValue){setZone(channel_,newValue);}
	uint8_t getZone(){return zone_[channel_];}
	void nextZone(bool way);
	uint8_t getCcFromCtrlCode(bool isFunc, uint8_t ctrlNum){return AtxCcSet::getCcFromCtrlCode(isFunc,bank_[channel_],zone_[channel_],ctrlNum);}
	uint8_t getCcFromFunc(){return AtxCcSet::getCcFromCtrlCode(true,bank_[channel_],zone_[channel_],func_[channel_][bank_[channel_]]);}
	uint8_t getCcFromCtrl(uint8_t ctrl){return AtxCcSet::getCcFromCtrlCode(false,bank_[channel_],zone_[channel_],ctrl);}
	uint8_t getCcFromZone();
	uint8_t getFuncFromCc(uint8_t cc){return (getCcInfoCtrlEnabled(cc)==true && getCcInfoCtrlIsFunc(cc)==true) ? getCcInfoCtrlNum(cc) : atx::UNUSED;}
	uint8_t getCtrlFromCc(uint8_t cc){return (getCcInfoCtrlEnabled(cc)==true && getCcInfoCtrlIsFunc(cc)==false) ? getCcInfoCtrlNum(cc) : atx::UNUSED;}
	uint8_t getBankFromCc(uint8_t cc){return (getCcInfoCtrlEnabled(cc)==true) ? getCcInfoCtrlBank(cc) : atx::UNUSED;}
	bool isCcOnActiveCtrl(uint8_t cc);
	void setCcValFromCtrl(uint8_t ctrl, uint8_t newValue);
	void setCcValFromFunc(uint8_t newValue);
	void setCcValFromMidiMsg(midi::MidiMsg* msg);
	void setAwaitActivate(AwaitActivateMode way);
	AwaitActivateMode getAwaitActivate(){return awaitActivate_;}
	//bool isFuncDrumPattern(){return card_[channel_]->getParamInfoFmtNum(getCcFromFunc())==AtxCard::FMT_DRMPATTERN;}
	protected:
	private:
	AtxCEditor() {}
	AtxCEditor(AtxCEditorBase* base);
	~AtxCEditor();
	AtxCEditor( const AtxCEditor &c );
	AtxCEditor& operator=( const AtxCEditor &c );
}; //AtxCEditor

#endif //__ATXCEDITOR_H__

///*
//* AtxCEditor.h
//*
//* Created: 19/01/2018 15:54:23
//* Author: info
//*/
//
//
//#ifndef __ATXCEDITOR_H__
//#define __ATXCEDITOR_H__
//
//#include <stdlib.h>
//#include <stdint.h>
//#include <string.h>
//#include "AtxCEditorBase.h"
//#include "AtxCard.h"
//#include "Atx.h"
//#include "AtxParamList.h"
//
//using namespace atx;
//
//class AtxCEditor
//{
////variables
//public:
//static const uint8_t EDIT_CTRLS = 6;
//static const uint8_t EDIT_FUNCS = 16;
//static const uint8_t EDIT_BANKS = 3;
//protected:
//private:
//static const uint8_t FUNC_CC[];
//static const uint8_t CTRL_CC[];
//AtxCEditorBase* base_;
////AtxCard * card_[atx::MAX_CARDS];  //pointers to the cards
//uint8_t channel_ = 0xFF;	//start out of bounds, so hello routine works
////uint8_t editBit_ = 0;
//uint8_t func_[atx::MAX_CARDS] = {0};
//uint8_t	bank_[atx::MAX_CARDS] = {0};
////AtxParamList paramList_;
////functions
//public:
//static AtxCEditor& getInstance()
//{
//static AtxCEditor instance; // Guaranteed to be destroyed.
//return instance;  // Instantiated on first use.
//}
////void construct(AtxCEditorBase* base, AtxCard* cardPtrs[]);
//void construct(AtxCEditorBase* base);
//void initialize();
//void setEditChannel(uint8_t newValue);
//uint8_t getEditChannel(){return channel_;}
//void nextEditChannel(bool way);
//void setEditBank(uint8_t newValue);
//uint8_t getEditBank(){return bank_[channel_];}
//void nextEditBank();
//void setEditFunc(uint8_t channel, uint8_t newValue);
//void setEditFunc(uint8_t newValue){setEditFunc(channel_,newValue);}
//uint8_t getEditFunc(){return func_[channel_];}
//void nextEditFunc(bool way);
//void setEditBit(uint8_t newBit);
//uint8_t getEditBit(){return editBit_;}
//uint8_t getCcFromCtrl(uint8_t channel, uint8_t bank, uint8_t ctrl);
//uint8_t getCcFromFunc(uint8_t channel, uint8_t bank, uint8_t func);
//uint8_t getCcFromCtrl(uint8_t bank, uint8_t ctrl){return getCcFromCtrl(channel_,bank,ctrl);}
//uint8_t getCcFromFunc(uint8_t bank, uint8_t func){return getCcFromFunc(channel_,bank,func);}
//uint8_t getCcFromCtrl(uint8_t ctrl){return getCcFromCtrl(bank_[channel_],ctrl);}
//uint8_t getCcFromFunc(uint8_t func){return getCcFromFunc(bank_[channel_],func);}
//uint8_t getCcFromFunc(){return getCcFromFunc(bank_[channel_],func_[channel_]);}
//uint8_t getFuncFromParam(uint8_t param);
//uint8_t getCtrlFromParam(uint8_t param);
//uint8_t getBankFromParam(uint8_t param);
//bool isParamUnused(uint8_t channel, uint8_t param){return (card_[channel]->getParamSize(param)==0);}
//bool isParamUnused(uint8_t param){return isParamUnused(channel_,param);}
//void setParamValFromCtrl(uint8_t ctrl, uint8_t newValue);
//void setParamValFromFunc(uint16_t newValue);
//void toggleBitParamValFromFunc();
//void setParamVal(uint8_t channel, uint8_t param, uint16_t value);
////bool isFuncDrumPattern(){return card_[channel_]->getParamInfoFmtNum(getCcFromFunc())==AtxCard::FMT_DRMPATTERN;}
//protected:
//private:
//AtxCEditor() {}
//AtxCEditor(AtxCEditorBase* base, AtxCard* cardPtrs[]);
//~AtxCEditor();
//AtxCEditor( const AtxCEditor &c );
//AtxCEditor& operator=( const AtxCEditor &c );
//uint_fast16_t mapCtrlVal(uint8_t param,uint8_t value);
////void buildParamList(uint8_t firmware);
//bool isBankEmpty();
//}; //AtxCEditor
//
//#endif //__ATXCEDITOR_H__
