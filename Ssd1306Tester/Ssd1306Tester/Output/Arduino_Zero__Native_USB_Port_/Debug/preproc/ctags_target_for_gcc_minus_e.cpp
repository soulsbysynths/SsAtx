# 1 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino"
# 2 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2
# 3 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2

# 5 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2

# 7 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2
# 8 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2
# 9 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2
# 10 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2
# 11 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2
# 12 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2
# 13 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2
# 14 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2
# 15 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2

# 17 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2
# 18 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2
# 19 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2

# 21 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2

# 23 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2
# 24 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Ssd1306Tester\\Ssd1306Tester\\sketches\\Ssd1306Tester.ino" 2

void initialiseDtrMux();
void setDtrMuxInhibit(bool inhibit);
void initialiseOledPins();
void writeOledGroupVcc(uint8_t value);
void writeOledGroupReset(uint8_t value);
void writeOledSelect(Ssd1306* ssd, uint8_t value);
void writeOledDc(Ssd1306* ssd, uint8_t value);
void setupGraphicsTest();
void writeGraphicsTest();
void drawRandomBuffer(Ssd1306* ssd, graphics::Colour colour);
void drawRandomRect(Ssd1306* ssd);
void drawRandomRectFullScreen(Ssd1306* ssd);
void drawRandomRoundRectFullScreen(Ssd1306* ssd);
void drawRandomChar(Ssd1306* ssd);
void drawRandomCharFullScreen(Ssd1306* ssd);
void drawRandomLabel(Ssd1306* ssd);
void drawRandomButton(Ssd1306* ssd);
void drawSlider(Ssd1306* ssd);
void updateSlider(Ssd1306* ssd);
void paintControl(graphics::Control* c, graphics::Graphics* g);
graphics::Rect getRandomRect(Ssd1306* ssd);
graphics::Colour getRandomColour();
graphics::DrawMode getRandomDrawMode();
bool getRandomBool();
std::string getRandomString(graphics::Rect* rect);
graphics::StringAlignment getRandomStringAlignment();

enum GraphicsTest
{
 GT_CANVAS,
 GT_RECTS,
 GT_RECTS_FULLSCREEN,
 GT_CHAR,
 GT_CHAR_FULLSCREEN,
 GT_LABEL,
 GT_ROUNDRECTS_FULLSCREEN,
 GT_BUTTON,
 GT_SLIDER
};

static const GraphicsTest GRAPHICS_TEST = GT_SLIDER;
static const int DELAY = 500;
static const uint8_t PIN_CLK = 6;
static const uint8_t PIN_DC = 7;
static const uint8_t MCP_PIN_DTR_INHIBIT = 13;
static const uint8_t MCP_PIN_RESET = 14;
static const uint8_t MCP_PIN_VCC = 15;
static const uint8_t WIDE_OLEDS = 6;
static const uint8_t TALL_OLEDS = 2;
static const uint8_t OLEDS = WIDE_OLEDS + TALL_OLEDS;

Adafruit_MCP23X17 mcp;
Sn74hc138d selectMux(4, 3, 2, 5);
Sn74hc138d commandMux(4, 3, 2, 5, 7);
Sn74hc138d dtrMux(4, 3, 2, &initialiseDtrMux, &setDtrMuxInhibit);

Ssd1306Group oledGroup(&initialiseOledPins, &writeOledGroupVcc, &writeOledGroupReset);
graphics::Slider* slider[OLEDS];

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
 setupGraphicsTest();
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

void setupGraphicsTest()
{
 for (uint8_t i = 0; i < OLEDS; i++)
 {
  Ssd1306* ssd = oledGroup.getOledPtr(i);
  switch (GRAPHICS_TEST)
  {
   case GT_SLIDER:
    {
     drawSlider(ssd);
     break;
    }
   default:
    break;
  }
 }
}

