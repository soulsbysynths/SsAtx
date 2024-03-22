/*
* MidiMsgToStream.h
*
* Created: 02/08/2021 13:24:08
* Author: info
*/


#ifndef __MIDIMSGTOSTREAM_H__
#define __MIDIMSGTOSTREAM_H__

#include <stdint.h>
#include <string.h>
#include "MidiMsg.h"
#include "SysexMsg.h"

namespace midi
{
	class MidiMsgToStream
	{
		//variables
		public:
		protected:
		private:
		int bufferSize_ = 0;
		int bufferMask_ = 0;
		uint8_t* buffer_;
		int head_ = 0;
		int tail_ = 0;
		uint8_t runningStatus_ = 0;
		
		//functions
		public:
		MidiMsgToStream();
		MidiMsgToStream(uint_fast8_t bufferSize);
		~MidiMsgToStream();
		void writeMidi(const MidiMsg* msg);
		void writeByte(uint8_t b);
		void writeSysex(const SysexMsg* sysex);
		int read();
		int peek();
		inline void clear(){head_ = 0; tail_ = 0;}
		inline int available() {int delta = head_ - tail_; return (delta < 0) ? (bufferSize_ + delta) : (delta);}
		inline int availableForStore() {return (head_ >= tail_) ? (bufferSize_ - 1 - head_ + tail_) : (tail_ - head_ - 1);}
		bool isFull(){return (nextIndex(head_) == tail_);}
		inline bool isEmpty(){return (head_ == tail_);}
		protected:
		private:
		MidiMsgToStream( const MidiMsgToStream &c );
		MidiMsgToStream& operator=( const MidiMsgToStream &c );
		inline int nextIndex(int8_t index){ return (uint32_t)(index + 1) & bufferMask_; }
	}; //MidiMsgToStream
}



#endif //__MIDIMSGTOSTREAM_H__
