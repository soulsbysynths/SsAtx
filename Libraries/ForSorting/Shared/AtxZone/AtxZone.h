/*
* AtxZone.h
*
* Created: 18/06/2018 15:28:55
* Author: info
*/


#ifndef __ATXZONE_H__
#define __ATXZONE_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "Atx.h"
#include "AtxCcSet.h"
#include "AtxCard.h"
#include "AtxZoneBase.h"
#include "MidiMsg.h"

#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif

namespace atx
{
	class AtxZone : public AtxCcSet
	{
		//variables
		public:
		enum VoiceAssignMode : uint8_t
		{
			VA_BUFFERED = 0,
			VA_ROUNDROBIN,
			VA_UNISON
		};
		enum LfoTriggerMode : uint8_t
		{
			LT_FREERUNNING1 = 0,
			LT_FREERUNNING2,
			LT_RETRIGGERALL,
			LT_RETRIGGERCURRENT
		};
		
		protected:
		private:
		AtxZoneBase* base_ = NULL;
		static const uint8_t MIDI_NOTES = 128;
		AtxSynthCard * sCard_[atx::MAX_SYNTHCARDS] = {NULL};
		uint8_t index_ = 0;
		uint8_t masterSynth_ = 0;
		uint8_t synths_ = 0;  //THIS IS BETWEEN 0-7 to represent 1-8 cards. Change enabled to turn off zone
		bool enabled_ = false;
		VoiceAssignMode voiceAssignMode_ = VA_ROUNDROBIN;
		LfoTriggerMode lfoTriggerMode_ = LT_FREERUNNING1;
		uint8_t upperNote_ = 127;
		uint8_t lowerNote_ = 0;
		uint8_t noteMap_[MIDI_NOTES] = {0};  //byte 0 = notedown, byte 1 = used, byte 2,3,4 = synth, byte 5,6,7 = voice
		uint8_t synthMap_[atx::MAX_SYNTHCARDS] = {0};  //byte 0-7 = notedown voice 1-8
		uint16_t totalVoices_ = 0;
		uint8_t notesDown_ = 0;

		uint8_t curSynth_ = 0x00;  //This was static in sub, but I think it was shared with all zones
		uint8_t curVoice_[atx::MAX_SYNTHCARDS] = {0};

		//functions
		public:
		AtxZone(){}
		AtxZone(uint8_t index, AtxSynthCard* sCardPtrs[], AtxZoneBase* base);
		~AtxZone();
		AtxZone( const AtxZone &c );
		AtxZone& operator=( const AtxZone &c );
		void construct(uint8_t index, AtxSynthCard* sCardPtrs[], AtxZoneBase* base);
		//const AtxSynthCard* getCardPtr(uint8_t card) const { return sCard_[card]; }
		//AtxSynthCard* getCardPtr(uint8_t card) { return sCard_[card]; }
		void setEnabled(bool newEnabled);
		bool getEnabled(){return enabled_;}
		void setMasterSynth(uint8_t newMasterSynth);
		uint8_t getMasterSynth(){return masterSynth_;}
		void setSynths(uint8_t newSynths);
		uint8_t getSynths(){return synths_;}
		void setLowerNote(uint8_t newLowerNote);
		uint8_t getLowerNote(){return lowerNote_;}
		void setUpperNote(uint8_t newUpperNote);
		uint8_t getUpperNote(){return upperNote_;}
		void setVoiceAssignMode(VoiceAssignMode newMode);
		VoiceAssignMode getVoiceAssignMode(){return voiceAssignMode_;}
		void setLfoTriggerMode(LfoTriggerMode newMode){lfoTriggerMode_ = newMode;}
		LfoTriggerMode getLfoTriggerMode(){return lfoTriggerMode_;}
		uint8_t getSynthMap();
		void setFirmware(Firmware newFirmware) override ;
		void ccsetCcValueChanged(uint8_t index, uint8_t cc, uint8_t value);
		void rxMidiMsg(midi::MidiMsg* msg);
		void allNotesOff();
		uint8_t getNotesDown(){return notesDown_;}
		protected:
		private:
		void triggerNote(midi::MidiMsg* msg);
		void releaseNote(midi::MidiMsg* msg);
		
		void setSynthMapNotedown(uint8_t  card, uint8_t voice, bool newNotedown){bitWrite(synthMap_[card],voice,newNotedown);}
		bool getSynthMapNotedown(uint8_t card, uint8_t voice){return (bool)bitRead(synthMap_[card],voice);}
		void setNoteMapNote(uint8_t note, uint8_t card, uint8_t voice, bool noteDown){noteMap_[note] = (uint8_t)noteDown | 0b00000010 | (card << 2) | (voice << 5);}  //once it's used, it's used! (until note off)
		bool getNoteMapNotedown(uint8_t note){return (bool)(noteMap_[note] & 0x01);}
		bool getNoteMapUsed(uint8_t note){return (bool)((noteMap_[note] >> 1) & 0x01);}
		uint8_t getNoteMapSynth(uint8_t note){return (noteMap_[note] >> 2) & 0x07;}
		uint8_t getNoteMapVoice(uint8_t note){return (noteMap_[note] >> 5);}
		void refreshSynthCards();
	}; //AtxZone
}



#endif //__ATXZONE_H__
