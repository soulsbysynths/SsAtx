/*
* AtxCEditor.cpp
*
* Created: 19/01/2018 15:54:23
* Author: info
*/


#include "AtxCEditor.h"

AtxCEditor::AtxCEditor(AtxCEditorBase* base)
{
	construct(base);
} //AtxCEditor

// default destructor
AtxCEditor::~AtxCEditor()
{
} //~AtxCEditor


void AtxCEditor::construct(AtxCEditorBase* base)
{
	base_ = base;
}

void AtxCEditor::setChannel(uint8_t newValue)
{
	using namespace atx;
	channel_ = newValue;
	base_->editorChannelChanged(channel_);
	if(channel_>=MAX_ZONES)
	{
		if(getCcFromZone()==UNUSED)
		{
			nextZone(true);
		}
		else
		{
			setZone(zone_[channel_]);
		}
	}
	setBank(bank_[channel_]);
}


void AtxCEditor::setBank(uint8_t newValue)
{
	using namespace atx;
	bank_[channel_] = newValue;
	base_->editorBankChanged(channel_, bank_[channel_]);
	if(channel_<MAX_ZONES)  //it's done in setFunc for poly cards
	{
		for (uint8_t i=0;i<CTRLS;++i)
		{
			uint8_t cc = getCcFromCtrlCode(false,i);
			base_->editorCtrlCcChanged(i, cc);
			if(cc!=UNUSED) base_->editorCtrlValueChanged(i, getCcValue(cc));
		}
	}
	if(getCcFromFunc()==UNUSED)
	{
		nextFunc(true);
	}
	else
	{
		setFunc(func_[channel_][bank_[channel_]]);
	}
}

void AtxCEditor::nextBank()
{
	using namespace atx;
	uint8_t b = bank_[channel_];
	for (uint8_t i=0;i<BANKS;++i)
	{
		b++;
		b &= BANKS_MASK;
		for(uint8_t j=0;j<MAX_CCS;++j)
		{
			if(getCcInfoCtrlEnabled(j)==true && getCcInfoCtrlBank(j)==b && getCcInfoCtrlZone(j)==zone_[channel_])
			{
				setBank(b);
				return;   //NOTE returning, no code below this please
			}
		}
	}
}

void AtxCEditor::setFunc(uint8_t newValue)
{
	using namespace atx;
	
	func_[channel_][bank_[channel_]] = newValue;
	uint8_t cc = getCcFromFunc();
	base_->editorFuncCcChanged(func_[channel_][bank_[channel_]],cc);
	if(cc!=UNUSED) base_->editorFuncValueChanged(getCcValue(cc));
	//Below is needed if cycling through bank and no func in bank, but there are ctrls
	if(channel_>=MAX_ZONES)
	{
		for (uint8_t i=0;i<CTRLS;++i)
		{
			cc = getCcFromCtrlCode(false,i);
			base_->editorCtrlCcChanged(i, cc);
			if(cc!=UNUSED) base_->editorCtrlValueChanged(i, getCcValue(cc));
		}
	}
}

void AtxCEditor::nextFunc(bool way)
{
	using namespace atx;
	uint8_t f = func_[channel_][bank_[channel_]];
	for (uint8_t i=0;i<FUNCS;++i)
	{
		if (way)
		{
			f++;
		}
		else
		{
			f--;
		}
		f &= FUNCS_MASK;
		if(getCcFromCtrlCode(true,f)!=UNUSED)
		{
			setFunc(f);
			return;
		}
	}
	setFunc(f);  //still set the func so ctrls can update / clear down as necessary
}

void AtxCEditor::setZone(uint8_t channel, uint8_t newValue)
{
	using namespace atx;
	
	zone_[channel] = newValue;
	if(channel==channel_)
	{
		base_->editorZoneChanged(zone_[channel]);
		if(channel>=MAX_ZONES)
		{
			uint8_t cc = getCcFromZone();
			for (uint8_t i=0;i<CTRLS;++i)
			{
				cc = getCcFromCtrlCode(false,i);
				base_->editorCtrlCcChanged(i, cc);
				if(cc!=UNUSED) base_->editorCtrlValueChanged(i, getCcValue(cc));
			}
			if(getCcFromFunc()==UNUSED)
			{
				nextFunc(true);
			}
			else
			{
				setFunc(func_[channel][bank_[channel]]);
			}
		}
	}
}

uint8_t AtxCEditor::getCcFromZone()
{
	using namespace atx;
	uint8_t cc = getCcFromFunc();
	if(cc==UNUSED)
	{
		for(uint8_t i=0;i<CTRLS;++i)
		{
			cc = getCcFromCtrlCode(false,i);
			if(cc!=UNUSED)
			{
				break;
			}
		}
	}
	return cc;
}

