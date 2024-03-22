#include <Arduino.h>
#line 1 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Mcp23s17Tester\\Mcp23s17Tester\\sketches\\Mcp23s17Tester.ino"
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_MCP23X08.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_MCP23XXX.h>

#define CLK_OUT 26
#define CS 6
//#define MISO 22
//#define MOSI 23
//#define SCK 24
#define ADDR 1

Adafruit_MCP23X17 mcp;

#line 18 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Mcp23s17Tester\\Mcp23s17Tester\\sketches\\Mcp23s17Tester.ino"
void setup();
#line 34 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Mcp23s17Tester\\Mcp23s17Tester\\sketches\\Mcp23s17Tester.ino"
void loop();
#line 18 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Mcp23s17Tester\\Mcp23s17Tester\\sketches\\Mcp23s17Tester.ino"
void setup()
{
	pinMode(CLK_OUT, OUTPUT);
	if (!mcp.begin_SPI(CS, &SPI, 1))
	{
		printf("MCP23S17 not initialised");
		while (1) ;
	}
	
	mcp.enableAddrPins();
	for (uint8_t i = 0; i < 16; i++)
	{
		mcp.pinMode(i, OUTPUT);
	}
}

void loop()
{
	static int pin = 0;
	digitalWrite(CLK_OUT, HIGH);
	mcp.digitalWrite(pin, HIGH);
	delay(1000);
	digitalWrite(CLK_OUT, LOW);
	mcp.digitalWrite(pin, LOW);
	delay(1000);
	pin++;
	pin &= 0x0F;
}

