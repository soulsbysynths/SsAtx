/*
* AtxZone.cpp
*
* Created: 18/06/2018 15:28:55
* Author: info
*/


#include "AtxZone.h"

namespace atx
{
	// default constructor
	AtxZone::AtxZone(uint8_t index, AtxSynthCard* cardPtrs[], AtxZoneBase* base)
	{
		construct(index, cardPtrs, base);
	} //AtxZone

	// default destructor
	AtxZone::~AtxZone()
	{
		if (base_!=NULL)
		{
			delete base_;
		}
	} //~AtxZone

	void AtxZone::construct(uint8_t index, AtxSynthCard* sCardPtrs[], AtxZoneBase* base)
	{
		using namespace atx;
		index_ = index;
		base_ = base;
		//for (uint_fast8_t i=0;i<MIDI_NOTES;++i)
		//{
		//noteVoiceCardBuffer_[i] = 0xFFFF;  //unused in last and current
		//}
		//memset(noteVoiceCardBuffer_,UNUSED,sizeof(noteVoiceCardBuffer_));
		for (uint_fast8_t i=0;i<atx::MAX_SYNTHCARDS;++i)
		{
			sCard_[i] = sCardPtrs[i];
		}
		//this is to match the poly cards boot default
		enabled_ = true;
		masterSynth_ = index;
		synths_ = 0;
	}

	AtxZone::AtxZone(const AtxZone& c) : AtxCcSet(c)
	{
		using namespace atx;
		//masterChannel = wt.masterChannel;
		//perNotePitchbendRange = c.perNotePitchbendRange;
		//masterPitchbendRange = c.masterPitchbendRange;
		upperNote_ = c.upperNote_;
		lowerNote_ = c.lowerNote_;
		
		base_ = c.base_;
		for (uint_fast8_t i = 0;i<MAX_SYNTHCARDS;++i)
		{
			sCard_[i] = c.sCard_[i];
		}
		index_ = c.index_;
		synths_ = c.synths_;
		masterSynth_ = c.masterSynth_;
		voiceAssignMode_ = c.voiceAssignMode_;
		//notesOn_ = c.notesOn_;   //not the note stuff
	}

	AtxZone& AtxZone::operator = (const AtxZone& c)
	{
		using namespace atx;
		if (&c == this)
		{
			return *this;
		}

		//masterChannel = wt.masterChannel;
		//perNotePitchbendRange = c.perNotePitchbendRange;
		//masterPitchbendRange = c.masterPitchbendRange;
		upperNote_ = c.upperNote_;
		lowerNote_ = c.lowerNote_;
		
		base_ = c.base_;
		for (uint_fast8_t i = 0;i<MAX_SYNTHCARDS;++i)
		{
			sCard_[i] = c.sCard_[i];
		}
		index_ = c.index_;
		synths_ = c.synths_;
		masterSynth_ = c.masterSynth_;
		voiceAssignMode_ = c.voiceAssignMode_;
		AtxCcSet::operator=(c);
		//notesOn_ = c.notesOn_;   //not the note stuff
		return *this;
	}

	uint8_t AtxZone::getSynthMap()
	{
		uint8_t v = 0;
		if(enabled_==true)
		{
			for (uint_fast8_t i = masterSynth_;i<=(masterSynth_+synths_);++i)
			{
				v |= (1 << i);
			}
		}
		return v;
	}

	void AtxZone::setMasterSynth(uint8_t newMasterSynth)
	{
		masterSynth_ = newMasterSynth;
		curSynth_ = masterSynth_;
		refreshSynthCards();
		allNotesOff();
	}

	void AtxZone::setSynths(uint8_t newSynths)
	{
		synths_ = newSynths;
		memset(curVoice_,0,sizeof(curVoice_));
		totalVoices_ = ((uint16_t)synths_+1)*getFirmwareVoices();
		refreshSynthCards();
		allNotesOff();
	}

	void AtxZone::refreshSynthCards()
	{
		for(uint_fast8_t s=masterSynth_;s<=(masterSynth_+synths_);++s)
		{
			sCard_[s]->zone = index_;
			sCard_[s]->synth = (s-masterSynth_);
		}
	}
	void AtxZone::setEnabled(bool newEnabled)
	{
		enabled_ = newEnabled;
		allNotesOff();
	}