void AtxCEditor::nextZone(bool way)
{
	using namespace atx;
	uint8_t z = zone_[channel_];
	for (uint8_t i=0;i<ZONES;++i)
	{
		if (way)
		{
			z++;
		}
		else
		{
			z--;
		}
		z &= ZONES_MASK;
		if(AtxCcSet::getCcFromCtrlCode(true,bank_[channel_],z,func_[channel_][bank_[channel_]])!=UNUSED)
		{
			setZone(z);
			return;
		}
		for(uint8_t j=0;j<CTRLS;++j)
		{
			if(AtxCcSet::getCcFromCtrlCode(false,bank_[channel_],z,j)!=UNUSED)
			{
				setZone(z);
				return;
			}
		}
	}
	
}


void AtxCEditor::setCcValFromCtrl(uint8_t ctrl, uint8_t newValue)
{
	using namespace atx;
	using namespace midi;
	uint8_t cc = getCcFromCtrlCode(false,ctrl);
	if (cc==UNUSED)	return;
	setCcValue(cc,newValue);
	if(getCcChanged(cc))
	{
		base_->editorCtrlValueChanged(ctrl,newValue);
		if(getCcInfoCtrlUseActivate(cc)==false)
		{
			setCcChanged(cc,false);
			if(channel_!=SETTINGS_CHANNEL)
			{
				MidiMsg m;
				m.setChannel(channel_);
				m.setCommand(midi::MCMD_CC);
				m.setData1(cc);
				m.setData2(newValue);
				base_->editorCcChanged(&m);
			}
			else
			{
				base_->editorSettingChanged(cc,newValue);
			}
		}
		else
		{
			setAwaitActivate(AAM_ON);
		}

	}
}

void AtxCEditor::setCcValFromFunc(uint8_t newValue)
{
	using namespace atx;
	using namespace midi;
	uint8_t cc = getCcFromCtrlCode(true,func_[channel_][bank_[channel_]]);
	if (cc==UNUSED)	return;
	setCcValue(cc,newValue);
	if(getCcChanged(cc))
	{
		if(getCcInfoCtrlUseActivate(cc)==false)
		{
			setCcChanged(cc,false);
			if(channel_!=SETTINGS_CHANNEL)
			{
				MidiMsg m;
				m.setChannel(channel_);
				m.setCommand(MCMD_CC);
				m.setData1(cc);
				m.setData2(newValue);
				base_->editorCcChanged(&m);
			}
			else
			{
				base_->editorSettingChanged(cc,newValue);
			}
		}
		else
		{
			setAwaitActivate(AAM_ON);
		}
		base_->editorFuncValueChanged(newValue);  //is this order important?  Yes, engine must update first in case you need req gfx
	}
}

void AtxCEditor::setCcValFromMidiMsg(midi::MidiMsg* msg)
{
	using namespace atx;
	using namespace midi;
	if(msg->getChannel()!=channel_) return;

	uint8_t cc = msg->getData1();
	uint8_t newValue = msg->getData2();
	setCcValue(cc,newValue);
	if (getCcInfoCtrlEnabled(cc) && getCcInfoCtrlBank(cc)==bank_[channel_] && getCcInfoCtrlZone(cc)==zone_[channel_])
	{
		if(getCcInfoCtrlIsFunc(cc))
		{
			if(getCcInfoCtrlNum(cc)==func_[channel_][bank_[channel_]])
			{
				base_->editorFuncValueChanged(getCcValue(cc));
			}
		}
		else
		{
			base_->editorCtrlValueChanged(getCcInfoCtrlNum(cc), getCcValue(cc));
		}
	}
}

void AtxCEditor::setAwaitActivate(AwaitActivateMode newMode)
{
	using namespace atx;
	if(newMode!=awaitActivate_) //stop multiple calls to the led flash
	{
		awaitActivate_ = newMode;
		if (newMode==AAM_YES)
		{
			for (uint8_t cc=0;cc<MAX_CCS;++cc)
			{
				if(getCcChanged(cc) && channel_==SETTINGS_CHANNEL)  //so only settings at the mo
				{
					base_->editorSettingChanged(cc,getCcValue(cc));
				}
			}
		}
		else if(newMode==AAM_NO)
		{
			setChannel(channel_);   //get settings as they were and refresh
		}
		base_->editorAwaitActivateChanged(newMode);
	}
}

