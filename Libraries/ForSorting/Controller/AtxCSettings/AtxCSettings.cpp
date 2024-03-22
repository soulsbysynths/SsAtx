///*
//* AtxCSettings.cpp
//*
//* Created: 18/01/2018 11:48:42
//* Author: info
//*/
//
//
//#include "AtxCSettings.h"
//
//// default constructor
//AtxCSettings::AtxCSettings(AtxCSettingsBase* base)
//{
	//construct(base);
//} //AtxCSettings
//
//// default destructor
//AtxCSettings::~AtxCSettings()
//{
	//if (base_!=NULL)
	//{
		//delete base_;
	//}
//} //~AtxCSettings
//
//void AtxCSettings::construct(AtxCSettingsBase* base)
//{
	//using namespace atx;
	//base_ = base;
	//ccValue_[SET_HELLO] = SETTINGS_FWHW;  //for now
//}
//
//void AtxCSettings::initialize(uint_fast8_t initSynthsMap, uint8_t* initFwMap)
//{
	//using namespace atx;
	//
	//
	//ccValue_[SET_SEQ_BPM] = 120 - 30;
	//uint_fast8_t z = 0;
	//for (uint_fast8_t i=0;i<MAX_SYNTHCARDS;++i)
	//{
		//if (bitRead(initSynthsMap,i))
		//{
			//ccValue_[SET_ZONEENABLE_Z1+z] = true;
			//ccValue_[SET_FW_Z1+z] = initFwMap[z];
			//ccValue_[SET_MASTER_Z1+z] = i;
			//ccValue_[SET_CARDS_Z1+z] = 0;
			//ccValue_[SET_TRIGGERZONE_Z1+z] = z;
			//ccValue_[SET_LOWERNOTE_Z1+z] = 0;
			//ccValue_[SET_UPPERNOTE_Z1+z] = 127;
			//ccValue_[SET_MIDICHAN_Z1+z] = 0;
			//z++;
		//}
	//}
	////uint_fast8_t z = 0, f = atx::UNUSED, zs = 0;
	////for (uint_fast8_t i=0;i<MAX_SYNTHCARDS;++i)
	////{
	////if (bitRead(initSynthsMap,i))
	////{
	////if (f==atx::UNUSED)
	////{
	////f = i;
	////}
	////z++;
	////}
	////}
	////z--;
	////z &= 0x07;
	////ccValue_[SET_ZONEENABLE_Z1] = true;
	////ccValue_[SET_FW_Z1] = initFwMap[0];
	////ccValue_[SET_MASTER_Z1] = f;
	////ccValue_[SET_CARDS_Z1] = z;
	////ccValue_[SET_TRIGGERZONE_Z1] = 0;
	////ccValue_[SET_LOWERNOTE_Z1] = 0;
	////ccValue_[SET_UPPERNOTE_Z1] = 127;
	////ccValue_[SET_MIDICHAN_Z1] = 0;
//
	////base_->settingTxZoneLayout(false,true);
	//
	//setCcValue(SET_SEQ_BPM,90);
//}
//
//void AtxCSettings::setEditSetting(Setting newSetting)
//{
	//using namespace atx;
	//editSetting_ = newSetting;
	//refreshCtrlCcs();
	//refreshFuncCcs();
	//
	//base_->settingEditSettingChanged(newSetting);
	//
	//uint_fast8_t cc;
	//for (uint_fast8_t i=0;i<EDIT_CTRLS;++i)
	//{
		//cc = ctrlCcs_[i];
		////if (cc!=UNUSED)  //NO SEND UNSED TO CLEAR OLED
		////{
		//base_->settingEditCtrlValueChanged(i, cc);
		////}
	//}
	//setEditFunc(editFunc_);
	////cc = funcCcs_[editFunc_];
	////if (cc!=UNUSED)
	////{
	////base_->settingEditFuncChanged(editFunc_,cc);
	////base_->settingEditFuncValueChanged(cc);
	////}
//}
//
//
//
//void AtxCSettings::setEditFunc(uint8_t newValue)
//{
	//using namespace atx;
	//uint_fast8_t cc;
	//editFunc_ = newValue;
	//cc = funcCcs_[editFunc_];
	//base_->settingEditFuncChanged(editFunc_,cc);
	//if (cc!=UNUSED)
	//{
		////base_->settingEditFuncChanged(editFunc_,cc); //SEND UNUSED TO CLEAR OLED
		//base_->settingEditFuncValueChanged(cc);
		//if (isCardLayoutCc(cc))  //assumes contigious block of ccs
		//{
			//base_->settingTxZoneLayout(true,false);
		//}
	//}
	//if (SETTINGS_PERZONE[editSetting_]>0)  //poly card
	//{
		//refreshCtrlCcs();
		//for (uint_fast8_t i=0;i<EDIT_CTRLS;++i)
		//{
			//cc = ctrlCcs_[i];
			////if (cc!=UNUSED)  //SEND UNUSED TO CLEAR OLED
			////{
			//base_->settingEditCtrlValueChanged(i, cc);
			////}
		//}
	//}
//}
//
//void AtxCSettings::setCcValFromCtrl(uint_fast8_t ctrl, uint8_t newValue)
//{
	//using namespace atx;
	//uint_fast8_t cc;
	//uint16_t val;
	//bool changed = false;
	//cc = ctrlCcs_[ctrl];
	//if (cc==UNUSED)
	//{
		//return;
	//}
	//val = mapCtrlVal(cc,newValue);
	//if (val!=ccValue_[cc])
	//{
		//changed = true;
	//}
