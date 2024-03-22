/* ATXCEDITORBASE_H_ */

/*
 * AtxCEditorBase.h
 *
 * Created: 19/01/2018 15:56:37
 *  Author: info
 */ 


#ifndef ATXCEDITORBASE_H_
#define ATXCEDITORBASE_H_

#include "MidiMsg.h"

class AtxCEditorBase
{
	public:
	virtual void editorChannelChanged(uint8_t channel) = 0;
	virtual void editorBankChanged(uint8_t channel, uint8_t bank) = 0;
	virtual void editorCtrlCcChanged(uint8_t ctrl, uint8_t cc) = 0;
	virtual void editorCtrlValueChanged(uint8_t ctrl, uint8_t value) = 0;
	virtual void editorFuncCcChanged(uint8_t func, uint8_t cc) = 0;
	virtual void editorFuncValueChanged(uint8_t value) = 0;
	virtual void editorZoneChanged(uint8_t zone) = 0;
	virtual void editorCcChanged(midi::MidiMsg* msg) = 0;
	virtual void editorAwaitActivateChanged(uint8_t awaitActMode) = 0;
	virtual void editorSettingChanged(uint8_t cc, uint8_t value);
};




#endif /* ATXCEDITORBASE_H_ */

///* ATXCEDITORBASE_H_ */
//
///*
 //* AtxCEditorBase.h
 //*
 //* Created: 19/01/2018 15:56:37
 //*  Author: info
 //*/ 
//
//
//#ifndef ATXCEDITORBASE_H_
//#define ATXCEDITORBASE_H_
//
//class AtxCEditorBase
//{
	//public:
	//virtual void editorEditChannelChanged(uint8_t channel) = 0;
	//virtual void editorEditBitChanged(uint8_t b) = 0;
	//virtual void editorEditBankChanged(uint8_t channel, uint8_t bank) = 0;
	////virtual void editorEditCtrlBankChanged(uint8_t card, uint8_t bank) = 0;
	//virtual void editorEditCtrlValueChanged(uint8_t channel, uint8_t ctrl, uint8_t param) = 0;
	////virtual void editorEditFuncBankChanged(uint8_t card, uint8_t bank) = 0;
	//virtual void editorEditFuncChanged(uint8_t card, uint8_t func, uint8_t param) = 0;
	//virtual void editorEditFuncValueChanged(uint8_t card, uint8_t param) = 0;
	//virtual void editorCardParamChanged(uint8_t card, uint8_t param, uint_fast16_t value) = 0;
//};
//
//
//
//
//#endif /* ATXCEDITORBASE_H_ */