#include "Sn74hc138d.h"

Sn74hc138d::Sn74hc138d(uint8_t pinA, 
	uint8_t pinB, 
	uint8_t pinC)
{
	pinABC_[0] = pinA;
	pinABC_[1] = pinB;
	pinABC_[2] = pinC;
}

Sn74hc138d::Sn74hc138d(uint8_t pinA, 
	uint8_t pinB, 
	uint8_t pinC, 
	uint8_t pinInhibit) 
	: Sn74hc138d(pinA, pinB, pinC)
{
	pinInhibit_ = pinInhibit;
}

Sn74hc138d::Sn74hc138d(uint8_t pinA, 
	uint8_t pinB, 
	uint8_t pinC, 
	uint8_t pinInhibit, 
	uint8_t pinInput)
	: Sn74hc138d(pinA, pinB, pinC, pinInhibit)
{
	pinInput_ = pinInput;
}

Sn74hc138d::Sn74hc138d(uint8_t pinA, 
	uint8_t pinB, 
	uint8_t pinC, 
	void(*initialisePins)(void),
	void(*setInhibit)(bool))
	: Sn74hc138d(pinA, pinB, pinC)
{
	initialisePins_ = initialisePins;
	setInhibit_ = setInhibit;
}

Sn74hc138d::Sn74hc138d(uint8_t pinA, 
	uint8_t pinB, 
	uint8_t pinC, 
	void(*initialisePins)(void),
	void(*setInhibit)(bool),
	uint8_t pinInput)
	: Sn74hc138d(pinA, pinB, pinC, initialisePins, setInhibit)
{
	pinInput_ = pinInput;
}

void Sn74hc138d::initialise()
{
	for (uint8_t i = 0; i < SELECT_LINES; i++)
	{
		if (pinABC_[i] != UNUSED)
		{
			pinMode(pinABC_[i], OUTPUT);
		}
	}
	
	if (pinInhibit_ != UNUSED)
	{
		pinMode(pinInhibit_, OUTPUT);
	}
	
	if (pinInput_ != UNUSED)
	{
		pinMode(pinInput_, OUTPUT);
	}
	
	if (initialisePins_ != NULL)
	{
		initialisePins_();
	}
	
}

void Sn74hc138d::setLine(uint8_t line)
{
	for (int i = 0; i < SELECT_LINES; i++)
	{
		if (pinABC_[i] != UNUSED)
		{
			digitalWrite(pinABC_[i], bitRead(line, i));
		}
	}
}


void Sn74hc138d::setInhibit(bool inhibit)
{
	if (setInhibit_ != NULL)
	{
		setInhibit_(inhibit);
	}
	else if(pinInhibit_!=UNUSED)
	{
		digitalWrite(pinInhibit_, inhibit);
	}
}


void Sn74hc138d::setInput(u_int8_t input)
{
	if (pinInput_ != UNUSED)
	{
		digitalWrite(pinInput_, input);
	}
}
