/*
* MidiStreamToMsg.h
*
* Created: 02/08/2021 13:24:23
* Author: info
*/


#ifndef __MIDISTREAMTOMSG_H__
#define __MIDISTREAMTOMSG_H__

#include <stdint.h>
#include <string.h>
#include "MidiMsg.h"
#include "SysexMsg.h"

//#include <avr/io.h>


namespace midi
{
	class MidiStreamToMsg
	{
		//variables
		public:
		enum SysexRxMode : uint8_t
		{
			SYSRX_OFF = 0,
			SYSRX_MANU,
			SYSRX_PROD,
			SYSRX_LEN,
			SYSRX_RX,	
		};
		enum StreamMsgType : uint8_t
		{
			SMT_MIDI = 0,
			SMT_SYSEX,
			SMT_MAX	
		};
		protected:
		private:
		int bufferSize_[SMT_MAX] = {0};
		int bufferMask_[SMT_MAX] = {0};
		MidiMsg* midiBuffer_;
		SysexMsg* sysexBuffer_;
		int head_[SMT_MAX] = {0};
		int tail_[SMT_MAX] = {0};
		uint8_t runningStatus_ = 0;
		bool thirdByte_ = false;
		uint8_t clocks_ = 0;
		midi::MidiCmd transport_ = MCMD_NOTHING;
		uint8_t sysexRxMode_ = 0;   //0 = off, 1 = manu, 2 = product, 3 = rx, 4 = size, 5 = rx
		uint8_t sysexIndex_ = 0;
		//functions
		public:
		MidiStreamToMsg();
		MidiStreamToMsg(uint_fast8_t midiBufferSize, uint_fast8_t sysexBufferSize);
		~MidiStreamToMsg();
		void readMidi(MidiMsg* destMsg);
		void readSysex(SysexMsg* destMsg);
		void write(uint8_t b);
		inline void clear(){clear(SMT_MIDI);clear(SMT_SYSEX);}
		inline int available(StreamMsgType smt){int delta = head_[smt] - tail_[smt]; return (delta < 0) ? (bufferSize_[smt] + delta) : (delta);}
		inline int availableForStore(StreamMsgType smt){return (head_[smt] >= tail_[smt]) ? (bufferSize_[smt] - 1 - head_[smt] + tail_[smt]) : (tail_[smt] - head_[smt] - 1);}
		bool isFull(){return (isFull(SMT_MIDI) || isFull(SMT_SYSEX));}
		inline bool isEmpty(StreamMsgType smt){return (head_[smt] == tail_[smt]);}
		inline uint_fast8_t pollClkMsgs(){uint_fast8_t clk = clocks_; clocks_ = 0; return clk;}
		inline midi::MidiCmd pollTransMsgs(){midi::MidiCmd tns = transport_; transport_ = MCMD_NOTHING; return tns;}
		
		protected:
		private:
		MidiStreamToMsg( const MidiStreamToMsg &c );
		MidiStreamToMsg& operator=( const MidiStreamToMsg &c );
		inline int nextIndex(StreamMsgType smt, int_fast8_t index){ return (uint32_t)(index + 1) & bufferMask_[smt]; }
		bool isFull(StreamMsgType smt){return (nextIndex(smt,head_[smt]) == tail_[smt]);}
		inline void clear(StreamMsgType smt){head_[smt] = 0; tail_[smt] = 0;}
	}; //MidiStreamToMsg
}



#endif //__MIDISTREAMTOMSG_H__