	void AtxZone::setFirmware(Firmware newFirmware)
	{
		AtxCcSet::setFirmware(newFirmware);
		totalVoices_ = ((uint16_t)synths_+1)*getFirmwareVoices();
		allNotesOff();
	}

	void AtxZone::setLowerNote(uint8_t newLowerNote)
	{
		lowerNote_ = newLowerNote;
		allNotesOff();
	}

	void AtxZone::setUpperNote(uint8_t newUpperNote)
	{
		upperNote_ = newUpperNote;
		allNotesOff();
	}

	void AtxZone::setVoiceAssignMode(VoiceAssignMode newMode)
	{
		voiceAssignMode_ = newMode;
		//allNotesOff();
	}

	//void AtxZone::getNextVoiceCard()
	//{
	//using namespace atx;
	//uint8_t curCard, curVoice;
	//uint8_t maxCard = masterSynth_+synths_-1;
	//uint8_t maxVoice = AtxCard::getFirmwarePoly(firmware)-1;
	//
	//curVoice = curVoiceCard_ >> 4;
	//curCard = curVoiceCard_ & 0x0F;
	//
	//for (uint_fast8_t v=0;v<AtxCard::getFirmwarePoly(firmware);++v)
	//{
	//
	//for (uint_fast8_t c=0;c<synths_;++c)
	//{
	//////first thing is to ALWAYS move on a card
	//curCard++;
	//if (curCard>maxCard || curCard<masterSynth_)
	//{
	//curCard = masterSynth_;
	//}
	//if (sCard_[curCard]->getNote(curVoice)==UNUSED)  //COME AND SORT OUT POLY FIRMWARE  ..pending  .. done???
	//{
	//curVoiceCard_ = (curVoice << 4) | curCard;
	//return;
	////return curVoiceCard_;
	//}
	//}
	//curVoice++;
	//if (curVoice>maxVoice)
	//{
	//curVoice = 0;
	//}
	//}
	//
	//////this was here and worked, but surely the for loop ends on last card and voice in buffer, so no need for increment
	////curCard++;
	////if (curCard>maxCard)
	////{
	////curCard = masterSynth_;
	////}
	//
	////force release note
	////uint8_t midiKey = sCard_[curCard]->getNote(curVoice);
	////incNoteVoiceCardBuffer(midiKey);
	////setNoteVoiceCard(midiKey,UNUSED);  //UNUSED in Atx is 0xFF, ie unused voice and unused card
	////sCard_[curCard]->setNote(curVoice,UNUSED);  //COME BACK, sort poly firmware  ..pending
	////if (base_!=NULL)
	////{
	////base_->zoneTriggerNote(curCard, curVoice, midiKey, 0, false);  //want an event here??? also, need vel?
	////}
	//
	////no spare voices available. Move on 1 card. Now up to voice firmware to deal with collision
	//curCard++;
	//if (curCard>maxCard || curCard<masterSynth_)
	//{
	//curCard = masterSynth_;
	//}
	//curVoiceCard_ = (curVoice << 4) | curCard;
	//}


