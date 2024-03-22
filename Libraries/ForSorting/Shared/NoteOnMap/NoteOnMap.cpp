/*
* NoteOnMap.cpp
*
* Created: 29/02/2020 18:59:34
* Author: info
*/


#include "NoteOnMap.h"

// default constructor
NoteOnMap::NoteOnMap()
{
} //NoteOnMap

// default destructor
NoteOnMap::~NoteOnMap()
{
} //~NoteOnMap

void NoteOnMap::triggerNote(uint8_t noteNum)
{
	setNoteOn(noteNum,true);
	if (notesOn_==0 || noteNum>noteHighest_)
	{
		noteHighest_ = noteNum;
	}
	if (notesOn_==0 || noteNum<noteLowest_)
	{
		noteLowest_ = noteNum;
	}
	prevNotesOn_ = notesOn_;
	notesOn_++;
}

void NoteOnMap::releaseNote(uint8_t noteNum)
{
	setNoteOn(noteNum,false);
	prevNotesOn_ = notesOn_;
	notesOn_--;
	if (noteNum>=noteHighest_)
	{
		uint8_t newHighest = atx::UNUSED;
		for (uint_fast8_t i = NOTEON_MAP_SIZE ; i-- > 0 ; )
		{
			if (noteOnMap_[i]>0)
			{
				for (uint_fast8_t j = 8 ; j-- > 0 ; )
				{
					if (bitRead(noteOnMap_[i],j))
					{
						newHighest = (i<<3) + j;
						break;
					}
				}
			}
			if (newHighest!=atx::UNUSED)
			{
				noteHighest_ = newHighest;
				break;
			}
		}
	}
	if (noteNum<=noteLowest_)
	{
		uint8_t newLowest = atx::UNUSED;
		for (uint_fast8_t i = 0;i < NOTEON_MAP_SIZE; ++i )
		{
			if (noteOnMap_[i]>0)
			{
				for (uint_fast8_t j = 0;j < 8; ++j )
				{
					if (bitRead(noteOnMap_[i],j))
					{
						newLowest = (i<<3) + j;
						break;
					}
				}
			}
			if (newLowest!=atx::UNUSED)
			{
				noteLowest_ = newLowest;
				break;
			}
		}
	}
}
void NoteOnMap::allNotesOff()
{
	notesOn_ = 0;
	prevNotesOn_ = 0;
	noteHighest_ = 0;
	noteLowest_ = 127;
	memset(noteOnMap_,0,NOTEON_MAP_SIZE);
}