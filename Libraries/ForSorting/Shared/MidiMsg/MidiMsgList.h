/*
* MidiMsgList.h
*
* Created: 16/06/2018 15:53:10
*  Author: info
*/


#ifndef MIDIMSGLIST_H_
#define MIDIMSGLIST_H_

#include <string.h>
#include "MidiMsg.h"
#include "Atx.h"

namespace midi
{
	class MidiMsgList
	{
		//variables
		public:
		protected:
		private:
		MidiSeqMsg* list_; // Stores data
		size_t count_; // Stores no. of actually stored objects
		size_t capacity_; // Stores allocated capacity
		uint16_t clksPerQuarterNote_ = atx::CLKS_PER_QUARTER_NOTE;
		//functions
		public:
		MidiMsgList() : count_(0), capacity_(0), list_(0){}
		~MidiMsgList()
		{
			free(list_);
		}
		MidiMsgList( const MidiMsgList &c ) : count_(c.count_), capacity_(c.capacity_), list_(0)
		{
			list_ = (MidiSeqMsg*)malloc(capacity_ * sizeof(MidiSeqMsg));
			memcpy(list_, c.list_, count_ * sizeof(MidiSeqMsg));
		}
		MidiMsgList& operator=( const MidiMsgList &c )
		{
			free(list_);
			count_ = c.count_;
			capacity_ = c.capacity_;
			list_ = (MidiSeqMsg*)malloc(capacity_ * sizeof(MidiSeqMsg));
			memcpy(list_, c.list_, count_ * sizeof(MidiSeqMsg));
			return *this;
		}
		void append(MidiSeqMsg const &ev)
		{
			if (capacity_ == count_)
			{
				resize();
			}
			list_[count_++] = ev;
		} // Adds new value. If needed, allocates more space
		
		size_t getCount() const
		{
			return count_;
		} // Size getter
		size_t getCapacity() const
		{
			return capacity_;
		} // Capacity getter
		MidiSeqMsg const &operator[](size_t index) const
		{
			return list_[index];
		} // Const getter
		MidiSeqMsg &operator[](size_t index)
		{
			return list_[index];
		} // Changeable getter
		void setClksPerQuarterNote(uint16_t newValue){clksPerQuarterNote_ = newValue;}
		uint16_t getClksPerQuarterNote(){return clksPerQuarterNote_;}
		//void setSize(uint32_t newValue){size_ = newValue;}
		MidiSeqMsg& getEvent(size_t index) { return list_[index]; }
		const MidiSeqMsg& getEvent(size_t index) const { return list_[index]; }
		MidiSeqMsg& getLastEvent() { return list_[count_-1]; }
		const MidiSeqMsg& getLastEvent() const { return list_[count_-1]; }
		void clear()
		{
			free(list_);
			list_ = NULL;
			//size_ = 0;
			count_ = 0;
			capacity_ = 0;
		}
		bool isMsgAtClk(){return false;}  //come back !!!
		uint32_t getSize() //crazy madness to get size bytes in MIDI file
		{
			uint_fast8_t runningStatus = 0;
			uint32_t s = 0;
			for (uint32_t i=0;i<count_;++i)
			{
				uint_fast8_t shift = 21;
				uint8_t deltabytes = 1;
				uint32_t delta;
				if(i==0)
				{
					delta = list_[i].getClk();
				}
				else
				{
					delta = list_[i].getClk() - list_[i-1].getClk();
				}
				for (uint_fast8_t j=0;j<3;++j)
				{
					if(((delta >> shift) & 0x7f)>0)
					{
						deltabytes++;
					}
					shift -= 7;
				}
				s += deltabytes;
				if(list_[i].getStatus()!=runningStatus)  //deal with running status
				{
					s++;
					runningStatus = list_[i].getStatus();
				}
				if(list_[i].getMsgSize()>1)
				{
					s++;
					if(list_[i].getMsgSize()>2)
					{
						s++;
					}
				}
			}
			s += 4;  //00 FF 2F 00  - end of track meta
			return s;
		}
		protected:
		private:
		void resize()
		{
			capacity_ = capacity_ ? capacity_ << 1 : 1;
			MidiSeqMsg *newdata = (MidiSeqMsg *)malloc(capacity_*sizeof(MidiSeqMsg));
			memcpy(newdata, list_, count_ * sizeof(MidiSeqMsg));
			free(list_);
			list_ = newdata;
		};// Allocates double the old space
	}; //MidiSeqMsg

}




#endif /* MIDIMSGLIST_H_ */