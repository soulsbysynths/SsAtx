/*
* AtxCHardware.cpp
*
* Created: 11/01/2018 01:07:01
* Author: info
*/


#include "AtxCHardware.h"

volatile uint8_t rotEncoderState[3] = {LOW};
const int8_t rotEncoderIncrementAmount[16] = {0,1,-1,2,-1,0,-2,1,1,-2,0,-1,2,-1,1,0};
volatile int16_t  rotEncoderCount[ENCODERS] = {0};


// default constructor
AtxCHardware::AtxCHardware(AtxCHardwareBase* base)
{
	//midiUsbHost = new USBH_MIDI (&usbH);
	construct(base);
} //AtxCHardware

// default destructor
AtxCHardware::~AtxCHardware()
{
	if(base_!=NULL)
	{
		delete base_;
	}
} //~AtxCHardware

void AtxCHardware::construct(AtxCHardwareBase* base)
{
	using namespace atx;
	base_ = base;
	
	for(uint8_t i=PIN_MUX_A;i<PIN_ROT1A;++i)
	{
		pinMode(i,OUTPUT);
	}
	pinMode(ATN,OUTPUT);
	digitalWrite(ATN, HIGH);

	pinMode(PIN_EXTCLKIN,INPUT);
	pinMode(PIN_EXTCLKOUT,OUTPUT);
	digitalWrite(PIN_EXTCLKOUT,LOW);

	encoder_[0] = new Encoder(PIN_ROT1A,PIN_ROT1B);
	encoder_[1] = new Encoder(PIN_ROT2A,PIN_ROT2B);
	encoder_[2] = new Encoder(PIN_ROT3A,PIN_ROT3B);

	//pinMode(PIN_ROT1A, INPUT_PULLUP);
	//attachInterrupt(PIN_ROT1A, AtxCHardware::rot1InterruptA, CHANGE);
	//pinMode(PIN_ROT1B, INPUT_PULLUP);
	//attachInterrupt(PIN_ROT1B, AtxCHardware::rot1InterruptB, CHANGE);
	//pinMode(PIN_ROT2A, INPUT_PULLUP);
	//attachInterrupt(PIN_ROT2A, AtxCHardware::rot2InterruptA, CHANGE);
	//pinMode(PIN_ROT2B, INPUT_PULLUP);
	//attachInterrupt(PIN_ROT2B, AtxCHardware::rot2InterruptB, CHANGE);
	//pinMode(PIN_ROT3A, INPUT_PULLUP);
	//attachInterrupt(PIN_ROT3A, AtxCHardware::rot3InterruptA, CHANGE);
	//pinMode(PIN_ROT3B, INPUT_PULLUP);
	//attachInterrupt(PIN_ROT3B, AtxCHardware::rot3InterruptB, CHANGE);

	//for (uint8_t i=0;i<ENCODERS;++i)
	//{
	//encoder_[i] = new RotaryEncoder(i,this);
	//}


	for(uint8_t i = 0;i<EXPANDERS;++i)
	{
		expander_[i] = new MCP23S17(&SPI, PIN_EXP_CS, i);  //MCP23S17 Bank1(&SPI, PIN_MCP23S17_CS, 0);
	}

	for (uint8_t i=0;i<SWITCHES;++i)
	{
		switch_[i] = new Switch(i,this);// Switch(i,LOW,HOLD_EVENT_TICKS,this);
	}

	analogReadResolution(10);  //must be 8, 10 or 12
	for(uint8_t i=0;i<CTRLS;++i)
	{
		int16_t a = (((int32_t)analogRead(i) - 8) * 256) / 1008;
		a = constrainUChar(a);
		ctrl_[i] =  new AnalogueControl(i,(uint8_t)a,this);
	}

	//OLEDS
	for(uint8_t i=0;i<OLED_GLOBAL;++i)
	{
		oled_[i] = new Oled(i,this,96,32);
	}
	for(uint8_t i=OLED_GLOBAL;i<OLEDS;++i)
	{
		oled_[i] = new Oled(i,this,48,64);
	}
	//oled_[OLED_FUNC]->setOverlayRow(2);  //this may need to be done on fly if more than 1 overlay
	
	oledBuffer_.setOledPtr(oled_[OLED_FUNC]);
	oledBuffer_.setPrintRow(3);
	
}
uint8_t AtxCHardware::initialize()
{
	//things to do after construction

	//pinMode(SDA,INPUT);
	//pinMode(SCL,OUTPUT);
	//for (uint8_t i=0;i<10;++i)
	//{
	//digitalWrite(SCL,LOW);
	//delay(1);
	//digitalWrite(SCL,HIGH);
	//delay(1);
	//}

	Serial.begin(115200);

	for(uint8_t i = 0;i<EXPANDERS;++i)
	{
		expander_[i]->begin();
	}
	SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));  //force through SPI settings for oleds
	for (uint8_t i=0;i<3;++i)
	{
		for(uint8_t j=0;j<16;++j)
		{
			expander_[i]->pinMode(j, OUTPUT);
			expander_[i]->writePort(0x0000);
		}
	}
	for (uint8_t i=0;i<8;++i)
	{
		expander_[RG_BANK]->pinMode(i,OUTPUT);
	}
	for (uint8_t i=8;i<12;++i)
	{
		expander_[RG_BANK]->pinMode(i,INPUT_PULLUP);
	}
	for (uint8_t i=12;i<16;++i)
	{
		expander_[RG_BANK]->pinMode(i,OUTPUT);
	}
	uint8_t sw = expander_[RG_BANK]->readPort(1);
	for (uint8_t i=0;i<SWITCHES;++i)
	{
		if (bitRead(sw,i)==false)
		{
			switch_[i]->setPressed();
		}
	}
	//reset
	for (uint8_t i=0;i<atx::MAX_SYNTHCARDS;++i)
	{
		setSerialReset(i,LOW);
		delay(1);
		setSerialReset(i,HIGH);
		delay(1);
	}
	delay(1000);
	//OLEDS
	// Setup reset pin direction (used by both SPI and I2C)
	//pinMode(rst, OUTPUT);
	expander_[RG_BANK]->digitalWrite(EXPPIN_OLED_GPIO,LOW);
	expander_[RG_BANK]->digitalWrite(EXPPIN_OLED_RESET,HIGH); //digitalWrite(rst, HIGH);
	// VDD (3.3V) goes high at start, lets just chill for a ms
	delay(1);
	// bring reset low
	expander_[RG_BANK]->digitalWrite(EXPPIN_OLED_RESET,LOW); //digitalWrite(rst, LOW);
	// wait 10ms
	delay(10);
	// bring out of reset
	expander_[RG_BANK]->digitalWrite(EXPPIN_OLED_RESET,HIGH); //digitalWrite(rst, HIGH);
	for(uint8_t i=0;i<OLEDS;++i)
	{
		if(i<OLED_GLOBAL)
		{
			oled_[i]->init(0x1f);
		}
		else
		{
			oled_[i]->init(0xf1);
		}
	}
	delay(1);
	expander_[RG_BANK]->digitalWrite(EXPPIN_OLED_GPIO,HIGH);
	delay(1);
	for(uint8_t i=0;i<OLEDS;++i)
	{
		oled_[i]->txDisplayOnOff(true);
	}
	delay(100);
	for(uint8_t i=0;i<OLEDS;++i)
	{
		oled_[i]->clearDisplay();
	}

	setSerialSource(SS_MIDI);


	//before any i2c stuff
	uint8_t initCode = 0;
	for (uint8_t i=0;i<SWITCHES;++i)
	{
		if (switch_[i]->isPressed())
		{
			bitSet(initCode,i);
		}
	}
	beginWire();
	
	return initCode;
}

