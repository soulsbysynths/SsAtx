#pragma once
#include <Arduino.h>

class Sn74hc138d 
{
public:
	Sn74hc138d(uint8_t pinA, uint8_t pinB, uint8_t pinC);
	Sn74hc138d(uint8_t pinA, uint8_t pinB, uint8_t pinC, uint8_t pinInhibit);
	Sn74hc138d(uint8_t pinA, uint8_t pinB, uint8_t pinC, uint8_t pinInhibit, uint8_t pinInput);
	Sn74hc138d(uint8_t pinA, uint8_t pinB, uint8_t pinC, void(*initialisePins)(void), void(*setInhibit)(bool));
	Sn74hc138d(uint8_t pinA, uint8_t pinB, uint8_t pinC, void(*initialisePins)(void), void(*setInhibit)(bool), uint8_t pinInput);
	void initialise();
	void setLine(uint8_t line);
	void setInhibit(bool inhibit);
	void setInput(u_int8_t input);
private:
	static const u_int8_t UNUSED = 0xFF;
	static const u_int8_t SELECT_LINES = 3;
	uint8_t pinABC_[3] = { UNUSED, UNUSED, UNUSED };
	uint8_t pinInhibit_ = UNUSED;
	uint8_t pinInput_ = UNUSED;
	void(*initialisePins_)(void) = NULL;
	void(*setInhibit_)(bool) = NULL;
};