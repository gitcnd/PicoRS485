// Works

/*
  PicoRS485_example.cpp v1.0 - shows how to get 2 arduinos to talk between each other over a pair of RS485 wires using "automatic" RS485 modules.
  (C) Copyright 2018 (GNU GENERAL PUBLIC LICENSE, Version 3) - Chris Drake <cdrake@cpan.org>
*/

String VERSION;

#include <PicoRS485.h>
#define RX_PIN 0
#define TX_PIN 1
PicoRS485 PicoRS485;
#define RS485_POWER_PIN 4	// My RS485 module is powered by my Arduino digital pin 4 instead of VCC (so I can turn it on/off in code, because it blocks programming over serial if powered on always)

 
#include <NodeID.h>
NodeID NodeID(2);	// We use the 3rd and 4th bytes (zero-based offset 2 and 3) inside EEPROM to store our ID.


unsigned long lastSendTime = 0;        // last send time
unsigned long sinterval = 2000;        // interval between sends
int counter=0;
int got=0;
unsigned int myNodeID;


/***************************
 *   LED Indicator Init.   *
 ***************************/
const byte cnd_ledPin = 13; // We use blink-codes to indicate OK (breif flashes) or ERROR states (rapid blinking)
byte cnd_ledtoggle=0; // holds LED state
int cnd_ledon=20;      // Can lengthen this (e.g. 200) to indicate error states
int cnd_ledoff=1000;  // Can shorten this (e.g. 200) to indicate errors
unsigned long cnd_ledtimer=0; // for timing the LED flashes


char outbuf[256];

void setup() {
  VERSION=F("\n#\tv2.04 " __FILE__ "\t" __DATE__ " " __TIME__ "\n");
  pinMode(RS485_POWER_PIN, OUTPUT);             digitalWrite(RS485_POWER_PIN, 1); // Turn on the RS485
  pinMode(cnd_ledPin,OUTPUT);

  myNodeID=NodeID.get();
  PicoRS485.begin(9600,TX_PIN,RX_PIN);	// 115200 is not a good idea over very long cables
  PicoRS485.print(VERSION);
} // setup



void loop() {
  if (millis() - lastSendTime > sinterval) {
    sprintf(outbuf,"Hi from %d v%s seq #%d. Got %d (%d)\n",myNodeID,__TIME__,counter++,got,42);
    PicoRS485.print(outbuf);
    lastSendTime = millis();
    sinterval = random(2000) + 1000;     // 2-3 seconds
  }

  while(int c=PicoRS485.read()>=0) {
    got++; // count how many bytes we've recieved
  }


  // Blink the LED so we know we are alive
  if(cnd_ledtimer<millis()) {
    cnd_ledtoggle++; digitalWrite(cnd_ledPin, cnd_ledtoggle&1);
    cnd_ledtimer=millis();
    if(cnd_ledtoggle&1) { cnd_ledtimer+=cnd_ledon; } else {cnd_ledtimer+=cnd_ledoff;}
  }
  
} // loop

