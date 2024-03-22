/*
* AtxCSequencer.cpp
*
* Created: 14/06/2018 11:35:00
* Author: info
*/


#include "AtxCSequencer.h"
const uint16_t AtxCSequencer::QUANTISE_CLKS[16] = {384,192,96,48,24,18,16,12,9,8,6,5,3,2,2,1};

// default constructor
AtxCSequencer::AtxCSequencer(AtxCSequencerBase* base)
{
	construct(base);
} //AtxCSequencer

// default destructor
AtxCSequencer::~AtxCSequencer()
{
	if (base_!=NULL)
	{
		delete base_;
	}
} //~AtxCSequencer

void AtxCSequencer::construct(AtxCSequencerBase* base)
{
	using namespace atx;
	base_ = base;
}

void AtxCSequencer::initialize()
{
	setBpm(120);
	
	for (uint8_t t=0;t<atx::MAX_TRACKS;++t)
	{
		for (uint8_t s=0;s<atx::MAX_SEQS;++s)
		{
			*(sequence_[t][s].getNamePtr()) = (char)t+48;
			*(sequence_[t][s].getNamePtr()+1) = (char)s+48;
		}
	}
	
	//test notes
	//AtxSeqMsg m;
	//uint16_t t;
	//static const uint8_t STARTNOTE = 12;
	//for (uint8_t t=0;t<atx::MAX_TRACKS;++t)
	//{
	////base_->sequencerMemoryFree(freeMem);
	//for (uint8_t s=0;s<1;++s)
	//{
	//t = 0;
	//sequence_[t][s].clear();
	//if ((s&0x01)==0)
	//{
	//for (uint8_t n=STARTNOTE;n<(STARTNOTE+16);++n)
	//{
	//m.setHeader(AtxMsg::AMH_NOTEON);
	////m.setMsgByte(1,MidiMsg::MCMD_NOTEON | c);
	//uint8_t note = ((t+2)*12) + n;
	//m.setMidiData(0,note);
	//m.setMidiData(1,127);
	//m.setClkStamp(t);
	//sequence_[t][s].append(m);
	//m.setHeader(AtxMsg::AMH_NOTEOFF);
	////m.setMsgByte(1,MidiMsg::MCMD_NOTEOFF | c);
	//m.setMidiData(0,note);
	//m.setMidiData(1,0);
	//t += (3*(t+1));
	//m.setClkStamp(t-1);
	//sequence_[t][s].append(m);
	//
	//}
	//}
	//else
	//{
	//for (uint8_t n=(STARTNOTE+16);n>STARTNOTE;--n)
	//{
	//m.setHeader(AtxMsg::AMH_NOTEON);
	////m.setMidiData(1,MidiMsg::MCMD_NOTEON | c);
	//uint8_t note = ((t+2)*12) + n;
	//m.setMidiData(0,note);
	//m.setMidiData(1,127);
	//m.setClkStamp(t);
	//sequence_[t][s].append(m);
	//m.setHeader(AtxMsg::AMH_NOTEOFF);
	////m.setMsgByte(1,MidiMsg::MCMD_NOTEOFF | c);
	//m.setMidiData(0,note);
	//m.setMidiData(1,0);
	//t += (3*(t+1));
	//m.setClkStamp(t-1);
	//sequence_[t][s].append(m);
	//}
	//}
	//char name[AtxSeqMsgList::NAME_LEN] = "Zne Seq ";
	//name[3] = t + 49;
	//name[7] = s + 49;
	//sequence_[t][s].setName(name);
	//sequence_[t][s].looping = true;
	//}
	//}
	//initSeqBuffer();
}

void AtxCSequencer::setBpm(uint16_t newValue)
{
	if (newValue<20)
	{
		newValue = 20;
	}
	else if(newValue>300)
	{
		newValue = 300;
	}
	bpm_ = newValue;
	base_->sequencerBpmChanged(bpm_);
}

void AtxCSequencer::setSequenceName(uint8_t track, uint8_t seq, const char * newName)
{
	sequence_[track][seq].setName(newName);
	refreshDisplayElement(track,seq,RDE_NAME);
}

