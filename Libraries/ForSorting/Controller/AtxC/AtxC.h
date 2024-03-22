/*
* AtxC.h
*
* Created: 17/01/2018 16:07:02
* Author: info
*/


#ifndef __ATXC_H__
#define __ATXC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "Atx.h"
#include "AtxCcSet.h"
#include "AtxSynthZoneMap.h"
#include "AtxCSdCard.h"
#include "AtxCHardware.h"
#include "AtxCHardwareBase.h"
#include "AtxCSettings.h"
#include "AtxCSettingsBase.h"
#include "AtxCEditor.h"
#include "AtxCEditorBase.h"
#include "AtxCSequencer.h"
#include "AtxCSequencerBase.h"
#include "AtxCProgmem.h"
#include "AtxCcEnum.h"
#include "Oled.h"
#include "AtxCPrintDisp.h"
#include "MidiMsg.h"
#include "SysexMsg.h"
#include "AtxZone.h"
#include "Adafruit_ZeroTimer.h"
#include "stk500.h"


class AtxC : public AtxCEditorBase, AtxCHardwareBase, AtxCSequencerBase, atx::AtxZoneBase, AtxCSdCardBase
{
	//variables
public:
	enum GlobalBank : uint8_t
	{
		GB_EDIT      = 0,
		GB_SETTINGS,
		GB_SEQUENCER,
		GB_SDCARD,
		GB_MAX,
		GB_INIT      = 0xFF
	};

	static const uint8_t FLASH_TICKS = 4;
	static const uint8_t FLASH_FOREVER = 0xFF;
protected:
private:

	static AtxC * instance_;
	static void pollClkCallback()
	{
		if (AtxC::instance_ != NULL)
		{
			instance_->clks_++; //if there's more than 65535 clks buffered there's something really wrong!
		}
	}

	static volatile uint16_t clks_; //96 PPQN
	static const uint8_t AR_ROW = 2;
	AtxCHardware& hardware_;
	AtxCEditor& editor_;
	AtxCSequencer& sequencer_;
	AtxCSdCard& sdCard_;
	atx::AtxCard** card_ = new atx::AtxCard*[atx::MAX_CARDS];
	atx::AtxZone zone_[atx::MAX_ZONES];
	atx::AtxCcSet settings_;
	Adafruit_ZeroTimer clkTimer_ = Adafruit_ZeroTimer(3);
	
	GlobalBank globalBank_ = GB_INIT;
	bool globalBankActive_[GB_MAX] = { false };
	bool locked_ = false;
	bool debugMidi_ = false;
	midi::MidiMsg testMidiMsg_;
	bool updateZones_ = false;
	
#ifdef TESTER
	bool cancelFlash_[4] = { false };
#endif
	//functions
public:
	AtxC();
	~AtxC();
	void initialize();
	void pollFast();
	void poll(uint8_t ticksPassed);
	//void pollSequencer();
	void setGlobalBank(GlobalBank newBank);
	void printFreeMem();
	
	//**************Hardware***************************
	void hardwareSwitchHeld(uint8_t sw);
	void hardwareSwitchClicked(uint8_t sw);
	void hardwareSwitchDoubleClicked(uint8_t sw);
	void hardwareSwitchChanged(uint8_t sw, bool way);
	void hardwareCtrlValueChanged(uint8_t ctrl, uint8_t newValue);
	void hardwareRotaryEncoderValueChanged(uint8_t rot, int8_t newValue, bool direction);
	void hardwareExtMidiRxd(midi::MidiSource src, midi::MidiMsg * msg){dispatchMidi(src, msg); }
	void hardwareI2cMidiRxd(uint8_t card, midi::MidiMsg * msg);
	void hardwareI2cSysexRxd(uint8_t card, midi::SysexMsg* sysex);
	void hardwareInitCard(uint8_t card);
	void hardwareExtClk();
		
	//**************Editor***************************
	void editorChannelChanged(uint8_t channel);
	void editorBankChanged(uint8_t channel, uint8_t bank);
	void editorCtrlCcChanged(uint8_t ctrl, uint8_t cc);
	void editorCtrlValueChanged(uint8_t ctrl, uint8_t value);
	void editorFuncCcChanged(uint8_t func, uint8_t cc);
	void editorFuncValueChanged(uint8_t value);
	void editorZoneChanged(uint8_t zone);
	void editorCcChanged(midi::MidiMsg* msg){dispatchMidi(midi::MSRC_CONTROLLER, msg); }
	void editorAwaitActivateChanged(uint8_t awaitActMode);
	void editorSettingChanged(uint8_t cc, uint8_t value);
	
