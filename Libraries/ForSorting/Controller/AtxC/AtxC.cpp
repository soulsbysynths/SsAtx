/*
* AtxC.cpp
*
* Created: 17/01/2018 16:07:02
* Author: info
*/


#include "AtxC.h"
volatile uint16_t AtxC::clks_ = 0;
extern "C" char* sbrk(int incr);
// default constructor
AtxC::AtxC()
	: editor_(AtxCEditor::getInstance())
	, hardware_(AtxCHardware::getInstance())
	, sequencer_(AtxCSequencer::getInstance())
	, sdCard_(AtxCSdCard::getInstance())
{
	using namespace atx;
	AtxSynthCard* c[MAX_SYNTHCARDS];
	for (uint8_t i=0;i<MAX_SYNTHCARDS;++i)
	{
		card_[i] = new AtxSynthCard(i);
		c[i] = (AtxSynthCard*)card_[i];
	}
	for(uint8_t i=0;i<MAX_ZONES;++i)
	{
		zone_[i].construct(i,c,this);
	}
	for(uint8_t i=MAX_SYNTHCARDS;i<MAX_CARDS;++i)
	{
		card_[i] = new AtxPolyCard(i);
	}
	hardware_.construct(this);
	AtxCPrintDisp::getOledBuffer() = &hardware_.getOledBuffer();
	AtxCPrintDisp::getOledPtr() = hardware_.getOledPtr(AtxCHardware::OLED_FUNC);
	editor_.construct(this);
	sequencer_.construct(this);
	sdCard_.construct(this);
	settings_.setFirmware(FW_SETTINGS);
	settings_.initialize();
	testMidiMsg_.setData1(60);
	testMidiMsg_.setData2(127);
} //AtxC

// default destructor
AtxC::~AtxC()
{
} //~AtxC

void AtxC::initialize()
{

	using namespace atx;

	instance_ = this;
	clks_ = 0;
	sdCard_.initialize();
	hardware_.processInitCode(hardware_.initialize(),&sdCard_) ;

	delay(500);  //CARDS WON'T INIT WITHOUT A DELAY HERE, MUST BE FINE TUNED
	for(uint8_t i=0;i<MAX_CARDS;++i)
	{
		hardware_.initCard(card_[i],true);
	}

	initCardsZones();

	//delay(2000);
	//if (txZoneLayout(false)==0)pooooo0p;;;;;  //Thursday wrote this - keep

	sequencer_.initialize();

	setGlobalBank(GB_EDIT);

}

void AtxC::initCardsZones()
{
	using namespace atx;
	for (uint8_t s=0;s<MAX_SYNTHCARDS;++s)
	{
		if(card_[s]->connected)
		{
			AtxSynthZoneMap::synthCardType[s] = card_[s]->cardType;
		}
		
	}
	for (uint8_t c=MAX_SYNTHCARDS;c<MAX_CARDS;++c)
	{
		AtxCPrintDisp::channelFirmware[c] = ((AtxPolyCard*)card_[c])->getFirmware();  //best place for this???
	}
	AtxSynthZoneMap::updateMap(&settings_);
	updateZones();
	for(uint8_t z=0;z<MAX_ZONES;++z)
	{
		zone_[z].setLowerNote(settings_.getCcValueScaledFromZone1Cc(SET_LOWERNOTE_Z1,z));
		zone_[z].setUpperNote(settings_.getCcValueScaledFromZone1Cc(SET_UPPERNOTE_Z1,z));
	}
}

void AtxC::pollFast()
{
	
	hardware_.rxMidiUsb();
	hardware_.rxSerialMidiBuffer();
	sequencer_.fillSeqBuffers();
}

void AtxC::poll(uint8_t ticksPassed)
{
	using namespace atx;
	
	//if (msTicks_==0xFFFF)
	//{
	//hardware_.printHardwareErr(0,AtxCHardware::HWERR_MSTMROF);
	//}
	
	if (clks_==0xFFFF)
	{
		hardware_.printHardwareErr(0,AtxCHardware::HWERR_SEQTMROF);
	}
	
	uint8_t clksPassed;
	if (clks_>64)  //Wire can't buffer more than 64 bytes
	{
		clksPassed = 64;
	}
	else
	{
		clksPassed = clks_;
	}
	clks_ -= clksPassed;
	
	static uint16_t leftoverClks = 0;
	
	//SEQUENCER
	//if (settings_.getCcValue(atx::SET_SEQ_MIDICTRL)==false)
	//{
	uint16_t totClks = (uint16_t)clksPassed + leftoverClks;  //must be 24 PPQN
	uint16_t seqClks = totClks >> 2;
	leftoverClks =  totClks - (seqClks << 2);
	sequencer_.pollClk(seqClks);
	//}

	//I2C
	txCcs();
	hardware_.txI2cMidiBuffer(clksPassed,0);
	//hardware_.pollI2cMidiSysex(ticksPassed);
	static uint8_t rxCard = 0;

	hardware_.rxI2cMidiBuffer(rxCard);
	hardware_.pollI2cMidiBuffer(rxCard);
	for (uint8_t i=0;i<MAX_CARDS;++i)
	{
		rxCard++;
		rxCard &= 0x0F;
		if(card_[rxCard]->connected) break;
	}

	//static uint8_t i2cTick = 0;
	//static uint8_t txClks = 0;
	//static const uint8_t I2C_POLL_TICKS = 3;
	//i2cTick += ticksPassed;
	//txClks += clksPassed;
	//if (i2cTick>=I2C_POLL_TICKS)
	//{
	//i2cTick -= I2C_POLL_TICKS;
	//hardware_.pollI2cCards(txClks);
	//txClks = 0;
	//}
	
	//SEQ OVERVIEW TICK - keep it slow!
	static uint8_t seqOvTick = 0;
	static const uint8_t SEQ_OV_TICKS = 40;
	seqOvTick += ticksPassed;
	if (seqOvTick>=SEQ_OV_TICKS)
	{
		seqOvTick -= SEQ_OV_TICKS;
		sequencer_.pollOv();
	}
	
	
	//HARDWARE
	hardware_.refreshAll(ticksPassed);
	//printFreeMem();
}

void AtxC::txCcs()
{
	using namespace atx;
	using namespace midi;
	for (uint8_t z=0;z<MAX_ZONES;++z)
	{
		if(zone_[z].getEnabled())
		{
			for(uint8_t cc=0;cc<MAX_CCS;++cc)
			{
				if(zone_[z].getCcChanged(cc))
				{
					zone_[z].setCcChanged(cc,false);
					uint8_t fc = zone_[z].getMasterSynth();
					uint8_t lc = fc + zone_[z].getSynths();
					MidiMsg msg;
					msg.setCommand(MCMD_CC);  //channel set in zoneTxMidiMsg
					msg.setData1(cc);
					msg.setData2(zone_[z].getCcValue(cc));
					for (uint8_t c=fc;c<=lc;++c)
					{
						zoneTxMidiMsg(c,&msg,(cc==MCC_SUSTAINPEDAL));  //so it only passes sustain pedal on.  Anything else we want passed on?
					}
					return;  //only 1 cc per poll
				}
			}
		}
	}
	for (uint8_t c=MAX_ZONES;c<MAX_CARDS;++c)
	{
		if(((AtxPolyCard*)card_[c])->connected)
		{
			for(uint8_t cc=0;cc<MAX_CCS;++cc)
			{
				if(((AtxPolyCard*)card_[c])->getCcChanged(cc))
				{
					((AtxPolyCard*)card_[c])->setCcChanged(cc,false);
					MidiMsg msg;
					msg.setCommand(MCMD_CC);
					msg.setChannel(c);
					msg.setData1(cc);
					msg.setData2(((AtxPolyCard*)card_[c])->getCcValue(cc));
					hardware_.bufferI2cMidiMsg(c,&msg);
					return;  //only 1 cc per poll
				}
			}
		}
	}
}

void AtxC::updateZones()
{
	using namespace atx;
	bool txMap;
	updateZones_ = false;
	for (uint8_t z=0;z<MAX_ZONES;++z)
	{
		txMap = false;
		if(zone_[z].getEnabled()!=AtxSynthZoneMap::zoneEnabled[z])
		{
			zone_[z].setEnabled(AtxSynthZoneMap::zoneEnabled[z]);
			txMap = true;
		}
		if(zone_[z].getMasterSynth()!=AtxSynthZoneMap::zoneMasterSynth[z])
		{
			zone_[z].setMasterSynth(AtxSynthZoneMap::zoneMasterSynth[z]);
			txMap = true;
		}
		if(zone_[z].getSynths()!=AtxSynthZoneMap::zoneSynths[z])
		{
			zone_[z].setSynths(AtxSynthZoneMap::zoneSynths[z]);
			txMap = true;
		}
		zone_[z].setFirmware((Firmware)settings_.getCcValueScaledFromZone1Cc(SET_FW_Z1,z));
		if(zone_[z].getEnabled())
		{
			AtxCPrintDisp::channelFirmware[z] = zone_[z].getFirmware();
		}
		else
		{
			AtxCPrintDisp::channelFirmware[z] = FW_DISABL;
		}
		settings_.setCcValueScaledFromZone1Cc(SET_ZONEENABLE_Z1,z,AtxSynthZoneMap::zoneEnabled[z]);
		settings_.setCcValueScaledFromZone1Cc(SET_MASTER_Z1,z,AtxSynthZoneMap::zoneMasterSynth[z]);
		settings_.setCcValueScaledFromZone1Cc(SET_SYNTHS_Z1,z,AtxSynthZoneMap::zoneSynths[z]);
		txZoneFirmwareHex(z);  //ALWAYS tx, as the number of synths could have changed (poss in another zone), even if fw has not
		if(txMap) txZoneMap(z);
	}
	if(editor_.getChannel()==AtxCEditor::SETTINGS_CHANNEL) editor_.setChannel(editor_.getChannel());
}

void AtxC::txZoneFirmwareHex(uint8_t zone)
{
	if(zone_[zone].getEnabled()==false) return;
	for(uint8_t s=zone_[zone].getMasterSynth();s<=(zone_[zone].getMasterSynth()+zone_[zone].getSynths());++s)
	{
		if(card_[s]->connected==true && card_[s]->getFirmware()!=zone_[zone].getFirmware() && card_[s]->cardType==atx::CT_SYNTH)
		{
			hardware_.txSerialHex(&sdCard_,s,zone_[zone].getFirmware());
			hardware_.initCard(card_[s],false);
		}
	}
	zone_[zone].initialize();
}

void AtxC::txZoneMap(uint8_t zone)
{
	using namespace atx;
	using namespace midi;
	
	static const uint8_t SYSEX_MAP_SIZE = 4;
	static const uint8_t SYSEX_SYNTH_SIZE = 3;

	uint8_t zs = 0;
	for(uint8_t s=zone_[zone].getMasterSynth();s<=zone_[zone].getSynths();++s)
	{
		uint8_t txSynth[SYSEX_SYNTH_SIZE] = {MSXT_SYNTHZONE,zone,zs};
		SysexMsg sysexSynth = SysexMsg(txSynth,SYSEX_SYNTH_SIZE);
		hardware_.bufferI2cSysex(s,&sysexSynth);
		zs++;
	}
	uint8_t smap = zone_[zone].getSynthMap();
	uint8_t txMap[SYSEX_MAP_SIZE] = {MSXT_SYNTHMAP, zone, (uint8_t)(smap >> 4), (uint8_t)(smap & 0x0F)};
	SysexMsg sysexMap = SysexMsg(txMap,SYSEX_MAP_SIZE);
	for (uint8_t c=MAX_ZONES;c<MAX_CARDS;++c)
	{
		if(card_[c]->connected)
		{
			hardware_.bufferI2cSysex(c,&sysexMap);
			((AtxPolyCard*)card_[c])->initialize();
		}
		
	}
	hardware_.txI2cMidiBuffer();
}

void AtxC::setGlobalBank(GlobalBank newBank)
{

	//clear all hardware
	editor_.setAwaitActivate(AtxCEditor::AAM_OFF); //clear flash knobs
	for (uint8_t i=0;i<=AtxCHardware::OLED_FUNC;++i)  //needed?  screws up zone layout
	{
		hardware_.getOledPtr(i)->clearDisplay();
	}
	for (uint8_t i=0;i<=AtxCHardware::RG_BANK;++i)
	{
		if (i<AtxCHardware::RG_BANK)
		{
			hardware_.getLedCircular(i).flashOffAll();
			hardware_.getLedCircular(i).setState(0);
			hardware_.getLedCircular(i).setInvertState(0);
			hardware_.getRotaryEncoderPtr(i)->setValue(0);
			hardware_.getRotaryEncoderPtr(i)->setMaxValue(16);
			if(i<AtxCHardware::RG_VAL)
			{
				hardware_.getRotaryEncoderPtr(i)->setContinuous(true);
			}
			else
			{
				hardware_.getRotaryEncoderPtr(i)->setContinuous(false);
			}
		}
		hardware_.getLedSwitch(i).flashStop();
		hardware_.getLedSwitch(i).setColour(LedRgb::RED);
	}
	hardware_.setOledOnMap(0xFF);
	
	globalBank_ = newBank;
	
	//now set hardware
	hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,0,S_GLOBAL_BANK_NAME[(uint8_t)newBank],false,false);
	switch(globalBank_)
	{
		case GB_EDIT:
		hardware_.getLedSwitch(AtxCHardware::RG_GLOBAL).setColour(LedRgb::RED);
		editor_.setChannel(editor_.getChannel());
		hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->setValue(editor_.getChannel());
		hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->setMaxValue(atx::MAX_CARDS);
		break;
		case GB_SEQUENCER:
		hardware_.getLedSwitch(AtxCHardware::RG_GLOBAL).setColour(LedRgb::GREEN);
		hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->setValue(sequencer_.getEditTrack());
		hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->setMaxValue(atx::MAX_TRACKS);
		hardware_.getRotaryEncoderPtr(AtxCHardware::RG_FUNC)->setValue(sequencer_.getSeqMode());
		hardware_.getRotaryEncoderPtr(AtxCHardware::RG_FUNC)->setMaxValue(AtxCSequencer::S_MAX);
		sequencer_.setSeqMode(sequencer_.getSeqMode());
		break;
		case GB_SETTINGS:
		hardware_.getLedSwitch(AtxCHardware::RG_GLOBAL).setColour(LedRgb::YELLOW);
		editor_.setChannel(AtxCEditor::SETTINGS_CHANNEL);
		break;
		case GB_SDCARD:
		hardware_.getLedSwitch(AtxCHardware::RG_FUNC).setColour(LedRgb::YELLOW);
		hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->setValue(sdCard_.getFunction());
		hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->setMaxValue(AtxCSdCard::SD_MAX);
		hardware_.getLedCircular(AtxCHardware::RG_FUNC).setState(0);
		if(globalBankActive_[GB_EDIT])
		{
			sdCard_.setFunction(AtxCSdCard::SD_LOADPATCH);
		}
		else if(globalBankActive_[GB_SEQUENCER])
		{
			sdCard_.setFunction(AtxCSdCard::SD_LOADMIDI);
		}
		break;
	}
}

void AtxC::dispatchMidi(midi::MidiSource src, midi::MidiMsg* msg)
{
	using namespace atx;
	using namespace midi;
	if(msg->getStatus()<MCMD_CLOCK)
	{
		if(msg->getChannel()<MAX_ZONES)
		{
			zone_[msg->getChannel()].rxMidiMsg(msg);
			if(src!=MSRC_CONTROLLER && msg->getCommand()==MCMD_CC && msg->getChannel()==editor_.getChannel())
			{
				editor_.setCcValFromMidiMsg(msg);
			}
		}
		else
		{
			if(msg->getCommand()==MCMD_CC)
			{
				((AtxPolyCard*)card_[msg->getChannel()])->setCcValue(msg->getData1(),msg->getData2());
				//hardware_.bufferI2cMidiMsg(msg->getChannel(),msg);
			}
		}
	}
}

void AtxC::setNextEditorChannel(bool direction)
{
	using namespace atx;
	uint8_t ch = editor_.getChannel();
	for(uint8_t i=0;i<MAX_CHANNELS;++i)
	{
		if (direction==true)
		{
			ch++;
		}
		else
		{
			ch--;
		}
		ch &= 0x0F;
		if(ch<MAX_ZONES)
		{
			if(zone_[ch].getEnabled())
			{
				break;
			}
		}
		else
		{
			if(card_[ch]->connected)
			{
				break;
			}
		}
	}
	editor_.setChannel(ch);
}

void AtxC::txSysexGfxReq(uint8_t channel, uint8_t cc)
{
	using namespace atx;
	using namespace midi;
	if(cc==UNUSED) return;
	if(channel==AtxCEditor::SETTINGS_CHANNEL) return;
	int_fast8_t c = UNUSED;
	MidiSysexType mst = midi::MSXT_UNUSED;
	if(channel<MAX_ZONES)
	{
		if(zone_[channel].getEnabled()==false) return;
		mst = AtxCPrintDisp::getSysexTypeFromXtr(zone_[channel].getCcInfoFmtXtr(cc));
		c = zone_[channel].getMasterSynth();
	}
	else
	{
		mst = AtxCPrintDisp::getSysexTypeFromXtr(((AtxPolyCard*)card_[channel])->getCcInfoFmtXtr(cc));
		c = channel;
	}
	if(mst==MSXT_UNUSED) return;
	if(c<MAX_CARDS)
	{
		uint8_t txData[2] = {mst,cc};
		SysexMsg tx = SysexMsg(txData,2);
		hardware_.bufferI2cSysex(c,&tx);
	}
}

void AtxC::txEditZone(uint8_t zone)
{
	using namespace atx;
	using namespace midi;
	MidiMsg msg;
	msg.setCommand(MCMD_PROGCH);
	msg.setData1(zone);
	for(uint8_t i=MAX_SYNTHCARDS;i<MAX_CARDS;++i)
	{
		if(card_[i]->connected)
		{
			msg.setChannel(i);
			hardware_.bufferI2cMidiMsg(i,&msg);
		}
	}
}

//*****************Hardware events***********************
void AtxC::hardwareCtrlValueChanged(uint8_t ctrl, uint8_t newValue)
{
	using namespace atx;
	if (locked_) return;

	int16_t valScaled;
	switch (globalBank_)
	{
		case GB_EDIT:
		case GB_SETTINGS:
		editor_.setCcValFromCtrl(ctrl,newValue>>1);
		break;
		case GB_SEQUENCER:
		sequencer_.setEditSeqFromCtrl(ctrl,newValue);
		break;
	}
}

void AtxC::hardwareRotaryEncoderValueChanged(uint8_t rot, int8_t newValue, bool direction)
{
	using namespace atx;
	using namespace midi;
	#ifdef TESTER
	hardware_.getLedCircular(rot).select(newValue&0x0F);
	return;
	#endif
	if (locked_)
	{
		return;
	}
	switch (rot)
	{
		case AtxCHardware::RG_GLOBAL:
		switch(globalBank_)
		{
			case GB_EDIT:
			setNextEditorChannel(direction);
			hardware_.getRotaryEncoderPtr(rot)->setValue(editor_.getChannel());
			break;
			case GB_SEQUENCER:
			sequencer_.setEditTrack(newValue);
			break;
			case GB_SETTINGS:

			break;
			case GB_SDCARD:
			sdCard_.setFunction((AtxCSdCard::SdFunction)newValue);
			break;
		}
		break;
		case AtxCHardware::RG_FUNC:
		switch(globalBank_)
		{
			case GB_EDIT:
			case GB_SETTINGS:
			{
				if(editor_.getChannel()<MAX_ZONES)
				{
					editor_.nextFunc(direction);
					hardware_.getRotaryEncoderPtr(rot)->setValue(editor_.getFunc());
				}
				else
				{
					editor_.nextZone(direction);
					hardware_.getRotaryEncoderPtr(rot)->setValue(editor_.getZone());
				}
			}
			break;
			case GB_SEQUENCER:
			sequencer_.setSeqMode((AtxCSequencer::SeqMode)newValue);
			break;
			case GB_SDCARD:
			break;
		}
		break;
		case AtxCHardware::RG_VAL:
		switch(globalBank_)
		{
			case GB_EDIT:
			case GB_SETTINGS:
			if (hardware_.getSwitchPtr(AtxCHardware::RG_VAL)->isPressed()==true) //getValue()==HIGH)
			{
				uint8_t ch = editor_.getChannel();
				if(ch<atx::MAX_SYNTHCARDS)
				{
					testMidiMsg_.setChannel(ch);
					testMidiMsg_.setCommand(MCMD_NOTEOFF);
					dispatchMidi(midi::MSRC_CONTROLLER,&testMidiMsg_);
					if (hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->getDirection()==true)
					{
						testMidiMsg_.setData1((testMidiMsg_.getData1()+1) & 0x7F);
					}
					else
					{
						testMidiMsg_.setData1((testMidiMsg_.getData1()-1) & 0x7F);
					}
					testMidiMsg_.setCommand(MCMD_NOTEON);
					dispatchMidi(midi::MSRC_CONTROLLER,&testMidiMsg_);
				}

			}
			else
			{
				const CcInfo*  info = editor_.getCcInfoPtr(editor_.getCcFromFunc());
				if(info->bits>=4)
				{
					editor_.setCcValFromFunc(newValue<<3);
				}
				else
				{
					editor_.setCcValFromFunc(newValue<<(MIDI_DATA_BITS - info->bits));
				}
			}
			
			break;
			case GB_SEQUENCER:

			break;
			case GB_SDCARD:
			{
				bool sw = hardware_.getSwitchPtr(AtxCHardware::RG_VAL)->isPressed();//getValue();
				if (hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->getDirection()==true)
				{
					sdCard_.incUI(sw);
				}
				else
				{
					sdCard_.decUI(sw);
				}
			}

			break;
		}
		break;
	}

}

