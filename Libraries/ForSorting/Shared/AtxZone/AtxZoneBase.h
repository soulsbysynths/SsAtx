/*
 * AtxZoneBase.h
 *
 * Created: 18/06/2018 18:41:46
 *  Author: info
 */ 


#ifndef ATXZONEBASE_H_
#define ATXZONEBASE_H_

#include "MidiMsg.h"

namespace atx
{
	class AtxZoneBase
	{
		public:
		virtual void zoneTxMidiMsg(uint8_t synth, midi::MidiMsg* msg, bool txPolys) = 0;
		virtual void zoneTxResetClks(uint8_t synth);
	};
}


#endif /* ATXZONEBASE_H_ */