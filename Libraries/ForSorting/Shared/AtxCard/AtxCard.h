/*
* AtxCard.h
*
* Created: 19/01/2018 15:44:51
* Author: info
*/


#ifndef __ATXCARD_H__
#define __ATXCARD_H__

#include "Atx.h"
#if defined(ARDUINO_SAMD_ZERO)
#include "AtxCcSet.h"
#else
#include "AtxCcEnum.h"
#endif

namespace atx
{
	#if defined(ARDUINO_SAMD_ZERO)
	static const char* const CARDTYPE_NAME[] =   {"SYNTHS","INPUTS","FILTER","MIX+FX",    "YM", "SETUP"};
	//not found any need for these yet
	//static const uint8_t CARDTYPE_POLY[] =       {       1,       8,       8,       8,       1,       0};
	//static const uint8_t CARDTYPE_FW_INDEX[] =   {       0,      16,      17,      18,      19,      16};
	//static const uint8_t CARDTYPE_FW_TOTAL[] =   {      16,       1,       1,       1,       1,       1};
	//static const uint8_t CARDTYPE_ID[] =         {       0,      10,       8,       9,       0,       0};
	#endif
	
	class AtxCard
	{
		//variables
		public:
		CardType cardType = CT_DISCON;  //UNUSED;
		bool connected = false;
		//bool external = false;
		
		protected:
		private:
		uint8_t id_ = 0; //UNUSED
		//functions
		public:
		AtxCard(){}
		explicit AtxCard(uint8_t id){id_ = id;}
		~AtxCard(){}
		AtxCard( const AtxCard &c )
		{
			cardType = c.cardType;
			connected = c.connected;
			//external = c.external;
		}
		AtxCard& operator=( const AtxCard &c )
		{
			cardType = c.cardType;
			connected = c.connected;
			//external = c.external;
		}
		uint8_t getId(){return id_;}
		virtual void setFirmware(Firmware newValue) = 0;
		virtual Firmware getFirmware() = 0;
		#if defined(ARDUINO_SAMD_ZERO)
		static const char * const getCardTypeNamePtr(CardType ct){return CARDTYPE_NAME[ct];}
		const char * const getCardTypeNamePtr(){return getCardTypeNamePtr(cardType);}
		//uint_fast8_t getCardTypePoly(){return CARDTYPE_POLY[cardType];}
		//static uint_fast8_t getCardTypePoly(uint8_t hw){return CARDTYPE_POLY[hw];}
		//uint_fast8_t getCardTypeFirstFW(){return CARDTYPE_FW_INDEX[cardType];}
		//static uint_fast8_t getCardTypeFirstFW(uint8_t hw){return CARDTYPE_FW_INDEX[hw];}
		//uint_fast8_t getCardTypeTotalFWs(){return CARDTYPE_FW_TOTAL[cardType];}
		//static uint_fast8_t getCardTypeTotalFWs(uint8_t hw){return CARDTYPE_FW_TOTAL[hw];}
		//uint_fast8_t getCardTypeID(){return CARDTYPE_ID[cardType];}
		//static uint_fast8_t getCardTypeID(uint8_t hw){return CARDTYPE_ID[hw];}
		#endif
		protected:
		private:
		//AtxCard( const AtxCard &c );
		//AtxCard& operator=( const AtxCard &c );

	}; //AtxCard
	#if defined(ARDUINO_SAMD_ZERO)
	class AtxSynthCard : public AtxCard
	{
		
		//variables
		public:
		uint8_t zone = 0;// UNUSED;
		uint8_t synth = 0;//UNUSED;
		protected:
		private:
		Firmware firmware_ = FW_DISABL;  //UNUSED;
		//functions
		public:
		using AtxCard::AtxCard;
		~AtxSynthCard(){}
		AtxSynthCard( const AtxSynthCard &c ) : AtxCard(c)
		{
			zone = c.zone;
			synth = c.synth;
			firmware_ = c.firmware_;
		}
		AtxSynthCard& operator=( const AtxSynthCard &c )
		{
			zone = c.zone;
			synth = c.synth;
			firmware_ = c.firmware_;
			AtxCard::operator=(c);
		}
		void setFirmware(Firmware newValue) override {firmware_ = newValue;}
		Firmware getFirmware() override {return firmware_;}
		protected:
		private:
		//AtxSynthCard( const AtxSynthCard &c );
		//AtxSynthCard& operator=( const AtxSynthCard &c );
	}; //AtxSynthCard
	
	class AtxPolyCard : public AtxCard, public AtxCcSet
	{
		//variables
		public:
		protected:
		private:
		//functions
		public:
		using AtxCard::AtxCard;
		~AtxPolyCard(){}
		AtxPolyCard( const AtxPolyCard &c ) : AtxCard(c), AtxCcSet(c)
		{

		}
		AtxPolyCard& operator=( const AtxPolyCard &c )
		{
			AtxCcSet::operator=(c);
			AtxCard::operator=(c);
		}
		
		void setFirmware(Firmware newValue) override {AtxCcSet::setFirmware(newValue);}
		Firmware getFirmware() override {return AtxCcSet::getFirmware();}
		protected:
		private:
		//AtxPolyCard( const AtxPolyCard &c );
		//AtxPolyCard& operator=( const AtxPolyCard &c );
	};
	//AtxPolyCard
	#endif
}

#endif //__ATXCARD_H__