void AtxCHardware::processInitCode(uint8_t code, AtxCSdCard* sdCard /* = NULL */)
{
	using namespace atx;
	if (code>0 & code<=9)
	{
		uint8_t f,l;
		if (code==9)
		{
			f = 0; l = (MAX_SYNTHCARDS-1);
		}
		else
		{
			f = code-1; l = code-1;
		}
		delay(2000);
		for (uint8_t i = f;i<=l;++i)
		{
			uint8_t o = i >> 1;
			uint8_t r = i & 0x01;
			oled_[o]->printStringBuffer(0,r,"Init ATM:",false,false);
			oled_[o]->printIntBuffer(10,r,i+1,false,true,2);
			oled_[o]->refresh();
			txSerialHex(sdCard,i,FW_ATMEGATRON);
			delay(1000);
		}
	}
	else if (code==15)
	{
		delay(2000);
		oled_[5]->printStringBuffer(0,0,"FREE I2C",false,true);
		oled_[5]->refresh();
		freeI2Clines();  //may just want this as an option
	}
}

void AtxCHardware::beginWire()
{
	Wire.begin();
	Wire.setClock(400000);
}

void AtxCHardware::freeI2Clines()
{
	//pinMode(SDA,INPUT);
	//pinMode(SCL,OUTPUT);
	//for (uint8_t i=0;i<10;++i)
	//{
	//digitalWrite(SCL,LOW);
	//delay(1);
	//digitalWrite(SCL,HIGH);
	//delay(1);
	//}

	//delay(2000);
	//try i2c bus recovery at 100kHz = 5uS high, 5uS low
	pinMode(SDA, OUTPUT);//keeping SDA high during recovery
	digitalWrite(SDA, HIGH);
	pinMode(SCL, OUTPUT);
	for (int i = 0; i < 10; i++) { //9nth cycle acts as NACK
		digitalWrite(SCL, HIGH);
		delayMicroseconds(5);
		digitalWrite(SCL, LOW);
		delayMicroseconds(5);
	}

	//a STOP signal (SDA from low to high while CLK is high)
	digitalWrite(SDA, LOW);
	delayMicroseconds(5);
	digitalWrite(SCL, HIGH);
	delayMicroseconds(2);
	digitalWrite(SDA, HIGH);
	delayMicroseconds(2);
	//bus status is now : FREE

	//return to power up mode
	pinMode(SDA, INPUT);
	pinMode(SCL, INPUT);
	delay(2000);

}

void AtxCHardware::selectOled(uint8_t oled)
{
	if(oled==DESELECT_OLEDS)
	{
		digitalWrite(PIN_MUX_INH,HIGH);  //don't need to pick
	}
	else
	{
		for(uint8_t i=0;i<3;++i)
		{
			digitalWrite(M0PRO_PINSWAP(i+PIN_MUX_A),bitRead(oled,i));
		}
		digitalWrite(PIN_MUX_INH,LOW);
	}
}