void AtxCSequencer::setSequenceBars(uint8_t track, uint8_t seq, uint16_t newBars)
{
	//sequence_[track][seq].setLength(getClksPerBar(track,seq) * newBars);
	refreshDisplayElement(track,seq,RDE_NAME);
}

void AtxCSequencer::toggleCue()
{
	using namespace atx;
	if (sequence_[editTrack_][editSeq_[editTrack_]].playState==PS_STOP)
	{
		setPlayState(editTrack_,editSeq_[editTrack_],PS_PLAYCUE);
	}
	else
	{
		setPlayState(editTrack_,editSeq_[editTrack_],PS_STOP);
	}
}

void AtxCSequencer::toggleLoop()
{
	setLoop(editTrack_,editSeq_[editTrack_],!sequence_[editTrack_][editSeq_[editTrack_]].looping);
}

void AtxCSequencer::fillSeqBuffers()
{
	using namespace atx;
	for(uint8_t t=0;t<MAX_TRACKS;++t)
	{
		for(uint8_t s=0;s<MAX_SEQS;++s)
		{
			if(sequence_[t][s].fileOpen==true)
			{
				base_->sequencerSeqRead(t,s,sequence_[t][s]);
			}
		}
	}
}

void AtxCSequencer::pollClk(uint16_t clksPassed)
{
	using namespace atx;
	using namespace midi;
	bool newBar = false;
	if (seqPlayMode_==SPM_STOP)
	{
		return;
	}
	while(clksPassed)
	{
		clksPassed--;
		if (clk_ % clksPerBar_==0)  //new bar
		{
			//AtxMsg m;  //NO THIS ISN@T THE WAY TO MCMD_SPP
			//m.setHeader(AtxMsg::AMH_COMMON3);
			//m.setMsgByte(1,MidiMsg::MCMD_SPP);
			//uint32_t beats = clk_ / 6;
			//m.setMsgByte(2,beats & 0x7F);
			//m.setMsgByte(3,(beats >> 7) & 0x7F);
			//base_->sequencerCardTx(I2C_GENERAL_CALL,&m);
			newBar = true;
			if (countInBar>0)
			{
				countInBar--;
			}
			else
			{
				if (seqPlayMode_==SPM_COUNTIN)
				{
					setPlayMode(SPM_REC);
				}
				for (uint8_t i=0;i<MAX_TRACKS;++i)
				{
					for (uint8_t j=0;j<MAX_SEQS;++j)
					{
						if (sequence_[i][j].playState==PS_PLAYCUE)
						{
							setPlayState(i,j,PS_PLAY);
						}
					}
				}
			}
			clickMetronome(true,true);
		}
		else if(clk_==clickOffClk_[1])
		{
			clickMetronome(true,false);
		}
		else if (clk_ % clksPerBeat_==0)  //new beat
		{
			clickMetronome(false,true);
		}
		else if(clk_==clickOffClk_[0])
		{
			clickMetronome(false,false);
		}
		if (seqPlayMode_!=SPM_COUNTIN)
		{
			for (uint8_t i=0;i<MAX_TRACKS;++i)
			{
				MidiMsgListStream* mmls = &sequence_[i][playSeq_[i]];
				if (mmls->fileOpen && mmls->getLength() && (mmls->playState==PS_PLAY || mmls->playState==PS_REC))
				{
					readSeq(i,mmls);
					if (mmls->isEndOfTrack())
					{
						if (mmls->looping==false)
						{
							setPlayState(i,playSeq_[i],PS_STOP);
						}
						else
						{
							if(newBar)
							{
								setPlayState(i,playSeq_[i],PS_PLAY);
								readSeq(i,mmls);
							}
							else
							{
								setPlayState(i,playSeq_[i],PS_PLAYCUE);
							}
							
						}
					}
				}
				

			}
		}

		if (seqPlayMode_==SPM_PLAY || seqPlayMode_==SPM_RECCUE)
		{
			//COME BACK!
			//while (bufSeq_.isMsgAtClk())
			//{
			//base_->sequencerZoneTx(recZone_,&bufSeq_.getPlayMsg());
			//if (bufSeq_.getPlayMsg().getHeader()==AtxSeqMsg::AMH_NOTEON)
			//{
			//bufMap_.triggerNote(bufSeq_.getPlayMsg().getMidiData(0));
			//}
			//else if(bufSeq_.getPlayMsg().getHeader()==AtxSeqMsg::AMH_NOTEOFF)
			//{
			//bufMap_.releaseNote(bufSeq_.getPlayMsg().getMidiData(0));
			//}
			//if (!bufSeq_.incIndex())
			//{
			//bufSeq_.setIndex(0);
			//break;
			//}
			//}
		}
		if (seqMode_!=SPM_COUNTIN)
		{
			//COME BACK!
			//bufSeq_.clk++;
			//if (bufSeq_.clk>=bufSeq_.getLength())
			//{
			//bufSeq_.reset();
			//if (seqPlayMode_==SPM_REC)
			//{
			//setPlayMode(SPM_PLAY);
			//}
			//else if(seqPlayMode_==SPM_RECCUE)
			//{
			//setPlayMode(SPM_REC);
			//}
			//}
		}
		clk_++;
	}
}