void AtxC::hardwareSwitchClicked(uint8_t sw)
{
	using namespace atx;

	if (locked_)
	{
		return;
	}
	switch(sw)
	{
		case AtxCHardware::RG_GLOBAL:
		switch(globalBank_)
		{
			case GB_EDIT:
			setGlobalBank(GB_SEQUENCER);
			globalBankActive_[GB_EDIT] = false;
			globalBankActive_[GB_SEQUENCER] = true;
			break;
			case GB_SEQUENCER:
			setGlobalBank(GB_EDIT);
			globalBankActive_[GB_EDIT] = true;
			globalBankActive_[GB_SEQUENCER] = false;
			break;
			case GB_SETTINGS:
			editor_.setChannel(hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->getValue());  //come back to this - not working
			hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->setLockValue(false);
			globalBankActive_[GB_SETTINGS] = false;
			(globalBankActive_[GB_SEQUENCER]) ? setGlobalBank(GB_SEQUENCER) : setGlobalBank(GB_EDIT);
			break;
			case GB_SDCARD:
			break;
		}
		break;
		case AtxCHardware::RG_FUNC:
		switch(globalBank_)
		{
			case GB_EDIT:
			case GB_SETTINGS:
			{
				if(editor_.getAwaitActivate())
				{
					editor_.setAwaitActivate(AtxCEditor::AAM_YES);
				}
				else
				{
					setGlobalBank(GB_SDCARD);
					globalBankActive_[GB_SDCARD] = true;
				}
			}
			break;
			case GB_SEQUENCER:
			setGlobalBank(GB_SDCARD);
			globalBankActive_[GB_SDCARD] = true;
			break;
			case GB_SDCARD:
			globalBankActive_[GB_SDCARD] = false;
			if(globalBankActive_[GB_SETTINGS]==true)
			{
				setGlobalBank(GB_SETTINGS);
			}
			else if(globalBankActive_[GB_SEQUENCER]==true)
			{
				setGlobalBank(GB_SEQUENCER);
			}
			else
			{
				setGlobalBank(GB_EDIT);
			}
			break;
		}
		break;
		case AtxCHardware::RG_VAL:
		switch(globalBank_)
		{
			case GB_EDIT:
			case GB_SETTINGS:
			{
				if(editor_.getAwaitActivate())
				{
					editor_.setAwaitActivate(AtxCEditor::AAM_NO);
				}
			}
			break;
			case GB_SEQUENCER:
			if (sequencer_.isOverviewMode())
			{
				sequencer_.toggleCue();
			}
			break;
			case GB_SDCARD:
			sdCard_.clickUI();
			break;
		}
		break;
		case AtxCHardware::RG_BANK:
		switch(globalBank_)
		{
			case GB_EDIT:
			case GB_SETTINGS:
			{
				editor_.nextBank();
			}
			break;
			case GB_SEQUENCER:
			if(sequencer_.getPlayMode()==AtxCSequencer::SPM_STOP)
			{
				sequencer_.setPlayMode(AtxCSequencer::SPM_PLAY);
			}
			else
			{
				sequencer_.setPlayMode(AtxCSequencer::SPM_STOP);
			}
			break;
		}
		break;
	}
}

void AtxC::hardwareSwitchDoubleClicked(uint8_t sw)
{
	if (locked_)
	{
		return;
	}
	using namespace atx;
	switch(sw)
	{
		case AtxCHardware::RG_GLOBAL:
		switch(globalBank_)
		{
			case GB_EDIT:
			{

			}
			break;
			case GB_SEQUENCER:
			{

			}
			break;
		}
		break;
		case AtxCHardware::RG_FUNC:

		switch(globalBank_)
		{
			case GB_EDIT:
			{

			}
			break;
			case GB_SEQUENCER:

			break;
		}
		break;
		case AtxCHardware::RG_VAL:

		switch(globalBank_)
		{
			case GB_EDIT:
			break;
			case GB_SEQUENCER:
			if(sequencer_.isOverviewMode())
			{
				sequencer_.toggleLoop();
			}
			break;
		}

		break;
		case AtxCHardware::RG_BANK:
		switch(globalBank_)
		{
			case GB_SEQUENCER:
			hardwareSwitchClicked(sw);
			break;
		}
	}
}

void AtxC::hardwareSwitchHeld(uint8_t sw)
{
	using namespace atx;
	switch(sw)
	{
		case AtxCHardware::RG_GLOBAL:
		hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->setLockValue(true);
		setGlobalBank(GB_SETTINGS);
		globalBankActive_[GB_SETTINGS] = true;
		break;
		case AtxCHardware::RG_FUNC:
		debugMidi_ = !debugMidi_;
		hardware_.getLedSwitch(AtxCHardware::RG_FUNC).flash(1,FLASH_TICKS,FLASH_TICKS,LedRgb::YELLOW);
		break;
		case AtxCHardware::RG_VAL:

		break;
		case AtxCHardware::RG_BANK:
		switch (globalBank_)
		{
			case GB_EDIT:
			locked_ = !locked_;
			if (locked_)
			{
				for (uint8_t i=0;i<AtxCHardware::OLED_GLOBAL;++i)
				{
					hardware_.printOledLogo(i);
				}
				hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,0," THE  ",false,false);
				hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,1,"ATMUL-",false,false);
				hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,2,"TITRON",false,false);
				hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,3," AND  ",false,false);
				hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,0,"ATMEGA",false,false);
				hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,1,"TRONII",false,false);
				hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,2,"COMING",false,false);
				hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,3,"SOON! ",false,false);
			}
			else
			{
				setGlobalBank(globalBank_);
			}
			break;
			case GB_SETTINGS:

			break;
		}
		break;
	}
}

void AtxC::hardwareSwitchChanged(uint8_t sw, bool way)
{
	using namespace atx;
	using namespace midi;
	switch(sw)
	{
		case AtxCHardware::RG_GLOBAL:
		break;
		case AtxCHardware::RG_FUNC:
		break;
		case AtxCHardware::RG_VAL:
		switch(globalBank_)
		{
			case GB_EDIT:
			{
				uint8_t ch = editor_.getChannel();
				testMidiMsg_.setChannel(ch);
				if(ch<atx::MAX_ZONES)
				{
					if (way)
					{
						testMidiMsg_.setCommand(MCMD_NOTEON);
						hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setLockValue(true);
						hardware_.getLedSwitch(AtxCHardware::RG_VAL).setColour(LedRgb::YELLOW);
					}
					else
					{
						testMidiMsg_.setCommand(MCMD_NOTEOFF);
						hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setLockValue(false);
						hardware_.getLedSwitch(AtxCHardware::RG_VAL).setColour(LedRgb::RED);
					}
				}
				dispatchMidi(midi::MSRC_CONTROLLER,&testMidiMsg_);
			}
			break;
			case GB_SEQUENCER:
			break;
			case GB_SETTINGS:
			break;
		}
		break;
	}
}

void AtxC::hardwareInitCard(uint8_t card)
{
	using namespace atx;
	uint8_t o,r;//,ct,fw;
	//ct = card_[card]->cardType;
	//fw = card_[card]->getFirmware();
	o = card>>1;
	r = card&0x01;
	if (card>=12)
	{
		r <<= 1;
		hardware_.getOledPtr(o)->printStringBuffer(3,r,card_[card]->getCardTypeNamePtr(),false,true);
		r++;
		hardware_.getOledPtr(o)->printStringBuffer(0,r,AtxCcSet::getFirmwareNamePtr(card_[card]->getFirmware(),true),false,true);
	}
	else
	{
		hardware_.getOledPtr(o)->printStringBuffer(3,r,card_[card]->getCardTypeNamePtr(),false,false);
		hardware_.getOledPtr(o)->printCharBuffer(7,r,':',false);
		hardware_.getOledPtr(o)->printStringBuffer(8,r,AtxCcSet::getFirmwareNamePtr(card_[card]->getFirmware(),true),false,false);
	}
	hardware_.getOledPtr(o)->refresh();
}

void AtxC::hardwareI2cMidiRxd(uint8_t card, midi::MidiMsg * msg)
{
	using namespace midi;
	using namespace atx;
	if(debugMidi_)
	{
		Oled* oled = hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL);
		oled->printIntBuffer(0,0,msg->getChannel(),false,true,0);
		oled->printIntBuffer(0,1,msg->getCommand(),false,true,0);
		oled->printIntBuffer(0,2,msg->getData1(),false,true,0);
		oled->printIntBuffer(0,3,msg->getData2(),false,true,0);
	}

	if(msg->getChannel()<MAX_ZONES)
	{
		if(card_[card]->getFirmware()==FW_KEYBED)
		{
			if(msg->getCommand()==MCMD_CC)
			{
				//msg->setChannel(AtxCPrintDisp::getCcList(msg->getChannel()).getCcListInfo(msg->getData1()).)
			}
			dispatchMidi(MSRC_ATXCARD,msg);
		}
	}
	else  //poly cards
	{
		switch(msg->getCommand())
		{
			case MCMD_CC:
			((AtxPolyCard*)card_[card])->setCcValue(msg->getData1(),msg->getData2());
			editor_.setCcValFromMidiMsg(msg);
			if(settings_.getCcValueScaled(SET_CTRL_FOLLOWCTRLS)==true)
			{
				editor_.setEditorFromCc(msg->getChannel(),msg->getData1());
			}
			break;
			case MCMD_PROGCH:
			{
				FollowCardMode fcm = (FollowCardMode)settings_.getCcValueScaled(SET_CTRL_FOLLOWCARD);
				if(fcm==FCM_FXCARD_SET_EDITSYNTH || fcm==FCM_FXCARD_SET_EDITSYNANDFXZONE)
				{
					if(editor_.getChannel()!=msg->getData1()) editor_.setChannel(msg->getData1());
				}
				if(fcm>=FCM_FXCARD_SET_EDITSYNANDFXZONE)
				{
					for(uint8_t i=MAX_SYNTHCARDS;i<MAX_CARDS;++i)
					{
						editor_.setZone(i,msg->getData1());
					}
				}
				if(fcm==FCM_FXCARD_SET_EDITFXCARD)
				{
					editor_.setChannel(msg->getChannel());
				}
			}
			break;
		}
	}
}

void AtxC::hardwareI2cSysexRxd(uint8_t card, midi::SysexMsg* sysex)
{
	using namespace midi;
	using namespace atx;
	switch(sysex->getData(SYSEX_MSG_INDEX))
	{
		case MSXT_WAVETABLE:
		case MSXT_ARPPATTERN:
		{
			uint8_t ch = (card<MAX_SYNTHCARDS) ? AtxSynthZoneMap::synthZone[card] : card;
			if(globalBank_==GB_EDIT && editor_.getChannel()==ch && editor_.isCcOnActiveCtrl(sysex->getData(SYSEX_CC_INDEX))==true)
			{
				if(sysex->getData(SYSEX_MSG_INDEX)==MSXT_WAVETABLE)
				{
					AtxCPrintDisp::printXtrWave(sysex);
				}
				else
				{
					AtxCPrintDisp::printXtrArp(sysex);
				}
				
			}
		}
		break;
	}
}

void AtxC::hardwareExtClk()
{
	//if (settings_.getCcValue(atx::SET_SEQ_EXTCLK))
	//{
	clks_ += 4;  //96 ppqn used for refresh rate
	//}
}


//***************************** Editor events **************************

void AtxC::editorChannelChanged(uint8_t channel)
{
	using namespace atx;
	if(globalBank_!=GB_EDIT && globalBank_!=GB_SETTINGS) return;
	
	Oled* oled = hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL);
	
	hardware_.getRotaryEncoderPtr(AtxCHardware::RG_FUNC)->setMaxValue(AtxCEditor::FUNCS);
	hardware_.getRotaryEncoderPtr(AtxCHardware::RG_FUNC)->setValue(editor_.getFunc());
	
	if(channel==AtxCEditor::SETTINGS_CHANNEL)
	{
		hardware_.getLedCircular(AtxCHardware::RG_GLOBAL).setState(0);
		editor_.setCcValues(settings_.getCcValues());
		editor_.setFirmware(FW_SETTINGS);
		oled->clearRow(1);
		oled->clearRow(2);
		oled->clearRow(3);
	}
	else if(channel<MAX_ZONES)
	{
		oled->printStringBuffer(0,1,card_[zone_[channel].getMasterSynth()]->getCardTypeNamePtr(),false,true);
		oled->printStringBuffer(0,2,S_ZONE,false,true);
		oled->printIntBuffer(5,2,channel+1,false,false,0);
		if(zone_[channel].getEnabled())
		{
			hardware_.getLedCircular(AtxCHardware::RG_GLOBAL).setState(zone_[channel].getSynthMap());
			oled->printStringBuffer(0,3,zone_[channel].getFirmwareNamePtr(true),false,true);
			//(AtxCcSet)editor_ = (AtxCcSet)zone_[channel];   //WHYS THIS NOT WORK?
			editor_.setCcValues(zone_[channel].getCcValues());
			editor_.setFirmware(zone_[channel].getFirmware());
			if(bitRead(settings_.getCcValueScaled(SET_CTRL_FOLLOWEDIT),0)==true)  //fx cards follow zone
			{
				txEditZone(channel);
			}
		}
		else
		{
			oled->printStringBuffer(0,1,"Disabl",false,true);
		}
	}
	else
	{
		hardware_.getLedCircular(AtxCHardware::RG_GLOBAL).select(channel);
		AtxPolyCard* c = ((AtxPolyCard*)card_[channel]);
		oled->printStringBuffer(0,1,c->getCardTypeNamePtr(),false,true);
		if(card_[channel]->connected)
		{
			editor_.setCcValues(c->getCcValues());
			editor_.setFirmware(c->getFirmware());
		}
		else
		{
			oled->printStringBuffer(0,1,"Discon",false,true);
		}
		oled->clearRow(3);
	}
}
void AtxC::editorBankChanged(uint8_t channel, uint8_t bank)
{
	using namespace atx;
	if(globalBank_!=GB_EDIT && globalBank_!=GB_SETTINGS) return;
	hardware_.resetCtrlMoving();  //also triggered when edit card changed or global bank changed
	switch(bank)
	{
		case 0:
		hardware_.getLedSwitch(AtxCHardware::RG_BANK).setColour(LedRgb::RED);
		break;
		case 1:
		hardware_.getLedSwitch(AtxCHardware::RG_BANK).setColour(LedRgb::GREEN);
		break;
		case 2:
		hardware_.getLedSwitch(AtxCHardware::RG_BANK).setColour(LedRgb::YELLOW);
		break;
		case 3:
		hardware_.getLedSwitch(AtxCHardware::RG_BANK).setColour(LedRgb::OFF);  //actually lime
		break;
	}
}


void AtxC::editorCtrlCcChanged(uint8_t ctrl, uint8_t cc)
{
	using namespace	atx;
	if(globalBank_!=GB_EDIT && globalBank_!=GB_SETTINGS) return;
	if (cc == UNUSED)
	{
		hardware_.getOledPtr(ctrl)->clearDisplay();
	}
	else
	{
		hardware_.getCtrlPtr(ctrl)->setValue(editor_.getCcValue(cc)<<1);
		const CcInfo* info = editor_.getCcInfoPtr(cc);
		hardware_.getOledPtr(ctrl)->printStringBuffer(0,0,info->label1,false,true);
	}
	hardware_.getOledPtr(ctrl)->clearRow(1);
}
void AtxC::editorCtrlValueChanged(uint8_t ctrl, uint8_t value)
{
	using namespace atx;
	if(globalBank_!=GB_EDIT && globalBank_!=GB_SETTINGS) return;

	//hardware_.getOledPtr(ctrl)->printIntBuffer(0,1,value,false,true,3);  //FOR REAL CC VALUES
	AtxCcSet ccSet = (AtxCcSet)editor_;
	uint8_t cc = editor_.getCcFromCtrlCode(false,ctrl);
	uint8_t xtr = ccSet.getCcInfoFmtXtr(cc);
	if(xtr==XTR_ZONELAYOUT) AtxSynthZoneMap::updateMap(&ccSet);
	AtxCPrintDisp::printDisp(hardware_.getOledPtr(ctrl),&ccSet,cc);
}
void AtxC::editorFuncCcChanged(uint8_t func, uint8_t cc)
{
	if(globalBank_!=GB_EDIT && globalBank_!=GB_SETTINGS) return;
	using namespace atx;
	
	hardware_.getLedCircular(AtxCHardware::RG_VAL).flashOffAll();
	hardware_.getLedCircular(AtxCHardware::RG_VAL).setInvertState(0);

	if(editor_.getChannel()<MAX_ZONES)
	{
		hardware_.getLedCircular(AtxCHardware::RG_FUNC).select(func);
		//hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->clearRow(3);  //?? why
	}

	if (cc==UNUSED)
	{
		hardware_.getLedCircular(AtxCHardware::RG_VAL).setState(0);
		hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearDisplay();
		hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setMaxValue(1);
		hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setValue(0);
	}
	else
	{
		const CcInfo*  info = editor_.getCcInfoPtr(cc);
		hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,0,info->label1,false,true);
		hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,1,info->label2,false,true);
		uint8_t value = editor_.getCcValue(cc);
		if(info->bits>=4)
		{
			hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setMaxValue(16);
			hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setValue(value>>3);
		}
		else
		{
			hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setMaxValue(1<<info->bits);
			hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setValue(value>>(MIDI_DATA_BITS - info->bits));
		}
		txSysexGfxReq(editor_.getChannel(),cc);
	}
	

}
void AtxC::editorFuncValueChanged(uint8_t value)
{
	using namespace atx;
	if(globalBank_!=GB_EDIT && globalBank_!=GB_SETTINGS) return;
	uint8_t cc = editor_.getCcFromFunc();
	if(editor_.getCcInfoPtr(cc)->bits>=4)
	{
		hardware_.getLedCircular(AtxCHardware::RG_VAL).select(value>>3);
	}
	else
	{
		hardware_.getLedCircular(AtxCHardware::RG_VAL).select(value>>(MIDI_DATA_BITS-editor_.getCcInfoPtr(cc)->bits));
	}
	
	//hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printIntBuffer(0,2,value,false,true,3);  //FOR REAL CC VALUES
	AtxCcSet ccSet = (AtxCcSet)editor_;
	AtxCPrintDisp::printDisp(hardware_.getOledPtr(AtxCHardware::OLED_FUNC),&ccSet,cc);
}

void AtxC::editorZoneChanged(uint8_t zone)
{
	using namespace atx;
	using namespace midi;
	if(globalBank_!=GB_EDIT && globalBank_!=GB_SETTINGS) return;

	Oled* oled = hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL);
	const uint8_t ROW = 2;
	if(zone==UNUSED)
	{
		oled->clearRow(ROW);
	}
	else if(zone<MAX_ZONES)
	{
		oled->printStringBuffer(0,ROW,S_ZONE,false,true);
		oled->printIntBuffer(5,ROW,zone+1,false,false,0);
	}
	else
	{
		oled->printStringBuffer(0,ROW,"Global",false,false);
	}
	if(zone==UNUSED)
	{
		hardware_.getLedCircular(AtxCHardware::RG_FUNC).setState(0);
	}
	else
	{
		hardware_.getLedCircular(AtxCHardware::RG_FUNC).select(zone);
	}
	if(bitRead(settings_.getCcValueScaled(SET_CTRL_FOLLOWEDIT),1)==true)  //fx cards follow zone
	{
		txEditZone(zone);
	}
}