void AtxCHardware::refreshAll(uint8_t ticksPassed)
{

	//These ones are polled every 5ms. They only do one item per poll, so not hang too long in subroutine
	static uint8_t refreshTick = 0;
	refreshTick += ticksPassed;
	while(refreshTick>=REFRESH_TICKS)
	{
		refreshTick -= REFRESH_TICKS;
		pollAnlControls();
		pollRotEncoders();
		pollSwitches();
		pollExtClk();
		refreshFlash();
		refreshLeds();
		refreshOleds();
	}
}

void AtxCHardware::refreshFlash()
{
	uint8_t tickInc = 0;

	ledFlashTickCnt_ += REFRESH_TICKS;
	while (ledFlashTickCnt_>=LED_FLASH_SCALE)
	{
		ledFlashTickCnt_ -= LED_FLASH_SCALE;
		tickInc++;
	}
	if(tickInc>0)
	{
		for(uint8_t i=0;i<ENCODERS;++i)
		{
			ledCircular_[i].refreshFlashLayered(tickInc);
		}

		for(uint8_t i=0;i<SWITCHES;++i)
		{
			ledSwitch_[i].refreshFlash(tickInc);
		}
	}
}

void AtxCHardware::refreshLeds()
{
	static uint8_t e = 0;
	//only do one of these expanders
	refreshLed((RotGroup)e);
	e++;
	if (e>=ENCODERS)
	{
		e = 0;
	}
	//but always do this one
	refreshLed(RG_BANK);
}

void AtxCHardware::refreshLed(uint8_t e)
{

	static uint16_t mcpState[EXPANDERS] = {0};
	static uint8_t bankToggle = 0;
	const uint8_t GOLD_BIAS = 3;
	const uint8_t LIME_BIAS = 1;

	if (e<ENCODERS)
	{
		uint16_t circ = ledCircular_[e].getOutput();
		if(circ!=mcpState[e])
		{
			expander_[e]->writePort(circ);
			mcpState[e] = circ;
		}
	}
	else
	{
		uint8_t output = 0;
		LedRgb::LedRgbColour col = ledSwitch_[RG_GLOBAL].getOutput();
		if(bitRead(col,0))
		{
			bitSet(output,1);
		}
		if(bitRead(col,1))
		{
			bitSet(output,0);
		}
		col = ledSwitch_[RG_FUNC].getOutput();
		if(bitRead(col,0))
		{
			bitSet(output,3);
		}
		if(bitRead(col,1))
		{
			bitSet(output,2);
		}
		col = ledSwitch_[RG_VAL].getOutput();
		if(bitRead(col,0))
		{
			bitSet(output,5);
		}
		if(bitRead(col,1))
		{
			bitSet(output,4);
		}

		col = ledSwitch_[RG_BANK].getOutput();
		if(col==LedRgb::RED)
		{
			bitSet(output,7);
		}
		else if(col==LedRgb::GREEN)
		{
			bitSet(output,6);
		}
		else if(col==LedRgb::YELLOW)
		{
			bankToggle++;
			if(bankToggle>GOLD_BIAS)
			{
				bankToggle = 0;
			}
			if(bankToggle<GOLD_BIAS)
			{
				bitSet(output,7);
			}
			else
			{
				bitSet(output,6);
			}
		}
		else
		{
			bankToggle++;
			if(bankToggle>LIME_BIAS)
			{
				bankToggle = 0;
			}
			if(bankToggle<LIME_BIAS)
			{
				bitSet(output,7);
			}
			else
			{
				bitSet(output,6);
			}
		}
		if(output!=mcpState[RG_BANK])
		{
			expander_[RG_BANK]->writePort(0,output);
			mcpState[RG_BANK] = output;
		}
	}
}

void AtxCHardware::refreshOleds()
{
	static uint8_t o = 0; //only do 1 each poll
	oled_[o]->refresh();
	if (oledBuffer_.getReadyToPrint()==true && oledBuffer_.getOledPtr()==oled_[o])
	{
		oledBuffer_.printBuffer();
	}
	o++;
	if (o>=OLEDS)
	{
		o = 0;
	}
}

void AtxCHardware::pollAnlControls()
{
	static const uint8_t TICKS_PASSED = REFRESH_TICKS * CTRLS;
	static uint8_t c = 0;
	int32_t ar = analogRead(c);
	//if (ctrlLog_[c])
	//{
	//if (ar<512)
	//{
	//ar >>= 2;
	//}
	//else
	//{
	//ar = ((ar-512) * 896) >> 9;
	//ar +=  128;
	//}
	//}
	int16_t as = ((ar - 8) * 256) / 1008;
	as = constrainUChar(as);
	ctrl_[c]->setValue((uint8_t)as);
	ctrl_[c]->poll(TICKS_PASSED);
	c++;
	if (c>=CTRLS)
	{
		c = 0;
	}
}

void AtxCHardware::resetCtrlMoving()
{
	for (uint8_t i=0;i<CTRLS;++i)
	{
		ctrl_[i]->setMoving(false);
	}
}