void AtxCSequencer::readSeq(uint8_t track, midi::MidiMsgListStream* mmls)
{
	using namespace midi;
	while (mmls->isMsgAtClk())
	{
		MidiSeqMsg msg;
		mmls->readMsg(&msg);
		if(msg.getStatus()<0xF0 && msg.getChannel()<atx::MAX_ZONES)
		{
			if(msg.getCommand()==midi::MCMD_NOTEON)
			{
				seqMap_[track][msg.getChannel()].setNoteOn(msg.getData1(),true);
			}
			else if(msg.getCommand()==midi::MCMD_NOTEOFF)
			{
				seqMap_[track][msg.getChannel()].setNoteOn(msg.getData1(),false);
			}
			base_->sequencerTxMidiMsg(&msg);
		}
	}
	mmls->incClk();
}

void AtxCSequencer::clearBuffer()
{
	txBufNotesOn(false);
	bufMap_.allNotesOff();
	bufSeq_.clear();
}

void AtxCSequencer::setPlayState(uint8_t track, uint8_t seq, atx::PlayState newState)
{
	using namespace atx;
	if(newState==PS_STOP)
	{
		txTrackNotesOff(track);
	}
	else
	{
		if(sequence_[track][seq].fileOpen==true)
		{
			sequence_[track][seq].reset();
			base_->sequencerSeqSeekStartMidi(track,seq);
			base_->sequencerSeqRead(track,seq,sequence_[track][seq]);
		}
		for (uint8_t i=0;i<MAX_SEQS;++i)  //stop all the other ones
		{
			if (i!=seq)// && sequence_[track][i].playState==newState)
			{
				setPlayState(track,i,PS_STOP);
			}
		}
	}
	switch(newState)
	{
		case PS_STOP:
		//zone_[track]->allNotesOff(); //no, find another way to do this
		break;
		case PS_PLAYCUE:
		break;
		case PS_PLAY:
		playSeq_[track] = seq;
		break;
	}
	sequence_[track][seq].playState = newState;
	refreshDisplayElement(track,seq,RDE_PLAY);
}


void AtxCSequencer::setLoop(uint8_t track, uint8_t seq, bool newLoop)
{
	sequence_[track][seq].looping = newLoop;
	refreshDisplayElement(track,seq,RDE_LOOP);
}

void AtxCSequencer::setEditSeq(uint8_t track, uint8_t newSeq)
{
	refreshDisplayElement(track,editSeq_[track],RDE_PLAY);  //turn prev off
	refreshDisplayElement(track,newSeq,RDE_PLAY);  //turn new on
	editSeq_[track] = newSeq;
}