	//**************Sequence***************************
	void sequencerEditTrackChanged(uint8_t track);
	void sequencerModeChanged(uint8_t seqMode);
	void sequencerBpmChanged(uint16_t bpm);
	void sequencerEventListChanged(uint16_t index, midi::MidiSeqMsg * msg);
	void sequencerTxMidiMsg(midi::MidiMsg * msg){dispatchMidi(midi::MSRC_SEQ, msg); }
	void sequencerPlayChanged(uint8_t playMode);
	void sequencerOvPlayChanged(uint8_t track, uint8_t seq, bool selected, uint8_t playState, uint8_t display, uint8_t row);
	void sequencerOvSeqChanged(uint8_t track, uint8_t seq, uint8_t display, uint8_t row);
	void sequencerOvSeqNameChanged(uint8_t track, uint8_t seq, const char * name, uint8_t display, uint8_t row);
	void sequencerOvSeqPosChanged(uint8_t track, uint8_t seq, uint8_t pos, uint8_t display, uint8_t row);
	void sequencerOvSeqLoopChanged(uint8_t track, uint8_t seq, bool looping, uint8_t display, uint8_t row);
	void sequencerOvClkChanged(uint16_t bar, uint8_t beat, uint8_t clk);
	void sequencerSeqSeekStartMidi(uint8_t track, uint8_t seq){sdCard_.seekStartMidiStream(track, seq); }//{sdCard_.seekSeq(track,seq,index);sdCard_.readSeq(track,seq,asmls);}
	void sequencerSeqRead(uint8_t track, uint8_t seq, midi::MidiMsgListStream& mmls){sdCard_.readMidiStream(track, seq, mmls); }//{sdCard_.readSeq(track,seq,asmls);}
	void sequencerRecQuantiseChanged(uint8_t quantise);
	void sequencerRecTrackChanged(uint8_t track);
	void sequencerRecSeqPosChanged(uint8_t pos);

	//**************SD***************************
	void sdPrintString(const char * string, const char icon = 0);
	void sdPrintFail(const char * msg);
	void sdHighlightChar(uint8_t charPos);
	void sdFunctionChanged(uint8_t func);
	void sdFunctionActivated(uint8_t sdFunction);
	void sdDestinationChanged(uint8_t dest);
	void sdRefreshStatusBar(uint8_t barFill);
	
	//**************Zone***************************
	inline void zoneTxMidiMsg(uint8_t synth, midi::MidiMsg* msg, bool txPolys)
	{
		using namespace atx;
		msg->setChannel(synth); //This should really happen in Zone.cpp, but save a lot of repeating by doing it here
		if (card_[synth]->connected) hardware_.bufferI2cMidiMsg(synth, msg);
		if (txPolys)
		{
			for (uint8_t i = MAX_SYNTHCARDS; i < MAX_CARDS; ++i)
			{
				if (card_[i]->connected)
				{
					hardware_.bufferI2cMidiMsg(i, msg);
				}
			}
		}
	}
	inline void zoneTxResetClks(uint8_t synth)
	{
		using namespace atx;
		using namespace midi;
		if (sequencer_.getPlayMode() != AtxCSequencer::SPM_STOP) return;
		MidiMsg msg;
		msg.setChannel(synth);
		msg.setCommand(MCMD_CC);
		msg.setData1(MCC_SOUNDOFF);
		if (card_[synth]->connected) hardware_.bufferI2cMidiMsg(synth, &msg);
		for (uint8_t i = MAX_SYNTHCARDS; i < MAX_CARDS; ++i)
		{
			if (card_[i]->connected)
			{
				hardware_.bufferI2cMidiMsg(i, &msg);
			}
		}
	}
protected:
private:
	AtxC(const AtxC &c);
	AtxC& operator=(const AtxC &c);
	void dispatchMidi(midi::MidiSource src, midi::MidiMsg* msg);
	void setNextEditorChannel(bool direction);
	void initCardsZones();
	void txCcs();
	void txZoneFirmwareHex(uint8_t zone);
	void txZoneMap(uint8_t zone);
	void txEditZone(uint8_t zone);
	void updateZones();
	void txSysexGfxReq(uint8_t channel, uint8_t cc);
}; //AtxC

#ifdef __cplusplus
}
#endif
#endif //__ATXC_H__





