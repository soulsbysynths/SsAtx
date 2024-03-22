//Switch.h  Hardware switch/button with debounce
//Copyright (C) 2015  Paul Soulsby info@soulsbysynths.com
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __SSSWITCH_H__
#define __SSSWITCH_H__

#include <stdlib.h>
#include <stdint.h>
#include "SwitchBase.h"

#ifndef HIGH
#define  HIGH 0x01
#endif
#ifndef LOW
#define LOW 0x00
#endif

class Switch
{
	//variables
	public:
	protected:
	private:
	SwitchBase* base_ = NULL;
	uint8_t index_ = 0;
	bool locked_ = false;
	static const uint8_t DEBOUNCE_DELAY = 20;
	//static const uint8_t CLICK_DELAY = 112;  //think this may never get used
	static const uint8_t DOUBLE_CLICK_DELAY = 200;
	#ifndef LONG_PRESS_DELAY
	static const uint16_t LONG_PRESS_DELAY = 1000;
	#endif
	static const uint8_t FLAG_DEFAULT_RELEASED_STATE = 0x01;
	static const uint8_t FLAG_DEBOUNCING = 0x02;
	static const uint8_t FLAG_PRESSED = 0x04;
	static const uint8_t FLAG_CLICKED = 0x08;
	static const uint8_t FLAG_DOUBLE_CLICKED = 0x10;
	static const uint8_t FLAG_LONG_PRESSED = 0x20;
	static const uint8_t FLAG_CLICK_POSTPONED = 0x80;
	static const uint8_t BUTTON_STATE_UNKNOWN = 2;
	uint8_t flags_ = 0;
	uint8_t lastButtonState_ = 0;
	uint16_t elapsedDebounceTime_ = 0; // ms
	uint16_t elapsedClickTime_ = 0; // ms
	uint16_t elapsedPressTime_ = 0; // ms
	//functions
	public:
	Switch() {}
	Switch(uint8_t index, SwitchBase* base);
	~Switch();
	void setLocked(bool newValue){locked_ = newValue;}
	bool getLocked(){return locked_;}
	// If this is set, then mLastPressTime is valid.
	bool isPressed() {return flags_ & FLAG_PRESSED;}
	bool isLongPressed() {return flags_ & FLAG_LONG_PRESSED;}
	void setPressed() {flags_ |= FLAG_PRESSED;}
	uint16_t getElapsedPressTime(){return elapsedPressTime_;}
	void poll(uint_fast8_t ticksPassed, uint8_t buttonState);
	protected:
	private:
	bool checkInitialized(uint8_t buttonState);
	bool checkDebounced(uint_fast8_t ticksPassed, uint8_t buttonState);
	void checkEvent(uint_fast8_t ticksPassed, uint8_t buttonState);
	void checkPostponedClick();
	void checkOrphanedClick();
	void checkLongPress(uint8_t buttonState);
	void checkChanged(uint8_t buttonState);
	void checkPressed(uint8_t buttonState);
	void checkReleased(uint8_t buttonState);
	void checkClicked();
	void checkDoubleClicked();
	bool isDebouncing() {return flags_ & FLAG_DEBOUNCING;}
	bool isClickPostponed() {return flags_ & FLAG_CLICK_POSTPONED;}
	bool isClicked() {return flags_ & FLAG_CLICKED;}
	bool isDoubleClicked() {return flags_ & FLAG_DOUBLE_CLICKED;}

	void clearDebouncing() {flags_ &= ~FLAG_DEBOUNCING;}
	void clearClickPostponed() {flags_ &= ~FLAG_CLICK_POSTPONED;}
	void clearClicked() {flags_ &= ~FLAG_CLICKED;}
	void clearDoubleClicked() {flags_ &= ~FLAG_DOUBLE_CLICKED;}
	void clearPressed() {flags_ &= ~FLAG_PRESSED;}
	void clearLongPressed() {flags_ &= ~FLAG_LONG_PRESSED;}
	uint8_t getLastButtonState() {return lastButtonState_;}
	uint8_t getDefaultReleasedState();
	void setDebouncing() {flags_ |= FLAG_DEBOUNCING;}
	void setLongPressed() {flags_ |= FLAG_LONG_PRESSED;}
	void setDoubleClicked() {flags_ |= FLAG_DOUBLE_CLICKED;}
	void setClicked() {flags_ |= FLAG_CLICKED;}
	void setClickPostponed() {flags_ |= FLAG_CLICK_POSTPONED;}
	void setDefaultReleasedState(uint8_t state);
	//Switch( const Switch &c );
	//Switch& operator=( const Switch &c );
}; //Switch

#endif //__SWITCH_H__