void AtxCSequencer::pollOv()
{
	using namespace atx;
	static uint32_t lastClk = 0;
	static uint8_t lastPos[MAX_TRACKS][MAX_SEQS] = {{0}};
	static uint8_t bufLastPos = 0;
	if (isOverviewMode())
	{
		for (uint8_t t=0;t<MAX_TRACKS;++t)
		{
			for (uint8_t s=0;s<MAX_SEQS;++s)
			{
				uint8_t newPos = calcPos(t,s);
				if (newPos!=lastPos[t][s])
				{
					lastPos[t][s] = newPos;
					refreshDisplayElement(t,s,RDE_POS);
				}
			}
		}
		refreshOverview();
	}
	if(seqMode_==S_OVERVIEW)
	{
		if (clk_!=lastClk)
		{
			lastClk = clk_;
			base_->sequencerOvClkChanged(calcBar(clk_),calcBeat(clk_),calcClk(clk_));
		}
	}
	else if (seqMode_==S_RECSEQ)
	{
		uint8_t newPos = calcBuffPos();
		if (newPos!=bufLastPos)
		{
			bufLastPos = newPos;
			base_->sequencerRecSeqPosChanged(newPos);
		}
	}

}

void AtxCSequencer::clickMetronome(bool bar, bool way)
{
	using namespace midi;
	MidiMsg m;
	if (clickMode==CM_OFF || (clickMode==CM_COUNT && countInBar==0) || (clickMode==CM_COUNTREC && seqPlayMode_==SPM_PLAY) )
	{
		return;
	}
	m.setCommand(MCMD_NOTEON);
	m.setChannel(clickChannel_);
	m.setData1(clickNote_[(uint8_t)bar]);
	if (way)
	{
		clickOffClk_[(uint8_t)bar] = clk_ + (clksPerBeat_ >> 2);
		m.setData2(127);
		
	}
	else
	{
		m.setData2(0);
	}
	base_->sequencerTxMidiMsg(&m);
}

void AtxCSequencer::setPlayMode(SeqPlayMode newMode)
{
	using namespace atx;
	using namespace midi;
	if (seqPlayMode_==SPM_REC && newMode!=SPM_REC)
	{
		//fixBufferStuckNotes();
	}
	switch(newMode)
	{
		case SPM_STOP:
		clickMetronome(false,false);
		clickMetronome(true,false);
		for (uint8_t i=0;i<MAX_TRACKS;++i)
		{
			setPlayState(i,playSeq_[i],PS_STOP);
		}
		txBufNotesOn(false);
		break;
		case SPM_PLAY:
		case SPM_REC:
		if (seqPlayMode_==SPM_STOP)
		{
			clk_ = 0;
			for (uint8_t i=0;i<MAX_TRACKS;++i)
			{
				for (uint8_t j=0;j<MAX_SEQS;++j)
				{
					sequence_[i][j].reset();
				}
			}
			
		}
		//bufSeq_.reset();  //COME BACK!
		if (newMode==SPM_REC)
		{
			clearBuffer();
		}
		for (uint8_t i=0;i<MAX_TRACKS;++i)
		{
			for (uint8_t j=0;j<MAX_SEQS;++j)
			{
				if (sequence_[i][j].playState==PS_PLAYCUE)
				{
					setPlayState(i,j,PS_PLAY);
				}
			}
		}
		break;
		case SPM_COUNTIN:
		countInBar = countInBars;
		clk_ = 0;
		break;
	}
	seqPlayMode_ = newMode;
	MidiMsg m;
	if (seqPlayMode_==SPM_STOP)
	{
		m.setStatus(MCMD_STOP);
		base_->sequencerTxMidiMsg(&m);
		//no longer needed. each track as note map
		//m.setCommand(MCMD_CC);
		//for(uint8_t i=0;i<MAX_ZONES;++i)
		//{
		//m.setChannel(i);
		//m.setData1(MCC_NOTESOFF);
		//base_->sequencerTxMidiMsg(&m);
		//}
	}
	else if(seqPlayMode_<SPM_RECCUE)
	{
		m.setStatus(MCMD_START);
		base_->sequencerTxMidiMsg(&m);
	}
	base_->sequencerPlayChanged((uint8_t)seqPlayMode_);
}


bool AtxCSequencer::isSelected(uint8_t track)
{
	return (track==editTrack_);
}