//
	////ccValue_[cc] = val;
	////if (isCardLayoutCc(cc))  //assumes contigious block of ccs
	////{
	////base_->settingTxZoneLayout(true);
	////}
	////base_->settingCcChanged(cc,val);
//
	//if (changed)
	//{
		//ccValue_[cc] = val;
		//if (isCardLayoutCc(cc))  //assumes contigious block of ccs
		//{
			//base_->settingTxZoneLayout(true,false);
		//}
		//base_->settingCcChanged(cc,val);
		//base_->settingEditCtrlValueChanged(ctrl,cc);
	//}
//}
//
//uint_fast16_t AtxCSettings::mapCtrlVal(uint_fast8_t cc,uint_fast8_t value)
//{
	//return ( value * getCcMaxValue(cc)) / 256; //won't work for 65536
//}
//
//void AtxCSettings::setCcValFromFunc(uint16_t newValue)
//{
	//using namespace atx;
	//uint_fast8_t cc;
	//bool changed = false;
	//cc = funcCcs_[editFunc_];
	//if (cc==UNUSED)
	//{
		//return;
	//}
	//if (newValue!=ccValue_[cc])
	//{
		//changed = true;
	//}
	//
	////ccValue_[cc] = newValue;
	////if (isCardLayoutCc(cc))
	////{
	////base_->settingTxZoneLayout(true);
	////}
	////base_->settingCcChanged(cc,newValue);
	//
	//if (changed)
	//{
		//ccValue_[cc] = newValue;
		//if (isCardLayoutCc(cc))
		//{
			//base_->settingTxZoneLayout(true,false);
		//}
		//base_->settingCcChanged(cc,newValue);
		//base_->settingEditFuncValueChanged(cc);
	//}
//}
//
//void AtxCSettings::refreshCtrlCcs()
//{
	////using namespace atx;
	////if (SETTINGS_PERZONE[editSetting_]>0)
	////{
		////if (editFunc_==MAX_SYNTHCARDS)
		////{
			////for (uint_fast8_t i=0;i<EDIT_CTRLS;++i)
			////{
				////ctrlCcs_[i] = UNUSED;
				////for (uint_fast8_t j=0;j<SETTINGS;++j)
				////{
					////if (getCcInfoCtrlNum(j)==i && getCcInfoCtrlSetting(j)==editSetting_)
					////{
						////ctrlCcs_[i] = j;
						////break;
					////}
				////}
			////}
		////}
		////else if(editFunc_<atx::MAX_SYNTHCARDS)
		////{
			////for (uint_fast8_t i=0;i<EDIT_CTRLS;++i)
			////{
				////ctrlCcs_[i] = UNUSED;
				////for (uint_fast8_t j = (editFunc_ + FIRST_POLY_PARAM);j<SETTINGS_PARAMS;j += MAX_SYNTHCARDS)
				////{
					////if(getCcInfoCtrlNum(j)==i && getCcInfoCtrlSetting(j)==editSetting_)
					////{
						////ctrlCcs_[i] = j;
						////break;
					////}
				////}
			////}
		////}
		////else
		////{
			////for (uint_fast8_t i=0;i<EDIT_CTRLS;++i)
			////{
				////ctrlCcs_[i] = UNUSED;
			////}
		////}
	////}
	////else
	////{
		////for (uint_fast8_t i=0;i<EDIT_CTRLS;++i)
		////{
			////ctrlCcs_[i] = UNUSED;
			////for (uint_fast8_t j=0;j<SETTINGS_PARAMS;++j)
			////{
				////if(getCcInfoCtrlNum(j)==i && getCcInfoCtrlSetting(j)==editSetting_)
				////{
					////ctrlCcs_[i] = j;
					////break;
				////}
			////}
		////}
	////}
//
//
//}
//
//void AtxCSettings::refreshFuncCcs()
//{
	////for (uint_fast8_t i=0;i<EDIT_FUNCS;++i)
	////{
		////funcCcs_[i] = atx::UNUSED;
		////for (uint_fast8_t j=0;j<SETTINGS_PARAMS;++j)
		////{
			////if(getCcInfoFuncNum(j)==i && getCcInfoFuncSetting(j)==editSetting_)
			////{
				////funcCcs_[i] = j;
				////break;
			////}
		////}
	////}
//}
//
//void AtxCSettings::setCcValue(uint_fast8_t cc, uint16_t newValue)
//{
	////using namespace atx;
	////ccValue_[cc] = newValue;
	////uint8_t c = getCcInfoCtrlNum(cc);
	////uint8_t f = getCcInfoFuncNum(cc);
	////uint8_t z = cc & 0x07;  //zone for poly cc
	////if (c!=UNUSED)
	////{
		////if (getCcInfoCtrlSetting(cc)==editSetting_ && (cc<FIRST_POLY_PARAM || (cc>=FIRST_POLY_PARAM && z==editFunc_)))
		////{
			////base_->settingEditCtrlValueChanged(c,cc);
		////}
	////}
	////else if(f!=UNUSED)
	////{
		////if (getCcInfoFuncSetting(cc)==editSetting_ && (cc<FIRST_POLY_PARAM || (cc>=FIRST_POLY_PARAM && z==editFunc_)))
		////{
			////base_->settingEditFuncValueChanged(cc);
		////}
////
	////}
	////base_->settingCcChanged(cc,newValue);
//}