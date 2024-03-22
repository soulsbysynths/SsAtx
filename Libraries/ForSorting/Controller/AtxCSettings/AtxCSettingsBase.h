/*
 * AtxCSettingsBase.h
 *
 * Created: 18/01/2018 11:52:08
 *  Author: info
 */ 


#ifndef ATXCSETTINGSBASE_H_
#define ATXCSETTINGSBASE_H_

class AtxCSettingsBase
{
	public:
	virtual void settingEditSettingChanged(uint_fast8_t setting) = 0;
	virtual void settingEditCtrlValueChanged(uint_fast8_t ctrl, uint_fast8_t param) = 0;
	virtual void settingEditFuncChanged( uint_fast8_t func, uint_fast8_t param) = 0;
	virtual void settingEditFuncValueChanged(uint_fast8_t param) = 0;
	virtual void settingCcChanged(uint_fast8_t param, uint_fast16_t value) = 0;
	virtual bool settingTxZoneLayout(bool testMode, bool initCards) = 0;
};



#endif /* ATXCSETTINGSBASE_H_ */