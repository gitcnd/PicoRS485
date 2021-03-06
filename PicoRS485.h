// A small RS485 Serial class in the spirit of Petit FatFs, adapted from PetitSerial.h.
#ifndef PicoRS485_h
#define PicoRS485_h
#include <Arduino.h>

// Set zero if you only need single character I/O.
#define PETIT_SERIAL_USE_PRINT 1

#if defined(UDR0) || defined(DOXYGEN) || defined(TEENSYDUINO)
const uint16_t MIN_2X_BAUD = F_CPU/(4*(2*0XFFF + 1)) + 1;
//------------------------------------------------------------------------------
/**
 * \class PicoRS485
 * \brief mini serial class for the %SdFat library.
 */
#if PETIT_SERIAL_USE_PRINT
class PicoRS485 : public Print {
 public:
  using Print::write;
#else   // PETIT_SERIAL_USE_PRINT
class PicoRS485 : public Print {
 public:
#endif  // PETIT_SERIAL_USE_PRINT
  /**
   * Set baud rate for serial port zero and enable in non interrupt mode.
   * Do not call this function if you use another serial library.
   * \param[in] baud rate
   */
  void begin(uint32_t baud,HardwareSerial *port,byte txpin,byte rxpin) {
    _txpin=txpin;_rxpin=rxpin;
#ifdef TEENSYDUINO      // We're using Serial5 on our Teensy to talk to this
    _port=port;
#endif
    pinMode(_txpin,OUTPUT); digitalWrite(_txpin,1); // must set TC high or else cannot recieve
    pinMode(_rxpin,INPUT);
#ifdef TEENSYDUINO      // We're using Serial5 on our Teensy to talk to this
    _port->begin(baud);
#else
    uint16_t baud_setting;
    // don't worry, the compiler will squeeze out F_CPU != 16000000UL
    if ((F_CPU != 16000000UL || baud != 57600) && baud > MIN_2X_BAUD) {
      // Double the USART Transmission Speed
      UCSR0A = 1 << U2X0;
      baud_setting = (F_CPU / 4 / baud - 1) / 2;
    } else {
      // hardcoded exception for compatibility with the bootloader shipped
      // with the Duemilanove and previous boards and the firmware on the 8U2
      // on the Uno and Mega 2560.
      UCSR0A = 0;
      baud_setting = (F_CPU / 8 / baud - 1) / 2;
    }
    // assign the baud_setting
    UBRR0H = baud_setting >> 8;
    UBRR0L = baud_setting;
    // enable transmit and receive
    UCSR0B |= (1 << TXEN0) | (1 << RXEN0);
    //this didn't work - delay(1) below did: while( (UCSR0A & (1 << TXC0)) || (((1 << UDRIE0) & UCSR0B) || !(UCSR0A & (1 << UDRE0))) ) {} // wait for the read and write buffers to be empty
    delay(1); // Arduino Pro Mini 3.3v FTDI chokes if this is missing - seems it takes time for the above stuff to be ready to use.
#endif // TEENSYDUINO    
  }    
  /**
   *  Unbuffered read
   *  \return -1 if no character is available or an available character.
   */
  int read() {
#ifdef TEENSYDUINO      // We're using Serial5 on our Teensy to talk to this
    _port->flush();	// let any remaining send complete first
    digitalWrite(_txpin,1); // trying to read?  ensure others can talk.
    if(_port->available())return _port->read();
#else    
    while((UCSR0A & (1 << TXC0))==0){}	// let any remaining send complete first
    digitalWrite(_txpin,1); // trying to read?  ensure others can talk.
    if (UCSR0A & (1 << RXC0)) {	// UCSR0A=status register, RXC0=USART_Receive_complete_flag#
      return UDR0;
    }
#endif
    return -1;
  }
  /**
   * Unbuffered write
   *
   * \param[in] b byte to write.
   * \return 1
   *
   * UDRIE0=USART Data Register Empty Interrupt Enable bit #
   * UDRE0 =USART Data Register Empty
   */
  size_t write(uint8_t b) {
#ifdef TEENSYDUINO      // We're using Serial5 on our Teensy to talk to this
    _port->print((char)b);
#else
    while (((1 << UDRIE0) & UCSR0B) || !(UCSR0A & (1 << UDRE0))) {}	// Wait until the write-buffer can be written
    UDR0 = b;
    while((UCSR0A & (1 << TXC0))==0){}					// Wait for it to go fully out (because if we do a "read" next, that blocks sending, so we need to be finished first)
#endif
    return 1;
  }
  size_t writestr(uint8_t *b,unsigned int len) {
    for(unsigned int i=0;i<len;i++)write(b[i]);
    return 1;
  }
  size_t writestr(uint8_t *b) {
    unsigned int len=strlen((char *)b);
    for(unsigned int i=0;i<len;i++)write(b[i]);
    return 1;
  }
  size_t writestr(__FlashStringHelper *b) {
    const char *s=(const char*)b;
    uint8_t c=pgm_read_byte_near((__FlashStringHelper *)s); s++;
    while(c!=0) {
      write(c); c=pgm_read_byte_near((__FlashStringHelper *)s); s++;
    }
    return 1;
  }
  private:
    byte _txpin;
    byte _rxpin;
#ifdef TEENSYDUINO      // We're using Serial5 on our Teensy to talk to this
    HardwareSerial *_port;
#endif
};
#endif // defined(UDR0) || defined(DOXYGEN)



#endif  // PicoRS485_h