	void AtxZone::triggerNote(midi::MidiMsg* msg)
	{
		using namespace atx;
		using namespace midi;
		uint8_t midiKey = msg->getData1();
		uint8_t midiVel = msg->getData2();
		bool retrigLfo = false;
		if (midiKey < lowerNote_ || midiKey > upperNote_ || base_==NULL || getNoteMapNotedown(midiKey)==true)
		{
			return;
		}
		if(notesDown_==0)
		{
			//to sync the lfos
			if(lfoTriggerMode_==LT_RETRIGGERALL)
			{
				for (uint8_t i=masterSynth_;i<=(masterSynth_+synths_);++i)
				{
					base_->zoneTxResetClks(i);
				}
			}
			else if(lfoTriggerMode_==LT_RETRIGGERCURRENT)
			{
				retrigLfo = true;
			}
		}
		notesDown_++;
		if (voiceAssignMode_==VA_BUFFERED && getNoteMapUsed(midiKey)==true && getSynthMapNotedown(getNoteMapSynth(midiKey),getNoteMapVoice(midiKey))==false)
		{
			uint8_t c = getNoteMapSynth(midiKey);
			uint8_t v = getNoteMapVoice(midiKey);
			setSynthMapNotedown(c,v,true);
			setNoteMapNote(midiKey,c,v,true);
			if(retrigLfo) base_->zoneTxResetClks(c);
			base_->zoneTxMidiMsg(c,msg,true);
		}
		else if(voiceAssignMode_==VA_UNISON)
		{
			for (uint8_t i=masterSynth_;i<=(masterSynth_+synths_);++i)
			{
				setSynthMapNotedown(i,curVoice_[i],true);
				setNoteMapNote(midiKey,i,curVoice_[i],true);
				if(retrigLfo) base_->zoneTxResetClks(i);
				base_->zoneTxMidiMsg(i,msg,true);
				if (i==masterSynth_)
				{
					for (uint8_t j=0;j<AtxCcSet::getFirmwareVoices();++j)
					{
						curVoice_[i]++;
						if (curVoice_[i]>=getFirmwareVoices())
						{
							curVoice_[i] = 0;
						}
						if (getSynthMapNotedown(curSynth_,curVoice_[i])==false)
						{
							break;
						}
					}
				}
				else
				{
					curVoice_[i] = curVoice_[masterSynth_];
				}

			}
		}
		else
		{
			setSynthMapNotedown(curSynth_,curVoice_[curSynth_],true);
			setNoteMapNote(midiKey,curSynth_,curVoice_[curSynth_],true);
			if(retrigLfo) base_->zoneTxResetClks(curSynth_);
			base_->zoneTxMidiMsg(curSynth_,msg,true);
			for (uint16_t i=0;i<totalVoices_;++i)
			{
				curSynth_++;
				if (curSynth_>(masterSynth_+synths_))
				{
					curSynth_ = masterSynth_;
					curVoice_[curSynth_]++;
					if (curVoice_[curSynth_]>=getFirmwareVoices())
					{
						curVoice_[curSynth_] = 0;
					}
				}
				if (getSynthMapNotedown(curSynth_,curVoice_[curSynth_])==false)
				{
					break;
				}
			}
		}

	}

	//void AtxZone::triggerNote(uint8_t midiKey, uint8_t midiVel)
	//{
	//using namespace atx;
	//if (midiKey < lowerNote || midiKey > upperNote)
	//{
	//return;
	//}
	//uint_fast8_t i = midiKey>>3;
	//
	//uint8_t vc = getNoteVoiceCard(midiKey);
	//if (vc!=UNUSED)
	//{
	//return; //note already down
	//}
	//uint8_t lastVc = getNoteLastVoiceCard(midiKey); //c last used for note
	//uint8_t lastVoice = lastVc >> 4;
	//uint8_t lastCard = lastVc & 0x0F;
	//if (lastVc==UNUSED)  //no record of V or C for last used for note
	//{
	//getNextVoiceCard();
	//vc = curVoiceCard_;
	//}
	//else if (sCard_[lastCard]->getNote(lastVoice)!=atx::UNUSED)  //c is not free
	//{
	//getNextVoiceCard();
	//vc = curVoiceCard_;
	//}
	//else
	//{
	//vc = lastVc;
	//}
	////safety
	//uint8_t v = vc >> 4;
	//uint8_t c = vc & 0x0F;
	//if (v >= AtxCard::getFirmwarePoly(firmware))
	//{
	//v = 0;
	//vc = (v << 4) | c;
	//}
	//if (c < masterSynth_ || c >= (masterSynth_ + synths_))
	//{
	//c = masterSynth_;
	//vc = (v << 4) | c;
	//}
	//sCard_[c]->setNote(v,midiKey);  //COME BACK do poly firmware
	//setNoteVoiceCard(midiKey,vc);
	//notesOn_++;
	//if (base_!=NULL)
	//{
	//base_->zoneTriggerNote(c,v,midiKey,midiVel,true);
	//}
	////hardware_.setAtxMesReqBuffer(AtxMsg::AMH_NOTEON,(MidiMsg::MCMD_NOTEON | midiCh), midiKey,midiVel);
	//}
	//

