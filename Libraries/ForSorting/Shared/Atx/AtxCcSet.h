/*
* AtxCcSet.h
*
* Created: 21/08/2021 13:41:12
* Author: info
*/


#ifndef __ATXCCSET_H__
#define __ATXCCSET_H__

#include "Atx.h"
#include "AtxCcEnum.h"
#include "AtxCcInfo.h"
#include <string.h>

#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif

namespace atx
{
	static const uint8_t FIRMWARE_VOICES[] =         {       1,       2,       8,       4,       0,       0,       0,       1,       1,       1,       0,       0,       0,       0,       0,       0,       0,        1,        1,       1,       1,        0};  //hmm 8 for poly cards??  Used in AtxZone
	static const CardType FIRMWARE_CARDTYPE[] =      {CT_SYNTH,CT_SYNTH,CT_SYNTH,CT_SYNTH,CT_SYNTH,CT_SYNTH,CT_SYNTH,CT_SYNTH,CT_SYNTH,CT_SYNTH,CT_SYNTH,CT_SYNTH,CT_SYNTH,CT_SYNTH,CT_SYNTH,CT_SYNTH,CT_SETUP,CT_KEYBED,CT_FILTER,CT_MIXER,   CT_YM,CT_DISCON};
 	static const char* const FIRMWARE_NAME_SHORT[] = {   "ATM",   "ODY",   "DRM",   "STR",   "DEL",   "CYC",   "DUO",   "AUD",   "BEZ",   "PNO",     "" ,      "",      "",      "",      "",      "",   "SET",    "KEY",    "FLT",   "MIX",    "YM",    "---"};
	static const char* const FIRMWARE_NAME_LONG[] =  {"AtmegX","Odytrn",  "Drum", "String","Delay","Atcycl","Duotrn", "Audio","Bezier", "Piano",      "",      "",      "",      "",      "","Extern", "Setup", "Inputs", "Filter","Mix+FX",    "YM", "------"};
	
	class AtxCcSet
	{
		//variables
		public:

