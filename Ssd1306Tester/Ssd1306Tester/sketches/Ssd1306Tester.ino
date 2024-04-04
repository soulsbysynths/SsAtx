#include <Graphics.h>
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_MCP23X08.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_MCP23XXX.h>
#include <Sn74hc138d.h>
#include <Ssd1306.h>
#include "Ssd1306OledSettings.h"
#include <Ssd1306Group.h>
#include <Font.h>
#include "DinMittel8x16Regular.h"
#include <Label.h>
#include <Display.h>


void initialiseDtrMux();
void setDtrMuxInhibit(bool inhibit);
void initialiseOledPins();
void writeOledGroupVcc(uint8_t value);
void writeOledGroupReset(uint8_t value);
void writeOledSelect(Ssd1306* ssd, uint8_t value);
void writeOledDc(Ssd1306* ssd, uint8_t value);
void writeRandomBuffer(Ssd1306* ssd, graphics::Colour colour);
void writeRandomRect(Ssd1306* ssd);
void writeRandomRectFullScreen(Ssd1306* ssd);
void writeRandomChar(Ssd1306* ssd);
void writeRandomCharFullScreen(Ssd1306* ssd);
void writeGraphicsTest();
void writeRandomLabel(Ssd1306* ssd);
void paintLabel(Label* l, graphics::Graphics* g);
graphics::Rect getRandomRect(Ssd1306* ssd);
graphics::Colour getRandomColour();
graphics::DrawMode getRandomDrawMode();
bool getRandomBool();

enum GraphicsTest
{
	GT_CANVAS,
	GT_RECTS,
	GT_RECTS_FULLSCREEN,
	GT_CHAR,
	GT_CHAR_FULLSCREEN,
	GT_LABEL
};

static const GraphicsTest GRAPHICS_TEST = GT_LABEL;
static const int DELAY = 500;
static const uint8_t PIN_CLK = 6;
static const uint8_t PIN_DC = 7;
static const uint8_t MCP_PIN_DTR_INHIBIT = 13;
static const uint8_t MCP_PIN_RESET = 14;
static const uint8_t MCP_PIN_VCC = 15;
static const uint8_t WIDE_OLEDS = 6;
static const uint8_t TALL_OLEDS = 2;

Adafruit_MCP23X17 mcp;
Sn74hc138d selectMux(4, 3, 2, 5);
Sn74hc138d commandMux(4, 3, 2, 5, 7);
Sn74hc138d dtrMux(4, 3, 2, &initialiseDtrMux, &setDtrMuxInhibit);

Ssd1306Group oledGroup(&initialiseOledPins, &writeOledGroupVcc, &writeOledGroupReset);

void setup()
{
	using namespace graphics;
	
	pinMode(PIN_CLK, OUTPUT);
	
	mcp.begin_SPI(6, &SPI, 3);
	mcp.enableAddrPins();
	
	selectMux.initialise();
	commandMux.initialise();
	dtrMux.initialise();
	
	const Size WIDE_OLED_SIZE = { 96, 32 };
	const Size TALL_OLED_SIZE = { 48, 64 };
	
	for (uint8_t i = 0; i < WIDE_OLEDS; i++)
	{
		oledGroup.add(i, WIDE_OLED_SIZE, &SPI, &writeOledSelect, &writeOledDc, &QG9632TSWEG01_SETTINGS);
	}
	
	for (uint8_t i = 0; i < TALL_OLEDS; i++)
	{
		oledGroup.add(i + WIDE_OLEDS, TALL_OLED_SIZE, &SPI, &writeOledSelect, & writeOledDc, &CG4864TSWEG04_SETTINGS);
	}
	
	oledGroup.initialise();
	
}

void loop()
{
	digitalWrite(PIN_CLK, HIGH);
	writeGraphicsTest();
	delay(DELAY);
	digitalWrite(PIN_CLK, LOW);
	writeGraphicsTest();
	delay(DELAY);
}

void writeGraphicsTest()
{
	for (uint8_t i = 0; i < (WIDE_OLEDS + TALL_OLEDS); i++)
	{
		Ssd1306* ssd = oledGroup.getOledPtr(i);
		switch (GRAPHICS_TEST)
		{
			case GT_CANVAS:
				{
					writeRandomBuffer(ssd, getRandomColour());
					break;
				}
			case GT_RECTS:
				{
					writeRandomRect(ssd);
					break;
				}
			case GT_RECTS_FULLSCREEN:
				{
					writeRandomRectFullScreen(ssd);
					break;
				}
			case GT_CHAR:
				{
					writeRandomChar(ssd);
					break;
				}
			case GT_CHAR_FULLSCREEN:
				{
					writeRandomCharFullScreen(ssd);
					break;
				}
			case GT_LABEL:
				{
					writeRandomLabel(ssd);
					break;
				}
		}
	}
}

void writeRandomBuffer(Ssd1306* ssd, graphics::Colour colour)
{
	using namespace graphics;
	
	Graphics graphics(getRandomRect(ssd), ssd->SIZE, colour);

	ssd->writeGraphics(&graphics);	
}

void writeRandomRect(Ssd1306* ssd)
{
	using namespace graphics;

	Graphics graphics(getRandomRect(ssd), ssd->SIZE);

	const Rect* rect = graphics.getRectPtr();

	graphics.drawRect(Rect{ 0, 0, rect->width, rect->height });
	graphics.drawLine(Point{ 0, 0 }, Point{ rect->width, rect->height });
	graphics.drawLine(Point{ 0, rect->height }, Point{ rect->width, 0 });
	ssd->writeGraphics(&graphics);
	
}

