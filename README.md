# PicoRS485
Adaptation of TinySerial to work for RS485 boards wired onto your arduino serial pins.

The cheap eBay/alibaba "automatic" RS845 boards use a hex inverter to "enable" the transmit when its time to send (cool trick).  Only problem - you can't transmit if anything else is transmitting, so extra serial code is needed to pull the TX line "high" after use (thus indicating that we are *not* transmitting), so that we are then able to recieve.


# SYNOPSIS

```C
  #include <PicoRS485.h>
  #define RX_PIN 0
  #define TX_PIN 1
  PicoRS485 PicoRS485;

  void setup() {
    PicoRS485.begin(115200,&Serial,TX_PIN,RX_PIN);
    String VERSION=F("\n#\tv2.04 " __FILE__ "\t" __DATE__ " " __TIME__ "\n");
    PicoRS485.print(VERSION);
  }
```

# FUNCTIONS

```C
  write();	// send a byte
  read();	// recieve a byte
  print();	// send many bytes
  println();	// " with CR
```


# HOW TO INSTALL

(Method 1)

1. Open a terminal (commandline, bash shell, whatever)
2. Change into your Arduino folder
```bash
   cd /Applications/Arduino.app/Contents/Java/libraries/
   (or)
   cd ~/Arduino/libraries/
```
3. grab and install this code
```bash
   git clone https://github.com/gitcnd/PicoRS485.git
```
4. restart your arduino IDE
5. Choose File => Examples => PicoRS485 => PicoRS485_example
6. Hit the "build" button an enjoy!

(Method 2) - see https://www.arduino.cc/en/Guide/Libraries

1. Download the ZIP of this repo: https://github.com/gitcnd/PicoRS485/archive/master.zip
2. In your IDE, select Sketch -> Include Library -> Add .ZIP Library
3. Choose File => Examples => PicoRS485 => hello
4. Hit the "build" button an enjoy!