		protected:
		Firmware firmware_ = FW_DISABL;
		uint8_t ccValue_[MAX_CCS] = {0};
		uint8_t ccChanged_[16] = {0};
		uint8_t changedTotal_ = 0;
		private:
		//functions
		public:
		AtxCcSet()
		{
			for (uint_fast8_t i=0;i<MAX_CCS;++i)
			{
				ccValue_[i] = 0xFF; //force through change
			}
		}
		~AtxCcSet(){}
		AtxCcSet( const AtxCcSet &c )
		{
			for (uint_fast8_t i=0;i<MAX_CCS;++i)
			{
				ccValue_[i] = c.ccValue_[i];
			}
			firmware_ = c.firmware_;
		}
		AtxCcSet& operator=( const AtxCcSet &c )
		{
			for (uint_fast8_t i=0;i<MAX_CCS;++i)
			{
				ccValue_[i] = c.ccValue_[i];
			}
			firmware_ = c.firmware_;
		}
		inline void initialize()
		{
			for (uint_fast8_t i=0;i<MAX_CCS;++i)
			{
				setCcValue(i,getCcInfoPtr(i)->initValue);
			}
		}
		inline void initializeZone(uint8_t zone)
		{
			for(uint_fast8_t i=0;i<MAX_CCS;++i)
			{
				if(getCcInfoCtrlEnabled(i) && getCcInfoCtrlZone(i)==zone)
				{
					setCcValue(i,getCcInfoPtr(i)->initValue);
				}
			}
		}
		inline void initializeGlobal()
		{
			for(uint_fast8_t i=0;i<MAX_CCS;++i)
			{
				if(getCcInfoCtrlEnabled(i) && getCcInfoCtrlZone(i)>=MAX_ZONES)
				{
					setCcValue(i,getCcInfoPtr(i)->initValue);
				}
			}			
		}
		inline uint8_t getCcFromCtrlCode(bool isFunc, uint8_t bank, uint8_t zone, uint8_t ctrlNum)
		{
			using namespace atx;
			uint8_t cc = UNUSED;
			uint16_t ctrlCode = calcCtrlCode(isFunc,bank,zone,ctrlNum);
			for (uint_fast8_t i=0;i<MAX_CCS;++i)
			{
				uint16_t code = getCcInfoPtr(i)->ctrlCode;  //this activate flag is nasty!
				bitClear(code,11);
				if(code==ctrlCode)
				{
					cc = i;
					break;
				}
			}
			return cc;
		}
		inline void setCcValue(uint_fast8_t cc, uint8_t newValue)
		{
			if(cc>=MAX_CCS) return;
			if(ccValue_[cc]!=newValue)
			{
				ccValue_[cc] = newValue;
				setCcChanged(cc,true);
			}
		}
		uint8_t getCcValue(uint_fast8_t cc){return ccValue_[cc];}
		uint8_t* getCcValues(){return ccValue_;}
		void setCcValues(uint8_t* ccValues){memcpy(ccValue_,ccValues,sizeof(ccValue_));}
		virtual void setFirmware(Firmware newValue){firmware_ = newValue;}
		Firmware getFirmware(){return firmware_;}
		inline void setCcChanged(uint8_t cc, bool way)
		{
			if (getCcChanged(cc)!=way)
			{
				bitWrite(ccChanged_[(cc>>3)],(cc&0x07),way);
				(way) ? changedTotal_++ : changedTotal_--;
			}
		}
		bool getCcChanged(uint_fast8_t cc){return (bool)bitRead(ccChanged_[(cc>>3)],(cc&0x07));}
		static const CcInfo * getCcInfoPtr(Firmware fw, uint_fast8_t cc){return FW_CC_INFO[fw] + cc;}
		const CcInfo * getCcInfoPtr(uint_fast8_t cc){return getCcInfoPtr(firmware_,cc);}
		static const DispFmt getCcInfoFmtNum(Firmware fw, uint_fast8_t cc){return (DispFmt)(getCcInfoPtr(fw,cc)->dispFmt & 0x0F);}
		const DispFmt getCcInfoFmtNum(uint_fast8_t cc){return getCcInfoFmtNum(firmware_,cc);}
		static const DispExtra getCcInfoFmtXtr(Firmware fw, uint_fast8_t cc){return (DispExtra)(getCcInfoPtr(fw,cc)->dispFmt >> 4);}
		const DispExtra getCcInfoFmtXtr(uint_fast8_t cc){return getCcInfoFmtXtr(firmware_,cc);}
		static const bool getCcInfoCtrlEnabled(Firmware fw, uint_fast8_t cc){return ((getCcInfoPtr(fw,cc)->ctrlCode>>15)&0x01);}
		const bool getCcInfoCtrlEnabled(uint_fast8_t cc){return getCcInfoCtrlEnabled(firmware_,cc);}
		static const bool getCcInfoCtrlUseActivate(Firmware fw, uint_fast8_t cc){return ((getCcInfoPtr(fw,cc)->ctrlCode>>11)&0x01);}
		const bool getCcInfoCtrlUseActivate(uint_fast8_t cc){return getCcInfoCtrlUseActivate(firmware_,cc);}
		static const bool getCcInfoCtrlIsFunc(Firmware fw, uint_fast8_t cc){return ((getCcInfoPtr(fw,cc)->ctrlCode>>10)&0x01);}
		const bool getCcInfoCtrlIsFunc(uint_fast8_t cc){return getCcInfoCtrlIsFunc(firmware_,cc);}
		static const uint8_t getCcInfoCtrlBank(Firmware fw, uint_fast8_t cc){return ((getCcInfoPtr(fw,cc)->ctrlCode>>8)&0x03);}
		const uint8_t getCcInfoCtrlBank(uint_fast8_t cc){return getCcInfoCtrlBank(firmware_,cc);}
		static const uint8_t getCcInfoCtrlZone(Firmware fw, uint_fast8_t cc){return ((getCcInfoPtr(fw,cc)->ctrlCode>>4)&0x0F);}
		const uint8_t getCcInfoCtrlZone(uint_fast8_t cc){return getCcInfoCtrlZone(firmware_,cc);}
		static const uint8_t getCcInfoCtrlNum(Firmware fw, uint_fast8_t cc){return (getCcInfoPtr(fw,cc)->ctrlCode&0x0F);}
		const uint8_t getCcInfoCtrlNum(uint_fast8_t cc){return getCcInfoCtrlNum(firmware_,cc);}
		static const uint16_t calcCtrlCode(bool isFunc, uint8_t bank, uint8_t zone, uint8_t ctrlNum){return (0x8000 | ((uint16_t)isFunc<<10) | ((uint16_t)bank<<8) | (zone<<4) | ctrlNum );}
		static uint32_t getCcMaxValue(Firmware fw,uint_fast8_t cc){return (1UL << getCcInfoPtr(fw,cc)->bits) ;}
		uint32_t getCcMaxValue(uint_fast8_t cc){return getCcMaxValue(firmware_,cc);}
		static uint_fast8_t getCcSize(Firmware fw,uint_fast8_t cc){return ((getCcInfoPtr(fw,cc)->bits-1) >> 3)+1;}
		uint_fast8_t getCcSize(uint_fast8_t cc){return getCcSize(firmware_,cc);}
		void setCcValueScaled(Firmware fw, uint_fast8_t cc, uint16_t newValueScaled){(getCcInfoPtr(fw,cc)->bits > MIDI_DATA_BITS) ? (setCcValue(cc,newValueScaled >> (getCcInfoPtr(fw,cc)->bits - MIDI_DATA_BITS))) : (setCcValue(cc,newValueScaled << (MIDI_DATA_BITS - getCcInfoPtr(fw,cc)->bits)));}
		void setCcValueScaled(uint_fast8_t cc, uint16_t newValueScaled){setCcValueScaled(firmware_,cc,newValueScaled);}
		static uint16_t getCcValueScaled(Firmware fw, uint_fast8_t cc, uint8_t value){return (getCcInfoPtr(fw,cc)->bits > MIDI_DATA_BITS) ? ((uint16_t)value << (getCcInfoPtr(fw,cc)->bits - MIDI_DATA_BITS)) : (value >> (MIDI_DATA_BITS - getCcInfoPtr(fw,cc)->bits));}
		uint16_t getCcValueScaled(uint_fast8_t cc, uint8_t value){return getCcValueScaled(firmware_,cc,value);}
		uint16_t getCcValueScaled(uint_fast8_t cc){return getCcValueScaled(cc,ccValue_[cc]);}
		bool isCcValueScaledDifferent(uint_fast8_t cc, uint8_t newValue){return (getCcValueScaled(cc)!=getCcValueScaled(cc,newValue)) ? true : false;}
		