void AtxCHardware::pollRotEncoders()
{
	static uint8_t e = 0;

	if (encoder_[e]->hasChanged())
	{
		base_->hardwareRotaryEncoderValueChanged(e,encoder_[e]->getValue(),encoder_[e]->getDirection());
	}
	e++;
	if (e>=ENCODERS)
	{
		e = 0;
	}
}

void AtxCHardware::pollSwitches()
{
	static const uint8_t TICKS_PASSED = REFRESH_TICKS * SWITCHES;
	static uint8_t input = 0;
	static uint8_t s = 0;
	if (s==0)
	{
		input = expander_[RG_BANK]->readPort(1);
	}
	//switch_[s]->setValue(1-bitRead(input,s));
	switch_[s]->poll(TICKS_PASSED,bitRead(input,s));
	s++;
	if (s>=SWITCHES)
	{
		s = 0;
	}
}

void AtxCHardware::setUsbMidiType(UsbMidiType newType)
{
	usbMidiType_ = newType;
	//if (usbMidiType_==USB_HOST)
	//{
	//usbH = new USBHost();
	//midiH = new USBH_MIDI(usbH);
	//if(usbH->Init())
	//{
	//while(1);
	//}
	//delay(200);
	//}
	//else
	//{
	//delete midiH;
	//delete usbH;
	//}
}

void AtxCHardware::rxMidiUsb()
{

	using namespace midi;
	if (usbMidiType_==USB_DEVICE)
	{
		midiEventPacket_t rx;  //rewrite to just use midimsg
		MidiMsg msg;  //ok not static?
		do
		{
			rx = MidiUSB.read();
			if (rx.header != 0)
			{
				//msg.setHeader(rx.header);
				msg.setStatus(rx.byte1);
				msg.setData1(rx.byte2);
				msg.setData2(rx.byte3);
				base_->hardwareExtMidiRxd(MSRC_USB, &msg);
			}
		} while (rx.header != 0);
	}
	else
	{
		//if (usbH!=NULL)
		//{
		//usbH->Task();
		//}
		//
		//if (midiH!=NULL)
		//{
		//uint8_t outBuf[ 3 ];
		//uint8_t size;
		//
		//do {
		//if ( (size = midiH->RecvData(outBuf)) > 0 ) {
		//MIDI Output
		//Serial1.write(outBuf, size);
		//}
		//} while (size > 0);
		//}
	}

}

void AtxCHardware::rxSerialMidiBuffer()
{
	using namespace midi;

	if (serialSource_==SS_FTDI)
	{
		return;
	}
	//always scan for MIDI & USB
	while(Serial1.available())
	{
		if(midiRxBuffer_.availableForStore(midi::MidiStreamToMsg::SMT_MIDI))  //what about sysex?
		{
			midiRxBuffer_.write(Serial1.read());
		}
		else
		{
			break;
		}
	}
	while(midiRxBuffer_.available(MidiStreamToMsg::SMT_MIDI))
	{
		MidiMsg rx;
		midiRxBuffer_.readMidi(&rx);
		base_->hardwareExtMidiRxd(MSRC_SERIAL, &rx);
	}
}

void AtxCHardware::txSerialMidiBuffer()
{
	if (serialSource_==SS_FTDI)
	{
		return;
	}
	while(midiTxBuffer_.availableForStore())
	{
		Serial1.write(midiTxBuffer_.read());
	}
}



//void AtxCHardware::pollI2cCards(uint8_t txClks)
//{
//using namespace atx;
//static uint8_t reqCard = UNUSED;
//uint8_t msgCntTotal = I2C_TX_PER_POLL;  //2.2ms max window at 275bpm.  mess @ 400khz approx 341uS. 6 mess max
//
//if (txClks>0 && msgCntTotal>0)
//{
//msgCntTotal--;
////txClks--;
//AtxMsg tx;
//tx.setHeader(AtxMsg::AMH_REALTIMEBYTE);
//tx.setMidiCmd(MidiMsg::MCMD_CLOCK);
////tx.setMidiData(2,0);
//tx.setValue(txClks);  //use setValue to force new Ubound, differentiating from a MIDI clk msg.
//txI2cAtxMsg(I2C_GENERAL_CALL, &tx,false);
//}
////if (txClks>0)
////{
////printHardwareErr(16,HWERR_CLKBUFF);
////}
//uint8_t lastCard = atx::UNUSED;
//while (i2cTxBuffer_.available() && msgCntTotal>0)  //empty upto x msgs from buffer
//{
//msgCntTotal--;
//AtxMsgBufferMaster::AtxTxMsg* txmsg = i2cTxBuffer_.readAtxMsgPtr();
//txI2cAtxMsg(txmsg->card, &txmsg->msg,false);
//if (txmsg->card==lastCard)  //absolute max 2 consecutive msgs to a card, to give slave time to catch up
//{
//break;
//}
//lastCard = txmsg->card;
//}
//msgCntTotal += I2C_RX_PER_POLL;  //must do x reads plus whatever is left from the writes
//while (msgCntTotal>0 && keybedCard_!=UNUSED)
//{
//msgCntTotal--;  //do this whatever so can't get stcuk in while
//AtxMsg rx;
//reqI2cAtxMsg(keybedCard_,&rx,false);
//if (rx.getHeader()!=AtxMsg::AMH_BLANK)
//{
//if (rx.getHeader()==AtxMsg::AMH_COMMON2)
//{
//printRxdErr(reqCard,rx.getValue());
//}
//else
//{
//base_->hardwareI2cPacketRxd(keybedCard_,&rx);
//}
//}
//else
//{
//break;
//}
//
//}
//reqCard = nextConnectedCard(reqCard);
//uint8_t firstCard = reqCard;
//while(msgCntTotal>0 && reqCard!=UNUSED)
//{
//msgCntTotal--;  //do this whatever so can't get stcuk in while
//AtxMsg rx;
//reqI2cAtxMsg(reqCard,&rx,false);
//if (rx.getHeader()!=AtxMsg::AMH_BLANK)
//{
//if (rx.getHeader()==AtxMsg::AMH_COMMON2)
//{
//printRxdErr(reqCard,rx.getValue());
//}
//else
//{
//base_->hardwareI2cPacketRxd(reqCard,&rx);
//}
//
//
//}
//uint8_t nextCard = nextConnectedCard(reqCard);
//if (nextCard==firstCard)  //already done that one this ppqn
//{
//break;
//}
//else
//{
//reqCard = nextCard;
//}
//}
//Wire.writeStopBit();
//}