void writeGraphicsTest()
{
 for (uint8_t i = 0; i < OLEDS; i++)
 {
  Ssd1306* ssd = oledGroup.getOledPtr(i);
  switch (GRAPHICS_TEST)
  {
   case GT_CANVAS:
    {
     drawRandomBuffer(ssd, getRandomColour());
     break;
    }
   case GT_RECTS:
    {
     drawRandomRect(ssd);
     break;
    }
   case GT_RECTS_FULLSCREEN:
    {
     drawRandomRectFullScreen(ssd);
     break;
    }
   case GT_CHAR:
    {
     drawRandomChar(ssd);
     break;
    }
   case GT_CHAR_FULLSCREEN:
    {
     drawRandomCharFullScreen(ssd);
     break;
    }
   case GT_LABEL:
    {
     drawRandomLabel(ssd);
     break;
    }
   case GT_ROUNDRECTS_FULLSCREEN:
    {
     drawRandomRoundRectFullScreen(ssd);
     break;
    }
   case GT_BUTTON:
    {
     drawRandomButton(ssd);
     break;
    }
   case GT_SLIDER:
    {
     updateSlider(ssd);
     break;
    }
  }
 }
}

void drawRandomBuffer(Ssd1306* ssd, graphics::Colour colour)
{
 using namespace graphics;

 Graphics graphics(getRandomRect(ssd), ssd->getSizePtr(), colour);

 ssd->writeGraphics(&graphics);
}

void drawRandomRect(Ssd1306* ssd)
{
 using namespace graphics;

 Graphics graphics(getRandomRect(ssd), ssd->getSizePtr());

 const Rect* rect = graphics.getRectPtr();

 graphics.drawRect(Rect{ 0, 0, rect->size.width, rect->size.height });
 graphics.drawLine({ { 0, 0 }, { rect->size.width, rect->size.height } });
 graphics.drawLine({ { 0, rect->size.height }, { rect->size.width, 0 } });
 ssd->writeGraphics(&graphics);

}

void drawRandomRectFullScreen(Ssd1306* ssd)
{
 using namespace graphics;

 Graphics graphics(Rect{ 0, 0, ssd->getWidth(), ssd->getHeight() }, ssd->getSizePtr());

 Rect rect = getRandomRect(ssd);
 Graphics::clip(&rect, ssd->getSizePtr());

 graphics.drawRect(rect);
 graphics.drawLine({
                    rect.location,
                   {
                    rect.location.x + rect.size.width - 1,
                    rect.location.y + rect.size.height - 1
                   }
                   });
 graphics.drawLine({
                   {
                    rect.location.x,
                    rect.location.y + rect.size.height - 1
                   },
                   {
                    rect.location.x + rect.size.width - 1,
                    rect.location.y
                   }
                   });

 ssd->writeGraphics(&graphics);

}

void drawRandomRoundRectFullScreen(Ssd1306* ssd)
{
 using namespace graphics;

 Graphics graphics(Rect{ 0, 0, ssd->getWidth(), ssd->getHeight() }, ssd->getSizePtr());

 Rect rect = getRandomRect(ssd);
 Graphics::clip(&rect, ssd->getSizePtr());

 int radius;
 if (rect.size.width < rect.size.height)
 {
  radius = random(rect.size.width);
 }
 else
 {
  radius = random(rect.size.height);
 }

 graphics.drawRoundRect(rect, radius);
 graphics.fillRoundRect(rect, radius);

 ssd->writeGraphics(&graphics);
}

void drawRandomChar(Ssd1306* ssd)
{
 using namespace graphics;

 char randomChar = random(dinMittel8x16Regular.getStartChar(), dinMittel8x16Regular.getEndChar() + 1);
 Rect rect;
 rect.location.x = random(ssd->getWidth());
 rect.location.y = random(ssd->getHeight());
 rect.size.width = dinMittel8x16Regular.getWidth();
 rect.size.height = dinMittel8x16Regular.getHeight();
 Graphics graphics(rect, ssd->getSizePtr());

 graphics.drawCharacter(Point{ 0, 0 }, &dinMittel8x16Regular, randomChar, CO_LIGHTGREY);
 ssd->writeGraphics(&graphics);
}

