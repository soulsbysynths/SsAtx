//Switch.cpp  Hardware switch/button with debounce
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

#include "Switch.h"

// default constructor
Switch::Switch(uint8_t index, SwitchBase* base)
{
	base_ = base;
	index_ = index;
	flags_ = 0;
	lastButtonState_ = BUTTON_STATE_UNKNOWN;
	elapsedDebounceTime_ = 0;
	elapsedClickTime_ = 0;
	elapsedPressTime_ = 0;
	setDefaultReleasedState(HIGH);
} //Switch

// default destructor
Switch::~Switch()
{
	if (base_ != NULL)
	{
		delete base_;
	}
} //~Switch

void Switch::poll(uint_fast8_t ticksPassed, uint8_t buttonState)
{
	// debounce the button
	if (checkDebounced(ticksPassed, buttonState)) {
		// check if the button was initialized (i.e. UNKNOWN state)
		if (checkInitialized(buttonState)) {
			checkEvent(ticksPassed, buttonState);
		}
	}
}

void Switch::setDefaultReleasedState(uint8_t state)
{
	if (state == HIGH)
	{
		flags_ |= FLAG_DEFAULT_RELEASED_STATE;
	}
	else
	{
		flags_ &= ~FLAG_DEFAULT_RELEASED_STATE;
	}
}