//uint8_t AtxCHardware::nextConnectedCard(uint8_t startCard)
//{
//using namespace atx;
//for(uint8_t i=0;i<MAX_CARDS;++i)
//{
//startCard++;
//startCard &= MAX_CARDS_MASK;
//if (card_[startCard]->connected)
//{
//return startCard;
//}
//}
//return UNUSED;
//}
void AtxCHardware::pollExtClk()
{
	static uint8_t lastState = LOW;
	uint8_t newState = digitalRead(PIN_EXTCLKIN);
	//ledSwitch_[RG_GLOBAL].setColour((LedRgb::LedRgbColour) newState);
	if (lastState==LOW && newState==HIGH)
	{

		base_->hardwareExtClk();
	}
	lastState = newState;
}

void AtxCHardware::initCard(atx::AtxCard* card, bool oledDisplay /* = true */)
{
	using namespace atx;
	using namespace midi;
	
	const uint8_t TRIES = 5;

	uint8_t o,r;
	int8_t id = card->getId();
	o = id >> 1;
	r = id & 0x01;
	if (id >= 12)
	{
		r <<= 1;
	}
	if (oledDisplay)
	{
		oled_[o]->printIntBuffer(0,r,id+1,false,false,2);
		oled_[o]->printStringBuffer(2,r,":TRY",false,true);
		oled_[o]->refresh();
	}

	card->connected = false;
	SysexMsg tx;// = SysexMsg(1);
	tx.setSize(1);
	tx.setData(0,MSXT_HELLO);
	bufferI2cSysex(id,&tx);
	for (uint8_t i=0;i<TRIES;++i)
	{
		txI2cMidiBuffer(0,0);
		delay(4);  //give slave a chance to fill the tx buffer
		rxI2cMidiBuffer(id);
		if(i2cRxBuffer_[id].available(midi::MidiStreamToMsg::SMT_SYSEX))
		{
			break;
		}
		else
		{
			delay(20);
		}
	}

	if (i2cRxBuffer_[id].available(midi::MidiStreamToMsg::SMT_SYSEX))
	{
		SysexMsg rx;// = SysexMsg(i2cRxBuffer_[id].peekSysexSize());
		i2cRxBuffer_[id].readSysex(&rx);
		if(rx.getData(midi::SYSEX_MSG_INDEX)==MSXT_HELLO)
		{
			card->connected = true;
			card->cardType = (CardType)rx.getData(1);
			if(id<MAX_ZONES)
			{
				((AtxSynthCard*)card)->setFirmware((Firmware)rx.getData(2));
			}
			else
			{
				((AtxPolyCard*)card)->setFirmware((Firmware)rx.getData(2));
			}
			if (oledDisplay)
			{
				base_->hardwareInitCard(id);
			}
		}
	}
	else if(oledDisplay)
	{
		//printHardwareErr(i,HWERR_CARDINITERR);
		oled_[o]->printStringBuffer(2,r,":DISCON",false,true);
		oled_[o]->refresh();
	}
}

void AtxCHardware::setSerialSource(SerialSource newSource)
{
	expander_[RG_BANK]->digitalWrite(EXPPIN_SERIALSRC_SW,(uint8_t)newSource);
	Serial1.end();
	if (newSource==SS_FTDI)
	{
		Serial1.begin(SERIAL_FTDI_BAUD);
		//Serial1.setTimeout(100);
	}
	else
	{
		Serial1.begin(SERIAL_MIDI_BAUD);
		//Serial1.setTimeout(1000);
	}
}

void AtxCHardware::toggleSerialReset(uint8_t synthCard)
{
	using namespace atx;
	if(synthCard>=MAX_SYNTHCARDS) return;
	setSerialReset(synthCard,LOW);
	delay(1);
	setSerialReset(synthCard,HIGH);

}

void AtxCHardware::setSerialReset(uint8_t synthCard, uint8_t newValue)
{
	using namespace atx;
	if(synthCard>=MAX_SYNTHCARDS) return;
	for(uint8_t i=0;i<3;++i)
	{
		digitalWrite(M0PRO_PINSWAP(i+PIN_MUX_A),bitRead(synthCard,i));
	}
	expander_[RG_BANK]->digitalWrite(EXPPIN_SERIALRST_INH,newValue);
}

