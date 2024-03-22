/*
* AtxCHardware.h
*
* Created: 11/01/2018 01:07:02
* Author: info
*/


#ifndef __ATXCHARDWARE_H__
#define __ATXCHARDWARE_H__

//#include <Arduino.h>
//#include <SPI.h>
//#include <Wire.h>
#include <string.h>
#include "Atx.h"
#include "AtxCcSet.h"
#include "AtxCard.h"
#include "AtxCHardwareBase.h"
#include "Switch.h"
#include "AnalogueControl.h"
#include "MCP23S17.h"
#include "Oled.h"
#include "OledBuffer.h"
#include "LedCircular.h"
#include "LedRgb.h"
#include "MIDIUSB.h"
#include "MidiMsg.h"
#include "MidiMsgToStream.h"
#include "MidiStreamToMsg.h"
//#include "AtxMsg.h"
//#include "AtxMsgBuffer.h"
#include "Helpers.h"
#include "Encoder.h"
#include "stk500.h"
#include "AtxCSdCard.h"
//#include "usbh_midi.h"

#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif

#define ENCODERS 3
#define EXPANDERS 4
//#define CTRLS 6
#define OLEDS 8
#define SWITCHES 4

#define PIN_MUX_A 2
#define PIN_MUX_INH 5
#define PIN_EXP_CS 6
#define PIN_OLED_DC 7
#define PIN_ROT1A 8
#define PIN_ROT1B 9
#define PIN_ROT2A 10
#define PIN_ROT2B 11
#define PIN_ROT3A 12
#define PIN_ROT3B 13
#define PIN_EXTCLKIN 25
#define PIN_EXTCLKOUT 26

#define EXPPIN_SERIALSRC_SW 12
#define EXPPIN_SERIALRST_INH 13
#define EXPPIN_OLED_RESET 14
#define EXPPIN_OLED_GPIO 15
#define DESELECT_OLEDS 8

#ifdef ARDUINO_SAMD_M0_PRO
#define M0PRO_PINSWAP(pin) (((pin)==2||(pin)==4) ? (6-(pin)) : (pin))
#else
#define M0PRO_PINSWAP(pin) (pin)
#endif


#define SERIAL_FTDI_BAUD 115200
#define SERIAL_MIDI_BAUD 31250

class AtxCHardware : public SwitchBase, AnalogueControlBase, OledBase
{
	//variables
	public:
	static const uint8_t CTRLS = 6;
	static const uint16_t HOLD_EVENT_TICKS = 2000;
	static const uint8_t LED_FLASH_SCALE = 16;
	static const uint8_t OLED_GLOBAL = 6;
	static const uint8_t OLED_FUNC = 7;
	
	static const int8_t I2C_SLAVE_OFFSET = 8;
	static const int8_t I2C_GENERAL_CALL = -8;
	static const uint8_t I2C_TX_PER_POLL = 5;
	static const uint8_t I2C_RX_PER_POLL = 2;
	static const uint8_t REFRESH_TICKS = 5;
	enum RotGroup : uint8_t
	{
		RG_GLOBAL = 0,
		RG_FUNC,
		RG_VAL,
		RG_BANK
	};
	enum SerialSource : uint8_t
	{
		SS_MIDI = 0,
		SS_FTDI
	};
	enum UsbMidiType : uint8_t
	{
		USB_DEVICE = 0,
		USB_HOST
	};
	enum HwError : uint8_t
	{
		HWERR_I2CTX = 0,
		HWERR_I2CRX,
		HWERR_I2CSYSEXBUFFER,
		HWERR_I2CSYSEXPOLL,
		HWERR_SERIALSYNC,
		HWERR_SERIALPARAM,
		HWERR_SERIALPROG,
		HWERR_SDCARDINIT,
		HWERR_SDCARDFILE,
		HWERR_CLKBUFF,
		HWERR_CARDINITERR,
		HWERR_FX,
		HWERR_SEQTMROF,
		HWERR_MSTMROF,
		HWERR_MAX
	};
	protected:
	private:
	static const uint_fast16_t MIDI_SYSEX_MAX_TICKS = 1024;
	static const uint_fast16_t MIDI_SYSEX_MIN_TICKS = 512;
	AtxCHardwareBase* base_;
	Encoder* encoder_[ENCODERS];
	Switch* switch_[SWITCHES];
	AnalogueControl* ctrl_[CTRLS];
	//bool ctrlLog_[CTRLS] = {false};
	Oled* oled_[OLEDS];
	OledBuffer oledBuffer_;
	MCP23S17* expander_[EXPANDERS];
	LedCircular ledCircular_[ENCODERS];
	LedRgb ledSwitch_[SWITCHES];
	uint8_t ledFlashTickCnt_ = 0;
	uint8_t oledOnMap_ = 0xFF;
	SerialSource serialSource_ = SS_MIDI;
	//USBHost * usbH;
	//USBH_MIDI * midiH;
	UsbMidiType usbMidiType_ = USB_DEVICE;
	midi::MidiMsgToStream i2cTxBuffer_[atx::MAX_CARDS];
	midi::MidiStreamToMsg i2cRxBuffer_[atx::MAX_CARDS];// = {32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32};
	midi::MidiMsgToStream midiTxBuffer_{64};
	midi::MidiStreamToMsg midiRxBuffer_{64,0};
	