///*
//* AtxC.h
//*
//* Created: 17/01/2018 16:07:02
//* Author: info
//*/
//
//
//#ifndef __ATXC_H__
//#define __ATXC_H__
//#include <stdlib.h>
//#include "Atx.h"
//#include "SdCard.h"
//#include "AtxCHardware.h"
//#include "AtxCHardwareBase.h"
//#include "AtxCSettings.h"
//#include "AtxCSettingsBase.h"
//#include "AtxCEditor.h"
//#include "AtxCEditorBase.h"
//#include "AtxCSequencer.h"
//#include "AtxCSequencerBase.h"
//#include "AtxCProgmem.h"
//#include "AteOscOscillator.h"
//#include "AtxParams.h"
//#include "Oled.h"
//#include "MidiMsg.h"
//#include "AtxZone.h"
//#include "Adafruit_ZeroTimer.h"
//#include "stk500.h"
//#include "ARMsgBox.h"
//#include "ARMsgBoxBase.h"
//
////class AtxC : public AtxCHardwareBase, AtxCEditorBase, AtxCSequencerBase, AtxZoneBase, SdCardBase, ARMsgBoxBase
//class AtxC : public AtxCHardwareBase, AtxCEditorBase, AtxZoneBase
//{
////variables
//public:
//enum GlobalBank : uint8_t
//{
//GB_EDIT = 0,
//GB_SEQUENCER,
//GB_SETTINGS,
//GB_INIT = 0xFF
//};
//
//static const uint8_t FLASH_TICKS = 4;
//static const uint8_t FLASH_DRUM = 0;
//static const uint8_t FLASH_FOREVER = 0xFF;
//static const uint8_t SETTINGS_DUMMYCARD = atx::MAX_CARDS;
//protected:
//private:
//
//static AtxC * instance_;
//static void pollClkCallback()
//{
//if (AtxC::instance_ != NULL)
//{
//instance_->clks_++;		 //if there's more than 65535 clks buffered there's something really wrong!
//}
//}
//static void pollMsCallback()
//{
//if (AtxC::instance_ != NULL)
//{
//instance_->msTicks_++;  //if there's more than 65535 ms buffered there's something really wrong!
//}
//}
//
//static volatile uint16_t msTicks_;
//static volatile uint16_t clks_;  //96 PPQN
////static uint32_t usPerSeqTick_;  //120bpm  remember seq tick is
//static const uint8_t AR_ROW = 2;
//AtxCHardware& hardware_;
////	AtxCSettings& settings_;
////	AtxCSequencer& sequencer_;
//AtxCEditor& editor_;
////SdCard& sdCard_;
////ARMsgBox& arMsgBox_;
//AtxZone zone_[atx::MAX_ZONES];
//AtxCard card_[atx::MAX_CARDS];  //what I want to store in a patch
//Adafruit_ZeroTimer clkTimer_ = Adafruit_ZeroTimer(3);
//Adafruit_ZeroTimer msTimer_ = Adafruit_ZeroTimer(4);
//GlobalBank globalBank_ = GB_INIT;
//bool locked_ = false;
//uint8_t testNote_ = 60;
//uint8_t oledZoneLayout_[atx::MAX_SYNTHCARDS] = {0};
//#ifdef TESTER
//bool cancelFlash_[4] = {false};
//#endif
////functions
//public:
//AtxC();
//~AtxC();
//void initialize();
//void poll();
////void pollSequencer();
//void setGlobalBank(GlobalBank newBank);
//void hardwareSwitchHeld(uint8_t sw);
//void hardwareSwitchClicked(uint8_t sw);
//void hardwareSwitchDoubleClicked(uint8_t sw);
//void hardwareSwitchChanged(uint8_t sw, bool way);
//void hardwareCtrlValueChanged(uint8_t ctrl, uint8_t newValue);
//void hardwareRotaryEncoderValueChanged(uint8_t rot, int8_t newValue, bool direction);
//void hardwareMidiPacketRxd(MidiMsg * msg);
////void hardwareI2cPacketRxd(uint8_t slave, MidiMsg * msg);
//void hardwareI2cPacketRxd(uint8_t card, MidiMsg * msg);
//void hardwareInitCard(uint8_t card);
//void hardwareExtClk();
////void settingEditSettingChanged(uint8_t setting);
////void settingEditCtrlValueChanged(uint8_t ctrl, uint8_t param);
////void settingEditFuncChanged( uint8_t func, uint8_t param);
////void settingEditFuncValueChanged(uint8_t param);
////void settingParamChanged(uint8_t param, uint16_t value);
////bool settingTxZoneLayout(bool testMode, bool initCards);
//void editorEditCardChanged(uint8_t card){if(globalBank_!=GB_EDIT) return; refreshEditCard(card);}
//void editorEditBitChanged(uint8_t b);
//void editorEditCtrlBankChanged(uint8_t card, uint8_t bank);
//void editorEditCtrlValueChanged(uint8_t card, uint8_t ctrl, uint8_t param);
//void editorEditFuncBankChanged(uint8_t card, uint8_t bank);
//void editorEditFuncChanged(uint8_t card, uint8_t func, uint8_t param);
//void editorEditFuncValueChanged(uint8_t card, uint8_t param);
//void editorCardParamChanged(uint8_t card, uint8_t param, uint16_t value);
////void sequencerEditZoneChanged(uint8_t zone){if(globalBank_!=GB_SEQUENCER) return; refreshEditCard(zone_[zone]->getMasterCard());}
////void sequencerModeChanged(uint8_t seqMode);
////void sequencerEventListChanged(uint16_t index, MidiSeqMsg * msg);
////void sequencerBpmChanged(uint16_t bpm);
////void sequencerCardTx(uint8_t card, MidiMsg * msg);
////void sequencerZoneTx(uint8_t zone, MidiMsg * msg);
////void sequencerPlayChanged(uint8_t playMode);
////void sequencerOvSeqChanged(uint8_t zone, uint8_t seq, uint8_t display, uint8_t row);
////void sequencerOvPlayChanged(uint8_t zone, uint8_t seq, bool selected, uint8_t playState, uint8_t display, uint8_t row);
////void sequencerOvSeqLoopChanged(uint8_t zone, uint8_t seq, bool looping, uint8_t display, uint8_t row);
////void sequencerOvSeqNameChanged(uint8_t zone, uint8_t seq, const char * name, uint8_t display, uint8_t row);
////void sequencerOvSeqPosChanged(uint8_t zone, uint8_t seq, uint8_t pos, uint8_t display, uint8_t row);
////void sequencerOvClkChanged(uint16_t bar, uint8_t beat, uint8_t clk);
////void sequencerAddTransValueChanged(uint8_t param, int16_t value);
////void sequencerAddTransOutputChanged(uint8_t * pixels, bool fillMode);
////void sequencerMsgModeChanged(uint8_t msgMode);
////void sequencerAddTransBouncedChanged(bool bounced);
////void sequencerDisplayOnMapChanged(uint8_t onMap){hardware_.setOledOnMap(onMap);}
////void sequencerSeqSeekRead(uint8_t zone, uint8_t seq, MidiMsgListStream& asmls, uint16_t index){sdCard_.seekSeq(zone,seq,index);sdCard_.readSeq(zone,seq,asmls);}
////void sequencerSeqRead(uint8_t zone, uint8_t seq, MidiMsgListStream& asmls){sdCard_.readSeq(zone,seq,asmls);}
////void sequencerRecQuantiseChanged(uint8_t quantise);
////void sequencerSeqWrite(uint8_t zone, uint8_t seq, MidiMsgList& asml);
////void sequencerRecZoneChanged(uint8_t zone);
////void sequencerRecSeqPosChanged(uint8_t pos);
////void zoneTriggerNote(uint8_t card, uint8_t voice, uint8_t midiNote, uint8_t midiVel, bool way);
////void zoneAllNotesOff(uint8_t card);
////void sdPrintString(const char * string, const char icon = 0);
////void sdPrintFail(const char * msg);
////void sdHighlightChar(uint8_t charPos);
////void sdFunctionChanged(uint8_t func);
////void sdRefreshStatusBar(uint8_t barFill);
////void sdFunctionActivated(uint8_t sdFunction);
////void sdLoadSetting(uint8_t param, uint16_t value){};//{settings_.setParamValue(param,value);} COME BACK!
////void sdLoadPatch(uint8_t cardNum, uint8_t param, uint16_t value){editor_.setParamVal(cardNum,param,value);}
////void arMsgBoxFrameChanged(const char* msg){hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->setOverlay(msg);}
////void arTaskChanged(uint8_t newTask);
////void arSetLedCol(LedRgb::LedRgbColour funcCol, LedRgb::LedRgbColour valCol){hardware_.getLedSwitch(AtxCHardware::RG_FUNC).setColour(funcCol);hardware_.getLedSwitch(AtxCHardware::RG_VAL).setColour(valCol);}
//protected:
//private:
//AtxC( const AtxC &c );
//AtxC& operator=( const AtxC &c );
//void refreshSdTask();
//void refreshEditCard(uint8_t card);
//void refreshOledValue(uint8_t oled, uint8_t card, uint8_t param);
////void dispatchMidi(AtxMsg * msg);
//void convMidiNoteString(uint8_t note, char * str);
//void printParamOneLine(uint8_t oled, uint8_t firmware, uint8_t param);
//void printTimestamp(uint8_t oled, uint16_t timeStamp);
//bool processRxMsg(uint8_t card, MidiMsg * msg);
//uint8_t nextEditCard(bool way, uint8_t startCard);
//uint8_t nextEditZone(bool way, uint8_t startZone);
//uint8_t nextEditFunc(bool way, uint8_t startFunc);
//void triggerZoneNote(uint8_t triggerZone, uint8_t midiNote, uint8_t midiVel, bool way);
//void initComplete();
//void resetZoneLayoutSettings();
//void printFreeMem();
////bool isMasterCard(uint8_t cardNum);
//}; //AtxC
//
//#endif //__ATXC_H__
