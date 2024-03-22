/*
* MidiMsgToStream.cpp
*
* Created: 02/08/2021 13:24:08
* Author: info
*/


#include "MidiMsgToStream.h"

namespace midi
{
	// default constructor
	MidiMsgToStream::MidiMsgToStream() : MidiMsgToStream(64)
	{
	} //MidiMsgToStream
	MidiMsgToStream::MidiMsgToStream(uint_fast8_t bufferSize)
	{
		bufferSize_ = bufferSize;
		bufferMask_ = bufferSize_ - 1;
		buffer_ = new uint8_t [bufferSize_]();
		memset(buffer_, 0, bufferSize_) ;
	}
	// default destructor
	MidiMsgToStream::~MidiMsgToStream()
	{
		delete[] buffer_;
	} //~MidiMsgToStream
	void MidiMsgToStream::writeMidi(const MidiMsg* msg)
	{
		if(msg->getStatus()>=MCMD_CLOCK)  //real time
		{
			writeByte(msg->getStatus());
		}
		//else if(msg->getStatus()==MCMD_SYSEX || msg->getStatus()==MCMD_SYSEXEND)  //system common
		else if(msg->getStatus()>=MCMD_SYSEX)  //system common
		{
			writeByte(msg->getStatus());
			runningStatus_ = 0;
		}
		else if(msg->getStatus()!=runningStatus_)  //everything else, needs status resending
		{
			writeByte(msg->getStatus());
			runningStatus_ = msg->getStatus();
		}
		if(msg->getMsgSize()>1)
		{
			writeByte(msg->getData1());
			if(msg->getMsgSize()>2)
			{
				writeByte(msg->getData2());
			}
		}
	}
	void MidiMsgToStream::writeByte(uint8_t b)
	{
		int i = nextIndex(head_);

		if ( i != tail_ )
		{
			buffer_[head_] = b;
			head_ = i ;
		}
	}
	
	void MidiMsgToStream::writeSysex(const SysexMsg* sysex)
	{
		writeByte(MCMD_SYSEX);
		writeByte(SYSEX_MANUFACTURER_ID);
		writeByte(SYSEX_PRODUCT_ID);
		writeByte(sysex->getSize());
		for (uint8_t i=0;i<sysex->getSize();++i)
		{
			writeByte(sysex->getData(i));
		}
		writeByte(MCMD_SYSEXEND);
		runningStatus_ = 0;
	}
	
	int MidiMsgToStream::read()
	{
		if(tail_ == head_)
		{
			return -1;
		}
		else
		{
			int ind = tail_;
			tail_ = nextIndex(tail_);
			return buffer_[ind];
		}

	}

	int MidiMsgToStream::peek()
	{
		if(tail_ == head_)
		{
			return -1;
		}
		else
		{
			return buffer_[tail_];
		}
	}

}