	//functions
	public:
	static AtxCHardware& getInstance()
	{
		static AtxCHardware instance; // Guaranteed to be destroyed.
		return instance;  // Instantiated on first use.
	}
	void construct(AtxCHardwareBase* base);
	uint8_t initialize();
	const Switch* getSwitchPtr(uint8_t index) const { return  switch_[index]; }
	Switch* getSwitchPtr(uint8_t index) { return switch_[index]; }
	const AnalogueControl* getCtrlPtr(uint8_t index) const { return  ctrl_[index]; }
	AnalogueControl* getCtrlPtr(uint8_t index) { return ctrl_[index]; }
	const Encoder* getRotaryEncoderPtr(uint8_t index) const { return  encoder_[index]; }
	Encoder* getRotaryEncoderPtr(uint8_t index) { return encoder_[index]; }
	const Oled* getOledPtr(uint8_t index) const { return  oled_[index]; }
	Oled* getOledPtr(uint8_t index) { return oled_[index]; }
	OledBuffer& getOledBuffer() { return oledBuffer_; }
	const OledBuffer& getOledBuffer() const { return oledBuffer_; }
	LedCircular& getLedCircular(uint8_t index) { return ledCircular_[index]; }
	const LedCircular& getLedCircular(uint8_t index) const { return ledCircular_[index]; }
	LedRgb& getLedSwitch(uint8_t index) { return ledSwitch_[index]; }
	const LedRgb& getLedSwitch(uint8_t index) const { return ledSwitch_[index]; }
	void refreshAll(uint8_t ticksPassed);
	void refreshOleds();
	void refreshLeds();
	void resetCtrlMoving();
	void refreshLed(uint8_t rg);
	void rxI2cMidiBuffer(int8_t card);
	void txI2cMidiBuffer(uint8_t clocks = 0, uint8_t transport = 0);
	void pollI2cMidiBuffer(int8_t card);
	void bufferI2cMidiMsg(int8_t card, const midi::MidiMsg* msg);
	void bufferI2cByte(int8_t card, const uint8_t b);
	void bufferI2cSysex(int8_t card, const midi::SysexMsg* sysex);
	uint8_t getSynthsConnected();
	uint8_t getSynthsConnectedMap();
	void initCard(atx::AtxCard* card, bool oledDisplay = true);
	void setSerialSource(SerialSource newSource);
	SerialSource getSerialSource(){return serialSource_;};
	void toggleSerialReset(uint8_t card);
	void setSerialReset(uint8_t card, uint8_t newValue);
	void txSerialHex(AtxCSdCard* sdCard, uint8_t card, uint8_t fw);
	void txSerialMidiBuffer();
	void rxSerialMidiBuffer();
	void bufferSerialMidiMsg(const midi::MidiMsg * msg){midiTxBuffer_.writeMidi(msg);}
	void rxMidiUsb();
	void txWireHex(AtxCSdCard& sdCard);
	void printOledLogo(uint8_t oled);
	void setOledOnMap(uint8_t newMap);
	void setUsbMidiType(UsbMidiType newType);
	static void setExtClkOut(bool way){digitalWrite(PIN_EXTCLKOUT,!way);}
	void printHardwareErr(uint8_t card, HwError error);
	void printRxdErr(uint8_t card, uint8_t error);
	void freeI2Clines();
	void processInitCode(uint8_t code, AtxCSdCard* sdCard = NULL);
	//void setCtrlLog(uint8_t ctrl, bool newValue){ctrlLog_[ctrl] = newValue;}
	//bool getCtrlLog(uint8_t ctrl){return ctrlLog_[ctrl];}
	//void clearCtrlLog(){memset(ctrlLog_,false,sizeof(ctrlLog_));}
	//uint8_t nextConnectedCard(uint8_t startCard);
	protected:
	private:
	void selectOled(uint8_t oled);
	void swValueChanged(uint8_t index, bool newValue){base_->hardwareSwitchChanged(index,newValue);}
	void swClicked(uint8_t index){base_->hardwareSwitchClicked(index);}
	void swDoubleClicked(uint8_t index){base_->hardwareSwitchDoubleClicked(index);}
	void swHeld(uint8_t index){base_->hardwareSwitchHeld(index);}
	void oledTxCommand(uint8_t index, uint8_t command);
	void oledTxData(uint8_t index, const uint8_t * data, uint_fast16_t size, bool wide);
	void oledTxZeroData(uint8_t index, uint_fast16_t size);
	void actrlValueChanged(uint8_t index, uint8_t newValue){base_->hardwareCtrlValueChanged(index,newValue);}
	void actrlMovingChanged(uint8_t index, bool newValue){}
	void actrlLatchedChanged(uint8_t index, bool newValue){}
	int_fast16_t clearI2c();
	const char * getFolderName(const char * path);
	void pollAnlControls();
	void pollSwitches();
	void pollRotEncoders();
	void pollExtClk();
	void refreshFlash();
	//void txI2cAtxMsg(int8_t card, const AtxMsg * msg, bool stopBit);
	//void reqI2cAtxMsg(uint8_t card, AtxMsg * msg, bool stopBit);
	
	void beginWire();
	//void rotValueChanged(uint8_t index, int8_t newValue){base_->hardwareRotaryEncoderValueChanged(index,newValue);}
	AtxCHardware() {}
	AtxCHardware(AtxCHardwareBase* base);
	~AtxCHardware();
	AtxCHardware( const AtxCHardware &c );
	AtxCHardware& operator=( const AtxCHardware &c );
}; //AtxCHardware

#endif //__ATXCHARDWARE_H__
