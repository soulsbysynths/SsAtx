/*
* SysexMsg.h
*
* Created: 17/11/2021 18:09:47
*  Author: info
*/


#ifndef SYSEXMSG_H_
#define SYSEXMSG_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

namespace midi
{
	class SysexMsg
	{
		//variables
		public:
		protected:
		private:
		uint8_t msgSize_ = 0;
		uint8_t* msg_ = NULL;
		//functions
		public:
		SysexMsg(){}
		SysexMsg(uint8_t msgSize)
		{
			msgSize_ = msgSize;
			msg_ = (uint8_t*)malloc(msgSize_);
		}
		SysexMsg(uint8_t* data, uint8_t msgSize) : SysexMsg(msgSize)
		{
			memcpy(msg_, data, msgSize_);		
		}
		~SysexMsg()
		{
			free(msg_);
			msgSize_ = 0;
		}
		SysexMsg( const SysexMsg &c ) : msgSize_(c.msgSize_), msg_(NULL)
		{
			msg_ = (uint8_t*)malloc(msgSize_);
			memcpy(msg_, c.msg_, msgSize_);
		}
		SysexMsg& operator=( const SysexMsg &c )
		{
			free(msg_);
			msgSize_ = c.msgSize_;
			msg_ = (uint8_t*)malloc(msgSize_);
			memcpy(msg_, c.msg_, msgSize_);
			return *this;
		}
		
		//uint8_t& operator[](int i) {return msg_[i];}
		//const uint8_t& operator[](int i) const {return msg_[i];}
		
		inline void setSize(uint8_t newSize)
		{
			free(msg_);
			msgSize_ = newSize;
			msg_ = (uint8_t*)malloc(newSize);
		}
		uint8_t getSize(){return msgSize_;}
		const uint8_t getSize() const { return msgSize_;}
		void setData(uint_fast8_t index, const uint8_t value){msg_[index] = value;}
		uint8_t getData(uint_fast8_t index){return msg_[index];}
		const uint8_t getData(uint_fast8_t index) const { return msg_[index];}
		protected:
		private:

		//SysexMsg( const SysexMsg &c );
		//SysexMsg& operator=( const SysexMsg &c );

	}; //SysexMsg
}



#endif /* SYSEXMSG_H_ */