void writeRandomRectFullScreen(Ssd1306* ssd)
{
	using namespace graphics;

	Graphics graphics(Rect{ 0, 0, ssd->SIZE.width, ssd->SIZE.height }, ssd->SIZE);
	
	Rect rect = getRandomRect(ssd);
	Graphics::constrainRectGraphics(&rect, ssd->SIZE);
	
	graphics.drawRect(rect);
	graphics.drawLine(Point{ rect.x, rect.y }, Point{ rect.x + rect.width - 1, rect.y + rect.height - 1 });
	graphics.drawLine(Point{ rect.x, rect.y + rect.height - 1 }, Point{ rect.x + rect.width - 1, rect.y });
	ssd->writeGraphics(&graphics);
	
}

void writeRandomChar(Ssd1306* ssd)
{
	using namespace graphics;

	char randomChar = random(dinMittel8x16Regular.START_CHAR, dinMittel8x16Regular.getEndChar() + 1);
	Rect rect;
	rect.x = random(ssd->SIZE.width);
	rect.y = random(ssd->SIZE.height);
	rect.width = dinMittel8x16Regular.getWidth();
	rect.height = dinMittel8x16Regular.getHeight();
	Graphics graphics(rect, ssd->SIZE);
	
	graphics.drawCharacter(Point{ 0, 0 }, &dinMittel8x16Regular, randomChar, CO_LIGHTGREY);
	ssd->writeGraphics(&graphics);
}

void writeRandomCharFullScreen(Ssd1306* ssd)
{
	using namespace graphics;

	char randomChar = random(dinMittel8x16Regular.START_CHAR, dinMittel8x16Regular.getEndChar() + 1);
	
	Point point;
	point.x = random(ssd->SIZE.width - dinMittel8x16Regular.getWidth());
	point.y = random(ssd->SIZE.height - dinMittel8x16Regular.getHeight());

	Graphics graphics(Rect{ 0, 0, ssd->SIZE.width, ssd->SIZE.height }, ssd->SIZE);
	
	graphics.drawCharacter(point, &dinMittel8x16Regular, randomChar, CO_WHITE, DM_NOT_MASK);
	ssd->writeGraphics(&graphics);
}

void writeRandomLabel(Ssd1306* ssd)
{
	using namespace graphics;
	ssd->clearDisplay();
	Label label(ssd->ID, 
	            &dinMittel8x16Regular, 
	            ssd->SIZE,
	            getRandomRect(ssd),
	            "HELLO WORLD!",
	            &paintLabel,
	            SA_NEAR,
	            SA_NEAR,
				true);
	
	//	Label label(ssd->ID, 
//	            &dinMittel8x16Regular, 
//	            ssd->SIZE,
//	            getRandomRect(ssd),
//	            "HELLO WORLD!",
//	            &paintLabel,
//	            SA_NEAR,
//	            SA_NEAR,
//	            getRandomBool(),
//	            getRandomColour(),
//	            getRandomColour(),
//	            getRandomDrawMode());
}


void initialiseDtrMux()
{
	mcp.pinMode(MCP_PIN_DTR_INHIBIT, OUTPUT);
	mcp.digitalWrite(MCP_PIN_DTR_INHIBIT, LOW);
}

void setDtrMuxInhibit(bool inhibit)
{
	mcp.digitalWrite(MCP_PIN_DTR_INHIBIT, inhibit);
}

void initialiseOledPins()
{
	pinMode(PIN_DC, OUTPUT);
	mcp.pinMode(MCP_PIN_VCC, OUTPUT);
	mcp.pinMode(MCP_PIN_RESET, OUTPUT);
}

void writeOledGroupVcc(uint8_t value)
{
	mcp.digitalWrite(MCP_PIN_VCC, value);
}

void writeOledGroupReset(uint8_t value)
{
	mcp.digitalWrite(MCP_PIN_RESET, value);
}

void writeOledSelect(Ssd1306* ssd, uint8_t value)
{
	if (value == LOW)
	{
		selectMux.setInhibit(true);
	}
	else
	{
		selectMux.setLine(ssd->ID);
		selectMux.setInhibit(false);
	}
}

void writeOledDc(Ssd1306* ssd, uint8_t value)
{
	digitalWrite(PIN_DC, value);
}

void paintLabel(Label* l, graphics::Graphics* g)
{
	oledGroup.getOledPtr(l->ID)->writeGraphics(g);
}

graphics::Rect getRandomRect(Ssd1306* ssd)
{
	using namespace graphics;

	Rect rect;
	
	rect.x = random(0, ssd->SIZE.width);
	rect.y = random(0, ssd->SIZE.height);
	rect.width = random(0, ssd->SIZE.width);
	rect.height = random(0, ssd->SIZE.height);
	
	//	Size fontSize = { dinMittel8x16Regular.getWidth(), dinMittel8x16Regular.getHeight() };
	//	rect.x = random(0, ssd->SIZE.width - fontSize.width);
	//	rect.y = random(0, ssd->SIZE.height - fontSize.height);
	//	rect.width = random(fontSize.width, (ssd->SIZE.width - rect.x));
	//	rect.height = random(fontSize.height, (ssd->SIZE.height - rect.y));
	
	//	rect.x = 0;
	//	rect.y = 0;
	//	rect.width = ssd->SIZE.width;
	//	rect.height = ssd->SIZE.height;
	
	return rect;
}

graphics::Colour getRandomColour()
{
	return (graphics::Colour)random(0, 5);
}

graphics::DrawMode getRandomDrawMode()
{
	return (graphics::DrawMode)random(0, 8);
}

bool getRandomBool()
{
	return (bool)random(0, 2);
}