//void AtxCSequencer::remapMidiToSequence(char * fileName)
//{
//using namespace atx;
//uint8_t ppqnRatio = midiList_.getClksPerQuarterNote() / atx::CLKS_PER_QUARTER_NOTE;
//sequence_[editTrack_][editSeq_[editTrack_]].clear();
//uint32_t ts = 0;
//for (uint16_t i=0;i<midiList_.getCount();++i)
//{
//uint8_t cmd,ch,n,v;
//AtxSeqMsg m;
//cmd = midiList_.getEvent(i).getStatus() & 0xF0;
////ch = midiList_.getEvent(i).getStatus() & 0x0F;
//n = midiList_.getEvent(i).getData1();
//v = midiList_.getEvent(i).getData2();
//if (cmd==MidiMsg::MCMD_NOTEON && v==0)
//{
//cmd = MidiMsg::MCMD_NOTEOFF;
//}
//ts += midiList_.getEvent(i).getDeltaTime();
//switch (cmd)
//{
//case MidiMsg::MCMD_NOTEON:
//{
//m.setHeader(AtxMsg::AMH_NOTEON);
////m.setMsgByte(1,(MidiMsg::MCMD_NOTEON | editTrack_));  //no point setting that here
//m.setMidiData(0,n);
//m.setMidiData(1,v);
//m.setClkStamp(ts / ppqnRatio);
//sequence_[editTrack_][editSeq_[editTrack_]].append(m);
//}
//break;
//case MidiMsg::MCMD_NOTEOFF:
//{
//m.setHeader(AtxMsg::AMH_NOTEOFF);
////m.setMsgByte(1,(MidiMsg::MCMD_NOTEOFF | editTrack_));  //no point setting that here
//m.setMidiData(0,n);
//m.setClkStamp(ts / ppqnRatio);
//sequence_[editTrack_][editSeq_[editTrack_]].append(m);
//}
//break;
//}
//}
//setSequenceName(editTrack_,editSeq_[editTrack_],fileName);
////sequence_[editTrack_][editSeq_[editTrack_]].setName(fileName,8);
////base_->sequencerOvSeqNameChanged(editTrack_,1,sequence_[editTrack_][editSeq_[editTrack_]].getNamePtr(),calcDisplay(editTrack_),calcRow(editTrack_,editSeq_[editTrack_]));
//}

//YOU NEED TO COME BACK TO THIS TO CHECK IT
void AtxCSequencer::remapMpeToSequence()
{
	//using namespace atx;
	//uint8_t cmd, ch;
	//uint16_t chInUse = 0;
	//for (uint16_t i=0;i<midiList_.getCount();++i)
	//{
	//cmd = midiList_.getEvent(i).getStatus() & 0xF0;
	//ch = midiList_.getEvent(i).getStatus() & 0x0F;
	//if (cmd<0xF0)  //ignore sys common msgs
	//{
	//bitSet(chInUse,ch);
	//}
	//}
	//uint16_t chToVoiceMap[16];
	//uint8_t v = 0;
	//for (uint8_t i = 0;i<16;++i)
	//{
	//chToVoiceMap[i] = UNUSED;
	//if (bitRead(chInUse,i)==true)
	//{
	//while (card_[v]->track!=card_[editTrack_]->track)
	//{
	//v++;
	//v &= 0x07;
	//}
	//chToVoiceMap[i] = v;
	//v++;
	//v &= 0x07;
	//}
	//}
	//for (uint8_t i=0;i<MAX_SYNTHCARDS;++i)
	//{
	//if (card_[i]->track==card_[editTrack_]->track)
	//{
	//sequence_[i][1-actSeq_[i]].clear();
	//}
	//}
	//uint16_t ts = 0;
	//for (uint16_t i=0;i<midiList_.getCount();++i)
	//{
	//cmd = midiList_.getEvent(i).getStatus() & 0xF0;
	//ch = midiList_.getEvent(i).getStatus() & 0x0F;
	//switch (cmd)
	//{
	//case MidiMsg::MCMD_NOTEON:
	//case MidiMsg::MCMD_NOTEOFF:
	//{
	//AtxMsg m;
	//if (cmd==MidiMsg::MCMD_NOTEON)
	//{
	//m.setHeader(AtxMsg::AMH_NOTEON);
	//m.setMsgByte(1,MidiMsg::MCMD_NOTEON | chToVoiceMap[ch]);
	//}
	//else
	//{
	//m.setHeader(AtxMsg::AMH_NOTEOFF);
	//m.setMsgByte(1,MidiMsg::MCMD_NOTEOFF | chToVoiceMap[ch]);
	//}
	//
	//m.setMsgByte(2,midiList_.getEvent(i).getData1());
	//m.setMsgByte(3,midiList_.getEvent(i).getData2());
	//m.setClkStamp(ts);
	//sequence_[chToVoiceMap[ch]][1-actSeq_[chToVoiceMap[ch]]].append(m);
	//}
	//break;
	//}
	//ts += midiList_.getEvent(i).getDeltaTime();
	//}
}

