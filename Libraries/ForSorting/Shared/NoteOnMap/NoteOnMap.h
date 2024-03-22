/* 
* NoteOnMap.h
*
* Created: 29/02/2020 18:59:34
* Author: info
*/


#ifndef __NOTEONMAP_H__
#define __NOTEONMAP_H__
#include <string.h>
#include "Atx.h"
#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif
#ifndef bitToggle
#define bitToggle(value, bit) ((value) ^= (1UL << (bit)))
#endif

class NoteOnMap
{
//variables
public:
	static const uint8_t NOTEON_MAP_SIZE = 16;
protected:
private:
	uint8_t noteOnMap_[NOTEON_MAP_SIZE] = {0};
	uint8_t notesOn_ = 0;
	uint8_t prevNotesOn_ = 0;
	uint8_t noteHighest_ = 0;  //thought about this. Don't use UNSED, use 0. Too risky	
	uint8_t noteLowest_ = 127;
//functions
public:
	NoteOnMap();
	~NoteOnMap();
	void triggerNote(uint8_t noteNum);
	void releaseNote(uint8_t noteNum);
	void setNoteOn(uint8_t note, bool way){bitWrite(noteOnMap_[(note>>3)],(note&0x07),way);}  //normally use trigger, to get highest tot notes etc
	bool getNoteOn(uint8_t note){return (bool)bitRead(noteOnMap_[(note>>3)],(note&0x07));}	
	uint8_t getNotesOn(){return notesOn_;}
	uint8_t getPrevNotesOn(){return prevNotesOn_;}
	uint8_t getHighest(){return noteHighest_;}
	uint8_t getLowest(){return noteLowest_;}
	void allNotesOff();
	uint8_t getNoteOnArray(uint8_t index){return noteOnMap_[index];}
protected:
private:
	NoteOnMap( const NoteOnMap &c );
	NoteOnMap& operator=( const NoteOnMap &c );
}; //NoteOnMap

#endif //__NOTEONMAP_H__
