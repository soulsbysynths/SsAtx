/*
* MidiMsg.h
*
* Created: 19/01/2018 12:58:43
* Author: info
*/


#ifndef __MIDIMSG_H__
#define __MIDIMSG_H__

#include <stdint.h>
#include <string.h>
#include "MidiMsgProgmem.h"
#include "Midi.h"

namespace midi
{
	class MidiMsg
	{
		//variables
		public:
		protected:
		static const uint8_t MSG_SIZE = 3;
		uint8_t msg_[MSG_SIZE] = {0};
		private:

		//functions
		public:
		MidiMsg(){}
		~MidiMsg(){}
		MidiMsg( const MidiMsg &c )
		{
			memcpy(msg_, c.msg_, MSG_SIZE);
		}
		MidiMsg& operator=( const MidiMsg &c )
		{
			memcpy(msg_, c.msg_, MSG_SIZE);
			return *this;
		}
		const uint8_t* getMsgPtr() const { return  msg_; }
		uint8_t* getMsgPtr() { return msg_; }
		void setStatus(uint8_t status){msg_[0] = status;}
		uint8_t getStatus(){return msg_[0];}
		const uint8_t getStatus() const { return msg_[0];}
		void setCommand(uint8_t command){msg_[0] = (msg_[0] & 0x0F) | command;}
		uint8_t getCommand(){return getCommand(msg_[0]);}
		const uint8_t getCommand() const { return getCommand(msg_[0]);}
		static uint8_t getCommand(uint8_t status){return (status & 0xF0);}
		//static const uint8_t getCommand(uint8_t status) { return (status & 0xF0);}
		void setChannel(uint8_t ch){msg_[0] = (msg_[0] & 0xF0) | ch;}
		uint8_t getChannel(){return getChannel(msg_[0]);}
		const uint8_t getChannel() const { return getChannel(msg_[0]);}
		static uint8_t getChannel(uint8_t status){return (status & 0x0F);}
		//static const uint8_t getChannel(uint8_t status) { return (status & 0x0F);}
		void setData1(uint8_t data1){msg_[1] = (data1 & 0x7F);}
		uint8_t getData1(){return msg_[1];}
		const uint8_t getData1() const { return msg_[1];}
		void setData2(uint8_t data2){msg_[2] = (data2 & 0x7F);}
		uint8_t getData2(){return msg_[2];}
		const uint8_t getData2() const { return msg_[2];}
		static uint8_t getMsgSize(uint8_t status) {return getMsgSizeFromHeader(getHeaderFromStatus(status));}
		const uint8_t getMsgSize() const {return getMsgSizeFromHeader(getHeaderFromStatus());}
		uint8_t getHeaderFromStatus(){return getHeaderFromStatus(msg_[0]);}
		const uint8_t getHeaderFromStatus() const {return getHeaderFromStatus(msg_[0]);}
		bool isNoteOn(){return (getCommand()==MCMD_NOTEON && getData2()>0);}
		bool isNoteOff(){return (getCommand()==MCMD_NOTEOFF || (getCommand()==MCMD_NOTEON && getData2()==0));}
		protected:
		static uint8_t getMsgSizeFromHeader(uint8_t hdr){return MIDI_MESSAGE_SIZE[(hdr & 0x0F)];}
		inline static uint8_t getHeaderFromStatus(uint8_t status)
		{
			if(status>=MCMD_CLOCK)
			{
				return MCIN_BYTE;
			}
			else if (status<MCMD_SYSEX)
			{
				return (status>>4);
			}
			else if (status==MCMD_SYSEX)
			{
				return MCIN_SYSEXSTART;
			}
			else if(status==MCMD_MTC || status==MCMD_SS)
			{
				return MCIN_COMMON2;
			}
			else if (status==MCMD_SPP)
			{
				return MCIN_COMMON3;
			}
			else
			{
				return MCIN_COMMON1;
			}
		}
		private:

		//MidiMsg( const MidiMsg &c );
		//MidiMsg& operator=( const MidiMsg &c );

	}; //MidiMsg
	class MidiUsbMsg : public MidiMsg
	{
		private:
		uint8_t header_ = 0;
		public:
		MidiUsbMsg(){}
		~MidiUsbMsg(){}
		MidiUsbMsg( const MidiUsbMsg &c ) : MidiMsg(c)
		{
			header_ = c.header_;
		}
		MidiUsbMsg& operator=( const MidiUsbMsg &c )
		{
			header_ = c.header_;
			MidiMsg::operator=(c);
			return *this;
		}
		void setHeader(uint8_t header){header_ = header;}
		uint8_t getHeader(){return header_;}
		const uint8_t getHeader() const { return header_;}
		void setHeaderFromStatus(){header_ = getHeaderFromStatus();}
		uint8_t getMsgSize(){return header_==MCIN_MISC ?  getMsgSizeFromHeader(getHeaderFromStatus(msg_[0])) : getMsgSizeFromHeader(header_);}
		const uint8_t getMsgSize() const {return header_==MCIN_MISC ?  getMsgSizeFromHeader(getHeaderFromStatus(msg_[0])) : getMsgSizeFromHeader(header_);}
	};
	class MidiSeqMsg : public MidiMsg
	{
		private:
		uint32_t clk_ = 0;
		//functions
		public:
		MidiSeqMsg(){}
		~MidiSeqMsg(){}
		MidiSeqMsg( const MidiSeqMsg &c ) : MidiMsg(c)
		{
			clk_ = c.clk_;
		}
		MidiSeqMsg& operator=( const MidiSeqMsg &c )
		{
			clk_ = c.clk_;
			MidiMsg::operator=(c);
			return *this;
		}
		void setClk(uint32_t clk){clk_ = clk;}
		uint32_t getClk(){return clk_;}
		const uint32_t getClk() const { return clk_;}		
	};
}



#endif //__MIDIMSG_H__
