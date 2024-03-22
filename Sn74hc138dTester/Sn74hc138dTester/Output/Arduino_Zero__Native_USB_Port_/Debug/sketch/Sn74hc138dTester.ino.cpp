#include <Arduino.h>
#line 1 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Sn74hc138dTester\\Sn74hc138dTester\\sketches\\Sn74hc138dTester.ino"
#include <Sn74hc138d.h>
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_MCP23X08.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_MCP23XXX.h>

#define PIN_CLK 26
#define PIN_DTR 13

void initialiseDemux3();
void setDemux3Inhibit(bool way);

Sn74hc138d demux1(4, 3, 2, 5);
Sn74hc138d demux2(4, 3, 2, 5, 7);
Sn74hc138d demux3(4, 3, 2, &initialiseDemux3, &setDemux3Inhibit);
Adafruit_MCP23X17 mcp;

#line 21 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Sn74hc138dTester\\Sn74hc138dTester\\sketches\\Sn74hc138dTester.ino"
void setup();
#line 29 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Sn74hc138dTester\\Sn74hc138dTester\\sketches\\Sn74hc138dTester.ino"
void loop();
#line 21 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Sn74hc138dTester\\Sn74hc138dTester\\sketches\\Sn74hc138dTester.ino"
void setup()
{
	pinMode(PIN_CLK, OUTPUT);
	demux1.initialise();
	demux2.initialise();
	demux3.initialise();
}

void loop()
{
	static uint8_t line = 0;
	static bool demux3inhibit = false;
	digitalWrite(PIN_CLK, HIGH);
	demux1.setLine(line);
	demux1.setInhibit(true);
	delay(100);
	digitalWrite(PIN_CLK, LOW);
	demux1.setInhibit(false);
	delay(100);
	line++;
	line &= 0x07;
	if (line == 0)
	{
		demux3inhibit = !demux3inhibit;
		demux3.setInhibit(demux3inhibit);
	}
}

void initialiseDemux3()
{
	mcp.begin_SPI(6, &SPI, 3);
	mcp.enableAddrPins();
	mcp.pinMode(PIN_DTR, OUTPUT);
}

void setDemux3Inhibit(bool way)
{
	mcp.digitalWrite(PIN_DTR, way);
}