void AtxC::editorAwaitActivateChanged(uint8_t awaitActMode)
{
	if(globalBank_!=GB_EDIT && globalBank_!=GB_SETTINGS) return;
	switch(awaitActMode)
	{
		case AtxCEditor::AAM_OFF:
		//hmm what to put here?
		hardware_.getLedSwitch(AtxCHardware::RG_FUNC).flashStop();
		hardware_.getLedSwitch(AtxCHardware::RG_VAL).flashStop();
		break;
		case AtxCEditor::AAM_ON:
		hardware_.getLedSwitch(AtxCHardware::RG_FUNC).flash(FLASH_FOREVER,FLASH_TICKS,FLASH_TICKS,LedRgb::GREEN,LedRgb::OFF,true);
		hardware_.getLedSwitch(AtxCHardware::RG_VAL).flash(FLASH_FOREVER,FLASH_TICKS,FLASH_TICKS,LedRgb::RED,LedRgb::OFF,true);
		break;
		case AtxCEditor::AAM_YES:
		hardware_.getLedSwitch(AtxCHardware::RG_FUNC).flash(5,FLASH_TICKS,FLASH_TICKS,LedRgb::GREEN);
		hardware_.getLedSwitch(AtxCHardware::RG_VAL).flash(5,FLASH_TICKS,FLASH_TICKS,LedRgb::OFF,LedRgb::OFF,true);  //weird little way to get knob off while confirm flash on other
		break;
		case AtxCEditor::AAM_NO:
		hardware_.getLedSwitch(AtxCHardware::RG_FUNC).flash(5,FLASH_TICKS,FLASH_TICKS,LedRgb::OFF,LedRgb::OFF,true);
		hardware_.getLedSwitch(AtxCHardware::RG_VAL).flash(5,FLASH_TICKS,FLASH_TICKS,LedRgb::RED,LedRgb::OFF,true);
		break;
	}
	
	//have to do this coz firmware upload immediately blocks
	hardware_.refreshLeds();
	
	if(updateZones_) updateZones();
}

void AtxC::editorSettingChanged(uint8_t cc, uint8_t value)
{
	using namespace atx;
	
	uint8_t z = settings_.getCcInfoCtrlZone(cc);
	if(z>=MAX_ZONES)
	{
		settings_.setCcValue(cc,value);
		switch (cc)
		{
			case SET_SEQ_BPM:
			sequencer_.setBpm(settings_.getCcValueScaled(cc)+settings_.getCcInfoPtr(cc)->tag);
			break;
			case SET_SEQ_CLICKTYPE:
			sequencer_.clickMode = (AtxCSequencer::ClickMode)settings_.getCcValueScaled(cc);
			break;
			case SET_SEQ_CLICKZONE:
			sequencer_.setClickChannel(settings_.getCcValueScaled(cc));
			break;
			default:
			break;
		}
	}
	else
	{
		uint8_t z1CC = settings_.getZone1CcFromCc(cc);
		if(z1CC!=SET_ZONEENABLE_Z1 && z1CC!=SET_MASTER_Z1 && z1CC!=SET_SYNTHS_Z1)
		{
			settings_.setCcValue(cc,value);
		}
		switch (z1CC)
		{
			case SET_ZONEENABLE_Z1:
			case SET_MASTER_Z1:
			case SET_SYNTHS_Z1:
			case SET_FW_Z1:  //def need this one?  if not this could go in an else above
			updateZones_ = true;
			break;
			case SET_LOWERNOTE_Z1:
			zone_[z].setLowerNote(settings_.getCcValueScaled(cc));
			break;
			case SET_UPPERNOTE_Z1:
			zone_[z].setUpperNote(settings_.getCcValueScaled(cc));
			break;
			case SET_LFOMODE_Z1:
			zone_[z].setLfoTriggerMode((AtxZone::LfoTriggerMode)settings_.getCcValueScaled(cc));
			break;
			default:
			break;
		}
	}

	
}


//*************************Sequencer events**************************************************
void AtxC::sequencerBpmChanged(uint16_t bpm)
{
	clkTimer_.enable(false);

	uint32_t compare = ((uint32_t)F_CPU * 5) / ((uint32_t)bpm << 3);

	tc_clock_prescaler prescaler;

	//this covers all bases at the moment. A lot of options could be removed.
	if (compare<65536)
	{
		prescaler = TC_CLOCK_PRESCALER_DIV1;
	}
	else if (compare<131072)
	{
		prescaler = TC_CLOCK_PRESCALER_DIV2;
		compare >>= 1;
	}
	else if (compare<262144)
	{
		prescaler = TC_CLOCK_PRESCALER_DIV4;
		compare >>= 2;
	}
	else if(compare<524288)
	{
		prescaler = TC_CLOCK_PRESCALER_DIV8;
		compare >>= 3;
	}
	else if (compare<1048576)
	{
		prescaler = TC_CLOCK_PRESCALER_DIV16;
		compare >>= 4;
	}
	else if (compare<4194304)
	{
		prescaler = TC_CLOCK_PRESCALER_DIV64;
		compare >>= 6;
	}
	else if (compare<16777216)
	{
		prescaler = TC_CLOCK_PRESCALER_DIV256;
		compare >>= 8;
	}

	//no other prescalers fall in range of acceptable bpm
	clkTimer_.configure(prescaler,TC_COUNTER_SIZE_16BIT,TC_WAVE_GENERATION_MATCH_FREQ);
	clkTimer_.setCompare(0, (uint16_t)compare);
	clkTimer_.setCallback(true, TC_CALLBACK_CC_CHANNEL0, AtxC::pollClkCallback);
	clkTimer_.enable(true);
}

void AtxC::sequencerEditTrackChanged(uint8_t track)
{
	Oled* oled = hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL);
	oled->printStringBuffer(0,1,"Track",false,false);
	oled->printIntBuffer(5,1,track+1,false,false,0);
	hardware_.getLedCircular(AtxCHardware::RG_GLOBAL).select(track);
}

void AtxC::sequencerModeChanged(uint8_t seqMode)
{
	if(globalBank_!=GB_SEQUENCER) return;
	if (sequencer_.isOverviewMode()==false)
	{
		for (uint8_t i=0;i<6;++i)
		{
			hardware_.getOledPtr(i)->clearDisplay();
		}
	}
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearDisplay();
	hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setMaxValue(16);  //default

	switch((AtxCSequencer::SeqMode)seqMode)
	{
		case AtxCSequencer::S_OVERVIEW:

		break;

		case AtxCSequencer::S_MERGESEQ:
		
		break;
		case AtxCSequencer::S_RECSEQ:
		hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,2,"Zone",false,true);
		hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setMaxValue(atx::MAX_ZONES);
		hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setValue(sequencer_.getRecZone());
		break;
		case AtxCSequencer::S_EDITSEQ:
		hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,2,"Quant",false,true);
		hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setValue(sequencer_.getRecQuantise());
		break;
		default:

		break;
	}
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,0,S_SEQMODE_TOP[seqMode],false,true);
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,1,S_SEQMODE_BOT[seqMode],false,true);
	hardware_.getLedCircular(AtxCHardware::RG_FUNC).select(seqMode);
}

void AtxC::sequencerEventListChanged(uint16_t index, midi::MidiSeqMsg * msg)
{
	if(globalBank_!=GB_SEQUENCER) return;
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printIntBuffer(0,3,index,false,false,6);
	if (sequencer_.getSequenceLength()==0)
	{
		for (uint8_t i=0;i<AtxCHardware::CTRLS;++i)
		{
			hardware_.getOledPtr(1)->printStringBuffer(0,1,"Empty",false,true);
		}
		hardware_.getLedCircular(AtxCHardware::RG_VAL).setState(0);
		return;
	}
	hardware_.getLedCircular(AtxCHardware::RG_VAL).select((index << 4) / sequencer_.getSequenceLength());
	
	hardware_.getOledPtr(0)->printIntBuffer(0,1,msg->getClk(),false,true,0);
	hardware_.getOledPtr(1)->printIntBuffer(0,1,msg->getChannel(),false,true,0);
	hardware_.getOledPtr(2)->printIntBuffer(0,1,msg->getData1(),false,true,0);
	hardware_.getOledPtr(3)->printIntBuffer(0,1,msg->getData2(),false,true,0);

	for (uint8_t i=4;i < AtxCHardware::CTRLS;++i)
	{
		hardware_.getOledPtr(i)->clearRow(1);
	}
}

void AtxC::sequencerPlayChanged(uint8_t playMode)
{
	if (globalBank_==GB_SEQUENCER)
	{
		hardware_.getLedSwitch(AtxCHardware::RG_BANK).flashStop();
		switch(sequencer_.getPlayMode())
		{
			case AtxCSequencer::SPM_STOP:
			hardware_.getLedSwitch(AtxCHardware::RG_BANK).setColour(LedRgb::YELLOW);
			break;
			case AtxCSequencer::SPM_PLAY:
			hardware_.getLedSwitch(AtxCHardware::RG_BANK).setColour(LedRgb::GREEN);
			break;
			case AtxCSequencer::SPM_REC:
			hardware_.getLedSwitch(AtxCHardware::RG_BANK).setColour(LedRgb::RED);
			break;
			case AtxCSequencer::SPM_RECCUE:
			case AtxCSequencer::SPM_COUNTIN:
			hardware_.getLedSwitch(AtxCHardware::RG_BANK).flash(FLASH_FOREVER,FLASH_TICKS,FLASH_TICKS,LedRgb::RED);
			break;
		}
	}
}

void AtxC::sequencerOvSeqChanged(uint8_t zone, uint8_t seq, uint8_t display, uint8_t row)
{
	if(globalBank_!=GB_SEQUENCER) return;
	hardware_.getOledPtr(display)->printIntBuffer(0,row,seq+1,false,false,false);
}

void AtxC::sequencerOvPlayChanged(uint8_t zone, uint8_t seq, bool selected, uint8_t playState, uint8_t display, uint8_t row)
{
	if(globalBank_!=GB_SEQUENCER) return;
	hardware_.getOledPtr(display)->printCharBuffer(1,row,Oled::FONT_PS + (uint8_t)playState + (selected * Oled::FONT_PS_SEL_OFF),true);
}

void AtxC::sequencerOvSeqLoopChanged(uint8_t zone, uint8_t seq, bool looping, uint8_t display, uint8_t row)
{
	if(globalBank_!=GB_SEQUENCER) return;
	char l;
	if (looping)
	{
		l = Oled::FONT_LOOP;
	}
	else
	{
		l = ' ';
	}
	hardware_.getOledPtr(display)->printCharBuffer(3,row,l,false);
}

void AtxC::sequencerOvSeqNameChanged(uint8_t zone, uint8_t seq, const char * name, uint8_t display, uint8_t row)
{
	if(globalBank_!=GB_SEQUENCER) return;
	hardware_.getOledPtr(display)->printStringBuffer(4,row,name,false,true);
}

void AtxC::sequencerOvSeqPosChanged(uint8_t zone, uint8_t seq, uint8_t pos, uint8_t display, uint8_t row)
{
	if(globalBank_!=GB_SEQUENCER) return;
	//for (uint8_t i=0;i<pos;++i)
	//{
	//hardware_.getOledPtr(display)->setInvertMap(i+4,row,true);
	//}
	//for (uint8_t i=pos;i<8;++i)
	//{
	//hardware_.getOledPtr(display)->setInvertMap(i+4,row,false);
	//}
	static const uint8_t INVERT_START = 4 * Oled::FONT_WIDTH;
	hardware_.getOledPtr(display)->setInvertRange(row,INVERT_START,INVERT_START+pos);
}

void AtxC::sequencerOvClkChanged(uint16_t bar, uint8_t beat, uint8_t clk)
{
	if(globalBank_!=GB_SEQUENCER) return;
	if (bar>999)
	{
		hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printIntBuffer(0,2,bar,false,false,6);
	}
	else
	{
		hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printIntBuffer(0,2,bar,true,false,3);
	}
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printCharBuffer(0,3,':',false);
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printIntBuffer(1,3,beat,false,false,2);
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printCharBuffer(3,3,':',false);
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printIntBuffer(4,3,clk,false,false,2);
}


void AtxC::sequencerRecQuantiseChanged(uint8_t quantise)
{
	if(globalBank_!=GB_SEQUENCER) return;
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearRow(3);
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printCharBuffer(0,3,quantise,true);
}

void AtxC::sequencerRecTrackChanged(uint8_t track)
{
	if(globalBank_!=GB_SEQUENCER) return;
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearRow(3);
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printIntBuffer(0,3,track+1,true,true,0);
}

void AtxC::sequencerRecSeqPosChanged(uint8_t pos)
{
	if(globalBank_!=GB_SEQUENCER) return;
	//COME BACK!
	//hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->setInvertMapLevelRow(2,3,pos,6);
	
}

//*******************************SD Card Events******************************
void AtxC::sdPrintString(const char * string, const char icon /* = 0 */)
{
	char name[2][7] = {0};

	if (icon)
	{
		strncpy(name[0],string,4);
		name[0][4] = '\0';
		//if (strlen(string)<=4)
		//{
		strncpy(name[1],string+4,6);
		name[1][6] = '\0';
		//}
		hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printCharBuffer(0,2,icon,true);
		hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(2,2,name[0],false,true);
		hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,3,name[1],false,true);
	}
	else
	{
		strncpy(name[0],string,6);
		name[0][6] = '\0';
		//if (strlen(string)<=6)
		//{
		strncpy(name[1],string+6,6);
		name[1][6] = '\0';
		//}
		hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,2,name[0],false,true);
		hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,3,name[1],false,true);
	}
}

void AtxC::sdPrintFail(const char * msg)
{
	hardware_.getOledPtr(AtxCHardware::RG_VAL)->printStringBuffer(0,2,"SDfail",false,true);
	hardware_.getOledPtr(AtxCHardware::RG_VAL)->printStringBuffer(0,3,msg,false,true);
}

void AtxC::sdHighlightChar(uint8_t charPos)
{
	uint8_t c = 2, r = 2;
	for (uint8_t i=0;i<10;++i)
	{
		if (i==charPos)
		{
			uint8_t start = c << Oled::FONT_WIDTH_BS;
			uint8_t end = start + Oled::FONT_WIDTH;
			hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->setInvertRange(r,start,end);
			break;
		}
		if (c==5)
		{
			c = 0;
			r++;
		}
		else
		{
			c++;
		}
	}
	for(uint8_t i=0;i<hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->getRows();++i)
	{
		if(i!=r) hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearInvertMapRow(i);
	}
}

void AtxC::sdFunctionChanged(uint8_t func)
{
	using namespace atx;
	for(uint8_t i=0;i<AtxCHardware::CTRLS;++i)
	{
		hardware_.getOledPtr(i)->clearDisplay();
	}
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearRow(0);  //may want to delete this
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearRow(1);  //may want to delete this
	hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearRow(3);  //may want to delete this
	
	hardware_.getLedCircular(AtxCHardware::RG_GLOBAL).select(func);
	hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,1,S_SD_FUNC_TOP[func],false,true);
	hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,2,S_SD_FUNC_BOT[func],false,true);
	Oled* oled = hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL);
	const uint8_t ROW = 3;
	switch(func)
	{
		case AtxCSdCard::SD_LOADPATCH:
		case AtxCSdCard::SD_SAVEPATCH:
		{
			uint8_t ch = editor_.getChannel();
			if(ch==AtxCEditor::SETTINGS_CHANNEL)
			{
				oled->printStringBuffer(0,ROW,"Setup",false,true);
			}
			else if(ch<MAX_ZONES)
			{
				oled->printStringBuffer(0,ROW,S_ZONE,false,false);
				oled->printIntBuffer(4,ROW,ch+1,false,false,2);
			}
			else
			{
				oled->printStringBuffer(0,ROW,S_CARD,false,false);
				oled->printIntBuffer(4,ROW,ch+1,false,false,2);
			}
			hardware_.getRotaryEncoderPtr(AtxCHardware::RG_FUNC)->setMaxValue(atx::MAX_CARDS);
		}
		break;
		case AtxCSdCard::SD_LOADMIDI:
		{
			uint8_t t = sequencer_.getEditTrack();
			oled->printStringBuffer(0,ROW,"Track",false,false);
			oled->printIntBuffer(5,ROW,t+1,false,false,2);
		}
		break;
	}
}

void AtxC::sdFunctionActivated(uint8_t sdFunction)
{
	using namespace midi;
	using namespace atx;
	switch(sdFunction)
	{
		case AtxCSdCard::SD_SAVEPATCH:
		{
			MidiMsgList mml;
			uint8_t ch = editor_.getChannel();
			if(ch==AtxCEditor::SETTINGS_CHANNEL)
			{
				for (uint8_t cc=0;cc<MAX_CCS;++cc)
				{
					MidiSeqMsg m;
					m.setCommand(MCMD_CC);
					m.setChannel(0);
					m.setData1(cc);
					m.setData2(settings_.getCcValue(cc));
					mml.append(m);
				}
				sdCard_.saveMidiMsgList(&mml);
			}
			else if(ch<MAX_ZONES)
			{
				for (uint8_t cc=0;cc<MAX_CCS;++cc)
				{
					if(zone_[ch].getCcInfoPtr(cc)->bits>0)
					{
						MidiSeqMsg m;
						m.setCommand(MCMD_CC);
						m.setData1(cc);
						m.setData2(zone_[ch].getCcValue(cc));
						mml.append(m);
					}
				}
				for (uint8_t c=MAX_ZONES;c<MAX_CARDS;++c)
				{
					AtxPolyCard* card = (AtxPolyCard*)card_[c];
					if(card->connected)
					{
						for (uint8_t cc=0;cc<MAX_CCS;++cc)
						{
							if(card->getCcInfoPtr(cc)->bits>0 && card->getCcInfoCtrlZone(cc)==ch)
							{
								uint8_t z1cc = card->getZone1CcFromCc(cc);
								MidiSeqMsg m;
								m.setCommand(MCMD_CC);
								m.setChannel(c);
								m.setData1(z1cc);
								m.setData2(card->getCcValue(cc));
								mml.append(m);
							}
						}
					}
				}
				sdCard_.saveMidiMsgList(&mml);
			}
			else
			{
				AtxPolyCard* card = (AtxPolyCard*)card_[ch];
				for (uint8_t cc=0;cc<MAX_CCS;++cc)
				{
					MidiSeqMsg m;
					m.setCommand(MCMD_CC);
					m.setChannel(ch);
					m.setData1(cc);
					m.setData2(card->getCcValue(cc));
					mml.append(m);
				}
				sdCard_.saveMidiMsgList(&mml);
			}
		}
		break;
		case AtxCSdCard::SD_LOADPATCH:
		{
			MidiMsgList mml;
			uint8_t writeCh = editor_.getChannel();
			sdCard_.loadMidiMsgList(&mml);
			for (uint32_t i=0;i<mml.getCount();++i)
			{
				MidiSeqMsg msmsg = mml.getEvent(i);
				if (msmsg.getCommand()==MCMD_CC)
				{
					uint8_t readCh = msmsg.getChannel();
					if(writeCh<MAX_ZONES)
					{
						if(readCh==0)  //if the cc is on any other channel, it wasn't written using SD_SAVEPATCH
						{
							MidiMsg m;
							m.setChannel(writeCh);
							m.setCommand(MCMD_CC);
							m.setData1(msmsg.getData1());
							m.setData2(msmsg.getData2());
							dispatchMidi(MSRC_SDCARD,&m);
						}
						else
						{
							AtxPolyCard* card = (AtxPolyCard*)card_[readCh];
							if(card->connected)
							{
								uint8_t cc = msmsg.getData1();
								if (card->getCcInfoCtrlZone(cc)==0)  //if not zone 1, it wasn't written using SD_SAVEPATCH
								{
									uint8_t zxcc = card->getCcFromZone1Cc(cc,writeCh);
									MidiMsg m;
									m.setChannel(readCh);
									m.setCommand(MCMD_CC);
									m.setData1(zxcc);
									m.setData2(msmsg.getData2());
									dispatchMidi(MSRC_SDCARD,&m);
								}
							}
						}
					}
					else
					{
						if(readCh==writeCh)  //check it's for correct card
						{
							AtxPolyCard* card = (AtxPolyCard*)card_[writeCh];
							MidiMsg m;
							m.setChannel(writeCh);
							m.setCommand(MCMD_CC);
							m.setData1(msmsg.getData1());
							m.setData2(msmsg.getData2());
							dispatchMidi(MSRC_SDCARD,&m);
						}
					}
				}
			}
		}
		break;
		case AtxCSdCard::SD_LOADMIDI:
		sdCard_.initMidiStream(sequencer_.getEditTrack(),sequencer_.getEditSeq(),sequencer_.getSequence());
		break;
	}
}