void AtxCEditor::setEditorFromCc(uint8_t channel, uint8_t cc)
{
	using namespace atx;
	//NO NO NO THIS CALLS FUNCCCHANGED TOO MUCH
	if(channel_!=channel) setChannel(channel);
	if (getCcInfoCtrlEnabled(cc)==false) return;
	uint8_t bank = getCcInfoCtrlBank(cc);
	if(channel<MAX_ZONES)
	{
		if(bank!=bank_[channel_]) setBank(bank);
		if(getCcInfoCtrlIsFunc(cc))
		{
			uint8_t func = getCcInfoCtrlNum(cc);
			if(func!=func_[channel_][bank_[channel_]]) setFunc(func);
		}
	}
	else
	{
		uint8_t zone = getCcInfoCtrlZone(cc);
		if(zone!=zone_[channel_]) setZone(zone);
		if(bank!=bank_[channel_]) setBank(bank);
	}
}

bool AtxCEditor::isCcOnActiveCtrl(uint8_t cc)
{
	for(uint8_t i=0;i<CTRLS;++i)
	{
		if(getCcFromCtrl(i)==cc)
		{
			return true;
		}
	}
	if(getCcFromFunc()==cc)
	{
		return true;
	}
	return false;
}

///*
//* AtxCEditor.cpp
//*
//* Created: 19/01/2018 15:54:23
//* Author: info
//*/
//
//
//#include "AtxCEditor.h"
//
//const uint8_t AtxCEditor::CTRL_CC[3] = {14,20,26};
//const uint8_t AtxCEditor::FUNC_CC[3] = {48,72,102};
//
////// default constructor
////AtxCEditor::AtxCEditor(AtxCEditorBase* base, AtxCard* cardPtrs[])
////{
////construct(base,cardPtrs);
////} //AtxCEditor
//
//AtxCEditor::AtxCEditor(AtxCEditorBase* base)
//{
//construct(base);
//} //AtxCEditor
//
//// default destructor
//AtxCEditor::~AtxCEditor()
//{
//} //~AtxCEditor
//
////void AtxCEditor::construct(AtxCEditorBase* base, AtxCard* cardPtrs[])
////{
////using namespace atx;
////base_ = base;
////for (uint8_t i=0;i<MAX_CARDS;++i)
////{
////card_[i] = cardPtrs[i];
////}
////buildParamList(0);  //default to ATM
////}
//void AtxCEditor::construct(AtxCEditorBase* base)
//{
//using namespace atx;
//base_ = base;
////buildParamList(0);  //default to ATM
//}
//
//void AtxCEditor::setEditCard(uint8_t newValue)
//{
//channel_ = newValue & 0x0F;  //stop weirdness!
////buildParamList(card_[channel_]->firmware);
//base_->editorEditChannelChanged(channel_);
//setEditBank(bank_[channel_]);
//}
//
//void AtxCEditor::nextEditChannel(bool way)
//{
//using namespace atx;
////for (uint8_t i=0;i<MAX_CARDS;++i)
////{
////if (way)
////{
////channel_++;
////}
////else
////{
////channel_--;
////}
////channel_ &= MAX_CARDS_MASK;
////if ((channel_>=MAX_SYNTHCARDS || (channel_<MAX_SYNTHCARDS && card_[channel_]->synth==0)) && card_[channel_]->connected)
////{
////setEditCard(channel_);
////break;
////}
////}
//if(way)
//{
//channel_++;
//}
//else
//{
//channel_--;
//}
//channel_ &= 0x0F;
//setEditChannel(channel_);
//}
//
//void AtxCEditor::setEditBank(uint8_t newValue)
//{
//bank_[channel_] = newValue;
//base_->editorEditBankChanged(channel_, bank_[channel_]);
//for (uint8_t i=0;i<EDIT_CTRLS;++i)
//{
//base_->editorEditCtrlValueChanged(channel_, i, getParamFromCtrl(i));
//}
//setEditFunc(func_[channel_][bank_[channel_]]);
//}
//
//void AtxCEditor::nextEditBank()
//{
//using namespace atx;
//bank_[channel_]++;
//if()
////for (uint8_t i=0;i<BANKS;++i)
////{
////bank_[channel_]++;
////if (bank_[channel_]>=BANKS)
////{
////bank_[channel_] = 0;
////}
////if (isBankEmpty()==false)
////{
////base_->editorEditCtrlBankChanged(channel_, bank_[channel_]);
////for (uint8_t j=0;j<EDIT_CTRLS;++j)
////{
////base_->editorEditCtrlValueChanged(channel_, j, getParamFromCtrl(i));
////}
////break;
////}
////}
////for (uint8_t i=0;i<BANKS;++i)
////{
////bank_[channel_]++;
////if (bank_[channel_]>=BANKS)
////{
////bank_[channel_] = 0;
////}
////if (channel_>=MAX_SYNTHCARDS)
////{
////if (isBankEmpty()==false && isParamUnused(getParamFromFunc())==false)
////{
////base_->editorEditFuncBankChanged(channel_,bank_[channel_]);
////setEditFunc(func_[channel_][bank_[channel_]]);
////break;
////}
////}
////else
////{
////if (isBankEmpty()==false)
////{
////for (uint8_t j=0;j<FUNCS;++j)
////{
////if (isParamUnused(getParamFromFunc())==false)
////{
////break;
////}
////else
////{
////func_[channel_][bank_[channel_]]++;
////func_[channel_][bank_[channel_]] &= FUNCS_MASK;
////}
////}
////base_->editorEditFuncBankChanged(channel_,bank_[channel_]);
////setEditFunc(func_[channel_][bank_[channel_]]);
////break;
////}
////}
////}
//}
//
//uint8_t AtxCEditor::getParamFromCtrl(uint8_t card, uint8_t bank, uint8_t ctrl)
//{
////card needed for determining poly cards
//return CTRL_CC[bank] + ctrl;
//}
//
//uint8_t AtxCEditor::getParamFromFunc(uint8_t card, uint8_t bank, uint8_t func)
//{
////card needed for determining poly cards
//return FUNC_CC[bank] + func;
//}
//
//uint8_t AtxCEditor::getCtrlFromParam(uint8_t param)
//{
//for (uint8_t i=0;i<EDIT_BANKS;++i)
//{
//if(param>=CTRL_CC[i] && param<(CTRL_CC[i]+EDIT_CTRLS))
//{
//return (param - CTRL_CC[i]);
//}
//}
//return UNUSED;
//}
//
//uint8_t AtxCEditor::getFuncFromParam(uint8_t param)
//{
//for (uint8_t i=0;i<EDIT_BANKS;++i)
//{
//if(param>=FUNC_CC[i] && param<(FUNC_CC[i]+EDIT_FUNCS))
//{
//return (param - FUNC_CC[i]);
//}
//}
//return UNUSED;
//}
//
//uint8_t AtxCEditor::getBankFromParam(uint8_t param)
//{
//for (uint8_t i=0;i<EDIT_BANKS;++i)
//{
//if(param>=CTRL_CC[i] && param<(CTRL_CC[i]+EDIT_CTRLS))
//{
//return (param - CTRL_CC[i]);
//}
//if(param>=FUNC_CC[i] && param<(FUNC_CC[i]+EDIT_FUNCS))
//{
//return (param - FUNC_CC[i]);
//}
//}
//return UNUSED;
//}
//
//
//
//void AtxCEditor::setEditBit(uint8_t newBit)
//{
//editBit_ = newBit;
//base_->editorEditBitChanged(editBit_);
//}
//
//void AtxCEditor::setParamValFromCtrl(uint8_t ctrl, uint8_t newValue)
//{
//using namespace atx;
//uint8_t param;
//uint16_t val;
//bool changed = false;
//param = getParamFromCtrl(ctrl);
//if (param==UNUSED)
//{
//return;
//}
//val = mapCtrlVal(param,newValue);
//if (val!=card_[channel_]->getParamValue(param))
//{
//changed = true;
//}
//
////card_[channel_]->setParamValue(param,val);
////base_->editorCardParamChanged(channel_,param,val,false););
//
//if (changed)
//{
//card_[channel_]->setParamValue(param,val);
//base_->editorCardParamChanged(channel_,param,val);
//base_->editorEditCtrlValueChanged(channel_,ctrl,param);
//}
//}
//
//uint_fast16_t AtxCEditor::mapCtrlVal(uint8_t param,uint8_t value)
//{
//if (card_[channel_]->getParamInfoFmtNum(param)==AtxCard::FMT_PARAMLIST)
//{
//return paramList_.getListParam((value * paramList_.getListSize()) / 256);
//}
//else
//{
//return ( value * card_[channel_]->getParamMaxValue(param)) / 256; //won't work for 65536
//}
//
//}
//
//bool AtxCEditor::isBankEmpty()
//{
//using namespace atx;
//for (uint8_t i=0;i<EDIT_CTRLS;++i)
//{
//if (isParamUnused(getParamFromCtrl(i))==false)
//{
//return false;
//}
//}
//for (uint8_t i=0;i<EDIT_FUNCS;++i)
//{
//if (isParamUnused(getParamFromFunc(i))==false)
//{
//return false;
//}
//}
//return true;
//}
//
//
//
//void AtxCEditor::setEditFunc(uint8_t card, uint8_t newValue)
//{
//using namespace atx;
//uint8_t param;
//func_[card] = newValue;
//if (card!=channel_) return;
//param = getParamFromFunc();
//base_->editorEditFuncChanged(card,func_[card],param);
//if (param!=UNUSED)
//{
////base_->editorEditFuncChanged(card,func_[card],param);  //no clear screen if unused
//base_->editorEditFuncValueChanged(card,param);
//}
//if (card_[card]->getFirstPolyParam()<UNUSED)  //poly params
//{
////could wrap this is in an if, to see if fw actually changed, like fro editCard above
//buildParamList(card_[card]->firmware);
//base_->editorEditCtrlBankChanged(card,bank_[card]);
//for (uint8_t i=0;i<EDIT_CTRLS;++i)
//{
////if (param!=UNUSED)  //CLEAR DISPLAY IF UNUSED
////{
//base_->editorEditCtrlValueChanged(card, i, getParamFromCtrl(i));
////}
//}
//}
//}
//
//void AtxCEditor::nextEditFunc(bool way)
//{
//using namespace atx;
//
//for (uint8_t i=0;i<MAX_CARDS;++i)
//{
//if (way)
//{
//func_[channel_][bank_[channel_]]++;
//}
//else
//{
//func_[channel_][bank_[channel_]]--;
//}
//func_[channel_][bank_[channel_]] &= FUNCS_MASK;
//if (channel_>=MAX_SYNTHCARDS)
//{
//if ((func_[channel_][bank_[channel_]]>=MAX_SYNTHCARDS && isParamUnused(getParamFromFunc())==false) || (func_[channel_][bank_[channel_]]<MAX_SYNTHCARDS && card_[func_[channel_][bank_[channel_]]]->synth==0 && card_[func_[channel_][bank_[channel_]]]->connected))
//{
//setEditFunc(channel_,func_[channel_][bank_[channel_]]);
//break;
//}
//}
//else
//{
//if (isParamUnused(getParamFromFunc())==false)
//{
//setEditFunc(channel_,func_[channel_][bank_[channel_]]);
//break;
//}
//}
//}
//}
//
//void AtxCEditor::setParamValFromFunc(uint16_t newValue)
//{
//using namespace atx;
//uint8_t param;
//bool changed = false;
//param = getParamFromFunc();
//if (isParamUnused(param)==true)
//{
//return;
//}
//if (newValue!=card_[channel_]->getParamValue(param))
//{
//changed = true;
//}
//
////card_[channel_]->setParamValue(param,newValue);
////base_->editorCardParamChanged(channel_,param,newValue,false);
//
//if (changed)
//{
//card_[channel_]->setParamValue(param,newValue);
//base_->editorCardParamChanged(channel_,param,newValue);
//base_->editorEditFuncValueChanged(channel_,param);
//}
//}
//
//void AtxCEditor::toggleBitParamValFromFunc()
//{
//using namespace atx;
//uint8_t param;
//param = getParamFromFunc();
//if (isParamUnused(param)==true)
//{
//return;
//}
//uint16_t newValue = card_[channel_]->getParamValue(param);
//newValue ^= 1U << editBit_;
//
//card_[channel_]->setParamValue(param,(uint16_t)newValue);
//base_->editorCardParamChanged(channel_,param,(uint_fast16_t)newValue);
//base_->editorEditFuncValueChanged(channel_,param);
//}
//
//void AtxCEditor::setParamVal(uint8_t card, uint8_t param, uint16_t value)
//{
//using namespace atx;
//card_[card]->setParamValue(param,value);
//uint8_t c = getCtrlFromParam(param);
//uint8_t f = getFuncFromParam(param);
//if (c!=UNUSED)
//{
//uint8_t fwPoly = card_[card]->getFirmwarePoly();
//uint8_t fwPolyParam = card_[card]->getFirstPolyParam();
//if (channel_==card && bank_[card]==getBankFromParam(param) && (fwPoly==1 || param<fwPolyParam))
//{
//base_->editorEditCtrlValueChanged(card,c,param);
//}
//}
//else if(f!=UNUSED)
//{
//if (channel_==card && func_[card]==f && bank_[card]==getBankFromParam(param))
//{
//base_->editorEditFuncValueChanged(card,param);
//}
//
//}
//base_->editorCardParamChanged(card,param,value);
//}
//
////void AtxCEditor::buildParamList(uint8_t firmware)
////{
////uint8_t fw;
////if (firmware==AtxCard::FW_KEYBED)
////{
////paramList_.buildParamList(card_[func_[channel_][bank_[channel_]]]->firmware);
////}
////else
////{
////paramList_.buildParamList(firmware);
////}
////}