		static const char * const getFirmwareNamePtr(Firmware fw, bool longName){return longName ? FIRMWARE_NAME_LONG[fw] : FIRMWARE_NAME_SHORT[fw];}
		const char * const getFirmwareNamePtr(bool longName){return getFirmwareNamePtr(firmware_,longName);}
		uint_fast8_t getFirmwareVoices(){return FIRMWARE_VOICES[firmware_];}
		static uint_fast8_t getFirmwareVoices(uint8_t fw){return FIRMWARE_VOICES[fw];}
		CardType getFirmwareCardType(){return FIRMWARE_CARDTYPE[firmware_];}
		static CardType getFirmwareCardType(uint8_t fw){return FIRMWARE_CARDTYPE[fw];}
		uint8_t getCcValueFromZone1Cc(uint8_t z1Cc, uint8_t zone){return getCcValue(getCcFromZone1Cc(z1Cc,zone));}
		void setCcValueFromZone1Cc(uint8_t z1Cc, uint8_t zone, uint8_t newValue){setCcValue(getCcFromZone1Cc(z1Cc,zone),newValue);}
		uint16_t getCcValueScaledFromZone1Cc(uint8_t z1Cc, uint8_t zone){return getCcValueScaled(getCcFromZone1Cc(z1Cc,zone));}
		void setCcValueScaledFromZone1Cc(uint8_t z1Cc, uint8_t zone, uint8_t newValueScaled){setCcValueScaled(getCcFromZone1Cc(z1Cc,zone),newValueScaled);}
		inline uint8_t getZone1CcFromCc(uint8_t cc)
		{
			while(getCcInfoCtrlZone(cc)>0 && cc>0)
			{
				cc--;
			}
			return cc;
		}
		inline uint8_t getCcFromZone1Cc(uint8_t z1Cc, uint8_t zone)
		{
			while(getCcInfoCtrlZone(z1Cc)!=zone && z1Cc<0xFF)
			{
				z1Cc++;
			}
			return z1Cc;
		}
		protected:
		private:

		//AtxCcSet( const AtxCcSet &c );
		//AtxCcSet& operator=( const AtxCcSet &c );

	}; //AtxCcSet
}



#endif //__ATXCCSET_H__
