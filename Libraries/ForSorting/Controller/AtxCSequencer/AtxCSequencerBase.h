/*
 * AtxCSequencerBase.h
 *
 * Created: 14/06/2018 11:38:31
 *  Author: info
 */ 


#ifndef ATXCSEQUENCERBASE_H_
#define ATXCSEQUENCERBASE_H_

#include "MidiMsg.h"
#include "MidiMsgListStream.h"


class AtxCSequencerBase
{
	public:
	virtual void sequencerEditTrackChanged(uint8_t track) = 0;
	virtual void sequencerModeChanged(uint8_t seqMode) = 0;
	virtual void sequencerEventListChanged(uint16_t index, midi::MidiSeqMsg * msg) = 0;
	virtual void sequencerBpmChanged(uint16_t bpm) = 0;
	virtual void sequencerTxMidiMsg(midi::MidiMsg * msg) = 0;
	virtual void sequencerPlayChanged(uint8_t playMode) = 0;
	virtual void sequencerOvPlayChanged(uint8_t track, uint8_t seq, bool selected, uint8_t playState, uint8_t display, uint8_t row) = 0;
	virtual void sequencerOvSeqChanged(uint8_t track, uint8_t seq, uint8_t display, uint8_t row) = 0;
	virtual void sequencerOvSeqNameChanged(uint8_t track, uint8_t seq, const char * name, uint8_t display, uint8_t row) = 0;
	virtual void sequencerOvSeqPosChanged(uint8_t track, uint8_t seq, uint8_t pos, uint8_t display, uint8_t row) = 0;
	virtual void sequencerOvSeqLoopChanged(uint8_t track, uint8_t seq, bool looping, uint8_t display, uint8_t row) = 0;
	virtual void sequencerOvClkChanged(uint16_t bar, uint8_t beat, uint8_t clk) = 0;
	virtual void sequencerSeqSeekStartMidi(uint8_t track, uint8_t seq) = 0;
	virtual void sequencerSeqRead(uint8_t track, uint8_t seq, midi::MidiMsgListStream& mmls) = 0;
	//virtual void sequencerSeqWrite(uint8_t track, uint8_t seq, AtxSeqMsgList& asml) = 0;
	virtual void sequencerRecQuantiseChanged(uint8_t quantise) = 0;
	virtual void sequencerRecTrackChanged(uint8_t track) = 0;
	virtual void sequencerRecSeqPosChanged(uint8_t pos) = 0;
};




#endif /* ATXCSEQUENCERBASE_H_ */