void AtxCSequencer::setEditTrack(uint8_t newTrack)
{
	using namespace atx;
	refreshDisplayElement(editTrack_,editSeq_[editTrack_],RDE_PLAY);  //turn off old
	refreshDisplayElement(newTrack,editSeq_[newTrack],RDE_PLAY);  //turn on new
	editTrack_ = newTrack;
	base_->sequencerEditTrackChanged(editTrack_);
}

//uint8_t AtxCSequencer::calcPos(uint8_t track, uint8_t index)  //play pos 0 = 0, otherwise 1 - 8
//{
//if (sequence_[track][index].clk==0)
//{
//return 0;
//}
//else
//{
//return ((sequence_[track][index].clk << 3) / sequence_[track][index].getLength())+1;
//}
//}

uint8_t AtxCSequencer::calcPos(uint8_t track, uint8_t index)  //play pos 0 - 63
{
	if (sequence_[track][index].getLength()==0)
	{
		return 0;
	}
	else
	{
		return (((uint32_t)sequence_[track][index].getPos() << 6) / sequence_[track][index].getLength());
	}
}

uint8_t AtxCSequencer::calcBuffPos()
{
	//if (bufSeq_.getLength()==0)
	//{
		//return 0;
	//}
	//else
	//{
		//return (((uint32_t)bufSeq_.clk << 6) / bufSeq_.getLength());
	//}
	return 0;  //fix this
}
void AtxCSequencer::setSeqMode(SeqMode newMode)
{
	using namespace atx;
	seqMode_ = newMode;
	if (isOverviewMode())  //YOU MAY BE ABLE TO GET RID OF THIS BIT
	{

		refreshDisplayPage();
	}

	base_->sequencerModeChanged(seqMode_);
	if (seqMode_==S_OVERVIEW)
	{
		base_->sequencerOvClkChanged(calcBar(clk_),calcBeat(clk_),calcClk(clk_));
	}
	else if(seqMode_==S_RECSEQ)
	{
		base_->sequencerRecTrackChanged(recZone_);
		base_->sequencerRecSeqPosChanged(calcBuffPos());
		//base_->sequencerRecQuantiseChanged(recQuantise_);
	}
	else if(seqMode_==S_EDITSEQ)
	{
		base_->sequencerRecQuantiseChanged(recQuantise_);
	}
}

void AtxCSequencer::setEditSeqFromCtrl(uint8_t ctrl, uint8_t ctrlValue)
{
	uint8_t s = ctrlValue >> (8 - atx::MAX_SEQS_BS);  //8 = anal ctrl res

	if (s==editSeq_[ctrl]) return;
	if (s<firstDisplaySeq_[ctrl])
	{
		firstDisplaySeq_[ctrl] = s;
		refreshDisplayZone(ctrl);
	}
	else if(s>=(firstDisplaySeq_[ctrl]+DISPLAY_ROWS))
	{
		firstDisplaySeq_[ctrl] = (s-DISPLAY_ROWS+1);
		refreshDisplayZone(ctrl);
	}
	if (ctrl!=editTrack_)
	{
		setEditTrack(ctrl);
	}
	setEditSeq(ctrl,s);
}

