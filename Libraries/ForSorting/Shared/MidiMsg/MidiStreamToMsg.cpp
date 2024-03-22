/*
* MidiStreamToMsg.cpp
*
* Created: 02/08/2021 13:24:23
* Author: info
*/


#include "MidiStreamToMsg.h"

namespace midi
{
	// default constructor
	MidiStreamToMsg::MidiStreamToMsg() : MidiStreamToMsg(16,8)  //if msg to stream default is 32, the most msgs received = 16
	{
	} //MidiStreamToMsg
	MidiStreamToMsg::MidiStreamToMsg(uint_fast8_t midiBufferSize, uint_fast8_t sysexBufferSize)
	{
		bufferSize_[SMT_MIDI] = midiBufferSize;
		bufferSize_[SMT_SYSEX] = sysexBufferSize;
		for(uint_fast8_t i=0;i<SMT_MAX;++i)
		{
			bufferMask_[i] = bufferSize_[i] - 1;
		}
		midiBuffer_ = new MidiMsg [bufferSize_[SMT_MIDI]];
		sysexBuffer_ = new SysexMsg[bufferSize_[SMT_SYSEX]];
	}
	// default destructor
	MidiStreamToMsg::~MidiStreamToMsg()
	{
		delete[] midiBuffer_;
		delete[] sysexBuffer_;
	} //~MidiStreamToMsg
	void MidiStreamToMsg::write(uint8_t b)
	{
		int i = nextIndex(SMT_MIDI,head_[SMT_MIDI]);

		if ( i != tail_[SMT_MIDI] || b>=MCMD_CLOCK)
		{
			if(b>=MCMD_NOTEOFF)
			{
				midiBuffer_[head_[SMT_MIDI]].setStatus(b);
				sysexRxMode_ = SYSRX_OFF;
				if(b==MCMD_CLOCK) //real time
				{
					clocks_++;
				}
				else if(b>=MCMD_START && b<=MCMD_STOP)
				{
					transport_ = (midi::MidiCmd)b;
				}
				else if(b>MCMD_STOP)
				{
					midiBuffer_[head_[SMT_MIDI]].setData1(0);
					midiBuffer_[head_[SMT_MIDI]].setData2(0);
					head_[SMT_MIDI] = i;
				}
				//system common
				else if(b==MCMD_SYSEX)
				{
					sysexRxMode_ = SYSRX_MANU;
					runningStatus_ = 0;
					thirdByte_ = false;
				}
				else if(b==MCMD_SYSEXEND)
				{
					sysexRxMode_ = SYSRX_OFF;
					head_[SMT_SYSEX] = nextIndex(SMT_SYSEX,head_[SMT_SYSEX]);
					runningStatus_ = 0;
					thirdByte_ = false;
				}
				//everything else
				else
				{
					runningStatus_ = b;
					thirdByte_ = false;
					if(b==MCMD_TUNEREQ)
					{
						head_[SMT_MIDI] = i;
					}
				}
			}
			else if(thirdByte_==false)
			{
				if(runningStatus_>0)
				{
					midiBuffer_[head_[SMT_MIDI]].setStatus(runningStatus_);
					if(runningStatus_<MCMD_PROGCH)
					{
						thirdByte_ = true;
						midiBuffer_[head_[SMT_MIDI]].setData1(b);
					}
					else if(runningStatus_<MCMD_PBEND)
					{
						midiBuffer_[head_[SMT_MIDI]].setData1(b);
						midiBuffer_[head_[SMT_MIDI]].setData2(0);
						head_[SMT_MIDI] = i;
					}
					else if(runningStatus_<MCMD_SYSEX)
					{
						thirdByte_ = true;
						midiBuffer_[head_[SMT_MIDI]].setData1(b);
					}
					else if(runningStatus_>MCMD_SYSEX)
					{
						if(runningStatus_==MCMD_SPP)
						{
							runningStatus_ = 0;
							thirdByte_ = true;
							midiBuffer_[head_[SMT_MIDI]].setData1(b);
						}
						else if(runningStatus_==MCMD_SS || runningStatus_==MCMD_MTC)
						{
							runningStatus_ = 0;
							midiBuffer_[head_[SMT_MIDI]].setData1(b);
							midiBuffer_[head_[SMT_MIDI]].setData2(0);
							head_[SMT_MIDI] = i;
						}
						else
						{
							runningStatus_ = 0;  //so doesn't handle sysex
						}
					}
				}
				else if(sysexRxMode_>SYSRX_OFF)
				{
					if(sysexRxMode_==SYSRX_MANU)
					{
						if(b==SYSEX_MANUFACTURER_ID)
						{
							sysexRxMode_++;
						}
						else
						{
							sysexRxMode_ = SYSRX_OFF;
						}
					}
					else if(sysexRxMode_==SYSRX_PROD)
					{
						if(b==SYSEX_PRODUCT_ID)
						{
							sysexRxMode_++;
						}
						else
						{
							sysexRxMode_ = SYSRX_OFF;
						}
					}
					else if(sysexRxMode_==SYSRX_LEN)
					{
						if(isFull(SMT_SYSEX)==false)
						{
							sysexBuffer_[head_[SMT_SYSEX]].setSize(b);
							sysexIndex_ = 0;
							sysexRxMode_++;
						}
					}
					else
					{
						if(sysexIndex_<sysexBuffer_[head_[SMT_SYSEX]].getSize())
						{
							sysexBuffer_[head_[SMT_SYSEX]].setData(sysexIndex_,b);
							sysexIndex_++;
						}
					}
				}
			}
			else
			{
				thirdByte_ = false;
				midiBuffer_[head_[SMT_MIDI]].setData2(b);
				head_[SMT_MIDI] = i;
			}
		}
	}

	void MidiStreamToMsg::readMidi(MidiMsg* destMsg)
	{
		if(available(SMT_MIDI)==false)
		{
			destMsg = NULL;
		}
		else
		{
			//memcpy(destMsg, &midiBuffer_[tail_[SMT_MIDI]], sizeof(MidiMsg));
			(*destMsg) = midiBuffer_[tail_[SMT_MIDI]];
			tail_[SMT_MIDI] = nextIndex(SMT_MIDI,tail_[SMT_MIDI]);
		}
	}
	void MidiStreamToMsg::readSysex(SysexMsg* destMsg)
	{
		if(available(SMT_SYSEX)==false)
		{
			destMsg = NULL;
		}
		else
		{
			(*destMsg) = sysexBuffer_[tail_[SMT_SYSEX]];
			tail_[SMT_SYSEX] = nextIndex(SMT_SYSEX,tail_[SMT_SYSEX]);			
		}
	}
}

