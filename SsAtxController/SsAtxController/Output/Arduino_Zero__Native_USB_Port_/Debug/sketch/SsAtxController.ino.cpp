#include <Arduino.h>
#line 1 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\SsAtxController\\SsAtxController\\sketches\\SsAtxController.ino"
#line 1 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\SsAtxController\\SsAtxController\\sketches\\SsAtxController.ino"
void setup();
#line 6 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\SsAtxController\\SsAtxController\\sketches\\SsAtxController.ino"
void loop();
#line 1 "C:\\Users\\info\\Documents\\GitHub\\SsAtx\\SsAtxController\\SsAtxController\\sketches\\SsAtxController.ino"
void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
	digitalWrite(LED_BUILTIN, HIGH);
	delay(1000);
	digitalWrite(LED_BUILTIN, LOW);
	delay(1000);
}