void AtxCSequencer::refreshOverview()
{
	using namespace atx;
	uint8_t d = 0, r = 0;
	for (uint8_t t=0;t<MAX_TRACKS;++t)
	{
		r = 0;
		for (uint8_t s=firstDisplaySeq_[t];s<(firstDisplaySeq_[t]+DISPLAY_ROWS);++s)
		{
			for (uint8_t e=0;e<RDE_MAX;++e)
			{
				if (bitRead(refreshDisplayElement_[t][s],e))
				{
					switch(e)
					{
						case RDE_SEQNUM:
						base_->sequencerOvSeqChanged(t,s,d,r);
						break;
						case RDE_PLAY:
						base_->sequencerOvPlayChanged(t,s,(t==editTrack_ && s==editSeq_[t]),sequence_[t][s].playState,d,r);
						break;
						case RDE_LOOP:
						base_->sequencerOvSeqLoopChanged(t,s,sequence_[t][s].looping,d,r);
						break;
						case RDE_NAME:
						{
							if(seqMode_==S_LENGTH)
							{
								//char name[9];
								//uint16_t l = sequence_[t][s].getLength();
								//uint16_t bars = l / getClksPerBar();
								//if (bars==1)
								//{
								//snprintf(name, 9, "%d bar", bars);
								//}
								//else
								//{
								//snprintf(name, 9, "%d bars", bars);
								//}
								//base_->sequencerOvSeqNameChanged(t,s,name,d,r);
							}
							else
							{
								base_->sequencerOvSeqNameChanged(t,s,sequence_[t][s].getNamePtr(),d,r);
							}
						}
						break;
						case RDE_POS:
						base_->sequencerOvSeqPosChanged(t,s,calcPos(t,s),d,r);
						break;
					}
				}
			}
			refreshDisplayElement_[t][s] = 0;
			r++;
		}
		d++;
	}
}

void AtxCSequencer::setRecQuantise(uint8_t newQuantise)
{
	recQuantise_ = newQuantise;
	if (seqMode_==S_RECSEQ)
	{
		base_->sequencerRecQuantiseChanged(newQuantise);
	}
}

void AtxCSequencer::txBufNotesOn(bool way)
{
	using namespace midi;
	for (uint8_t i=0;i<NoteOnMap::NOTEON_MAP_SIZE;++i)
	{
		uint8_t nm = bufMap_.getNoteOnArray(i);
		if (nm>0)
		{
			for (uint8_t j=0;j<8;++j)
			{
				if (bitRead(nm,j))
				{
					uint8_t n = (i<<3) + j;
					MidiMsg m;
					m.setCommand(MCMD_NOTEON);
					m.setData1(n);
					if (way)
					{
						m.setData2(127);  //argh this isn't ideal forcing vel to 127
					}
					else
					{
						m.setData2(0);
					}
					base_->sequencerTxMidiMsg(&m);
				}
			}
		}
	}
}

void AtxCSequencer::txTrackNotesOff(uint8_t track)
{
	using namespace midi;
	using namespace atx;
	for (uint8_t ch=0;ch<MAX_ZONES;++ch)
	{
		for (uint8_t i=0;i<NoteOnMap::NOTEON_MAP_SIZE;++i)
		{
			uint8_t nm = seqMap_[track][ch].getNoteOnArray(i);
			if (nm>0)
			{
				for (uint8_t j=0;j<8;++j)
				{
					if (bitRead(nm,j))
					{
						uint8_t n = (i<<3) + j;
						MidiMsg m;
						m.setChannel(ch);
						m.setCommand(MCMD_NOTEOFF);
						m.setData1(n);
						base_->sequencerTxMidiMsg(&m);
					}
				}
			}
		}
		seqMap_[track][ch].allNotesOff();
	}
}