bool Switch::checkDebounced(uint_fast8_t ticksPassed, uint8_t buttonState)
{
	if (isDebouncing())
	{
		// NOTE: This is a bit tricky. The elapsedTime will be valid even if the
		// uint16_t representation of 'now' rolls over so that (now <
		// mLastDebounceTime). This is true as long as the 'unsigned long'
		// representation of 'now' is < (65536 + mLastDebounceTime). We need to cast
		// this expression into an uint16_t before doing the '>=' comparison below
		// for compatability with processors whose sizeof(int) == 4 instead of 2.
		// For those processors, the expression (now - mLastDebounceTime >=
		// getDebounceDelay()) won't work because the terms in the expression get
		// promoted to an (int).
		if (elapsedDebounceTime_<UINT16_MAX)
		{
			elapsedDebounceTime_ += ticksPassed;
		}
		bool isDebouncingTimeOver = (elapsedDebounceTime_ >= DEBOUNCE_DELAY);

		if (isDebouncingTimeOver)
		{
			clearDebouncing();
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		// Currently not in debouncing phase. Check for a button state change. This
		// will also detect a transition from kButtonStateUnknown to HIGH or LOW.
		if (buttonState == getLastButtonState())
		{
			// no change, return immediately
			return true;
		}
		// button has changed so, enter debouncing phase
		setDebouncing();
		elapsedDebounceTime_ = 0;
		return false;
	}
}

bool Switch::checkInitialized(uint8_t buttonState)
{
	if (lastButtonState_ != BUTTON_STATE_UNKNOWN)
	{
		return true;
	}

	// If transitioning from the initial "unknown" button state, just set the last
	// valid button state, but don't fire off the event handler. This handles the
	// case where a momentary switch is pressed down, then the board is rebooted.
	// When the board comes up, it should not fire off the event handler. This
	// also handles the case of a 2-position switch set to the "pressed"
	// position, and the board is rebooted.
	lastButtonState_ = buttonState;
	return false;
}

void Switch::checkEvent(uint_fast8_t ticksPassed, uint8_t buttonState)
{
	// We need to remove orphaned clicks even if just Click is enabled. It is not
	// sufficient to do this for just DoubleClick. That's because it's possible
	// for a Clicked event to be generated, then 65.536 seconds later, the
	// ButtonConfig could be changed to enable DoubleClick. (Such real-time change
	// of ButtonConfig is not recommended, but is sometimes convenient.) If the
	// orphaned click is not cleared, then the next Click would be errorneously
	// considered to be a DoubleClick. Therefore, we must clear the orphaned click
	// even if just the Clicked event is enabled.
	//
	// We also need to check of any postponed clicks that got generated when
	// kFeatureSuppressClickBeforeDoubleClick was enabled.
	if (elapsedClickTime_<UINT16_MAX)
	{
		elapsedClickTime_ += ticksPassed;
	}
	checkPostponedClick();
	checkOrphanedClick();
	if (elapsedPressTime_<UINT16_MAX)
	{
		elapsedPressTime_ += ticksPassed;
	}
	checkLongPress(buttonState);

	if (buttonState != getLastButtonState())
	{
		checkChanged(buttonState);
	}
}

void Switch::checkPostponedClick()
{
	if (isClickPostponed() && elapsedClickTime_ >= DOUBLE_CLICK_DELAY)
	{
		if (!locked_)
		{
			base_->swClicked(index_);
		}
		clearClickPostponed();
	}
}

void Switch::checkOrphanedClick()
{
	// The amount of time which must pass before a click is determined to be
	// orphaned and reclaimed. If only DoubleClicked is supported, then I think
	// just getDoubleClickDelay() is correct. No other higher level event uses the
	// first Clicked event. If TripleClicked becomes supported, I think
	// orphanedClickDelay will be either (2 * getDoubleClickDelay()) or
	// (getDoubleClickDelay() + getTripleClickDelay()), depending on whether the
	// TripleClick has an independent delay time, or reuses the DoubleClick delay
	// time. But I'm not sure that I've thought through all the details.
	if (isClicked() && (elapsedClickTime_ >= DOUBLE_CLICK_DELAY))
	{
		clearClicked();
	}
}

void Switch::checkLongPress(uint8_t buttonState)
{
	if (buttonState == getDefaultReleasedState())
	{
		return;
	}

	if (isPressed() && !isLongPressed())
	{
		if (elapsedPressTime_ >= LONG_PRESS_DELAY)
		{
			setLongPressed();
			if (!locked_)
			{
				base_->swHeld(index_);
			}
		}
	}
}

uint8_t Switch::getDefaultReleasedState()
{
	return (flags_ & FLAG_DEFAULT_RELEASED_STATE) ? HIGH : LOW;
}

void Switch::checkChanged(uint8_t buttonState)
{
	lastButtonState_ = buttonState;
	checkPressed(buttonState);
	checkReleased(buttonState);
}

void Switch::checkPressed(uint8_t buttonState)
{
	if (buttonState == getDefaultReleasedState())
	{
		return;
	}

	// button was pressed
	elapsedPressTime_ = 0;
	setPressed();
	if (!locked_)
	{
		base_->swValueChanged(index_,true);
	}
}

void Switch::checkReleased(uint8_t buttonState)
{
	if (buttonState != getDefaultReleasedState())
	{
		return;
	}

	// Check for click (before sending off the Released event).
	// Make sure that we don't clearPressed() before calling this.
	checkClicked();

	// check if Released events are suppressed
	bool suppress = (isLongPressed() || isClicked() || isDoubleClicked());

	// button was released
	clearPressed();
	clearDoubleClicked();
	clearLongPressed();

	//if (!suppress && !locked_)  //CHANGE BACK TO THIS LINE IF ADE32 ALL GOES TITS UP 
	if (!locked_)
	{
		base_->swValueChanged(index_,false);
		if (!suppress)
		{
			base_->swClicked(index_);
		}
	}
}

void Switch::checkClicked()
{
	if (!isPressed())
	{
		// Not a Click unless the previous state was a Pressed state.
		// This can happen if the chip was rebooted with the button Pressed. Upon
		// Release, it shouldn't generated a click, even accidentally due to a
		// spurious value in mLastPressTime.
		clearClicked();
		return;
	}
	//if (elapsedPressTime_ >= CLICK_DELAY)
	//{
	//clearClicked();
	//return;
	//}
	if (elapsedPressTime_ >= DOUBLE_CLICK_DELAY)
	{
		clearClicked();
		return;
	}
	
	// check for double click
	checkDoubleClicked();

	// Suppress a second click (both buttonState change and event message) if
	// double-click detected, which has the side-effect of preventing 3 clicks
	// from generating another double-click at the third click.
	if (isDoubleClicked())
	{
		clearClicked();
		return;
	}

	// we got a single click
	elapsedClickTime_ = 0;
	setClicked();
	setClickPostponed();
}

void Switch::checkDoubleClicked()
{
	if (!isClicked())
	{
		clearDoubleClicked();
		return;
	}

	if (elapsedClickTime_ >= DOUBLE_CLICK_DELAY)
	{
		clearDoubleClicked();
		// There should be no postponed Click at this point because
		// checkPostponedClick() should have taken care of it.
		return;
	}

	// If there was a postponed click, suppress it because it could only have been
	// postponed if kFeatureSuppressClickBeforeDoubleClick was enabled. If we got
	// to this point, there was a DoubleClick, so we must suppress the first
	// Click as requested.
	if (isClickPostponed())
	{
		clearClickPostponed();
	}
	setDoubleClicked();
	if (!locked_)
	{
		base_->swDoubleClicked(index_);
	}
}