void AtxCHardware::txWireHex(AtxCSdCard& sdCard)
{
	//static const uint8_t FX_EEPROM_PAGE_SIZE = 32;
	//static const uint16_t FX_EEPROM_SIZE = 4096;
	//static const uint8_t FX_EEPROM_PAGES = (FX_EEPROM_SIZE / FX_EEPROM_PAGE_SIZE);
	//static const uint8_t card = AtxCard::getHardwareID(AtxCard::HW_MIXER);
	//stk500::avrmem fxEepromBuffer;
	//uint8_t fxEepromMem[FX_EEPROM_PAGE_SIZE] = {0};
	//fxEepromBuffer.buf = &fxEepromMem[0];
	//
	//if (sdCard.setFunction(AtxCSdCard::SD_LOADHEX)==false)
	//{
	//printHardwareErr(card,HWERR_FX);
	//return;
	//}
	//sdCard.setHexPageSize(FX_EEPROM_PAGE_SIZE);
	//char fp[20] = "/FIRMWARE/";
	////strcat(fp,AtxCard::getFiremwareNamePtr(fw));
	////strcat(fp,".hex");
	//File f;
	//if (sdCard.openFileRead(f,fp)==false)
	//{
	//sdCard.setFunction(AtxCSdCard::SD_IDLE);
	//printHardwareErr(card,HWERR_FX);
	//return;
	//}
	//
	//uint8_t page = 0, b = 0;
	//AtxMsg rx;
	//while (sdCard.readHexFilePage(&f,&fxEepromBuffer) > 0)
	//{
	//AtxMsg tx;
	//tx.setHeader(AtxMsg::AMH_PARAM);
	//tx.setParam(atx_params::MIX_FX_TX);
	//tx.setMsgByte(2,0);
	//tx.setMsgByte(3,fxEepromBuffer.buf[b]);
	//tx.setRequest(true);
	//txI2cAtxMsg(card,&tx,true);
	//b++;
	//if(b==FX_EEPROM_PAGE_SIZE)
	//{
	//b = 0;
	//page++;
	//delay(5);  //slave write time to fram
	//}
	//else
	//{
	//delay(1);  //time to allow slave to resond
	//}
	//ledCircular_[RG_VAL].fill(sdCard.calcStatusBar(&f) >> 4);
	//reqI2cAtxMsg(card,&rx,true);
	//for (uint8_t i=0;i<10;++i)
	//{
	//if (rx.getHeader()==AtxMsg::AMH_PARAM && rx.getParam()==atx_params::MIX_FX_TX)// && rx.getMsgByte(3)==page && rx.getMsgByte(2)==b)
	//{
	//page = rx.getMsgByte(3);
	//b = rx.getMsgByte(2);
	//if (page==FX_EEPROM_PAGES)
	//{
	//return;  //all done  success
	//}
	//else
	//{
	//break;
	//}
	//}
	//else
	//{
	//printHardwareErr(card,HWERR_FX);
	////return;
	//}
	//}
	//}
}

void AtxCHardware::txSerialHex(AtxCSdCard* sdCard, uint8_t synthCard, uint8_t fw)
{
	using namespace atx;
	if(synthCard>=MAX_SYNTHCARDS) return;
	Wire.end();
	uint16_t state = ledCircular_[RG_VAL].getState();
	static const uint8_t HEX_PAGE_SIZE = 128;
	setSerialSource(SS_FTDI);
	stk500::avrmem mybuf;
	uint8_t mempage[HEX_PAGE_SIZE] = {0};
	mybuf.buf = &mempage[0];
	setSerialReset(synthCard, HIGH);
	unsigned int major = 0;
	unsigned int  minor = 0;
	delay(100);
	setSerialReset(synthCard, LOW);
	//toggleSerialReset(card);
	delay(10);
	stk500::drain();
	stk500::getsync();
	stk500::getparm(Parm_STK_SW_MAJOR, &major);
	//DEBUGP("software major: ");
	//DEBUGPLN(major);
	stk500::getparm(Parm_STK_SW_MINOR, &minor);
	//DEBUGP("software Minor: ");
	//DEBUGPLN(minor);
	
	//NEED TO REPLACE THIS - WHAT TO DO IF HEX FILE NOT THERE
	//if (sdCard->setFunction(AtxCSdCard::SD_LOADHEX)==false)
	//{
	//printHardwareErr(0,HWERR_CARDINITERR);
	//beginWire();
	//return;
	//}

	sdCard->setHexPageSize(HEX_PAGE_SIZE);
	char fp[20] = "/FIRMWARE/";
	strcat(fp,AtxCcSet::getFirmwareNamePtr((Firmware)fw,false));
	strcat(fp,".hex");
	File f;
	if (sdCard->openFileRead(f,fp)==false)
	{
		//DEBUGP(filename);
		//DEBUGPLN(" doesn't exist");
		//sdCard->setFunction(AtxCSdCard::SD_IDLE);
		beginWire();
		return;
	}
	//enter program mode
	stk500::program_enable();

	while (sdCard->readHexFilePage(&f, &mybuf) > 0)
	{
		//pollI2cCards();
		stk500::loadaddr(mybuf.pageaddress>>1);
		stk500::paged_write(&mybuf, mybuf.size, mybuf.size);
		ledCircular_[RG_VAL].fill(sdCard->calcStatusBar(&f) >> 4);
		refreshLed(RG_VAL);
	}

	// could verify programming by reading back pages and comparing but for now, close out
	stk500::disable();
	delay(10);
	toggleSerialReset(synthCard);
	//sdCard_->loadComplete();
	//sdCard->setFunction(AtxCSdCard::SD_IDLE);
	//blinky(4,500);
	setSerialSource(SS_MIDI);
	f.close();
	ledCircular_[RG_VAL].setState(state);

	delay(500);
	beginWire();

}

