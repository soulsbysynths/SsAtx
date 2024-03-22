# 1 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Sn74hc138dTester\\Sn74hc138dTester\\sketches\\Sn74hc138dTester.ino"
# 2 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Sn74hc138dTester\\Sn74hc138dTester\\sketches\\Sn74hc138dTester.ino" 2
# 3 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Sn74hc138dTester\\Sn74hc138dTester\\sketches\\Sn74hc138dTester.ino" 2

# 5 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Sn74hc138dTester\\Sn74hc138dTester\\sketches\\Sn74hc138dTester.ino" 2

# 7 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Sn74hc138dTester\\Sn74hc138dTester\\sketches\\Sn74hc138dTester.ino" 2
# 8 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Sn74hc138dTester\\Sn74hc138dTester\\sketches\\Sn74hc138dTester.ino" 2
# 9 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\Sn74hc138dTester\\Sn74hc138dTester\\sketches\\Sn74hc138dTester.ino" 2




void initialiseDemux3();
void setDemux3Inhibit(bool way);

Sn74hc138d demux1(4, 3, 2, 5);
Sn74hc138d demux2(4, 3, 2, 5, 7);
Sn74hc138d demux3(4, 3, 2, &initialiseDemux3, &setDemux3Inhibit);
Adafruit_MCP23X17 mcp;

void setup()
{
 pinMode(26, OUTPUT);
 demux1.initialise();
 demux2.initialise();
 demux3.initialise();
}

void loop()
{
 static uint8_t line = 0;
 static bool demux3inhibit = false;
 digitalWrite(26, HIGH);
 demux1.setLine(line);
 demux1.setInhibit(true);
 delay(100);
 digitalWrite(26, LOW);
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
 mcp.pinMode(13, OUTPUT);
}

void setDemux3Inhibit(bool way)
{
 mcp.digitalWrite(13, way);
}