void AtxCSequencer::fixBufferStuckNotes()
{
	using namespace midi;
	for (uint8_t i=0;i<16;++i)
	{
		uint8_t nm = bufMap_.getNoteOnArray(i);
		if (nm>0)
		{
			for (uint8_t j=0;j<8;++j)
			{
				if (bitRead(nm,j))
				{
					uint8_t n = (i<<3) + j;
					MidiSeqMsg m;
					//m.setClkStamp(bufSeq_.getLength()-1); COME BACK!
					m.setCommand(MCMD_NOTEON);
					m.setData1(n);
					m.setData2(0);
					bufSeq_.append(m);
				}
			}
		}
	}
}
void AtxCSequencer::setRecZone(uint8_t newZone)
{
	txBufNotesOn(false);
	recZone_ = newZone;
	txBufNotesOn(true);
	base_->sequencerRecTrackChanged(newZone);
}

void AtxCSequencer::appendSeqBuffer(midi::MidiMsg * msg)
{
	using namespace atx;
	using namespace midi;
	MidiSeqMsg smsg;
	smsg.setStatus(msg->getStatus());
	smsg.setData1(msg->getData1());
	smsg.setData2(msg->getData2());
	//msg->setClkStamp(quantiseClk(bufSeq_.clk)); COME BACK
	if (bufSeq_.getCount()==0)
	{
		bufSeq_.append(smsg);
	}
	else
	{
		//if (msg->getClkStamp()<bufSeq_.getLastMsg().getClkStamp())  //COME BACK AND SORT THIS FOR MIDI
		//{
		//clearBuffer();
		//}
		bufSeq_.append(smsg);
	}
	if (msg->getCommand()==MCMD_NOTEON)
	{
		bufMap_.triggerNote(msg->getData1());
	}
	else if(msg->getCommand()==MCMD_NOTEOFF || (msg->getCommand()==MCMD_NOTEON && msg->getData2()==0))
	{
		bufMap_.releaseNote(msg->getData1());
	}
}

void AtxCSequencer::setEventListIndex(uint8_t track, uint16_t newIndex)
{
	editIndex_ = newIndex;
	//base_->sequencerEventListChanged(newIndex,bufSeq_.getMsgPtr(editIndex_)); COME BACK
}

uint16_t AtxCSequencer::calcBar(uint32_t clkStamp)
{
	return (clkStamp / getClksPerBar()) + 1;
}

uint8_t AtxCSequencer::calcBeat(uint32_t clkStamp)
{
	return ((clkStamp % getClksPerBar()) / getClksPerBeat()) + 1;
}

uint8_t AtxCSequencer::calcClk(uint32_t clkStamp)
{
	return clkStamp % getClksPerBeat();
}

uint16_t AtxCSequencer::quantiseClk(uint16_t clk)
{
	uint16_t pos = clk % QUANTISE_CLKS[recQuantise_];
	if ((QUANTISE_CLKS[recQuantise_]-pos)>pos)  //nearer prev quant clk
	{
		return (clk - pos);
	}
	else  //nearer next quant clk
	{
		return (clk + (QUANTISE_CLKS[recQuantise_]-pos));
	}
}

void AtxCSequencer::mergeSeq(uint8_t track, uint8_t seq)
{
	
	//COME BACK!!!
	
	//AtxSeqMsgList amlMerge;
	//bool done[2] = {false};
	//
	//sequence_[track][seq].reset();
	//bufSeq_.reset();
	//base_->sequencerSeqSeekRead(track,seq,sequence_[track][seq],0);
	//while(done[0]==false && done[1]==false)
	//{
	//if(done[0]==false && bufSeq_.getPlayMsg().getClkStamp()<sequence_[track][seq].getMsg().getClkStamp())
	//{
	//amlMerge.append(bufSeq_.getPlayMsg());
	//if (!bufSeq_.incIndex())
	//{
	//done[0] = true;
	//}
	//}
	//else if(done[1]==false)
	//{
	//amlMerge.append(sequence_[track][seq].getMsg());
	//if (!sequence_[track][seq].incIndex())
	//{
	//done[1] = true;
	//}
	//else
	//{
	//base_->sequencerSeqRead(track,seq,sequence_[track][seq]);
	//}
	//}
	//}
	////base_->sequencerSeqWrite(track,seq,amlMerge);
	//clearBuffer();
	//for (uint16_t i=0;i<amlMerge.getCount();++i)
	//{
	//bufSeq_.append(amlMerge.getMsg(i));
	//}
	
}