void AtxCHardware::bufferI2cMidiMsg(int8_t card, const midi::MidiMsg * msg)
{
	if(i2cTxBuffer_[card].availableForStore()<msg->getMsgSize())
	{
		txI2cMidiBuffer(0,0);
		delay(10); //allow for buffer to be processed
		pollI2cMidiBuffer(card);
		delay(10);
	}
	i2cTxBuffer_[card].writeMidi(msg);
}
void AtxCHardware::bufferI2cByte(int8_t card, const uint8_t b)
{
	i2cTxBuffer_[card].availableForStore() ? i2cTxBuffer_[card].writeByte(b) : printHardwareErr(card,HWERR_I2CTX);
}
void AtxCHardware::bufferI2cSysex(int8_t card, const midi::SysexMsg* sysex)
{
	i2cTxBuffer_[card].availableForStore()>=(sysex->getSize()+5) ? i2cTxBuffer_[card].writeSysex(sysex) : printHardwareErr(card,HWERR_I2CSYSEXBUFFER);
}
void AtxCHardware::txI2cMidiBuffer(uint8_t clocks /* = 0 */, uint8_t transport /* = 0 */)
{
	using namespace midi;
	if(clocks>0)
	{
		Wire.beginTransmission(I2C_GENERAL_CALL + I2C_SLAVE_OFFSET);
		for(uint8_t i=0;i<clocks;++i)
		{
			if(Wire.write((uint8_t)MCMD_CLOCK)==0)
			{
				Wire.endTransmission(true);
				return;
			}
		}
		Wire.endTransmission(false);
	}
	if(transport>0)
	{
		Wire.beginTransmission(I2C_GENERAL_CALL + I2C_SLAVE_OFFSET);
		if(Wire.write(transport)==0)
		{
			Wire.endTransmission(true);
			return;
		}
		Wire.endTransmission(false);
	}
	for(uint8_t i=0;i<atx::MAX_CARDS;++i)
	{
		if(i2cTxBuffer_[i].available())
		{
			Wire.beginTransmission(i + I2C_SLAVE_OFFSET);
			while(i2cTxBuffer_[i].available())
			{
				if(Wire.write(i2cTxBuffer_[i].read())==0)
				{
					Wire.endTransmission(true);
					return;
				}
			}
			Wire.endTransmission(false);
		}
	}
	Wire.writeStopBit();
}
void AtxCHardware::rxI2cMidiBuffer(int8_t card)
{
	using namespace midi;
	uint8_t bytes = Wire.requestFromAtx(card + I2C_SLAVE_OFFSET,true);
	//if(bytes>i2cRxBuffer_[card].availableForStore())  //NO because availableForStore is bytes, not MidiMsgs. Could delete availableForStore actually
	//{
	//printHardwareErr(card, HWERR_I2CRX);
	//return;
	//}
	//else
	//{
	//while(Wire.available())
	//{
	//uint8_t b = Wire.read();
	//i2cRxBuffer_[card].write(b);
	//}
	//}
	while(Wire.available())
	{
		if(!i2cRxBuffer_[card].isFull())  //and sysex full?
		{
			uint8_t b = Wire.read();
			i2cRxBuffer_[card].write(b);
		}
		else
		{
			printHardwareErr(card, HWERR_I2CRX);
			break;
		}
	}
}

void AtxCHardware::pollI2cMidiBuffer(int8_t card)
{
	using namespace midi;
	using namespace atx;
	while(i2cRxBuffer_[card].available(MidiStreamToMsg::SMT_MIDI))
	{
		MidiMsg rx;
		i2cRxBuffer_[card].readMidi(&rx);
		base_->hardwareI2cMidiRxd(card,&rx);
	}

	while(i2cRxBuffer_[card].available(MidiStreamToMsg::SMT_SYSEX))
	{
		SysexMsg sysex;
		i2cRxBuffer_[card].readSysex(&sysex);
		base_->hardwareI2cSysexRxd(card,&sysex);
	}
}

//void AtxCHardware::txI2cAtxMsg(int8_t card, const AtxMsg * msg, bool stopBit)
//{
//if (Wire.beginTransmission(card + I2C_SLAVE_OFFSET));
//{
//if (Wire.write(msg->getMsgPtr(),AtxMsg::MSG_SIZE)==AtxMsg::MSG_SIZE)
//{
//if (Wire.endTransmission(stopBit)==0)
//{
//return;
//}
//}
//}
//printHardwareErr(card,HWERR_I2CTX);
//}
//
//void AtxCHardware::bufferI2cAtxMsg(int8_t card, const AtxMsg * msg)
//{
//if (i2cTxBuffer_.isFull())
//{
//printHardwareErr(card,HWERR_I2CBUFF);
////drain the buffer
//uint8_t cnt = 0;
//while (i2cTxBuffer_.available())
//{
//AtxMsgBufferMaster::AtxTxMsg* txmsg = i2cTxBuffer_.readAtxMsgPtr();
//txI2cAtxMsg(txmsg->card, &txmsg->msg,false);
//cnt++;
//if (cnt>=I2C_TX_PER_POLL)
//{
//Wire.writeStopBit();
//delay(3); //there may be many consecutive calls to same card, so need long delay for slave to process each one
//cnt = 0;
//}
//}
//}
//i2cTxBuffer_.writeAtxMsg(card,msg);
//}

