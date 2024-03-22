/*
* MidiMsgListStream.h
*
* Created: 28/05/2020 17:10:44
*  Author: info
*/


#ifndef MIDIMSGLISTSTREAM_H_
#define MIDIMSGLISTSTREAM_H_


#include <string.h>
#include "MidiMsg.h"
#include "Atx.h"

namespace midi
{
	class MidiMsgListStream
	{
		//variables
		public:
		static const uint16_t MAX_TIMESTAMP = 383;  //4 bars  REALLY?
		static const uint8_t NAME_LEN = 9;
		//static const uint8_t PATCH_HEADER_BYTES = 9;
		
		atx::PlayState playState = atx::PS_STOP;
		bool looping = true;
		bool fileOpen = false;
		uint8_t runningStatus = 0;
		
		protected:
		private:
		char name_[NAME_LEN] = "  _EMPTY";
		uint32_t pos_ = 0; //position withinfile
		uint32_t length_ = 0;  //bytes of Midi (in header)
		uint16_t clksPerQuarterNote_ = atx::CLKS_PER_QUARTER_NOTE;
		uint8_t clksPerQuarterNoteScale_ = 1;
		uint32_t clks_ = 0;
		uint32_t clk_ = 0;
		//MidiSeqMsg msbBuffer_;

		static const int BUFFER_SIZE = 16;
		static const int BUFFER_MASK = BUFFER_SIZE - 1;
		MidiSeqMsg buffer_[BUFFER_SIZE];
		int head_ = 0;
		int tail_ = 0;
		uint8_t runningStatus_ = 0;

		//functions
		public:
		MidiMsgListStream()
		{
		}
		~MidiMsgListStream()
		{
		}
		MidiMsgListStream( const MidiMsgListStream &c ) : clksPerQuarterNote_(c.clksPerQuarterNote_), length_(c.length_), pos_(c.pos_), playState(c.playState), looping(c.looping), fileOpen(c.fileOpen), runningStatus(c.runningStatus), clk_(c.clk_)
		{
			//strncpy(name_,c.name_,NAME_LEN);
		}
		MidiMsgListStream& operator=( const MidiMsgListStream &c )
		{
			clksPerQuarterNote_ = c.clksPerQuarterNote_;
			length_ = c.length_;
			pos_ = c.pos_;
			playState = c.playState;
			looping = c.looping;
			fileOpen = c.fileOpen;
			runningStatus = c.runningStatus;
			clk_ = c.clk_;
			strncpy(name_,c.name_,NAME_LEN);
			return *this;
		}
		uint32_t getPos() const { return pos_;} // Size getter
		void setPos(uint32_t newPos){pos_ = newPos; }
		void incPos(uint_fast8_t bytes){pos_+=bytes;}
		uint32_t getClks() const { return clks_;} // Size getter
		void setClks(uint32_t newClks){clks_ = newClks; }
		void incClks(uint32_t clks){clks_+=clks;}
		uint32_t getClk(){return clk_;}
		void setClk(uint32_t clk){clk_ = clk;}
		void incClk(){clk_ += clksPerQuarterNoteScale_;}
		//MidiSeqMsg& getMsg() { return msbBuffer_; }
		//const MidiSeqMsg& getMsg() const { return msbBuffer_; }
		//MidiSeqMsg* getMsgPtr(){return &msbBuffer_;}
		//const MidiSeqMsg* getMsgPtr() const {return &msbBuffer_;}
		const char* getNamePtr() const { return  name_; }
		char* getNamePtr() { return name_; }
		inline void setName(const char* newName)
		{
			strncpy(name_,newName,NAME_LEN);
			name_[NAME_LEN-1] = '\0';
		}
		void setClksPerQuarterNote(uint16_t newValue){clksPerQuarterNote_ = newValue;clksPerQuarterNoteScale_ = clksPerQuarterNote_ / atx::CLKS_PER_QUARTER_NOTE;}
		uint16_t getClksPerQuarterNote(){return clksPerQuarterNote_;}
		void setLength(uint32_t newLength){length_ = newLength;}
		uint32_t getLength(){return length_;}
		//bool isMsgAtClk(){return pos_ ? (msbBuffer_.getClkStamp()==clk) : false;}
		bool isMsgAtClk(){return isEmpty() ? false : (clk_>=buffer_[tail_].getClk());}
		bool isEndOfFile(){return (pos_>=length_);}
		bool isEndOfTrack(){return isEmpty() ? true : (buffer_[tail_].getStatus()==0xFF && buffer_[tail_].getData1()==0x2F);}

		inline MidiSeqMsg readMsg()
		{
			if(isEmpty())
			{
				MidiSeqMsg blank;
				return blank;
			}
			else
			{
				int ind = tail_;
				tail_ = nextIndex(tail_);
				return buffer_[ind];
			}

		}
		inline void readMsg(MidiSeqMsg* destMsg)
		{
			if(isEmpty())
			{
				destMsg = NULL;
			}
			else
			{
				int ind = tail_;
				tail_ = nextIndex(tail_);
				memcpy(destMsg, &buffer_[ind], sizeof(MidiSeqMsg));
			}
		}
		inline MidiSeqMsg peekMsg()
		{
			if(isEmpty())
			{
				MidiSeqMsg blank;
				return blank;
			}
			else
			{
				return buffer_[tail_];
			}
		}
		inline void writeMsg(MidiSeqMsg* msg)
		{
			int i = nextIndex(head_);
			if ( i != tail_ )
			{
				memcpy(&buffer_[head_],msg,sizeof(MidiSeqMsg));
				head_ = i;
			}
		}
		inline int available(){int delta = head_ - tail_; return (delta < 0) ? (BUFFER_SIZE + delta) : (delta);}
		inline int availableForStore(){return (head_ >= tail_) ? (BUFFER_SIZE - 1 - head_ + tail_) : (tail_ - head_ - 1);}
		bool isFull(){return (nextIndex(head_) == tail_);}
		inline bool isEmpty(){return (head_ == tail_);}

		void reset()
		{
			clk_ = 0;
			clks_ = 0;  //yes, because this is the last buffered clock (not total), reset ALWAYS = seek start
			pos_ = 0;
			head_ = 0;
			tail_ = 0;
		}
		protected:
		private:
		inline int nextIndex(int_fast8_t index){ return (uint32_t)(index + 1) & BUFFER_MASK; }
	}; //MidiMsgListStream
}

#endif /* MIDIMSGLISTSTREAM_H_ */