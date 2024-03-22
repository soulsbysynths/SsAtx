/*
* AtxCSequencer.h
*
* Created: 14/06/2018 11:35:00
* Author: info
*/


#ifndef __ATXCSEQUENCER_H__
#define __ATXCSEQUENCER_H__
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include "Atx.h"
#include "AtxCSequencerBase.h"
//#include "AtxSeqMsgList.h"
//#include "AtxSeqMsgListStream.h"
#include "MidiMsg.h"
#include "MidiMsgList.h"
#include "AtxCard.h"
#include "AtxZone.h"
#include "AtxCSequencerProgmem.h"
#include "Helpers.h"
#include "FreeStack.h"
#include "AtxCSdCard.h"
#include "NoteOnMap.h"

#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif

class AtxCSequencer
{
	//variables
	public:
	enum SeqMode : uint8_t
	{
		S_OVERVIEW = 0,
		S_LENGTH,
		S_MERGESEQ,
		S_RECSEQ,
		S_EDITSEQ,
		S_MAX
	};
	enum SeqPlayMode : uint8_t
	{
		SPM_STOP = 0,
		SPM_PLAY,
		SPM_REC,
		SPM_RECCUE,
		SPM_COUNTIN
	};
	enum RefreshDisplayElement : uint8_t
	{
		RDE_SEQNUM = 0,
		RDE_PLAY,
		RDE_LOOP,
		RDE_NAME,
		RDE_POS,
		RDE_MAX
	};
	enum ClickMode : uint8_t
	{
		CM_OFF = 0,
		CM_COUNT,
		CM_COUNTREC,
		CM_ON	
	};
	static const uint8_t DISPLAY_COLS = 6;
	static const uint8_t DISPLAY_ROWS = 2;
	const static uint8_t STP_TYPE_INDEX = 6;
	uint8_t countInBars = 2;
	ClickMode clickMode = CM_OFF;
	protected:
	private:
	const static uint16_t SEQ_MODE_OVERVIEW = 0b0000000001111111;
	static const int8_t I2C_GENERAL_CALL = -8;
	static const uint16_t QUANTISE_CLKS[16];
	AtxCSequencerBase* base_;
	SeqMode seqMode_ = S_OVERVIEW;
	uint8_t seqModeIndex_ = 0;	//used for accessing zero aligned arrays for AddTrans
	midi::MidiMsgListStream sequence_[atx::MAX_TRACKS][atx::MAX_SEQS];
	NoteOnMap seqMap_[atx::MAX_TRACKS][atx::MAX_ZONES];  //stores which notes are on on each channel in each track
	midi::MidiMsgList bufSeq_;
	NoteOnMap bufMap_;
	uint16_t editIndex_ = 0;
	uint8_t editTrack_ = 0;	//all these don't need to be stored in a patch
	uint8_t editFunc_[atx::MAX_TRACKS] = {0};
	uint8_t editSeq_[atx::MAX_TRACKS] = {0};
	uint8_t playSeq_[atx::MAX_TRACKS] = {0};	
	uint8_t recZone_ = 0;
	uint8_t recQuantise_ = 10;  //quarter beat
	uint16_t bpm_ = 120;
	uint32_t clk_ = 0;
	uint8_t clksPerBeat_ = atx::CLKS_PER_QUARTER_NOTE;  //for the sequencer, not the sequence
	uint8_t beatsPerBar_ = 4;  //for the sequencer, not the sequence
	uint8_t clksPerBar_ = clksPerBeat_ * beatsPerBar_;  //for sequencer.  Must keep this synced.
	SeqPlayMode seqPlayMode_ = SPM_STOP;
	uint8_t clickChannel_ = 0;
	uint8_t clickNote_[2] = {96,108};
	uint32_t clickOffClk_[2] = {0,0};
	uint8_t firstDisplaySeq_[atx::MAX_TRACKS] = {0};
	uint8_t refreshDisplayElement_[atx::MAX_TRACKS][atx::MAX_SEQS] = {0};
	uint8_t countInBar = 0;
	//functions
	public:
	static AtxCSequencer& getInstance()
	{
		static AtxCSequencer instance; // Guaranteed to be destroyed.
		return instance;  // Instantiated on first use.
	}
	AtxCSequencer(AtxCSequencer const&) = delete;
	void operator=(AtxCSequencer const&) = delete;
	void pollClk(uint16_t clksPassed);
	void pollOv();
	void initialize();
	void fillSeqBuffers();
	void construct(AtxCSequencerBase* base);
	void setSeqMode(SeqMode newMode);
	SeqMode getSeqMode(){return seqMode_;}
	midi::MidiMsgListStream& getSequence(uint8_t track, uint8_t seq){return sequence_[track][seq];}
	const midi::MidiMsgListStream& getSequence(uint8_t track, uint8_t seq) const {return sequence_[track][seq];}
	midi::MidiMsgListStream& getSequence(uint8_t track){return sequence_[track][editSeq_[editTrack_]];}
	const midi::MidiMsgListStream& getSequence(uint8_t track) const {return sequence_[track][editSeq_[editTrack_]];}
	midi::MidiMsgListStream& getSequence(){return getSequence(editTrack_);}
	const midi::MidiMsgListStream& getSequence() const {return getSequence(editTrack_);}
	midi::MidiMsgList& getBuffer(){return bufSeq_;}
	const midi::MidiMsgList& getBuffer() const {return bufSeq_;}
	void setEditTrack(uint8_t newTrack);
	uint8_t getEditTrack(){return editTrack_;}
	void setEditSeq(uint8_t track, uint8_t newSeq);
	void setEditSeq(uint8_t newSeq){setEditSeq(editTrack_,newSeq);}
	uint8_t getEditSeq(uint8_t track){return editSeq_[track];}
	uint8_t getEditSeq(){return editSeq_[editTrack_];}
	void setEditZoneFromRot(uint8_t rotValue);
	void setEditSeqFromCtrl(uint8_t ctrl, uint8_t ctrlValue);
	void setEventListIndex(uint8_t track, uint16_t newIndex);
	void setEventListIndex(uint16_t newIndex){setEventListIndex(editTrack_,newIndex);}
	uint8_t getEventListIndex(){return editIndex_;}
	//uint8_t getClksPerBar(uint8_t track, uint8_t seq){return sequence_[track][seq].getClksPerQuarterNote() * 4;}  //sequence_[track][seq].getBeatsPerBar();}  //come back, you'll have to fathom out the meta!
	//uint8_t getClksPerBar(uint8_t track){return getClksPerBar(track,editSeq_[track]);}
	//uint8_t getClksPerBar(){return getClksPerBar(editTrack_);}
	//uint8_t getClksPerBeat(uint8_t track, uint8_t seq){return sequence_[track][seq].getClksPerQuarterNote();}
	//uint8_t getClksPerBeat(uint8_t track){return getClksPerBeat(track,editSeq_[track]);}
	//uint8_t getClksPerBeat(){return getClksPerBeat(editTrack_);}
	//uint8_t getBeatsPerBar(uint8_t track, uint8_t seq){return sequence_[track][seq].getBeatsPerBar();}
	//uint8_t getBeatsPerBar(uint8_t track){return getBeatsPerBar(track,editSeq_[track]);}
	//uint8_t getBeatsPerBar(){return getBeatsPerBar(editTrack_);}
	uint8_t getClksPerBar(){return atx::CLKS_PER_QUARTER_NOTE<<2;}
	uint8_t getClksPerBeat(){return atx::CLKS_PER_QUARTER_NOTE;}
	uint16_t getSequenceLength(uint8_t track, uint8_t seq){return sequence_[track][seq].getLength();}
	uint16_t getSequenceLength(uint8_t track){return getSequenceLength(track,editSeq_[track]);}
	uint16_t getSequenceLength(){return getSequenceLength(editTrack_);}
	void setSequenceName(uint8_t track, uint8_t seq, const char * newName);
	void setSequenceName(const char * newName){setSequenceName(editTrack_,editSeq_[editTrack_],newName);}
	void setSequenceBars(uint8_t track, uint8_t seq, uint16_t newBars);
	void setSequenceBars(uint16_t newBars){setSequenceBars(editTrack_,editSeq_[editTrack_],newBars);}
	void setBpm(uint16_t newValue);
	uint16_t getBpm(){return bpm_;}
	atx::PlayState getPlayState(uint8_t track, uint8_t seq){return sequence_[track][seq].playState;}
	atx::PlayState getPlayState(uint8_t track){return getPlayState(track,editSeq_[track]);}
	atx::PlayState getPlayState(){return getPlayState(editTrack_);}
	void setPlayState(uint8_t track, uint8_t seq, atx::PlayState newState);
	void setPlayState(uint8_t seq, atx::PlayState newState){setPlayState(editTrack_,newState);}
	void setPlayState(atx::PlayState newState){setPlayState(editTrack_,newState);}
	void setLoop(uint8_t track, uint8_t seq, bool newLoop);
	void setLoop(uint8_t track, bool newLoop){setLoop(track,editSeq_[track],newLoop);}
	void setLoop(bool newLoop){setLoop(editTrack_,newLoop);}
	void setPlayMode(SeqPlayMode newMode);
	SeqPlayMode getPlayMode(){return seqPlayMode_;}
	void appendSeqBuffer(midi::MidiMsg * msg);
	uint16_t calcBar(uint32_t clkStamp);
	uint8_t calcBeat(uint32_t clkStamp);
	uint8_t calcClk(uint32_t clkStamp);
	uint8_t calcPos(uint8_t track, uint8_t index);
	bool isSelected(uint8_t track);
	void remapMpeToSequence();
	//void remapMidiToSequence(char * fileName);
	void toggleCue();
	void toggleLoop();
	bool isOverviewMode(){return bitRead(SEQ_MODE_OVERVIEW,(uint8_t)seqMode_);}
	void refreshDisplayElement(RefreshDisplayElement element){refreshDisplayElement(editTrack_,editSeq_[editTrack_],element);}
	void refreshDisplayElement(uint8_t track, uint8_t seq, RefreshDisplayElement element){bitSet(refreshDisplayElement_[track][seq],(uint8_t)element);}
	void setRecZone(uint8_t newZone);
	uint8_t getRecZone(){return recZone_;}
	void setRecQuantise(uint8_t newQuantise);
	uint8_t getRecQuantise(){return recQuantise_;}
	void setClickChannel(uint8_t newChannel){clickChannel_ = newChannel;}
	uint8_t getClickChannel(){return clickChannel_;}
	void mergeSeq(){mergeSeq(editTrack_,editSeq_[editTrack_]);}
	void clearBuffer();
	protected:
	private:
	AtxCSequencer() {}
	AtxCSequencer(AtxCSequencerBase* base);
	~AtxCSequencer();
	//AtxCSequencer( const AtxCSequencer &c );
	//AtxCSequencer& operator=( const AtxCSequencer &c );
	void initSeqBuffer(uint8_t track, uint8_t seq);
	void refreshOverview();
	void refreshDisplaySeq(uint8_t track, uint8_t seq){refreshDisplayElement_[track][seq]=0xFF;}
	void refreshDisplayZone(uint8_t track){memset(refreshDisplayElement_[track],0xFF,atx::MAX_SEQS);}
	void refreshDisplayPage(){memset(refreshDisplayElement_,0xFF,sizeof(refreshDisplayElement_));}
	void clickMetronome(bool bar, bool way);
	uint16_t quantiseClk(uint16_t clk); 
	void mergeSeq(uint8_t track, uint8_t seq);
	void txBufNotesOn(bool way);
	void txTrackNotesOff(uint8_t track);
	void fixBufferStuckNotes();
	uint8_t calcBuffPos();
	void readSeq(uint8_t track, midi::MidiMsgListStream* mmls);
}; //AtxCSequencer

#endif //__ATXCSEQUENCER_H__