void drawRandomCharFullScreen(Ssd1306* ssd)
{
 using namespace graphics;

 char randomChar = random(dinMittel8x16Regular.getStartChar(), dinMittel8x16Regular.getEndChar() + 1);

 Point point;
 point.x = random(ssd->getWidth() - dinMittel8x16Regular.getWidth());
 point.y = random(ssd->getHeight() - dinMittel8x16Regular.getHeight());

 Graphics graphics(Rect{ 0, 0, ssd->getWidth(), ssd->getHeight() }, ssd->getSizePtr());

 graphics.drawCharacter(point, &dinMittel8x16Regular, randomChar, CO_WHITE, DM_NOT_MASK);
 ssd->writeGraphics(&graphics);
}

void drawRandomLabel(Ssd1306* ssd)
{
 using namespace graphics;
 ssd->clearDisplay();

 Rect rect = getRandomRect(ssd);

 Label label(ssd->getId(),
             &dinMittel8x16Regular,
             ssd->getSizePtr(),
             &rect,
             getRandomString(&rect),
             &paintControl,
             getRandomStringAlignment(),
             getRandomStringAlignment(),
             true,
             CO_DARKGREY,
             CO_WHITE,
             DM_OR_MASK);
 label.paintAll();
}

void drawRandomButton(Ssd1306* ssd)
{
 using namespace graphics;
 ssd->clearDisplay();

 Rect rect = getRandomRect(ssd);

 Button button(ssd->getId(),
               &dinMittel8x16Regular,
               ssd->getSizePtr(),
               &rect,
               "OK",
               &paintControl);
 button.paintAll();
}

void drawSlider(Ssd1306* ssd)
{
 using namespace graphics;
 int id = ssd->getId();

 Rect rect = getRandomRect(ssd);

 slider[id] = new Slider(id,
                               ssd->getSizePtr(),
                               &rect,
                               0,
                               100,
                               &paintControl,
                               50);
 slider[id]->paintAll();
}

void updateSlider(Ssd1306* ssd)
{

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
  selectMux.setLine(ssd->getId());
  selectMux.setInhibit(false);
 }
}

void writeOledDc(Ssd1306* ssd, uint8_t value)
{
 digitalWrite(PIN_DC, value);
}

void paintControl(graphics::Control* c, graphics::Graphics* g)
{
 oledGroup.getOledPtr(c->getId())->writeGraphics(g);
}

graphics::Rect getRandomRect(Ssd1306* ssd)
{
 using namespace graphics;

 Rect rect;

 rect.location.x = random(0, ssd->getWidth());
 rect.location.y = random(0, ssd->getHeight());
 rect.size.width = random(0, ssd->getWidth());
 rect.size.height = random(0, ssd->getHeight());

 //	Size fontSize = { dinMittel8x16Regular.getWidth(), dinMittel8x16Regular.getHeight() };
 //	rect.x = random(0, ssd->getSizePtr().width - fontSize.width);
 //	rect.y = random(0, ssd->getSizePtr().height - fontSize.height);
 //	rect.width = random(fontSize.width, (ssd->getSizePtr().width - rect.x));
 //	rect.height = random(fontSize.height, (ssd->getSizePtr().height - rect.y));

 //	rect.x = 0;
 //	rect.y = 0;
 //	rect.width = ssd->getSizePtr().width;
 //	rect.height = ssd->getSizePtr().height;

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

std::string getRandomString(graphics::Rect* rect)
{
 const int COLUMNS = rect->size.width >> dinMittel8x16Regular.getSizeBitShiftWidth();
 const int ROWS = rect->size.height >> dinMittel8x16Regular.getSizeBitShiftHeight();
 int lines = random(0, ROWS);
 lines++;
 std::string s;
 char c = 'a';
 for (int line = 0; line < lines; line++)
 {
  int lineLength = random(0, COLUMNS);
  lineLength++;
  for (int letter = 0; letter < lineLength; letter++)
  {
   s.push_back(c);
   c++;
  }
  if (line < (lines - 1))
  {
   s.push_back('\n');
  }
 }

 return s;
}

graphics::StringAlignment getRandomStringAlignment()
{
 return (graphics::StringAlignment)random(0, 3);
}