//void AtxCHardware::reqI2cAtxMsg(uint8_t card, AtxMsg * msg, bool stopBit)
//{
//if (Wire.requestFrom(card+I2C_SLAVE_OFFSET, AtxMsg::MSG_SIZE,stopBit)==AtxMsg::MSG_SIZE)
//{
//for (uint8_t i=0;i<AtxMsg::MSG_SIZE;++i)
//{
//msg->setMsgByte(i,Wire.read());
//}
//}
//else
//{
//while(Wire.available()){Wire.read();}
//printHardwareErr(card,HWERR_I2CRX);
//}
//}

void AtxCHardware::printOledLogo(uint8_t oled)
{
	uint8_t x = 0;
	uint8_t y = 0;
	for (uint8_t i=Oled::FONT_LOGO;i<(Oled::FONT_LOGO+24);++i)
	{
		oled_[oled]->printCharBuffer(x,y,i,false);
		if (x==11)
		{
			x = 0;
			y = 1;
		}
		else
		{
			x++;
		}
	}
}

void AtxCHardware::printHardwareErr(uint8_t card, HwError error)
{
	static uint16_t errCnt = 0;
	static uint8_t oledPos = 0;
	uint8_t oled = card % 6;
	static const char* const S_MSG[HWERR_MAX] {"I2C TX","I2C RX","I2C XB","I2C XP","SR SYN","SR PRM","SR PRG","SD INI","SD FLE","CLK BF","CD INI","FX TX","MS O/F","SQ O/F"};
	errCnt++;
	oled_[oled]->printIntBuffer(0,1,errCnt,false,false,5);
	oled_[oled]->printStringBuffer(5,1,":",false,true);
	oled_[oled]->printStringBuffer(0,0,"Er:  :",false,false);
	oled_[oled]->printIntBuffer(3,0,card,false,false,1);
	oled_[oled]->printStringBuffer(6,0,S_MSG[error],false,false);
	oled_[oled]->refresh();
}

void AtxCHardware::printRxdErr(uint8_t card, uint8_t error)
{
	static uint16_t errCnt = 0;
	static uint8_t oledPos = 0;
	const uint8_t OLED = 3;
	const uint8_t ROW = 0;
	errCnt++;
	oled_[OLED-1]->printIntBuffer(0,ROW,errCnt,false,false,5);
	oled_[OLED-1]->printStringBuffer(5,ROW,":",false,true);
	oled_[OLED]->printStringBuffer(0,ROW,"Rx:  :",false,false);
	oled_[OLED]->printIntBuffer(3,ROW,card,false,false,1);
	for (uint8_t i=0;i<6;++i)
	{
		oled_[OLED]->printCharBuffer(11-i,0,'0' + bitRead(error,i),false);
	}
}

void AtxCHardware::setOledOnMap(uint8_t newMap)
{

	for (uint8_t i=0;i<OLEDS;++i)
	{
		if(bitRead(newMap,i)!=bitRead(oledOnMap_,i))
		{
			oled_[i]->txDisplayOnOff(bitRead(newMap,i));
		}
	}
	oledOnMap_ = newMap;
}

//*************** OLED EVENTS***********************
void AtxCHardware::oledTxCommand(uint8_t index, uint8_t command)
{
	selectOled(DESELECT_OLEDS); //CS=0;
	digitalWrite(PIN_OLED_DC,HIGH); //DC=0;
	selectOled(index); //CS=0;
	SPI.transfer(command);//P1=ins; /*inst*/
	selectOled(DESELECT_OLEDS);//CS=1;
}

void AtxCHardware::oledTxData(uint8_t index, const uint8_t * data, uint_fast16_t size, bool wide)
{
	// SPI
	selectOled(DESELECT_OLEDS); //CS=0;
	digitalWrite(PIN_OLED_DC,LOW); //DC=0;
	selectOled(index); //CS=0;
	if(wide)
	{
		for (uint_fast16_t i=0; i<size; i++) {
			SPI.transfer(data[i]);
			SPI.transfer(data[i]);
		}
	}
	else
	{
		for (uint16_t i=0; i<size; i++) {
			SPI.transfer(data[i]);
		}
	}
	selectOled(DESELECT_OLEDS);//CS=1;
}

void AtxCHardware::oledTxZeroData(uint8_t index, uint_fast16_t size)
{
	selectOled(DESELECT_OLEDS); //CS=0;
	digitalWrite(PIN_OLED_DC,LOW); //DC=0;
	selectOled(index); //CS=0;
	for (uint16_t i=0; i<size; i++) {  //page end addr+1 * column end addr+1
		SPI.transfer(0);
	}
	selectOled(DESELECT_OLEDS);//CS=1;
}