void AtxC::sdRefreshStatusBar(uint8_t barFill)
{
	hardware_.getLedCircular(AtxCHardware::RG_VAL).fill(barFill>>4);
}





///*
//* AtxC.cpp
//*
//* Created: 17/01/2018 16:07:02
//* Author: info
//*/
//
//
//#include "AtxC.h"
//volatile uint16_t AtxC::msTicks_ = 0;
//volatile uint16_t AtxC::clks_ = 0;
//extern "C" char* sbrk(int incr);
//// default constructor
//AtxC::AtxC() : hardware_(AtxCHardware::getInstance()), editor_(AtxCEditor::getInstance())//, sequencer_(AtxCSequencer::getInstance()), sdCard_(AtxCSdCard::getInstance()), arMsgBox_(ARMsgBox::getInstance())
//{
//using namespace atx;
//AtxCard * c[MAX_CARDS];
//for (uint8_t i=0;i<MAX_CARDS;++i)
//{
//c[i] = &card_[i];
//}
//for(uint8_t i=0;i<MAX_ZONES;++i)
//{
//zone_[i] = new AtxZone(i,c,this);
//}
//for (uint8_t i=0;i<2;++i)
//{
//zone_[i]->setEnabled(true);
//zone_[i]->setSynths(0);
//zone_[i]->setMasterSynth(i);
//}
//hardware_.construct(this,c);
////sdCard_.construct(this,c);
////settings_.construct(this);
//editor_.construct(this,c);
////sequencer_.construct(this,zone_);
////arMsgBox_.construct(this);
//
//} //AtxC
//
//// default destructor
//AtxC::~AtxC()
//{
//} //~AtxC
//
//void AtxC::initialize()
//{
//
//using namespace atx;
//
//instance_ = this;
////updateFast_ = false;
//msTicks_ = 0;
//clks_ = 0;
//
//uint8_t initCode = hardware_.initialize();
//
////could do thsese lines in hardware init?
//for(uint8_t i=0;i<2;++i)
//{
//hardware_.getRotaryEncoderPtr(i)->setContinuous(true);
//}
//hardware_.getRotaryEncoderPtr(2)->setContinuous(false);
//for (uint8_t i=0;i<3;++i)
//{
//hardware_.getLedCircular(i).flashOffAll();
//}
//hardware_.getLedCircular(AtxCHardware::RG_VAL).flash(FLASH_DRUM,FLASH_FOREVER,FLASH_TICKS,FLASH_TICKS<<1,true);
//
//#ifndef TESTER
//sdCard_.initialize();
//if (initCode>0 & initCode<=9)
//{
//uint8_t f,l;
//if (initCode==9)
//{
//f = 0; l = (MAX_SYNTHCARDS-1);
//}
//else
//{
//f = initCode-1; l = initCode-1;
//}
//delay(2000);
//for (uint8_t i = f;i<=l;++i)
//{
//uint8_t oled = i >> 1;
//uint8_t row = i & 0x01;
//hardware_.getOledPtr(oled)->printStringBuffer(0,row,"Init ATM:",false,false);
//hardware_.getOledPtr(oled)->printIntBuffer(10,row,i+1,false,true,2);
//hardware_.getOledPtr(oled)->refresh();
//hardware_.txSerialHex(sdCard_,i,0,false);
//delay(1000);
//}
//}
//else if (initCode==15)
//{
//delay(2000);
//hardware_.getOledPtr(5)->printStringBuffer(0,0,"FREE I2C",false,true);
//hardware_.getOledPtr(5)->refresh();
//hardware_.freeI2Clines();  //may just want this as an option
//}
////sequencer_.initialize();
//
//delay(500); //to make sure all slaves are up and running
//
////delay(5000);
//
//hardware_.initCards();
//sdCard_.setFunction(AtxCSdCard::SD_LOADSETS);
//
////if(initCode!=10)
//if(1==2)
//{
//sdCard_.loadSettings();
//}
//else
//{
//sdCard_.setFunction(AtxCSdCard::SD_IDLE);
//
//uint8_t fwMap[MAX_SYNTHCARDS];
//for (uint8_t i=0;i<MAX_SYNTHCARDS;++i)
//{
//if (card_[i].getConnectedOrExternal())
//{
//fwMap[i] = card_[i].firmware;
//}
//}
////settings_.initialize(hardware_.getSynthsConnectedMap(),fwMap);
//initComplete();
////THIS NEEDS READDING WHEN I CAN SAVE JUST SETS NOT EVERYTHING
////sdCard_.saveSettings();
////sdTask_ = SD_SAVESETS;
//}
//#endif
//
////if (txZoneLayout(false)==0)pooooo0p;;;;;  //Thursday wrote this - keep
//
////fastTimer_.configure(TC_CLOCK_PRESCALER_DIV1024,TC_COUNTER_SIZE_8BIT,TC_WAVE_GENERATION_NORMAL_FREQ);   //every 20ms  ((isn't this actually 5.46ms?? 480000 / (1024 * 256)
////fastTimer_.configure(TC_CLOCK_PRESCALER_DIV256,TC_COUNTER_SIZE_8BIT,TC_WAVE_GENERATION_NORMAL_FREQ);  //very tentatively try this
////fastTimer_.setCallback(true, TC_CALLBACK_CC_CHANNEL0, AtxC::pollFastCallback);  // set DAC in the callback
////fastTimer_.enable(true);
//
////clkTimer_.configure(TC_CLOCK_PRESCALER_DIV64,TC_COUNTER_SIZE_16BIT,TC_WAVE_GENERATION_MATCH_FREQ);   //every 1ms
////clkTimer_.setPeriodMatch(750, 375, 0); // 1 match, channel 0
////clkTimer_.setCallback(true, TC_CALLBACK_CC_CHANNEL0, AtxC::pollSequencerCallback);  // set DAC in the callback
////clkTimer_.enable(true);
//
//
//msTimer_.configure(TC_CLOCK_PRESCALER_DIV64,TC_COUNTER_SIZE_16BIT,TC_WAVE_GENERATION_MATCH_FREQ);   //every 5ms
//msTimer_.setPeriodMatch(750, 375, 0); // 1 match, channel 0
//msTimer_.setCallback(true, TC_CALLBACK_CC_CHANNEL0, AtxC::pollMsCallback);  // set DAC in the callback
//msTimer_.enable(true);
//}
//
//#ifdef TESTER
//void AtxC::poll()
//{
//static uint8_t animTick = 0;
//static uint8_t animFrame = 255;
//static uint8_t animFillType = 3;
//
//if (msTicks_)
//{
//msTicks_ --;
//animTick++;
//
//if (animTick>=50)
//{
//animTick = 0;
//animFrame++;
//if ((animFrame%24)==0)
//{
//animFillType++;
//animFillType &= 0x03;
//}
//for(int i=0;i<8;++i)
//{
//uint8_t c;
//if (animFillType==0)
//{
//c = 173;
//}
//else if(animFillType==2)
//{
//c = 48 + i;
//}
//else
//{
//c = 32;
//}
//hardware_.getOledPtr(i)->setThermometerRow(atx::UNUSED);
//if (i<6)
//{
//hardware_.getOledPtr(i)->printCharBuffer((animFrame%12),((animFrame/12)&0x01),c,false);
//}
//else
//{
//hardware_.getOledPtr(i)->printCharBuffer((animFrame%6),((animFrame/6)&0x03),c,false);
//}
//}
//for (uint8_t i=0;i<3;++i)
//{
//if (cancelFlash_[i]==false)
//{
//hardware_.getLedCircular(i).fill((animFrame&0x0F));
//}
//}
//for (uint8_t i=0;i<4;++i)
//{
//if (cancelFlash_[i]==false)
//{
//hardware_.getLedSwitch(i).setColour((LedRgb::LedRgbColour)(animFrame&0x03));
//}
//}
//}
//hardware_.refreshAll(1);
//}
//}
//#else
//void AtxC::poll()
//{
//using namespace atx;
//
//if (msTicks_==0xFFFF)
//{
//hardware_.printHardwareErr(0,AtxCHardware::HWERR_MSTMROF);
//}
//if (clks_==0xFFFF)
//{
//hardware_.printHardwareErr(0,AtxCHardware::HWERR_SEQTMROF);
//}
//
//uint8_t ticksPassed;
//if (msTicks_>255)
//{
//ticksPassed = 255;
//}
//else
//{
//ticksPassed = msTicks_;
//}
//msTicks_ -= ticksPassed;
//uint8_t clksPassed;
//if (clks_>255)
//{
//clksPassed = 255;
//}
//else
//{
//clksPassed = clks_;
//}
//clks_ -= clksPassed;
//
//static uint16_t leftoverClks = 0;
////SEQUENCER
////if (settings_.getCcValue(atx::SET_SEQ_MIDICTRL)==false)
////{
//uint16_t totClks = (uint16_t)clksPassed + leftoverClks;  //must be 24 PPQN
//uint16_t seqClks = totClks >> 2;
//leftoverClks =  totClks - (seqClks << 2);
////sequencer_.pollClk(seqClks);
////}
//
////I2C
//
//for(uint8_t i=0;i<clksPassed;++i)
//{
////hardware_.bufferI2cByte(MidiMsg::MCMD_CLOCK);
//}
//hardware_.txI2cMidiBuffer();
//static uint8_t rxCard = 16;
//rxCard = hardware_.nextConnectedCard(rxCard);
//hardware_.rxI2cMidiBuffer(rxCard);
//hardware_.pollI2cMidiBuffer(rxCard);
//
////static uint8_t i2cTick = 0;
////static uint8_t txClks = 0;
////static const uint8_t I2C_POLL_TICKS = 3;
////i2cTick += ticksPassed;
////txClks += clksPassed;
////if (i2cTick>=I2C_POLL_TICKS)
////{
////i2cTick -= I2C_POLL_TICKS;
////hardware_.pollI2cCards(txClks);
////txClks = 0;
////}
//
////SEQ OVERVIEW TICK - keep it slow!
//static uint8_t seqOvTick = 0;
//static const uint8_t SEQ_OV_TICKS = 40;
//seqOvTick += ticksPassed;
//if (seqOvTick>=SEQ_OV_TICKS)
//{
//seqOvTick -= SEQ_OV_TICKS;
////sequencer_.pollOv();
//}
//
//arMsgBox_.refresh(ticksPassed);
//
////HARDWARE
//hardware_.refreshAll(ticksPassed);
//
//
//
//}
//#endif
//
//void AtxC::initComplete()
//{
//for (uint8_t i=0;i<atx::MAX_CARDS;++i)
//{
//if (card_[i].connected)
//{
//editor_.setEditCard(i);
//break;
//}
//}
//setGlobalBank(GB_EDIT);
//}
//
////void AtxC::pollSequencer()
////{
////sequencer_.pollTick();
////}
//
//void AtxC::setGlobalBank(GlobalBank newBank)
//{
//if (globalBank_==GB_SETTINGS && newBank!=GB_SETTINGS)
//{
////sdCard_.saveSettings(settings_.getCcValuePtr());
//}
//globalBank_ = newBank;
////clear all hardware
//for (uint8_t i=0;i<=AtxCHardware::RG_BANK;++i)
//{
//if (i<AtxCHardware::RG_BANK)
//{
//hardware_.getLedCircular(i).flashOffAll();
//hardware_.getLedCircular(i).setState(0);
//hardware_.getLedCircular(i).setInvertState(0);
//hardware_.getRotaryEncoderPtr(i)->setValue(0);
//hardware_.getRotaryEncoderPtr(i)->setMaxValue(16);
//}
//hardware_.getLedSwitch(i).flashStop();
//hardware_.getLedSwitch(i).setColour(LedRgb::OFF);
//}
//for (uint8_t i=0;i<=AtxCHardware::OLED_FUNC;++i)  //needed?
//{
//hardware_.getOledPtr(i)->clearDisplay();
//}
//hardware_.setOledOnMap(0xFF);
//hardware_.clearCtrlLog();
//arMsgBox_.setTask(ARMsgBox::AR_IDLE);
////now set hardware
//hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,0,S_GLOBAL_BANK_NAME[(uint8_t)newBank],false,false);
//switch(globalBank_)
//{
//case GB_EDIT:
//hardware_.getLedSwitch(AtxCHardware::RG_GLOBAL).setColour(LedRgb::RED);
//editor_.setEditCard(editor_.getEditCard());
//hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->setValue(editor_.getEditCard());
//hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->setMaxValue(atx::MAX_CARDS);
//break;
//case GB_SEQUENCER:
//hardware_.getLedSwitch(AtxCHardware::RG_GLOBAL).setColour(LedRgb::GREEN);
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->setValue(sequencer_.getEditZone());
//hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->setMaxValue(atx::MAX_ZONES);
//hardware_.getRotaryEncoderPtr(AtxCHardware::RG_FUNC)->setMaxValue(AtxCSequencer::S_MAX);
////sequencer_.setEditZone(sequencer_.getEditZone());
////sequencer_.setSeqMode(sequencer_.getSeqMode());
////sequencerPlayChanged(sequencer_.getPlayMode());
//break;
//case GB_SETTINGS:
//hardware_.getLedSwitch(AtxCHardware::RG_GLOBAL).setColour(LedRgb::YELLOW);
////settings_.setEditSetting(settings_.getEditSetting());
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->setValue(settings_.getEditSetting());
//hardware_.getRotaryEncoderPtr(AtxCHardware::RG_GLOBAL)->setMaxValue(AtxCSettings::SETTINGS);
//break;
//}
//}
//
//void AtxC::resetZoneLayoutSettings()
//{
//using namespace atx;
////for (uint8_t i=0;i<MAX_ZONES;++i)
////{
////settings_.setCcValue(SET_FW_Z1+i,zone_[i]->getFirmware());
////settings_.setCcValue(SET_ZONEENABLE_Z1+i,zone_[i]->getEnabled());
////settings_.setCcValue(SET_TRIGGERZONE_Z1+i,zone_[i]->triggerZone);
////settings_.setCcValue(SET_MASTER_Z1+i,zone_[i]->getMasterSynth());
////settings_.setCcValue(SET_CARDS_Z1+i,zone_[i]->getCards());
////settings_.setCcValue(SET_LOWERNOTE_Z1+i,zone_[i]->lowerNote);
////settings_.setCcValue(SET_UPPERNOTE_Z1+i,zone_[i]->upperNote);
////}
//}
//
//void AtxC::refreshOledValue(uint8_t oled, uint8_t card, uint8_t param)
//{
//using namespace atx;
//if (!(globalBank_==GB_EDIT || (globalBank_==GB_SETTINGS && card==SETTINGS_DUMMYCARD))) return;
//uint8_t row;
//uint8_t fmt, bits, tag, xtra;
//uint16_t value;
//bool clearThermom = true, clearInvert = true;
//Oled * oledPtr = hardware_.getOledPtr(oled);
//if (card==SETTINGS_DUMMYCARD)
//{
////row = (AtxCSettings::getCcInfoFuncNum(param)!=UNUSED ? 2 : 1); //COME BACK!
////fmt = AtxCSettings::getCcInfoFmtNum(param);
////xtra = AtxCSettings::getCcInfoFmtXtr(param);
////bits = AtxCSettings::getCcInfo(param).bits;
////tag = AtxCSettings::getCcInfo(param).tag;
////value = settings_.getCcValue(param);
//}
//else
//{
//row = (AtxCard::isFunctionCc(param)==true ? 2 : 1);
//fmt = card_[card].getCcInfoFmtNum(param);
//bits = card_[card].getCcInfoPtr(param)->bits;
//tag = card_[card].getCcInfoPtr(param)->tag;
//xtra = card_[card].getCcInfoFmtXtr(param);
//value = card_[card].getCcValue(param);
//}
//if (bits<15) //COME BACK - you need to make all wave sample params 16 bit!!!!
//{
//uint16_t mask = (1 << bits) - 1;
//value &= mask;  //safety
//}
//
//switch(fmt)
//{
//case AtxCard::FMT_INT:
//oledPtr->printIntBuffer(0,row,value,false,true,0);
//oledPtr->setInvertMapLevelRow(0,row,value,bits);
//oledPtr->setThermometerRow(row);
//oledPtr->setThermometerLine(false);
//clearThermom = false;
//break;
//case AtxCard::FMT_ONOFF:
//oledPtr->printStringBuffer(0,row,S_OFFON[value],false,true);
//break;
//case AtxCard::FMT_SIGNED:
//{
//uint16_t off = (1 << (bits - 1));
//int16_t o = (int16_t)value - off;
//if (o>0)
//{
//oledPtr->printCharBuffer(0,row,'+',false);
//oledPtr->printIntBuffer(1,row,o,false,true,0);
//}
//else
//{
//oledPtr->printIntBuffer(0,row,o,false,true,0);
//}
//oledPtr->setInvertMapLevelRow(0,row,value,bits);
//oledPtr->setThermometerRow(row);
//oledPtr->setThermometerLine(true);
//clearThermom = false;
//}
//break;
//case AtxCard::FMT_DRMPATTERN:
////wrap this is if drum firmware
//{
//hardware_.clearOledBuffer();
//uint8_t firstCc = (param >> 3) << 3;
//for (uint8_t i=0;i<8;++i)
//{
//for (uint8_t j=0;j<16;++j)
//{
//if (bitRead(card_[card].getCcValue(firstCc+i),j))
//{
//hardware_.drawOledPixelBuffer(j*3,i<<1,true,false);
//}
//}
//}
//hardware_.printOledBuffer(AtxCHardware::OLED_FUNC,row);
//}
//break;
//case AtxCard::FMT_SIGNEDFLOAT:
//{
//float halfscale = (float)(1 << (bits-1));
//float f = (((float)value - halfscale) * tag) / halfscale;
//if (f>0)
//{
//oledPtr->printCharBuffer(0,1,'+',false);
//oledPtr->printFloatBuffer(1,1,f,false,true,1);
//}
//else
//{
//oledPtr->printFloatBuffer(0,1,f,false,true,1);
//}
//oledPtr->setInvertMapLevelRow(0,row,value,bits);
//oledPtr->setThermometerRow(row);
//oledPtr->setThermometerLine(true);
//clearThermom = false;
//}
//break;
//case AtxCard::FMT_MIDINOTE:
//{
//char s[5];
//convMidiNoteString(value,s);
//oledPtr->printStringBuffer(0,row,s,true,true);
//}
//break;
//case AtxCard::FMT_DIV:
//oledPtr->printCharBuffer(0,row,value,true);
//oledPtr->printStringBuffer(1,row,"",false,true);
//break;
//case AtxCard::FMT_PPQN:
//oledPtr->printIntBuffer(0,row,I_PPQN[value],true,true,0);
//break;
//case AtxCard::FMT_INTPLAIN:
//oledPtr->printIntBuffer(0,row,value,true,true,0);  //regular int
//break;
//case AtxCard::FMT_2BIT:
//oledPtr->clearRow(row);
//oledPtr->printStringBuffer(0,row,S_OFFON[bitRead(value,0)],false,false);
//oledPtr->printStringBuffer(3,row,S_OFFON[bitRead(value,1)],false,false);
//break;
//case AtxCard::FMT_BINARY:
//{
//oledPtr->clearRow(row);
//for (uint8_t i=0;i<bits;++i)
//{
//uint8_t v = bitRead(value,i);
//oledPtr->printIntBuffer(i,row,v,false,true,0);
//}
//}
//break;
//case AtxCard::FMT_OFFSETINT:
//{
//uint16_t v = value + (uint16_t)tag;
//oledPtr->printIntBuffer(0,row,v,true,true,0);
//}
//break;
//case AtxCard::FMT_FIRMWARE:
//oledPtr->printStringBuffer(0,row,S_FIRMWARE[value],false,true);
//break;
//case AtxCard::FMT_PARAMLIST:
//{
//uint8_t fw;
//if (card_[card].firmware==AtxCard::FW_KEYBED)
//{
//fw = card_[editor_.getEditFunc()].firmware;
//}
//else
//{
//fw = card_[card].firmware;
//}
//printCcOneLine(oled,fw,value);
//}
//break;
//case AtxCard::FMT_TAG:
//{
//
//switch(tag)
//{
//case AtxCard::TAG_ATM_FILT:
//oledPtr->printStringBuffer(0,row,S_FILTERTYPE[value],false,true);
//break;
//case AtxCard::TAG_ATM_RES:
//oledPtr->printIntBuffer(0,row,I_RES[value],true,true,0);
//break;
//case AtxCard::TAG_ATM_SMPLEN:
//oledPtr->printIntBuffer(0,row,I_AUDIO_MIN_LENGTH[value],true,true,0);
//break;
//case AtxCard::TAG_ATM_QUANTSCALE:
//oledPtr->printStringBuffer(0,row,S_FMT_QUANT[value],false,true);
//break;
//case AtxCard::TAG_ODY_FILT:
//oledPtr->printStringBuffer(0,row,S_ODY_FILT[value],false,true);
//break;
//case AtxCard::TAG_SET_VOICEASSIGN:
//oledPtr->printStringBuffer(0,row,S_SET_VOICEASSIGN[value],false,true);
//break;
//}
//}
//break;
//}
//if (clearThermom)
//{
//oledPtr->setThermometerRow(atx::UNUSED);
//oledPtr->clearInvertMapRow(row);
//}
//row = 3;
//oledPtr = hardware_.getOledPtr(AtxCHardware::OLED_FUNC);
//switch(xtra)
//{
//case AtxCard::XTR_WAVE:  //CARD ONLY
//case AtxCard::XTR_ARP:
//{
//if (fmt!=0)  //must be the main func, not the req func
//{
//break;
//}
//
//static uint8_t iLast = 0;
//uint8_t v = value >> 8;  //value
//uint8_t i = value & 0xFF;   //index
//uint8_t inc = tag >> 4;  //plots 16 points, ie 16 increments //tag = max index (+1)
//if (!(i==0 || i==(iLast + inc)))
//{
//break;
//}
//iLast = i;
//
//uint8_t x = (uint16_t)i * 24 / tag;
//uint8_t y;
//if (xtra==AtxCard::XTR_ARP)
//{
//if ((int8_t)v<0)
//{
//y = -(int8_t)v - 1;
//}
//else
//{
//y = 16 - (int8_t)v;
//}
//}
//else
//{
//
//y = 15 - (uint8_t)(((int8_t)v + 128) / 16);
//}
//if (x==0)
//{
//hardware_.clearOledBuffer();
//hardware_.drawOledPixelBuffer(x<<1,y,true);
//}
//hardware_.drawOledLineBuffer(x<<1,y,true);
//
//i += inc;
//if (i<tag)  //tag is number of samples
//{
////COME BACK NRPN
////AtxMsg tx;
////tx.setHeader(AtxMsg::AMH_PARAM);
////tx.setCc(param);
////tx.setRequest(true);
////tx.setMsgByte(2,i);
////if (card_[card].getFirmwarePoly()>1 && card_[card].getFirstPolyCc()<UNUSED)
////{
////tx.setMsgByte(3,card_[editor_.getEditFunc()].zone);
////}
////else
////{
////tx.setMsgByte(3,card);
////}
////
////hardware_.bufferI2cAtxMsg(card,&tx);
//}
//else
//{
//hardware_.printOledBuffer(AtxCHardware::OLED_FUNC,row);
//}
//}
//break;
//case AtxCard::XTR_ENV:  //CARD ONLY
//{
//hardware_.clearOledBuffer();
//uint8_t inc;
//if (card_[card].getFirmwarePoly()>1 && card_[card].getFirstPolyCc()<UNUSED && param>=atx::MAX_SYNTHCARDS)
//{
//inc = 8;
//}
//else
//{
//inc = 1;
//}
//uint8_t x[5], y[5];
//x[0] = 0; y[0] = 15;
//x[1] = (card_[card].getCcValue(tag) * 12) >> 4;
//y[1] = 0;
//x[2] = x[1] + ((card_[card].getCcValue(tag+inc) * 12) >> 4);
//y[2] = 15 - card_[card].getCcValue(tag+(inc*2));
//x[3] = 47 - ((card_[card].getCcValue(tag+(inc*3)) * 12) >> 4);
//y[3] = y[2];
//x[4] = 47;
//y[4] = 15;
//bool e,i;
//e = (bool)bitRead(card_[card].getCcValue(tag+(inc*4)),0);
//i = (bool)bitRead(card_[card].getCcValue(tag+(inc*4)),1);
//if (i)
//{
//for (uint8_t i=0;i<5;++i)
//{
//y[i] = 15 - y[i];
//}
//}
//hardware_.drawOledPixelBuffer(x[0],y[0],true,e);
//for (uint8_t i=0;i<5;++i)
//{
//hardware_.drawOledLineBuffer(x[i],y[i],true,e);
//}
//hardware_.printOledBuffer(AtxCHardware::OLED_FUNC,row);
//}
//break;
//case AtxCard::XTR_FILTER:  //ATM ONLY
//{
//uint8_t x = 0;
//
//hardware_.clearOledBuffer();
//hardware_.drawOledPixelBuffer(x,I_ATMFILT_COORD[card_[card].getCcValue(ATM_FILTTYPE)][0],false);
//for (uint16_t i=1;i<32;++i)
//{
//x = (i * 48) >> 5;
//hardware_.drawOledLineBuffer(x,I_ATMFILT_COORD[card_[card].getCcValue(ATM_FILTTYPE)][i],false);
//}
//hardware_.printOledBuffer(AtxCHardware::OLED_FUNC,row);
//}
//break;
//case AtxCard::XTR_SEQPOS:  //this is for drum sequences...  could it also do seq sequences?
////choose tag = unused val of tag.  set tag of req function and display fucntions both to value
//if (card_[card].getCcInfoPtr(editor_.getCcFromFunc())->tag==tag)
//{
//value++;
////uint8_t inv = (value * 6) >> 4;
////for (uint8_t i=0;i<inv;++i)
////{
////oledPtr->setInvertMap(i,row,true);
////}
////for (uint8_t i=inv;i<6;++i)
////{
////oledPtr->setInvertMap(i,row,false);
////}
//oledPtr->setInvertMapLevelRow(0,row,value,4);
//oledPtr->printIntBuffer(0,row,value,false,true,1);
//clearInvert = false;
//}
//break;
//case AtxCard::XTR_ZONELAYOUT:
//{
////oledPtr->clearRow(3);
//for(uint8_t i=0;i<MAX_SYNTHCARDS;++i)
//{
//if (card_[i].getConnectedOrExternal())
//{
//oledPtr->printDigit6x8(i,6,i+1);
//if (oledZoneLayout_[i]==UNUSED)
//{
//oledPtr->printDigit6x8(i,7,Oled::FONT_6x8_CLEAR);
//}
//else
//{
//oledPtr->printDigit6x8(i,7,oledZoneLayout_[i]+11);
//}
//}
//}
//}
//break;
//case AtxCard::XTR_ODYICON:
//{
//uint8_t icon;
//if (tag<0x80)
//{
//icon = tag + value;
//}
//else
//{
//uint8_t mastCc = tag & 0x7F;
//icon = card_[card].getCcInfoPtr(mastCc)->tag + card_[card].getCcValue(mastCc);
//}
//hardware_.clearOledBuffer();
//bool buff = true;
//switch (icon)
//{
//case AtxCard::ODI_ADSR1:
//case AtxCard::ODI_ADSR2:
//case AtxCard::ODI_ADSR3:
//hardware_.drawOledPixelBuffer(0,15,false,false);
//hardware_.drawOledLineBuffer(8,15,false,false);
//hardware_.drawOledLineBuffer(16,0,false,false);
//hardware_.drawOledLineBuffer(24,8,false,false);
//hardware_.drawOledLineBuffer(32,8,false,false);
//hardware_.drawOledLineBuffer(40,15,false,false);
//hardware_.drawOledLineBuffer(47,15,false,false);
//break;
//case AtxCard::ODI_AR:
//hardware_.drawOledPixelBuffer(0,15,false,false);
//hardware_.drawOledLineBuffer(8,15,false,false);
//hardware_.drawOledLineBuffer(16,0,false,false);
//hardware_.drawOledLineBuffer(32,0,false,false);
//hardware_.drawOledLineBuffer(40,15,false,false);
//hardware_.drawOledLineBuffer(47,15,false,false);
//break;
//case AtxCard::ODI_NOISE:
//oledPtr->printStringBuffer(0,row,"Noise",false,true);
//buff = false;
//break;
//case AtxCard::ODI_RMOD:
//oledPtr->printStringBuffer(0,row,"R Mod",false,true);
//buff = false;
//break;
//case AtxCard::ODI_SAW:
//hardware_.drawOledPixelBuffer(0,15,false,false);
//hardware_.drawOledLineBuffer(16,0,false,false);
//hardware_.drawOledLineBuffer(16,15,false,false);
//hardware_.drawOledLineBuffer(32,0,false,false);
//hardware_.drawOledLineBuffer(32,15,false,false);
//hardware_.drawOledLineBuffer(47,0,false,false);
//hardware_.drawOledLineBuffer(47,15,false,false);
//break;
//case AtxCard::ODI_SH1:
//case AtxCard::ODI_SH2:
//oledPtr->printStringBuffer(0,row," S/H",false,true);
//buff = false;
//break;
//case AtxCard::ODI_SIN1:
//case AtxCard::ODI_SIN2:
//case AtxCard::ODI_SIN3:
//hardware_.drawOledPixelBuffer(0,I_ODY_SINE[0],false,false);
//for (uint8_t i=1;i<48;++i)
//{
//hardware_.drawOledLineBuffer(i,(15-I_ODY_SINE[(i%24)]),false,false);
//}
//break;
//case AtxCard::ODI_SQ1:
//case AtxCard::ODI_SQ2:
//hardware_.drawOledPixelBuffer(0,15,false,false);
//hardware_.drawOledLineBuffer(6,15,false,false);
//hardware_.drawOledLineBuffer(6,0,false,false);
//hardware_.drawOledLineBuffer(18,0,false,false);
//hardware_.drawOledLineBuffer(18,15,false,false);
//hardware_.drawOledLineBuffer(30,15,false,false);
//hardware_.drawOledLineBuffer(30,0,false,false);
//hardware_.drawOledLineBuffer(42,0,false,false);
//hardware_.drawOledLineBuffer(42,15,false,false);
//hardware_.drawOledLineBuffer(47,15,false,false);
//break;
//}
//if (buff)
//{
//hardware_.printOledBuffer(AtxCHardware::OLED_FUNC,row);
//}
//}
//break;
//default:
//if (oled==AtxCHardware::OLED_FUNC && sdCard_.isIdle())  //don't wipe bottom row of file name!
//{
//oledPtr->clearRow(row);
//}
//break;
//}
//if (clearInvert)
//{
//oledPtr->clearInvertMapRow(row);
//}
//
//}
//
//void AtxC::triggerZoneNote(uint8_t triggerZone, uint8_t midiNote, uint8_t midiVel, bool way)
//{
//using namespace atx;
//
//if (!sdCard_.isIdle() && way==true)
//{
//sdCard_.midiUI(midiNote);
//return;
//}
//if (way)
//{
//for (uint8_t i=0;i<MAX_ZONES;++i)
//{
//if (zone_[i]->getEnabled()==true && zone_[i]->triggerZone==triggerZone)
//{
//zone_[i]->triggerNote(midiNote,midiVel);
//}
//}
//
//}
//else
//{
//for (uint8_t i=0;i<MAX_ZONES;++i)
//{
//if (zone_[i]->getEnabled()==true && zone_[i]->triggerZone==triggerZone)
//{
//zone_[i]->releaseNote(midiNote,midiVel);
//}
//}
//
//}
////Midioutput loopthrough **COME BACK**
////MidiMsg m;
////m.setStatus(MidiMsg::MCMD_NOTEON);
////m.setChannel(zone);
////m.setData1(msg->getMidiData(0));
////m.setData2(msg->getMidiData(1));
////hardware_.txSerialMidi(&m);
//}
//
//void AtxC::printCcOneLine(uint8_t oled, uint8_t firmware, uint8_t param)
//{
//const char * s = AtxCard::getCcInfoPtr(firmware,param)->label1;
//uint8_t len = strlen(s);
//hardware_.getOledPtr(oled)->printStringBuffer(0,1,s,false,false);
//hardware_.getOledPtr(oled)->printCharBuffer(len,1,' ',false);
//hardware_.getOledPtr(oled)->printStringBuffer(len+1,1,AtxCard::getCcInfoPtr(firmware,param)->label2,false,true);
//}
//
//void AtxC::printTimestamp(uint8_t oled, uint16_t timeStamp)
//{
////hardware_.getOledPtr(oled)->printIntBuffer(0,1,sequencer_.calcBar(timeStamp),false,false,3);
////hardware_.getOledPtr(oled)->printCharBuffer(3,1,':',false);
////hardware_.getOledPtr(oled)->printIntBuffer(4,1,sequencer_.calcBeat(timeStamp),false,false,2);
////hardware_.getOledPtr(oled)->printCharBuffer(6,1,':',false);
////hardware_.getOledPtr(oled)->printIntBuffer(7,1,sequencer_.calcClk(timeStamp),false,true,2);
//}
//
//bool AtxC::processRxMsg(uint8_t card, MidiMsg * msg)
//{
////THIS IS SPECIAL CASES FOR RX MSGS WITH NO CTRL
//using namespace atx;
//
//switch(card_[card].hardware)
//{
//case AtxCard::HW_KEYBED:
//{
//uint8_t zone = card_[card].getCcValue(KEY_ACTIVEZONE);
//uint8_t tz = zone_[zone]->triggerZone;
//switch(msg->getCommand())
//{
//case MidiMsg::MCMD_NOTEON:
//case MidiMsg::MCMD_NOTEOFF:
//if (sequencer_.getPlayMode()==AtxCSequencer::SPM_REC && tz==sequencer_.getRecZone())
//{
//sequencer_.appendSeqBuffer(msg);
//}
//triggerZoneNote(tz,msg->getData1(),msg->getData2(),((msg->getData2()>0) ? true : false));
//return true;
//break;
//case MidiMsg::MCMD_CC:
//switch(msg->getData1())
//{
//case KEY_JOYX:  //don't return true for these, coz still want to process them for oled
//case KEY_JOYUP:
//case KEY_JOYDOWN:
//case KEY_AFTERTOUCH:
//{
//uint8_t dparam;
//switch(msg->getData1())
//{
//case KEY_JOYX:
//dparam = card_[card].getCcValue(KEY_JOYXDEST_Z1+zone);
//break;
//case KEY_JOYUP:
//dparam = card_[card].getCcValue(KEY_JOYUDEST_Z1+zone);
//break;
//case KEY_JOYDOWN:
//dparam = card_[card].getCcValue(KEY_JOYDDEST_Z1+zone);
//break;
//case KEY_AFTERTOUCH:
//dparam = card_[card].getCcValue(KEY_AFTDEST_Z1+zone);
//break;
//}
//
//uint8_t dc = zone_[zone]->getMasterSynth();
//uint8_t dbits = card_[dc].getCcInfoPtr(dparam)->bits;
//uint8_t sbits = card_[card].getCcInfoPtr(msg->getData1())->bits;
//uint16_t svalue = msg->getData2();
//uint16_t dvalue;
//if (card_[dc].getCcInfoPtr(dparam)->bits==0)
//{
//break;
//}
//if (dbits>sbits)
//{
//dvalue = svalue << (dbits - sbits);
//}
//else
//{
//dvalue = svalue >> (sbits - dbits);
//}
//editor_.setCcVal(dc,dparam,dvalue);
//}
//break;
//}
//break;
//
//
//}
//}
//break;
//case AtxCard::HW_SYNTH:
//{
//uint8_t fw = card_[card].firmware;
//switch(fw)
//{
//case AtxCard::FW_ATMEGATRON:
//switch(msg->getData1())
//{
//case ATM_EXTCV:
////editor_.setCcVal(card,card_[card].getCcValue(ATM_CVDEST),((msg->getValue() * card_[card].getCcMaxValue(param)) >> 12));  //COME BACK!
//break;
//}
//break;
//}
//}
//break;
//}
//
//return false;
//}
//
//uint8_t AtxC::nextEditZone(bool way, uint8_t startZone)
//{
//using namespace atx;
//uint8_t newValue = startZone & 0x07; //safety
//for (uint8_t i=0;i<MAX_ZONES;++i)
//{
//if (zone_[newValue]->getEnabled())
//{
//return newValue;
//}
//if (way)
//{
//newValue++;
//}
//else
//{
//newValue--;
//}
//newValue &= 0x07;
//}
//return newValue;
//}
//
//void AtxC::convMidiNoteString(uint8_t note, char * str)
//{
//uint8_t n = note % 12;
//strncpy(&str[0],S_FMT_NOTES[n],2);
//int8_t oct = note / 12;
//oct--;
//snprintf(&str[2],3,"%d",oct);
//}
//void AtxC::refreshEditCard(uint8_t card)
//{
//using namespace atx;
//char label[3][7] = {{0}};
//
//if (globalBank_==GB_EDIT)
//{
//hardware_.getRotaryEncoderPtr(AtxCHardware::RG_FUNC)->setMaxValue(AtxCEditor::EDIT_FUNCS);
//hardware_.getRotaryEncoderPtr(AtxCHardware::RG_FUNC)->setValue(editor_.getEditFunc());
//}
//else
//{
//hardware_.getRotaryEncoderPtr(AtxCHardware::RG_FUNC)->setMaxValue(AtxCSequencer::S_MAX);
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_FUNC)->setValue((uint8_t)sequencer_.getSeqMode());
//}
//
//if(card_[card].connected == false)
//{
//strcpy(label[0],"Card  ");
//itoa(card,label[0] + 4,10);
//strcpy(label[1],"Discon");
//strcpy(label[2],"nected");
//}
//else
//{
//
//if (card<MAX_SYNTHCARDS)
//{
//hardware_.getLedCircular(AtxCHardware::RG_GLOBAL).setState(zone_[card_[card].zone]->getZoneSynth());
//strcpy(label[2],"Zone  ");
//label[2][5] = card_[card].zone + 49;
//}
//else
//{
//hardware_.getLedCircular(AtxCHardware::RG_GLOBAL).select(card);
//strcpy(label[2],"      ");
//}
//if(card_[card].hardware>=AtxCard::FW_MAX)
//{
//strcpy(label[0],"---");
//}
//else
//{
//strcpy(label[0],S_HARDWARE[card_[card].hardware]);
//}
//if(card_[card].hardware>=AtxCard::FW_MAX)
//{
//strcpy(label[1],"---");
//}
//else
//{
//strcpy(label[1],S_FIRMWARE[card_[card].firmware]);
//}
//
//}
//for (uint8_t i=0;i<3;++i)
//{
//hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,i+1,label[i],false,true);
//}
//
//}
//
void AtxC::printFreeMem()
{
	char top;
	int freeMem = &top - reinterpret_cast<char*>(sbrk(0));
	hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printIntBuffer(0,0,freeMem,false,false,6);
}
//
////*******************AR Msgbox events*********************
//void AtxC::arTaskChanged(uint8_t newTask)
//{
//if (newTask>ARMsgBox::AR_IDLE)
//{
//hardware_.getLedSwitch(AtxCHardware::RG_FUNC).setColour(LedRgb::GREEN);
//hardware_.getLedSwitch(AtxCHardware::RG_VAL).setColour(LedRgb::RED);
//hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearOverlay();
//}
//else
//{
//hardware_.getLedSwitch(AtxCHardware::RG_FUNC).setColour(LedRgb::OFF);
//hardware_.getLedSwitch(AtxCHardware::RG_VAL).setColour(LedRgb::OFF);
//}
//}
//
////*****************Hardware events***********************
//void AtxC::hardwareCtrlValueChanged(uint8_t ctrl, uint8_t newValue)
//{
//#ifdef TESTER
//hardware_.getOledPtr(ctrl)->printIntBuffer(0,0,newValue,false,true,3);
//return;
//#endif
//using namespace atx;
//if (locked_) return;
//
//int16_t valScaled;
//switch (globalBank_)
//{
//case GB_EDIT:
//editor_.setCcValFromCtrl(ctrl,newValue);
//break;
//case GB_SEQUENCER:
//{
////int16_t v;
////switch(sequencer_.getSeqMode())
////{
////default:
////{
////sequencer_.setEditSeqFromCtrl(ctrl,newValue);
////}
////break;
////}
//}
//break;
//case GB_SETTINGS:
////settings_.setCcValFromCtrl(ctrl,newValue);
//break;
////{
////uint8_t s = settings_.getSetting();
////switch(s)
////{
////case AtxCSettings::S_ZONES:
////switch(ctrl)
////{
////case AtxCSettings::PZ_MASTER_CARD:
////if (hardware_.getSynthsConnected()>0)
////{
////valScaled = map(newValue,0,256,0,MAX_SYNTHCARDS);
////while(hardware_.getCardConnected(valScaled)==false && valScaled<MAX_SYNTHCARDS)
////{
////valScaled++;
////}
////if (hardware_.getCardConnected(valScaled)==false)
////{
////return;
////}
////}
////
////break;
////case AtxCSettings::PZ_CARDS:
////valScaled = map(newValue,0,256,0,hardware_.getSynthsConnected()+1);
////break;
////case AtxCSettings::PZ_MASTER_PBEND:
////case AtxCSettings::PZ_PN_PBEND:
////valScaled = map(newValue,0,256,-96,97);
////break;
////case AtxCSettings::PZ_UPPER_NOTE:
////case AtxCSettings::PZ_LOWER_NOTE:
////valScaled = map(newValue,0,256,0,128);
////break;
////}
////break;
////case AtxCSettings::S_SEQUENCER:
////switch(ctrl)
////{
////case AtxCSettings::PS_BPM:
////valScaled = newValue + 30;
////break;
////}
////break;
////}
////settings_.setCceterValue(ctrl,valScaled);
////}
//}
//}
//
//void AtxC::hardwareMidiPacketRxd(MidiMsg * msg)
//{
//using namespace atx;
////forceNoteOff(msg);
//uint8_t cmd = msg->getStatus();
//hardware_.bufferI2cMidiMsg(msg);
//return;
//switch(cmd)
//{
//case MidiMsg::MCMD_NOTEON:
//case MidiMsg::MCMD_NOTEOFF:
////if (sequencer_.getPlayMode()==AtxCSequencer::SPM_REC && msg->getChannel()==sequencer_.getRecZone())
////{
////MidiMsg m;  //DOESN'T THIS GO OUT OF SCOPE??
////m.setCommand(MidiMsg::MCMD_NOTEON);
////m.setData1(msg->getData1());
////m.setData2(msg->getData2());
////sequencer_.appendSeqBuffer(&m);
////}
////triggerZoneNote(msg->getChannel(),msg->getData1(),msg->getData2(),((cmd==MidiMsg::MCMD_NOTEON) ? true : false));
//
////hardware_.getLedSwitch(AtxCHardware::RG_VAL).flash(1,FLASH_TICKS,FLASH_TICKS,LedRgb::GREEN,LedRgb::OFF,true);
//break;
//case MidiMsg::MCMD_START:
////sequencer_.setPlayMode(AtxCSequencer::SPM_PLAY);
//break;
//case MidiMsg::MCMD_STOP:
////sequencer_.setPlayMode(AtxCSequencer::SPM_STOP);
//break;
//case MidiMsg::MCMD_CLOCK:
////sequencer_.pollClk(1);
//break;
//}
//}
//
//void AtxC::hardwareRotaryEncoderValueChanged(uint8_t rot, int8_t newValue, bool direction)
//{
//using namespace atx;
//#ifdef TESTER
//hardware_.getLedCircular(rot).select(newValue&0x0F);
//return;
//#endif
//if (locked_)
//{
//return;
//}
//switch (rot)
//{
//case AtxCHardware::RG_GLOBAL:
//switch(globalBank_)
//{
//case GB_EDIT:
//{
//editor_.nextEditCard(direction);
//hardware_.getRotaryEncoderPtr(rot)->setValue(editor_.getEditCard());
//}
//break;
//case GB_SEQUENCER:
//newValue = nextEditZone(hardware_.getRotaryEncoderPtr(rot)->getDirection(),newValue);
//hardware_.getRotaryEncoderPtr(rot)->setValue(newValue);
////sequencer_.setEditZoneFromRot(newValue);
//break;
//case GB_SETTINGS:
////settings_.setEditSetting((AtxCSettings::Setting)newValue);
//break;
//}
//break;
//case AtxCHardware::RG_FUNC:
//switch(globalBank_)
//{
//case GB_EDIT:
//{
//editor_.nextEditFunc(direction);
//hardware_.getRotaryEncoderPtr(rot)->setValue(editor_.getEditFunc());
//}
//break;
//case GB_SEQUENCER:
////sequencer_.setSeqMode((AtxCSequencer::SeqMode)newValue);
//break;
//case GB_SETTINGS:
////settings_.setEditFunc(newValue);
//break;
//}
//break;
//case AtxCHardware::RG_VAL:
//if (!sdCard_.isIdle())
//{
//bool sw = hardware_.getSwitchPtr(AtxCHardware::RG_VAL)->isPressed();//getValue();
//if (hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->getDirection()==true)
//{
////hardware_.getLedSwitch(AtxCHardware::RG_VAL).flash(1,FLASH_TICKS,FLASH_TICKS,LedRgb::GREEN);
//sdCard_.incUI(sw);
//}
//else
//{
////hardware_.getLedSwitch(AtxCHardware::RG_VAL).flash(1,FLASH_TICKS,FLASH_TICKS,LedRgb::RED);
//sdCard_.decUI(sw);
//}
//}
//else
//{
//switch(globalBank_)
//{
//case GB_EDIT:
//if (hardware_.getSwitchPtr(AtxCHardware::RG_VAL)->isPressed()==true) //getValue()==HIGH)
//{
//uint8_t c = editor_.getEditCard();
//zone_[card_[c].zone]->releaseNote(testNote_,0);
//if (hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->getDirection()==true)
//{
//testNote_++;
//}
//else
//{
//testNote_--;
//}
//testNote_ &= 0x7F;
//zone_[card_[c].zone]->triggerNote(testNote_,127);
//}
////else if (editor_.isFuncDrumPattern())
////{
////editor_.setEditBit(newValue);
////}
//else
//{
//editor_.setCcValFromFunc(newValue);
//}
//
//break;
//case GB_SEQUENCER:
////switch(sequencer_.getSeqMode())
////{
////case AtxCSequencer::S_LENGTH:
////sequencer_.setSequenceBars(newValue);
////break;
////case AtxCSequencer::S_RECSEQ:
////sequencer_.setRecZone(newValue);
////break;
////}
//break;
//case GB_SETTINGS:
////settings_.setCcValFromFunc(newValue);
//break;
//}
//break;
//}
//
//
//
//
//}
//
//}
//
//void AtxC::hardwareSwitchClicked(uint8_t sw)
//{
//using namespace atx;
//#ifdef TESTER
//cancelFlash_[sw] = true;
//return;
//#endif
//if (locked_)
//{
//return;
//}
//switch(sw)
//{
//case AtxCHardware::RG_GLOBAL:
//{
//uint8_t b = globalBank_;
//b++;
//if (b>1)
//{
//b = 0;
//}
//setGlobalBank((GlobalBank)b);
//break;
//}
//case AtxCHardware::RG_FUNC:
//if (arMsgBox_.getTask()>ARMsgBox::AR_IDLE)
//{
//switch(arMsgBox_.getTask())
//{
//case ARMsgBox::AR_SET_ZONES:
////settingTxZoneLayout(false,true);
//break;
//}
//}
//else
//{
//switch(globalBank_)
//{
//case GB_EDIT:
//{
////editor_.nextEditBank();
//break;
//}
//case GB_SEQUENCER:
////switch(sequencer_.getSeqMode())
////{
////default:
////break;
////}
//break;
//case GB_SETTINGS:
//{
//break;
//}
//}
//
//}
//break;
//case AtxCHardware::RG_VAL:
//if(arMsgBox_.getTask()>ARMsgBox::AR_IDLE)
//{
//switch(arMsgBox_.getTask())
//{
//case ARMsgBox::AR_SET_ZONES:
//resetZoneLayoutSettings();
////settingTxZoneLayout(true,false);
//break;
//}
//}
//else if (!sdCard_.isIdle())
//{
//sdCard_.clickUI();
//}
//else
//{
//switch (globalBank_)
//{
//case GB_EDIT:
////if (editor_.isFuncDrumPattern())
////{
////editor_.toggleBitCcValFromFunc();
////}
//break;
//case GB_SEQUENCER:
////switch (sequencer_.getSeqMode())
////{
////default:
////if (sequencer_.isOverviewMode())
////{
////sequencer_.toggleCue();
////}
////break;
////}
//break;
//case GB_SETTINGS:
//break;
//}
//}
//break;
//case AtxCHardware::RG_BANK:
//switch(globalBank_)
//{
//case GB_EDIT:
//{
//editor_.nextEditBank();
//}
//break;
//case GB_SEQUENCER:
////if (sequencer_.getPlayMode()==AtxCSequencer::SPM_STOP)
////{
////if (sequencer_.getSeqMode()==AtxCSequencer::S_RECSEQ)
////{
////sequencer_.setPlayMode(AtxCSequencer::SPM_COUNTIN);
////}
////else
////{
////sequencer_.setPlayMode(AtxCSequencer::SPM_PLAY);
////}
////}
////else
////{
////if (sequencer_.getSeqMode()==AtxCSequencer::S_RECSEQ)
////{
////sequencer_.setPlayMode(AtxCSequencer::SPM_RECCUE);
////}
////else
////{
////sequencer_.setPlayMode(AtxCSequencer::SPM_STOP);
////}
////}
//}
//break;
//}
//}
//
//void AtxC::hardwareSwitchDoubleClicked(uint8_t sw)
//{
//if (locked_)
//{
//return;
//}
//using namespace atx;
//switch(sw)
//{
//case AtxCHardware::RG_GLOBAL:
//switch(globalBank_)toggleLoop
//{
//case GB_EDIT:
//{
//uint8_t c = editor_.getEditCard();
//if (c<MAX_SYNTHCARDS)
//{
//for (uint8_t i=MAX_SYNTHCARDS;i<MAX_CARDS;++i)
//{
//if (card_[i].connected)
//{
//editor_.setEditFunc(i,c);
//editor_.setCcVal(i,AtxCard::MESS_ACTZONE_PARAM,card_[c].zone);
//}
//}
//}
//}
//break;
//case GB_SEQUENCER:
//{
////uint8_t c = zone_[sequencer_.getEditZone()]->getMasterSynth();
////if (c<MAX_SYNTHCARDS)
////{
////for (uint8_t i=MAX_SYNTHCARDS;i<MAX_CARDS;++i)
////{
////if (card_[i].connected)
////{
////editor_.setEditFunc(i,c);
////editor_.setCcVal(i,AtxCard::MESS_ACTZONE_PARAM,card_[c].zone);
////}
////}
////}
//}
//break;
//}
//break;
//case AtxCHardware::RG_FUNC:
//if(arMsgBox_.getTask()>ARMsgBox::AR_IDLE)
//{
//
//}
//else
//{
//switch(globalBank_)
//{
//case GB_EDIT:
//{
//uint8_t c = editor_.getEditFunc();
//if (c<MAX_SYNTHCARDS)
//{
//for (uint8_t i=MAX_SYNTHCARDS;i<MAX_CARDS;++i)
//{
//if (card_[i].connected)
//{
//if (i!=c)
//{
//editor_.setEditFunc(i,c);
//}
//editor_.setCcVal(i,AtxCard::MESS_ACTZONE_PARAM,card_[c].zone);
//}
//}
//}
//}
//break;
//case GB_SEQUENCER:
////if(sequencer_.isOverviewMode())
////{
////}
//break;
//}
//}
//break;
//case AtxCHardware::RG_VAL:
//if(arMsgBox_.getTask()>ARMsgBox::AR_IDLE)
//{
//
//}
//else if (!sdCard_.isIdle())
//{
//}
//else
//{
//switch(globalBank_)
//{
//case GB_EDIT:
//break;
//case GB_SEQUENCER:
////if(sequencer_.isOverviewMode())
////{
////sequencer_.toggleLoop();
////}
//break;
//}
//}
//break;
//case AtxCHardware::RG_BANK:
//switch(globalBank_)
//{
//case GB_SEQUENCER:
//hardwareSwitchClicked(sw);
//break;
//}
//}
//}
//
//void AtxC::hardwareSwitchHeld(uint8_t sw)
//{
//using namespace atx;
//switch(sw)
//{
//case AtxCHardware::RG_GLOBAL:
//setGlobalBank(GB_SETTINGS);
//break;
//case AtxCHardware::RG_FUNC:
//if(arMsgBox_.getTask()>ARMsgBox::AR_IDLE)
//{
//
//}
//else
//{
//switch(globalBank_)
//{
//case GB_SEQUENCER:
//break;
//}
//}
//break;
//case AtxCHardware::RG_VAL:
//if(arMsgBox_.getTask()>ARMsgBox::AR_IDLE)
//{
//
//}
//else if (!sdCard_.isIdle())
//{
//sdCard_.terminate();
//}
//else
//{
//switch(globalBank_)
//{
//case GB_EDIT:
////if (editor_.isFuncDrumPattern())
////{
////editor_.setCcValFromFunc(0);
////hardware_.getLedSwitch(AtxCHardware::RG_VAL).flash(1,FLASH_TICKS,FLASH_TICKS,LedRgb::YELLOW);
////}
//break;
//}
//}
//break;
//case AtxCHardware::RG_BANK:
//switch (globalBank_)
//{
//case GB_EDIT:
//locked_ = !locked_;
//if (locked_)
//{
//for (uint8_t i=0;i<AtxCHardware::OLED_GLOBAL;++i)
//{
//hardware_.printOledLogo(i);
//}
//hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,0," THE  ",false,false);
//hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,1,"ATMUL-",false,false);
//hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,2,"TITRON",false,false);
//hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,3," AND  ",false,false);
//hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,0,"ATMEGA",false,false);
//hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,1,"TRONII",false,false);
//hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,2,"COMING",false,false);
//hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,3,"SOON! ",false,false);
//}
//else
//{
//setGlobalBank(globalBank_);
//}
//break;
//case GB_SETTINGS:
//sdCard_.setFunction(AtxCSdCard::SD_SAVESETS);
//sdCard_.deleteSettings();
//sdCard_.setFunction(AtxCSdCard::SD_IDLE);
//hardware_.getLedSwitch(AtxCHardware::RG_BANK).flash(4,FLASH_TICKS,FLASH_TICKS,LedRgb::RED);
//break;
//}
//break;
//}
//}
//
//void AtxC::hardwareSwitchChanged(uint8_t sw, bool way)
//{
//using namespace atx;
//switch(sw)
//{
//case AtxCHardware::RG_GLOBAL:
//break;
//case AtxCHardware::RG_FUNC:
//break;
//case AtxCHardware::RG_VAL:
//switch(globalBank_)
//{
//case GB_EDIT:
//{
////if (editor_.isFuncDrumPattern() || !sdCard_.isIdle()) break;
//uint8_t c;
//if (editor_.getEditCard()>=MAX_SYNTHCARDS)
//{
//c = editor_.getEditFunc();
//if (c>MAX_SYNTHCARDS) break;
//}
//else
//{
//c = editor_.getEditCard();
//}
//if (way)
//{
//zone_[card_[c].zone]->triggerNote(testNote_,127);
//hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setLockValue(true);
//}
//else
//{
//zone_[card_[c].zone]->releaseNote(testNote_,0);
//hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setLockValue(false);
//}
//}
//break;
//case GB_SEQUENCER:
//break;
//case GB_SETTINGS:
//break;
//}
//break;
//}
//}
//
//void AtxC::hardwareInitCard(uint8_t card)
//{
//uint8_t o,r;
//o = card>>1;
//r = card&0x01;
//if (card>=12)
//{
//r <<= 1;
//hardware_.getOledPtr(o)->printStringBuffer(3,r,S_HARDWARE[card_[card].hardware],false,true);
//r++;
//hardware_.getOledPtr(o)->printStringBuffer(0,r,S_FIRMWARE[card_[card].firmware],false,true);
//}
//else
//{
//hardware_.getOledPtr(o)->printStringBuffer(3,r,S_HARDWARE[card_[card].hardware],false,false);
//hardware_.getOledPtr(o)->printCharBuffer(7,r,':',false);
//hardware_.getOledPtr(o)->printStringBuffer(8,r,S_FIRMWARE[card_[card].firmware],false,false);
//}
//hardware_.getOledPtr(o)->refresh();
////card_[card].active = true;
//}
//
//void AtxC::hardwareI2cPacketRxd(uint8_t card, MidiMsg * msg)
//{
//using namespace atx;
//uint8_t hw = card_[card].hardware;
//uint8_t fw = card_[card].firmware;
//uint8_t changed = 0;  //1 = func, 2 = ctrl
//
//switch(msg->getCommand())
//{
//case MidiMsg::MCMD_CC:
//{
//bool processed = processRxMsg(card,msg);
//uint8_t p = msg->getData1();
//uint16_t v = msg->getData2();
//card_[card].setCcValue(p,v);
//
//if (processed==false && card==editor_.getEditCard())  //not special case and on edit card
//{
//uint8_t fp = card_[card].getFirstPolyCc();  //this works for all poly, coz mono fp = 255
//uint8_t z = 0;  //zone (or instrument for mono hardware)
//uint8_t e = 0;
//if (p>=fp)
//{
//z = p % MAX_SYNTHCARDS;  //zone (or instrument for mono hardware)
//if (card_[card].getHardwarePoly()>1)  //poly hardware card
//{
//e = card_[editor_.getEditFunc()].zone;
//}
//else  //mono hardware card, but poly firmware (e.g. drums)
//{
//e = editor_.getEditFunc();
//}
//}
//if (z==e)  //we're in same zone/instrument  (or 0==0 for mono)
//{
//uint8_t ep = editor_.getCcFromFunc();
//if (p==ep)  //it's a func
//{
//changed = 1;
//}
//else
//{
//for (uint8_t i=0;i<AtxCEditor::EDIT_CTRLS;++i)
//{
//if (p==editor_.getCcFromCtrl(i))
//{
//changed = 2;
//break;
//}
//}
//}
//}
//
//
//switch(changed)
//{
//case 1:
//editorEditFuncValueChanged(card,p);
//break;
//case 2:
//editorEditCtrlValueChanged(card,editor_.getCtrlFromCc(p),p);
//break;
//default:
//{
//AtxCard::DispExtra xtra = (AtxCard::DispExtra)card_[card].getCcInfoFmtXtr(p);
//if (sdCard_.isIdle() && xtra>AtxCard::XTR_NOTHING && (xtra!=AtxCard::XTR_WAVE || (xtra==AtxCard::XTR_WAVE && p==card_[card].getCcInfoPtr(editor_.getCcFromFunc())->tag)))
//{
//refreshOledValue(AtxCHardware::OLED_FUNC,card,p);
//}
//}
//break;
//}
//}
//}
//break;
//}
//}
//
//void AtxC::hardwareExtClk()
//{
////if (settings_.getCcValue(atx::SET_SEQ_EXTCLK))
////{
//clks_ += 4;  //96 ppqn used for refresh rate
////}
//}
//
//////********************* SD card events **************************
////
////void AtxC::sdPrintString(const char * string, const char icon /* = 0 */)
////{
////char name[2][7] = {0};
////
////if (icon)
////{
////strncpy(name[0],string,4);
////name[0][4] = '\0';
//////if (strlen(string)<=4)
//////{
////strncpy(name[1],string+4,6);
////name[1][6] = '\0';
//////}
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printCharBuffer(0,2,icon,true);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(2,2,name[0],false,true);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,3,name[1],false,true);
////}
////else
////{
////strncpy(name[0],string,6);
////name[0][6] = '\0';
//////if (strlen(string)<=6)
//////{
////strncpy(name[1],string+6,6);
////name[1][6] = '\0';
//////}
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,2,name[0],false,true);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,3,name[1],false,true);
////}
////}
////
////void AtxC::sdPrintFail(const char * msg)
////{
////hardware_.getOledPtr(AtxCHardware::RG_VAL)->printStringBuffer(0,2,"SDfail",false,true);
////hardware_.getOledPtr(AtxCHardware::RG_VAL)->printStringBuffer(0,3,msg,false,true);
////}
////
////void AtxC::sdHighlightChar(uint8_t charPos)
////{
////uint8_t c = 2, r = 2;
////for (uint8_t i=0;i<10;++i)
////{
////if (i==charPos)
////{
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->setInvertMapLevel(c,r,Oled::INVERT_CHAR);
////}
////else
////{
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->setInvertMapLevel(c,r,0);
////}
////if (c==5)
////{
////c = 0;
////r++;
////}
////else
////{
////c++;
////}
////}
////}
////
////void AtxC::sdFunctionChanged(uint8_t func)
////{
////if (func==AtxCSdCard::SD_IDLE)
////{
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setLockValue(false);
////}
////else
////{
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setLockValue(true);
////}
////}
////
////void AtxC::sdRefreshStatusBar(uint8_t barFill)
////{
////hardware_.getLedCircular(AtxCHardware::RG_VAL).fill(barFill>>4);
////}
////
////void AtxC::sdFunctionActivated(uint8_t sdFunction)
////{
////switch(sdFunction)
////{
////case AtxCSdCard::SD_LOADSETS:
////sdCard_.loadSettings();
////break;
////case AtxCSdCard::SD_LOADPATCH:
////sdCard_.loadPatch(editor_.getEditCard());
////break;
////case AtxCSdCard::SD_LOADMIDI:
////sdCard_.convertMidi();
////break;
////case AtxCSdCard::SD_LOADSEQ:
////sdCard_.initSeq(sequencer_.getEditZone(),sequencer_.getEditSeq(),sequencer_.getSequence());
////sequencer_.refreshDisplayElement(AtxCSequencer::RDE_NAME);
//////printFreeMem();
////break;
////case AtxCSdCard::SD_LOADFX:
////clkTimer_.enable(false);
////sdCard_.loadFx();
////sequencerBpmChanged(sequencer_.getBpm());
////break;
////case AtxCSdCard::SD_SAVEPATCH:
////sdCard_.savePatch(editor_.getEditCard());
////break;
////case AtxCSdCard::SD_SAVESETS:
//////sdCard_.saveSettings(settings_.getCcValuePtr());
////break;
////case AtxCSdCard::SD_MERGESEQ:
////sequencer_.mergeSeq();
////sdCard_.saveSeq(sequencer_.getBuffer());
////sequencer_.clearBuffer();
////sdCard_.initSeq(sequencer_.getEditZone(),sequencer_.getEditSeq(),sequencer_.getSequence());
////sequencer_.refreshDisplayElement(AtxCSequencer::RDE_NAME);
////break;
////case AtxCSdCard::SD_SAVESEQ:
////sdCard_.saveSeq(sequencer_.getBuffer());
////sequencer_.clearBuffer();
////sdCard_.initSeq(sequencer_.getEditZone(),sequencer_.getEditSeq(),sequencer_.getSequence());
////sequencer_.refreshDisplayElement(AtxCSequencer::RDE_NAME);
//////sdCard_.initSeq(sequencer_.getRecZone(),sequencer_.getRecSeq(),sequencer_.getSequence(sequencer_.getRecZone(),sequencer_.getRecSeq()));
//////sequencer_.refreshDisplayElement(sequencer_.getRecZone(),sequencer_.getRecSeq(),AtxCSequencer::RDE_NAME);
////break;
////case AtxCSdCard::SD_SAVEMIDI:
////break;
////}
////}
//
////************* Editor events **************************
//void AtxC::editorEditCtrlBankChanged(uint8_t card, uint8_t bank)
//{
//if(globalBank_!=GB_EDIT) return;
//hardware_.resetCtrlMoving();  //also triggered when edit card changed or global bank changed
//hardware_.getLedSwitch(AtxCHardware::RG_BANK).setColour((LedRgb::LedRgbColour)(bank+1));
//for (uint8_t i=0;i<AtxCEditor::EDIT_CTRLS;++i)
//{
//if (editor_.getCcFromCtrl(i) == atx::UNUSED)
//{
//hardware_.getOledPtr(i)->clearRow(0);
//}
//else
//{
//hardware_.getOledPtr(i)->printStringBuffer(0,0,card_[card].getCcInfoPtr(editor_.getCcFromCtrl(i))->label1,false,true);
////hardware_.setCtrlLog(i,card_[card].getCcInfoLogCtrl(editor_.getCcFromCtrl(i)));   //COME BACK AND DEAL WITH LOG CONTROLS
//}
//hardware_.getOledPtr(i)->clearRow(1);
//}
//}
//
//void AtxC::editorEditCtrlValueChanged(uint8_t card, uint8_t ctrl, uint8_t param)
//{
//if(globalBank_!=GB_EDIT) return;
//if (param==atx::UNUSED)
//{
//hardware_.getOledPtr(ctrl)->clearDisplay();
//return;
//}
//refreshOledValue(ctrl,card,param);
//}
//
//void AtxC::editorEditFuncBankChanged(uint8_t card, uint8_t bank)
//{
//if(globalBank_!=GB_EDIT) return;
//switch(bank)
//{
//case 0:
//hardware_.getLedSwitch(AtxCHardware::RG_FUNC).setColour(LedRgb::RED);
//break;
//case 1:
//hardware_.getLedSwitch(AtxCHardware::RG_FUNC).setColour(LedRgb::GREEN);
//break;
//case 2:
//hardware_.getLedSwitch(AtxCHardware::RG_FUNC).setColour(LedRgb::YELLOW);
//break;
//}
//}
//
//void AtxC::editorEditFuncChanged(uint8_t card, uint8_t func, uint8_t param)
//{
//if(globalBank_!=GB_EDIT) return;
//using namespace atx;
//AtxCard::DispExtra dispExtra;
//hardware_.getLedCircular(AtxCHardware::RG_VAL).flashOffAll();
//hardware_.getLedCircular(AtxCHardware::RG_VAL).setInvertState(0);
//
//if (card_[card].getHardwarePoly()>1)
//{
//char label[7] = {0};
//if (func<MAX_SYNTHCARDS)
//{
//hardware_.getLedCircular(AtxCHardware::RG_FUNC).setState(zone_[card_[func].zone]->getZoneSynth());
//strcpy(label,"Zone  ");
//label[5] = card_[func].zone + 49;
//}
//else
//{
//hardware_.getLedCircular(AtxCHardware::RG_FUNC).select(func);
//strcpy(label,"Global");
//}
//
//hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,3,label,false,true);
//}
//else
//{
//hardware_.getLedCircular(AtxCHardware::RG_FUNC).select(func);
//}
//
//if (param==UNUSED)
//{
//hardware_.getLedCircular(AtxCHardware::RG_VAL).setState(0);
//hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setMaxValue(1);
//hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setValue(0);
//hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearDisplay();
//sdCard_.setFunction(AtxCSdCard::SD_IDLE);
//return;
//}
//
//hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,0,card_[card].getCcInfoPtr(param)->label1,false,true);
//hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,1,card_[card].getCcInfoPtr(param)->label2,false,true);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearRow(2);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearRow(3);
//
////if (editor_.isFuncDrumPattern())
////{
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setMaxValue(16);
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setValue(editor_.getEditBit());
////}
////else
////{
//hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setMaxValue(card_[card].getCcMaxValue(param));
//hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setValue(card_[card].getCcValue(param));
////	}
//
//dispExtra = (AtxCard::DispExtra)card_[card].getCcInfoFmtXtr(param);
//switch(dispExtra)
//{
//////REDO AS NRPN
////case AtxCard::XTR_WAVE:  //force through refresh, this is normally done when param set
////case AtxCard::XTR_ARP:
////{
////MidiMsg tx;
////tx.setStatus(MidiMsg::MCMD_CC);
////tx.setData1(card_[card].getCcInfoPtr(param)->tag);  //the tag contains the func to get the samps
////tx.setData2(2,0);
////if (card_[card].getFirmwarePoly()>1 && param>=card_[card].getFirstPolyCc())
////{
////tx.setMsgByte(3,card_[func].zone);
////}
////else
////{
////tx.setMsgByte(3,card);
////}
////
////tx.setRequest(true);
////hardware_.bufferI2cAtxMsg(editor_.getEditCard(),&tx);
////}
////break;
//}
//
//if (dispExtra==AtxCard::XTR_FILE)
//{
//sdCard_.setFileExtFilt(card_[card].getFiremwareNamePtr());
//sdCard_.setFunction((AtxCSdCard::SdTask)card_[card].getCcInfoPtr(param)->tag);  //tag contains file mode
//}
////else if(!sdCard_.isIdle())
////{
////sdCard_.setFunction(AtxCSdCard::SD_IDLE);
////}
//}
//
//void AtxC::editorEditFuncValueChanged(uint8_t card, uint8_t param)
//{
//if(globalBank_!=GB_EDIT) return;
////if (editor_.isFuncDrumPattern())
////{
////uint16_t state = card_[card].getCcValue(param);
////hardware_.getLedCircular(AtxCHardware::RG_VAL).setState(state);
////uint8_t b = editor_.getEditBit();
////hardware_.getLedCircular(AtxCHardware::RG_VAL).setSegmentFlashIndex(b,FLASH_DRUM);
////hardware_.getLedCircular(AtxCHardware::RG_VAL).setInvert(b,bitRead(state,b));
////}
////else
////{
//hardware_.getLedCircular(AtxCHardware::RG_VAL).select((card_[card].getCcValue(param) & 0x0F));
////	}
//refreshOledValue(AtxCHardware::OLED_FUNC,card,param);
//}
//
//void AtxC::editorEditBitChanged(uint8_t b)
//{
//if(globalBank_!=GB_EDIT) return;
//uint16_t state = card_[editor_.getEditCard()].getCcValue(editor_.getCcFromFunc());
//for (uint8_t i=0;i<16;++i)
//{
//if (i==b)
//{
//hardware_.getLedCircular(AtxCHardware::RG_VAL).setSegmentFlashIndex(i,FLASH_DRUM);
//hardware_.getLedCircular(AtxCHardware::RG_VAL).setInvert(i,bitRead(state,b));
//}
//else
//{
//hardware_.getLedCircular(AtxCHardware::RG_VAL).setSegmentFlashIndex(i,LedCircular::FLASH_OFF);
//hardware_.getLedCircular(AtxCHardware::RG_VAL).setInvert(i,false);
//}
//}
//}
//
//void AtxC::editorCardCcChanged(uint8_t card, uint8_t param, uint16_t value)
//{
//MidiMsg m;
////safety
//uint32_t mask = (1UL << card_[card].getCcInfoPtr(param)->bits) - 1;
//value &= mask;
//m.setCommand(MidiMsg::MCMD_CC);
//m.setChannel(card);
//m.setData1(param);
//m.setData2(value);
//if (card>=atx::MAX_SYNTHCARDS)
//{
//hardware_.bufferI2cMidiMsg(&m);
//}
//else
//{
//uint8_t z = card_[card].zone;
//uint8_t c = zone_[card_[card].zone]->getMasterSynth();
//uint8_t cs = zone_[card_[card].zone]->getCards();
//for (uint8_t i=0;i<=cs;++i)
//{
//card_[c].setCcValue(param,value);
//hardware_.bufferI2cMidiMsg(&m);
//c++;
//}
////if (sequencer_.getPlayMode()==AtxCSequencer::SPM_REC && z==sequencer_.getRecZone())
////{
////MidiMsg sm = m;
////sequencer_.appendSeqBuffer(&sm);
////}
//}
//}
//
//////************* Global Settings events*******************
////
////void AtxC::settingEditSettingChanged(uint8_t setting)
////{
////if(globalBank_!=GB_SETTINGS) return;
////for (uint8_t i=0;i<2;++i)
////{
////hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,i+1,AtxCSettings::getSettingName(i,(AtxCSettings::Setting)setting),false,true);
////}
////hardware_.getLedCircular(AtxCHardware::RG_GLOBAL).select(setting);
////hardware_.resetCtrlMoving();
////arMsgBox_.setTask(ARMsgBox::AR_IDLE);
////for (uint8_t i=0;i<AtxCEditor::EDIT_CTRLS;++i)
////{
//////COME BACK
//////if (settings_.getCcFromCtrl(i) == atx::UNUSED)
//////{
//////hardware_.getOledPtr(i)->clearRow(0);
//////}
//////else
//////{
//////hardware_.getOledPtr(i)->printStringBuffer(0,0,AtxCSettings::getCcInfo(settings_.getCcFromCtrl(i)).label1,false,true);
//////}
////hardware_.getOledPtr(i)->clearRow(1);
////}
////hardware_.getLedSwitch(AtxCHardware::RG_FUNC).setColour(LedRgb::OFF);  //what the hell is this button gonna do?
////switch(setting)
////{
////case AtxCSettings::S_ZONES:
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_FUNC)->setMaxValue(atx::MAX_SYNTHCARDS);
////settingTxZoneLayout(true,false);
////break;
////default:
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_FUNC)->setMaxValue(16);
////hardware_.setOledOnMap(0xFF);
////break;
////}
////}
////
////void AtxC::settingEditFuncChanged(uint8_t func, uint8_t param)
////{
////if(globalBank_!=GB_SETTINGS) return;
////using namespace atx;
////AtxCard::DispExtra dispExtra;
////
////hardware_.getLedSwitch(AtxCHardware::RG_VAL).flashStop();
////
////if (settings_.isPerZoneSetting())
////{
////char label[7] = {0};
////if (func<MAX_ZONES)
////{
////strcpy(label,"Zone  ");
////label[5] = func + 49;
////}
////else
////{
////strcpy(label,"Global");
////}
////
////hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,3,label,false,true);
////}
////else if (settings_.isPerSynthSetting())
////{
////char label[7] = {0};
////if (func<MAX_SYNTHCARDS)
////{
////strcpy(label,"Synth  ");
////label[5] = func + 49;
////}
////hardware_.getOledPtr(AtxCHardware::OLED_GLOBAL)->printStringBuffer(0,3,label,false,true);
////}
////hardware_.getLedCircular(AtxCHardware::RG_FUNC).select(func);
////
////if (param==UNUSED)
////{
////hardware_.getLedCircular(AtxCHardware::RG_VAL).setState(0);
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setMaxValue(0);
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setValue(0);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearDisplay();
////sdCard_.setFunction(AtxCSdCard::SD_IDLE);
////return;
////}
////
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,0,AtxCSettings::getCcInfo(param).label1,false,true);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,1,AtxCSettings::getCcInfo(param).label2,false,true);
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setMaxValue(AtxCSettings::getCcMaxValue(param));
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setValue(settings_.getCcValue(param));
////
////dispExtra = (AtxCard::DispExtra)AtxCSettings::getCcInfoFmtXtr(param);
////
////if (dispExtra==AtxCard::XTR_FILE)
////{
////sdCard_.setFunction((AtxCSdCard::SdTask)AtxCSettings::getCcInfo(param).tag);  //tag contains file mode
////}
////else if(!sdCard_.isIdle())
////{
////sdCard_.setFunction(AtxCSdCard::SD_IDLE);
////}
////}
////
////void AtxC::settingEditFuncValueChanged(uint8_t param)
////{
////using namespace atx;
////if(globalBank_!=GB_SETTINGS) return;
////hardware_.getLedCircular(AtxCHardware::RG_VAL).select((settings_.getCcValue(param) & 0x0F));
////refreshOledValue(AtxCHardware::OLED_FUNC,SETTINGS_DUMMYCARD,param);
////}
////
////void AtxC::settingEditCtrlValueChanged(uint8_t ctrl, uint8_t param)
////{
////using namespace atx;
////if(globalBank_!=GB_SETTINGS) return;
////if (param==atx::UNUSED)
////{
////hardware_.setOledOnMap(0xFF);
////hardware_.getOledPtr(ctrl)->clearDisplay();
////return;
////}
////refreshOledValue(ctrl,SETTINGS_DUMMYCARD,param);
////if (((param>>3)<<3)==SET_ZONEENABLE_Z1)
////{
////if (settings_.getCcValue(param))
////{
////hardware_.setOledOnMap(0xFF);
////}
////else
////{
////hardware_.setOledOnMap(0b11000001);
////}
////}
////if (param==SET_HW_EXTENABLE)
////{
////if (settings_.getCcValue(param))
////{
////hardware_.setOledOnMap(0xFF);
////}
////else
////{
////hardware_.setOledOnMap(0b11110011);
////}
////}
////}
////
////
////void AtxC::settingCcChanged(uint8_t param, uint16_t value)
////{
////using namespace atx;
////switch (param)
////{
////case SET_HW_EXTENABLE:
////case SET_HW_EXTMASTER:
////case SET_HW_EXTCARDS:
////if (settings_.getCcValue(SET_HW_EXTENABLE))
////{
////uint8_t f = settings_.getCcValue(SET_HW_EXTMASTER);
////uint8_t l = settings_.getCcValue(SET_HW_EXTMASTER) + settings_.getCcValue(SET_HW_EXTCARDS);
////for (uint8_t i=0;i<atx::MAX_SYNTHCARDS;++i)
////{
////if (i>=f && i<=l)
////{
////card_[i].external = true;
////}
////else
////{
////card_[i].external = false;
////}
////}
////}
////else
////{
////for (uint8_t i=0;i<atx::MAX_SYNTHCARDS;++i)
////{
////card_[i].external = false;
////}
////}
//////settingTxZoneLayout(false);
////break;
////case SET_SEQ_BPM:
////sequencer_.setBpm(value + 30);
////break;
////case SET_HW_USBHOST:
////hardware_.setUsbMidiType((AtxCHardware::UsbMidiType)value);
////break;
////case SET_SEQ_EXTCLK:
////break;
////case SET_VOICEASSIGN_Z1 ... SET_VOICEASSIGN_Z8:
////zone_[param&atx::MAX_ZONES_MASK]->setVoiceAssignMode((AtxZone::VoiceAssignMode)value);
////break;
////}
////}
////
////bool AtxC::settingTxZoneLayout(bool testMode, bool initCards)
////{
////using namespace atx;
////bool match = true;
////bool doLeds = (testMode==false && globalBank_==GB_SETTINGS && settings_.getEditSetting()==AtxCSettings::S_ZONES);
////uint8_t cardZone[MAX_SYNTHCARDS];
////uint8_t cardSynth[MAX_SYNTHCARDS];
////uint8_t cardFw[MAX_SYNTHCARDS];
////uint8_t zoneMasterC[MAX_ZONES];
////uint8_t zoneCards[MAX_ZONES];
////uint8_t zoneFw[MAX_ZONES];
////uint8_t zoneLn[MAX_ZONES];
////uint8_t zoneUn[MAX_ZONES];
////uint8_t zoneTrigger[MAX_ZONES];
////uint8_t zoneMap[MAX_ZONES];
////
////if (doLeds)
////{
////hardware_.getLedSwitch(AtxCHardware::RG_VAL).flashStop();
////hardware_.getLedSwitch(AtxCHardware::RG_VAL).setColour(LedRgb::YELLOW);
////hardware_.refreshLeds();
////}
////
////for (uint8_t i=0;i<MAX_SYNTHCARDS;++i)
////{
////cardZone[i] = UNUSED;
////cardSynth[i] = UNUSED;
////cardFw[i] = card_[i].firmware;
////oledZoneLayout_[i] = UNUSED;
////}
////for (uint8_t i=0;i<MAX_ZONES;++i)
////{
////zoneMasterC[i] = zone_[i]->getMasterSynth();
////zoneCards[i] = zone_[i]->getCards();
////zoneFw[i] = zone_[i]->getFirmware();
////zoneLn[i] = zone_[i]->lowerNote;
////zoneUn[i] = zone_[i]->upperNote;
////zoneTrigger[i] = zone_[i]->triggerZone;
////zoneMap[i] = 0;
////}
////uint8_t z;  //assume the zone you're editing is most important to enable
////if (globalBank_==GB_SETTINGS && settings_.getEditSetting()==AtxCSettings::S_ZONES)
////{
////z = settings_.getEditFunc();
////}
////else
////{
////z = 0;
////}
////uint8_t c,s;
////for (uint8_t i=0;i<MAX_ZONES;++i)
////{
////if (settings_.getCcValue(SET_ZONEENABLE_Z1+z)>0)
////{
////c = settings_.getCcValue(SET_MASTER_Z1+z);
////s = 0;
////if (card_[c].getConnectedOrExternal() && cardZone[c]==UNUSED)
//////if (card_[c].connected && cardZone[c]==UNUSED)
////{
////zoneMasterC[z] = c;
////zoneCards[z] = 0;
////zoneFw[z] = settings_.getCcValue(SET_FW_Z1+z);
////zoneLn[z] = settings_.getCcValue(SET_LOWERNOTE_Z1+z);
////zoneUn[z] = settings_.getCcValue(SET_UPPERNOTE_Z1+z);
////zoneTrigger[z] = settings_.getCcValue(SET_TRIGGERZONE_Z1+z);
////bitSet(zoneMap[z],c);
////cardZone[c] = z;
////cardSynth[c] = s;
////cardFw[c] = zoneFw[z];
////oledZoneLayout_[c] = z;
////for (uint8_t j=0;j<settings_.getCcValue(SET_CARDS_Z1+z);++j)
////{
////c++;
////s++;
////if (c<MAX_SYNTHCARDS)
////{
////if (card_[c].getConnectedOrExternal() && cardZone[c]==UNUSED)
//////if (card_[c].connected && cardZone[c]==UNUSED)
////{
////zoneCards[z]++;
////bitSet(zoneMap[z],c);
////cardZone[c] = z;
////cardSynth[c] = s;
////cardFw[c] = zoneFw[z];
////oledZoneLayout_[c] = z;
////}
////else
////{
////break;
////}
////}
////}
////}
////}
////z++;
////z &= 0x07;
////}
////
////for (uint8_t i=0;i<MAX_ZONES;++i)
////{
////if (zone_[i]->getEnabled()!=settings_.getCcValue(SET_ZONEENABLE_Z1+i))
////{
////if (testMode)
////{
////match = false;
////break;
////}
////else
////{
////zone_[i]->setEnabled((bool)settings_.getCcValue(SET_ZONEENABLE_Z1+i));
////}
////}
////if (zone_[i]->getCards()!=zoneCards[i])
////{
////if (testMode)
////{
////match = false;
////break;
////}
////else
////{
////zone_[i]->setSynths(zoneCards[i]);
////}
////}
////if (settings_.getCcValue(SET_ZONEENABLE_Z1+i)>0)  //ie zone enabled
////{
////if (settings_.getCcValue(SET_CARDS_Z1+i)!=zoneCards[i])
////{
////if (testMode)
////{
////match = false;
////break;
////}
////else
////{
////settings_.setCcValue(SET_CARDS_Z1+i,zoneCards[i]);
////}
////}
////if (zone_[i]->getMasterSynth()!=zoneMasterC[i])
////{
////if (testMode)
////{
////match = false;
////break;
////}
////else
////{
////zone_[i]->setMasterSynth(zoneMasterC[i]);
////}
////}
////if (zone_[i]->getFirmware()!=zoneFw[i])
////{
////if (testMode)
////{
////match = false;
////break;
////}
////else
////{
////zone_[i]->setFirmware((AtxCard::Firmware)zoneFw[i]);
////}
////}
////if (zone_[i]->lowerNote!=zoneLn[i])
////{
////if (testMode)
////{
////match = false;
////break;
////}
////else
////{
////zone_[i]->lowerNote = zoneLn[i];
////}
////}
////if (zone_[i]->upperNote!=zoneUn[i])
////{
////if (testMode)
////{
////match = false;
////break;
////}
////else
////{
////zone_[i]->upperNote = zoneUn[i];
////}
////}
////if (zone_[i]->triggerZone!=zoneTrigger[i])
////{
////if (testMode)
////{
////match = false;
////break;
////}
////else
////{
////zone_[i]->triggerZone = zoneTrigger[i];
////}
////}
////}
////else if(settings_.getCcValue(SET_ZONEENABLE_Z1+i)!=0)
////{
////if (testMode)
////{
////match = false;
////break;
////}
////else
////{
////settings_.setCcValue(SET_ZONEENABLE_Z1+i,0);  //fix zone that should be off
////}
////}
////}
////for (uint8_t i=0;i<MAX_SYNTHCARDS;++i)
////{
////if (doLeds)
////{
////hardware_.getLedCircular(AtxCHardware::RG_GLOBAL).select(i);
////hardware_.refreshLed(AtxCHardware::RG_GLOBAL);  //show which card you're updating
////}
////if (card_[i].getConnectedOrExternal())
//////if (card_[i].connected)
////{
////if (card_[i].zone!=cardZone[i])
////{
////if (testMode)
////{
////match = false;
////break;
////}
////else
////{
////card_[i].zone = cardZone[i];
////}
////}
////if (cardZone[i]!=UNUSED)
////{
////if (card_[i].synth!=cardSynth[i])
////{
////if (testMode)
////{
////match = false;
////break;
////}
////else
////{
////card_[i].synth = cardSynth[i];
////}
////}
////if (card_[i].firmware!=cardFw[i])
////{
////if (testMode)
////{
////match = false;
////break;
////}
////else
////{
////if (cardFw[i]!=UNUSED && card_[i].connected)  //unnessary safety!
////{
////hardware_.txSerialHex(sdCard_,i,cardFw[i],initCards);
////}
////}
////}
////}
////}
////}
////if (!testMode)
////{
////clkTimer_.enable(false);
////for (uint8_t i=0;i<MAX_ZONES;++i)
////{
////for (uint8_t j=MAX_SYNTHCARDS;j<MAX_CARDS;++j)
////{
////if (card_[j].connected && card_[j].getFirmwarePoly()>1)
////{
////uint8_t param = card_[j].getFirstPolyCc()+i;
////editor_.setCcVal(j,param,zoneMap[i]);  //through editor to force through tx
////}
////}
////if (zone_[i]->getEnabled() && zone_[i]->getCards()>0)  //this is forcing all other voice cards in zone to have same params as master (e.g. what if card was part of ATM in one zone ,now still ATM but in another zone)
////{
////uint8_t m = zone_[i]->getMasterSynth();
////for (uint8_t j=0;j<AtxCard::MAX_PARAMS;++j)
////{
////editor_.setCcVal(m,j,card_[m].getCcValue(j));  //editorCardCcChanged txs to the other cards
////}
////}
////}
////sequencerBpmChanged(sequencer_.getBpm());
////}
////
////if (globalBank_==GB_SETTINGS && settings_.getEditSetting()==AtxCSettings::S_ZONES)
////{
////if (match)
////{
////arMsgBox_.setTask(ARMsgBox::AR_IDLE);
////}
////else if(hardware_.getLedSwitch(AtxCHardware::RG_VAL).getFlashing()==false)
////{
////arMsgBox_.setTask(ARMsgBox::AR_SET_ZONES,hardware_.getLedSwitch(AtxCHardware::RG_FUNC).getColour(),hardware_.getLedSwitch(AtxCHardware::RG_VAL).getColour());
////}
////if (doLeds)
////{
////hardware_.getLedCircular(AtxCHardware::RG_GLOBAL).select(AtxCSettings::S_ZONES);
////}
////}
////return match;
////}
//
//
////****************************Sequencer events
////void AtxC::sequencerModeChanged(uint8_t seqMode)
////{
////if(globalBank_!=GB_SEQUENCER) return;
////if (sequencer_.isOverviewMode()==false)
////{
////for (uint8_t i=0;i<6;++i)
////{
////hardware_.getOledPtr(i)->clearDisplay();
////}
////}
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearDisplay();
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setMaxValue(16);  //default
////
////switch((AtxCSequencer::SeqMode)seqMode)
////{
////case AtxCSequencer::S_OVERVIEW:
//////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearRow(2);  //EH?
////sdCard_.setFunction(AtxCSdCard::SD_IDLE);
////break;
////case AtxCSequencer::S_IMPORTMIDI:
////sdCard_.setFunction(AtxCSdCard::SD_LOADMIDI);
////break;
////case AtxCSequencer::S_LOADSEQ:
////sdCard_.setFunction(AtxCSdCard::SD_LOADSEQ);
////break;
////case AtxCSequencer::S_SAVESEQ:
////case AtxCSequencer::S_MERGESEQ:
////sdCard_.setFunction(AtxCSdCard::SD_SAVESEQ);
////break;
////case AtxCSequencer::S_RECSEQ:
////sdCard_.setFunction(AtxCSdCard::SD_IDLE);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,2,"Zone",false,true);
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setMaxValue(atx::MAX_ZONES);
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setValue(sequencer_.getRecZone());
////break;
////case AtxCSequencer::S_EDITSEQ:
////sdCard_.setFunction(AtxCSdCard::SD_IDLE);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,2,"Quant",false,true);
////hardware_.getRotaryEncoderPtr(AtxCHardware::RG_VAL)->setValue(sequencer_.getRecQuantise());
////break;
////default:
////sdCard_.setFunction(AtxCSdCard::SD_IDLE);
////break;
////}
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,0,S_SEQMODE_TOP[seqMode],false,true);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printStringBuffer(0,1,S_SEQMODE_BOT[seqMode],false,true);
////hardware_.getLedCircular(AtxCHardware::RG_FUNC).select(seqMode);
////}
////
////void AtxC::sequencerEventListChanged(uint16_t index, MidiSeqMsg * msg)
////{
////if(globalBank_!=GB_SEQUENCER) return;
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printIntBuffer(0,3,index,false,false,6);
////if (sequencer_.getSequenceCount()==0)
////{
////for (uint8_t i=0;i<AtxCHardware::CTRLS;++i)
////{
////hardware_.getOledPtr(1)->printStringBuffer(0,1,"Empty",false,true);
////}
////hardware_.getLedCircular(AtxCHardware::RG_VAL).setState(0);
////return;
////}
////hardware_.getLedCircular(AtxCHardware::RG_VAL).select((index << 4) / sequencer_.getSequenceCount());
//////printTimestamp(0,msg->getClkStamp());
////printTimestamp(0,msg->getDeltaTime());
////hardware_.getOledPtr(1)->printStringBuffer(0,1,S_ATXMSG_HDR[msg->getHeaderFromStatus()],false,true);
////uint8_t cl = 2;
////switch (msg->getStatus() & 0xF0)
////{
////case MidiMsg::MCMD_CC:
////printCcOneLine(2,zone_[sequencer_.getEditZone()]->getFirmware(),msg->getData1());
////hardware_.getOledPtr(3)->printIntBuffer(0,1,msg->getData2(),false,true,0);
////cl = 4;
////break;
////case MidiMsg::MCMD_NOTEON:
////hardware_.getOledPtr(2)->printIntBuffer(0,1,msg->getData1(),false,true,0);
////hardware_.getOledPtr(3)->printIntBuffer(0,1,msg->getData2(),false,true,0);
////cl = 4;
////break;
////case MidiMsg::MCMD_NOTEOFF:
////hardware_.getOledPtr(2)->printIntBuffer(0,1,msg->getData1(),false,true,0);
////hardware_.getOledPtr(3)->printIntBuffer(0,1,msg->getData2(),false,true,0);
////cl = 4;
////break;
////}
////for (uint8_t i=cl;i < AtxCHardware::CTRLS;++i)
////{
////hardware_.getOledPtr(i)->clearRow(1);
////}
////}
////
////void AtxC::sequencerBpmChanged(uint16_t bpm)
////{
////clkTimer_.enable(false);
//////24PPQN
//////frequency range = 12hz - 114Hz for 30 - 285bpm
//////freq = (1 / 60000 / bpm / 24 / 1000) = bpm / 2.5 = (bpm << 1) / 5
//////compare range = 4000000 - 421052  for 30 - 285bpm
//////uint32_t compare = ((uint32_t)F_CPU * 5) / ((uint32_t)bpm << 1);
////
//////96PPQN
//////frequency range = 48hz - 456 for 30 - 285bpm
//////freq = (1 / 60000 / bpm / 96 / 1000) = bpm * 1.6 = (bpm << 3) / 5
//////compare range = 1000000 - 105263  for 30 - 285bpm
////uint32_t compare = ((uint32_t)F_CPU * 5) / ((uint32_t)bpm << 3);
////
////tc_clock_prescaler prescaler;
////
//////this covers all bases at the moment. A lot of options could be removed.
////if (compare<65536)
////{
////prescaler = TC_CLOCK_PRESCALER_DIV1;
////}
////else if (compare<131072)
////{
////prescaler = TC_CLOCK_PRESCALER_DIV2;
////compare >>= 1;
////}
////else if (compare<262144)
////{
////prescaler = TC_CLOCK_PRESCALER_DIV4;
////compare >>= 2;
////}
////else if(compare<524288)
////{
////prescaler = TC_CLOCK_PRESCALER_DIV8;
////compare >>= 3;
////}
////else if (compare<1048576)
////{
////prescaler = TC_CLOCK_PRESCALER_DIV16;
////compare >>= 4;
////}
////else if (compare<4194304)
////{
////prescaler = TC_CLOCK_PRESCALER_DIV64;
////compare >>= 6;
////}
////else if (compare<16777216)
////{
////prescaler = TC_CLOCK_PRESCALER_DIV256;
////compare >>= 8;
////}
////
//////no other prescalers fall in range of acceptable bpm
////clkTimer_.configure(prescaler,TC_COUNTER_SIZE_16BIT,TC_WAVE_GENERATION_MATCH_FREQ);
////clkTimer_.setCompare(0, (uint16_t)compare);
////clkTimer_.setCallback(true, TC_CALLBACK_CC_CHANNEL0, AtxC::pollClkCallback);
////clkTimer_.enable(true);
////}
////
////void AtxC::sequencerCardTx(uint8_t card, MidiMsg * msg)
////{
////switch(msg->getCommand())
////{
////case MidiMsg::MCMD_CC:
////editor_.setCcVal(card,msg->getData1(),msg->getData2());
////break;
////default:
////hardware_.bufferI2cMidiMsg(msg);
////break;
////}
////}
////
////void AtxC::sequencerZoneTx(uint8_t zone, MidiMsg * msg)
////{
////switch(msg->getCommand())
////{
////case MidiMsg::MCMD_CC:
////editor_.setCcVal(zone_[zone]->getMasterSynth(),msg->getData1(),msg->getData2());
////break;
////case MidiMsg::MCMD_NOTEON:
////triggerZoneNote(zone,msg->getData1(),msg->getData2(),true);
////break;
////case MidiMsg::MCMD_NOTEOFF:
////triggerZoneNote(zone,msg->getData1(),msg->getData2(),false);
////break;
////}
////}
////
////void AtxC::sequencerPlayChanged(uint8_t playMode)
////{
////if (globalBank_==GB_SEQUENCER)
////{
////hardware_.getLedSwitch(AtxCHardware::RG_BANK).flashStop();
////switch(sequencer_.getPlayMode())
////{
////case AtxCSequencer::SPM_STOP:
////hardware_.getLedSwitch(AtxCHardware::RG_BANK).setColour(LedRgb::YELLOW);
////break;
////case AtxCSequencer::SPM_PLAY:
////hardware_.getLedSwitch(AtxCHardware::RG_BANK).setColour(LedRgb::GREEN);
////break;
////case AtxCSequencer::SPM_REC:
////hardware_.getLedSwitch(AtxCHardware::RG_BANK).setColour(LedRgb::RED);
////break;
////case AtxCSequencer::SPM_RECCUE:
////case AtxCSequencer::SPM_COUNTIN:
////hardware_.getLedSwitch(AtxCHardware::RG_BANK).flash(FLASH_FOREVER,FLASH_TICKS,FLASH_TICKS,LedRgb::RED);
////break;
////}
////}
////}
////
////void AtxC::sequencerOvSeqChanged(uint8_t zone, uint8_t seq, uint8_t display, uint8_t row)
////{
////if(globalBank_!=GB_SEQUENCER) return;
////hardware_.getOledPtr(display)->printIntBuffer(0,row,seq+1,false,false,false);
////}
////
////void AtxC::sequencerOvPlayChanged(uint8_t zone, uint8_t seq, bool selected, uint8_t playState, uint8_t display, uint8_t row)
////{
////if(globalBank_!=GB_SEQUENCER) return;
////hardware_.getOledPtr(display)->printCharBuffer(1,row,Oled::FONT_PS + (uint8_t)playState + (selected * Oled::FONT_PS_SEL_OFF),true);
////}
////
////void AtxC::sequencerOvSeqLoopChanged(uint8_t zone, uint8_t seq, bool looping, uint8_t display, uint8_t row)
////{
////if(globalBank_!=GB_SEQUENCER) return;
////char l;
////if (looping)
////{
////l = Oled::FONT_LOOP;
////}
////else
////{
////l = ' ';
////}
////hardware_.getOledPtr(display)->printCharBuffer(3,row,l,false);
////}
////
////void AtxC::sequencerOvSeqNameChanged(uint8_t zone, uint8_t seq, const char * name, uint8_t display, uint8_t row)
////{
////if(globalBank_!=GB_SEQUENCER) return;
////hardware_.getOledPtr(display)->printStringBuffer(4,row,name,false,true);
////}
////
////void AtxC::sequencerOvSeqPosChanged(uint8_t zone, uint8_t seq, uint8_t pos, uint8_t display, uint8_t row)
////{
////if(globalBank_!=GB_SEQUENCER) return;
//////for (uint8_t i=0;i<pos;++i)
//////{
//////hardware_.getOledPtr(display)->setInvertMap(i+4,row,true);
//////}
//////for (uint8_t i=pos;i<8;++i)
//////{
//////hardware_.getOledPtr(display)->setInvertMap(i+4,row,false);
//////}
////hardware_.getOledPtr(display)->setInvertMapLevelRow(4,row,pos,6);
////}
////
////void AtxC::sequencerOvClkChanged(uint16_t bar, uint8_t beat, uint8_t clk)
////{
////if(globalBank_!=GB_SEQUENCER) return;
////if (bar>999)
////{
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printIntBuffer(0,2,bar,false,false,6);
////}
////else
////{
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printIntBuffer(0,2,bar,true,false,3);
////}
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printCharBuffer(0,3,':',false);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printIntBuffer(1,3,beat,false,false,2);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printCharBuffer(3,3,':',false);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printIntBuffer(4,3,clk,false,false,2);
////}
////
////
////void AtxC::sequencerRecQuantiseChanged(uint8_t quantise)
////{
////if(globalBank_!=GB_SEQUENCER) return;
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearRow(3);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printCharBuffer(0,3,quantise,true);
////}
////
////void AtxC::sequencerRecZoneChanged(uint8_t zone)
////{
////if(globalBank_!=GB_SEQUENCER) return;
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->clearRow(3);
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->printIntBuffer(0,3,zone+1,true,true,0);
////}
////
////void AtxC::sequencerRecSeqPosChanged(uint8_t pos)
////{
////if(globalBank_!=GB_SEQUENCER) return;
////hardware_.getOledPtr(AtxCHardware::OLED_FUNC)->setInvertMapLevelRow(2,3,pos,6);
////}
//
////void AtxC::sequencerSeqWrite(uint8_t zone, uint8_t seq, AtxSeqMsgList& asml)
////{
////sdCard_.createSeq(zone,seq,asml);
////sdCard_.initSeq(zone,seq,sequencer_.getSequence(zone,seq));
////sequencer_.refreshDisplayElement(zone,seq,AtxCSequencer::RDE_NAME);
////}
//
////*************************ATX zone events
//void AtxC::zoneTriggerNote(uint8_t card, uint8_t voice, uint8_t midiNote, uint8_t midiVel, bool way)
//{
//MidiMsg m;
//m.setCommand(MidiMsg::MCMD_NOTEON);
//m.setChannel(card);
//m.setData1(midiNote);
//m.setData2((way ? midiVel : 0));
//hardware_.bufferI2cMidiMsg(&m);
//#ifdef DEBUGMIDI
//hardware_.getOledPtr(card)->printIntBuffer(0,1,(uint8_t)way,false,false,false);
//hardware_.getOledPtr(card)->printCharBuffer(1,1,':',false);
//hardware_.getOledPtr(card)->printIntBuffer(2,1,card,false,false,0);
//hardware_.getOledPtr(card)->printIntBuffer(3,1,voice,false,false,0);
//hardware_.getOledPtr(card)->printCharBuffer(4,1,':',false);
//hardware_.getOledPtr(card)->printIntBuffer(5,1,midiNote,false,false,3);
//hardware_.getOledPtr(card)->printCharBuffer(8,1,':',false);
//hardware_.getOledPtr(card)->printIntBuffer(9,1,midiVel,false,true,3);
//#endif
//}
//void AtxC::zoneAllNotesOff(uint8_t card)
//{
//MidiMsg m;
//m.setCommand(MidiMsg::MCMD_CC);
//m.setChannel(card);
//m.setData1(MidiMsg::MCC_NOTESOFF);
//hardware_.bufferI2cMidiMsg(&m);
//}