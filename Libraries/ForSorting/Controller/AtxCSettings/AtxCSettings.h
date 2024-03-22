///*
//* AtxCSettings.h
//*
//* Created: 18/01/2018 11:48:42
//* Author: info
//*/
//
//
//#ifndef __ATXCSETTINGS_H__
//#define __ATXCSETTINGS_H__
//
//#include <stdlib.h>
//#include <stdint.h>
//#include "Atx.h"
//#include "AtxCcEnum.h"
//#include "AtxCSettingsBase.h"
//#include "AtxCSettingsProgmem.h"
//
//#ifndef bitRead
//#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
//#endif
//#ifndef bitSet
//#define bitSet(value, bit) ((value) |= (1UL << (bit)))
//#endif
//#ifndef bitClear
//#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
//#endif
//#ifndef bitWrite
//#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
//#endif
//
//class AtxCSettings
//{
	////variables
	//public:
	//enum Setting : uint8_t
	//{
		//S_ZONES = 0,
		//S_MIDI,
		//S_SEQUENCER,
		//S_LOADSAVE,
		//S_MAX
	//};
	//static const uint8_t SETTINGS = 16;
	//static const uint8_t SETTINGS_PARAMS = 232;
	//static const uint16_t SETTINGS_MAXPARAMS = 256;
	//static const uint16_t SETTINGS_FWHW = 0xFFFF;
	//protected:
	//private:
	//static const uint8_t FIRST_POLY_PARAM = 128;
	//static const uint8_t FIRST_CTRL_NUM = 128;
	//static const uint8_t FUNC_SW_CTRL_NUM = 224;
	//static const uint8_t EDIT_CTRLS = 6;
	//static const uint8_t EDIT_FUNCS = 8;  //this may need to be per setting
	//AtxCSettingsBase* base_;
	//Setting editSetting_ = S_ZONES;
	////uint16_t ccValue_[SETTINGS_PARAMS] = {0};
	//uint16_t ccValue_[SETTINGS_MAXPARAMS] = {0};
	//uint8_t editFunc_ = 0;
 	//uint8_t ctrlCcs_[EDIT_CTRLS] = {0};
 	//uint8_t funcCcs_[EDIT_FUNCS] = {0};
	////functions
	//public:
	//static AtxCSettings& getInstance()
	//{
		//static AtxCSettings instance; // Guaranteed to be destroyed.
		//return instance;  // Instantiated on first use.
	//}
	//void construct(AtxCSettingsBase* base);
	//void initialize(uint_fast8_t initSynthsMap, uint8_t* initFwMap);
	//void setEditSetting(Setting newSetting);
	//Setting getEditSetting(){return editSetting_;}
	//static const char * getSettingName(uint_fast8_t line, Setting setting){return (line==0) ? S_SETTINGS_TOP[setting] : S_SETTINGS_BOT[setting]; }
	//const char * getSettingName(uint_fast8_t line){return getSettingName(line,editSetting_);}
	//void setEditFunc(uint8_t newValue);
	//uint8_t getEditFunc(){return editFunc_;}
	//void setCcValue(uint_fast8_t cc, uint16_t newValue);
	//uint16_t getCcValue(uint_fast8_t cc){return ccValue_[cc];}
	//uint16_t* getCcValuePtr() { return ccValue_; }
	//void setCcValFromCtrl(uint_fast8_t ctrl, uint8_t newValue);
	//uint_fast16_t mapCtrlVal(uint_fast8_t cc,uint_fast8_t value);
	//void setCcValFromFunc(uint16_t newValue);
	//void refreshCtrlCcs();
	//void refreshFuncCcs();
	//uint_fast8_t getCcIndexFromCtrl(uint_fast8_t ctrl){return ctrlCcs_[ctrl];}
	//uint_fast8_t getCcIndexFromFunc(){return funcCcs_[editFunc_];}
	//bool isPerZoneSetting(Setting setting){return (SETTINGS_PERZONE[setting]==0);}
	//bool isPerZoneSetting(){return isPerZoneSetting(editSetting_);}
	//bool isPerSynthSetting(Setting setting){return (SETTINGS_PERZONE[setting]==1);}
	//bool isPerSynthSetting(){return isPerSynthSetting(editSetting_);}
	//bool isCcSetting(uint_fast8_t cc, uint_fast8_t setting){return false;}//COME BACK!!!!  //(getCcInfoCtrlSetting(cc)==setting || getCcInfoFuncSetting(cc)==setting);}
	//static const CcInfo getCcInfo(uint_fast8_t cc){return SET_PARAM_INFO[cc];}
	//static const uint32_t getCcMaxValue(uint_fast8_t cc){return (1UL << SET_PARAM_INFO[cc].bits) ;}
	//static const uint8_t getCcInfoFmtNum(uint_fast8_t cc){return (SET_PARAM_INFO[cc].dispFmt & 0x0F);}
	//static const uint8_t getCcInfoFmtXtr(uint_fast8_t cc){return (SET_PARAM_INFO[cc].dispFmt >> 4);}
	////static const uint8_t getCcInfoFuncNum(uint_fast8_t cc){return (SET_PARAM_INFO[cc].ctrl < FIRST_CTRL_NUM) ? (SET_PARAM_INFO[cc].ctrl & 0x07) : atx::UNUSED;}
	////void refreshZoneLayout();
	//protected:
	//private:
	////static const uint8_t getCcInfoCtrlSetting(uint_fast8_t cc){return SET_PARAM_INFO[cc].ctrl==atx::UNUSED ? atx::UNUSED : (SET_PARAM_INFO[cc].ctrl >= FIRST_CTRL_NUM) ? ((SET_PARAM_INFO[cc].ctrl - FIRST_CTRL_NUM) / EDIT_CTRLS) : atx::UNUSED;}
	////static const uint8_t getCcInfoCtrlNum(uint_fast8_t cc){return SET_PARAM_INFO[cc].ctrl==atx::UNUSED ? atx::UNUSED : (SET_PARAM_INFO[cc].ctrl >= FIRST_CTRL_NUM) ? ((SET_PARAM_INFO[cc].ctrl - FIRST_CTRL_NUM) % EDIT_CTRLS) : atx::UNUSED;}
	////static const uint8_t getCcInfoFuncSetting(uint_fast8_t cc){return (SET_PARAM_INFO[cc].ctrl < FIRST_CTRL_NUM) ? (SET_PARAM_INFO[cc].ctrl >> 3) : atx::UNUSED;}
	//bool isCardLayoutCc(uint_fast8_t cc){return (cc>=atx::SET_FW_Z1 && cc<=atx::SET_TRIGGERZONE_Z8) ? true : false;}
	//AtxCSettings() {}
	//AtxCSettings(AtxCSettingsBase* base);
	//~AtxCSettings();
	//AtxCSettings( const AtxCSettings &c );
	//AtxCSettings& operator=( const AtxCSettings &c );
//
//}; //AtxCSettings
//
//#endif //__ATXCSETTINGS_H__
