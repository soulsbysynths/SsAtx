# 1 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Mcp23s17Tester\\Mcp23s17Tester\\sketches\\Mcp23s17Tester.ino"
# 2 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Mcp23s17Tester\\Mcp23s17Tester\\sketches\\Mcp23s17Tester.ino" 2

# 4 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Mcp23s17Tester\\Mcp23s17Tester\\sketches\\Mcp23s17Tester.ino" 2

# 6 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Mcp23s17Tester\\Mcp23s17Tester\\sketches\\Mcp23s17Tester.ino" 2
# 7 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Mcp23s17Tester\\Mcp23s17Tester\\sketches\\Mcp23s17Tester.ino" 2
# 8 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Mcp23s17Tester\\Mcp23s17Tester\\sketches\\Mcp23s17Tester.ino" 2



//#define MISO 22
//#define MOSI 23
//#define SCK 24


Adafruit_MCP23X17 mcp;

void setup()
{
 pinMode(26, OUTPUT);
 if (!mcp.begin_SPI(6, &SPI, 1))
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
 digitalWrite(26, HIGH);
 mcp.digitalWrite(pin, HIGH);
 delay(1000);
 digitalWrite(26, LOW);
 mcp.digitalWrite(pin, LOW);
 delay(1000);
 pin++;
 pin &= 0x0F;
}