	void AtxZone::releaseNote(midi::MidiMsg* msg)
	{
		using namespace atx;
		using namespace midi;
		uint8_t midiKey = msg->getData1();
		uint8_t midiVel = msg->getData2();
		if (midiKey < lowerNote_ || midiKey > upperNote_ || base_==NULL || getNoteMapNotedown(midiKey)==false)
		{
			return;
		}notesDown_--;
		if (voiceAssignMode_==VA_UNISON)
		{
			uint8_t v = getNoteMapVoice(midiKey);
			for (uint8_t i=masterSynth_;i<=(masterSynth_+synths_);++i)
			{
				setNoteMapNote(midiKey,i,v,false);
				setSynthMapNotedown(i,v,false);
				base_->zoneTxMidiMsg(i,msg,true);
				if (getSynthMapNotedown(i,curVoice_[i])==true) //if poly used up, then this is free now
				{
					curVoice_[i] = v;
				}
			}

		}
		else
		{
			uint8_t c = getNoteMapSynth(midiKey);
			uint8_t v = getNoteMapVoice(midiKey);
			setNoteMapNote(midiKey,c,v,false);
			setSynthMapNotedown(c,v,false);
			base_->zoneTxMidiMsg(c,msg,true);
			if (getSynthMapNotedown(curSynth_,curVoice_[curSynth_])==true) //if poly used up, then this is free now
			{
				curSynth_ = c;
				curVoice_[curSynth_] = v;
			}
		}

	}

	//void AtxZone::releaseNote(uint8_t midiKey, uint8_t midiVel)
	//{
	//using namespace atx;
	//if (midiKey < lowerNote || midiKey > upperNote)
	//{
	//return;
	//}
	//uint_fast8_t i = midiKey>>3;
	//
	//uint8_t vc = getNoteVoiceCard(midiKey);
	//uint8_t v = vc >> 4;
	//uint8_t c = vc & 0x0F;
	//if (vc==UNUSED || c < masterSynth_ || c >= (masterSynth_ + synths_) || v >= AtxCard::getFirmwarePoly(firmware))  //lot of potentially unneeded safety here
	//{
	//return;// note already off
	//}
	//incNoteVoiceCardBuffer(midiKey);
	//setNoteVoiceCard(midiKey,UNUSED);
	//sCard_[c]->setNote(v,UNUSED);  //COME BACK, sort poly firmware, done??
	//notesOn_--;
	//if (base_!=NULL)
	//{
	//base_->zoneTriggerNote(c,v,midiKey,midiVel,false);
	//}
	////hardware_.setAtxMesReqBuffer(AtxMsg::AMH_NOTEOFF,(MidiMsg::MCMD_NOTEOFF | midiCh),midiKey,0);
	//}
	//

	void AtxZone::allNotesOff()
	{
		using namespace midi;
		MidiMsg m;
		m.setChannel(index_);
		m.setCommand(MCMD_CC);
		m.setData1(MCC_NOTESOFF);
		for (uint_fast8_t c=masterSynth_;c<=(masterSynth_+synths_);++c)
		{
			for(uint_fast8_t v=0;v<AtxCcSet::getFirmwareVoices();++v)
			{
				if(getSynthMapNotedown(c,v))
				{
					base_->zoneTxMidiMsg(c, &m, true);
					break;
				}
			}
		}
		curSynth_ = masterSynth_;
		notesDown_ = 0;
		memset(curVoice_,0,sizeof(curVoice_));
		memset(synthMap_,0,sizeof(synthMap_));
		memset(noteMap_,0,sizeof(noteMap_));
	}

	void AtxZone::rxMidiMsg(midi::MidiMsg* msg)
	{
		using namespace midi;
		if(msg->isNoteOn())
		{
			triggerNote(msg);
		}
		else if(msg->isNoteOff())
		{
			releaseNote(msg);
		}
		else if(msg->getCommand()==MCMD_CC)
		{
			if(msg->getData1()<MAX_CCS)
			{
				setCcValue(msg->getData1(),msg->getData2());//now done by CcChanged, to stop overflow of midi
			}
			else
			{
				for (uint8_t i=masterSynth_;i<=(masterSynth_+synths_);++i)
				{
					base_->zoneTxMidiMsg(i,msg,true);  //so it only passes sustain pedal on.  Anything else we want passed on?
				}
			}
		}
	}

	//void AtxZone::allNotesOff()
	//{
	//using namespace atx;
	//if (notesOn_)
	//{
	//for (uint_fast8_t c=masterSynth_;c<(masterSynth_+synths_);++c)
	//{
	//for (uint_fast8_t v=0;v<AtxCard::getFirmwarePoly(firmware);++v)
	//{
	//sCard_[c]->setNote(v,UNUSED);
	//}
	//base_->zoneAllNotesOff(c);
	//}
	//memset(noteVoiceCardBuffer_,UNUSED,sizeof(noteVoiceCardBuffer_));
	//curVoiceCard_ = 0x00;
	//notesOn_ = 0;
	//}
	//}
}

