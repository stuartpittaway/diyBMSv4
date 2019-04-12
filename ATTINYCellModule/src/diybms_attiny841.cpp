/*
LICENSE
Attribution-NonCommercial-ShareAlike 2.0 UK: England & Wales (CC BY-NC-SA 2.0 UK)
https://creativecommons.org/licenses/by-nc-sa/2.0/uk/

* Non-Commercial — You may not use the material for commercial purposes.
* Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made.
  You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
* ShareAlike — If you remix, transform, or build upon the material, you must distribute your
  contributions under the same license as the original.
* No additional restrictions — You may not apply legal terms or technological measures
  that legally restrict others from doing anything the license permits.

  PIN MAPPINGS
  Diagram
  https://github.com/SpenceKonde/ATTinyCore/blob/master/avr/extras/ATtiny_x41.md

  PA1 = PIN 12 SERIAL TRANSMIT (TXD0)
  PA2 = PIN 11 SERIAL RECEIVE (RXD0)

  PA3 = DUMP LOAD ENABLE / PIN 10 /  ARDUINO PIN 7/A3
  PA4 = ADC4 PIN 9 ARDUINO PIN 6/A4 = ON BOARD TEMP sensor
  PA5 = RED_LED / PIN 8 / ARDUINO PIN 5/A5  (SERIAL PORT 1 TXD1)
  PA6 = GREEN_LED / PIN 7 / ARDUINO PIN 4/A6
  PA7 = ADC7 = PIN 6 = ARDUINO PIN 3/A7 = 2.048V REFERENCE ENABLE

  PB2 = ADC8 PIN 5 ARDUINO PIN 2/A8 = VOLTAGE reading
  PB0 = ADC11 PIN 2 ARDUINO PIN 0/A11 = REMOTE TEMP sensor
  PB1 = ADC10 PIN 3 ARDUINO PIN 1/A10 = SPARE INPUT/OUTPUT

  //TODO: We should have mapped PA5 to spare PIN on sensor header its TXD1 and mapped RED_LED to PB1 to assist debugging
*/

#include "diybms_attiny841.h"


/*
void FadeRedLED() {
  //Redled is on PA5 which maps to TOCC4

  // TOCC[0:2] = OC1A, OC1B, OC2B (01, 01, 10)
  TOCPMSA0 = 0;
  //Enable OC2B for TOCC4
  TOCPMSA1 = (1<<TOCC4S1);
  // Timer/Counter Output Compare Pin Mux Channel Output Enable
  TOCPMCOE = (1<<TOCC4OE);
  //PA5 OUTPUT
  DDRA |=  _BV(DDA5);

  // Fast PWM, mode 14, non inverting, presc 1:8
  //TCCR1A = (1<<COM1A1)|(1<<COM1B1) | 1<<WGM11;
  //TCCR1B = 1<<WGM12 | 1<<CS11  |  1<<WGM13 | 1<<WGM12;
  TCCR2A = (1<<COM2B1) | 1<<WGM21;
  TCCR2B = 1<<WGM22 | 1<<CS21  |  1<<WGM23 | 1<<WGM22;

  // Set TOP for 20ms period
  //ICR1 = 30000 - 1;
  ICR2 = 10000 - 1;

  //OCR1A = 1200;
  //OCR1B = 1500;
  //OCR2B = 1800;


  uint16_t value=0;

  uint8_t direction=0;

  while (1) {
    if (direction==0) {
    value+=64;

    if (value>15000) {
      direction=1;
    }

  } else {
    value-=64;
    if (value<100) {
      direction=0;
    }
  }

    OCR2B=value;

    delay(10);
   }
}
*/

void DiyBMSATTiny841::WaitForSerial0TXFlush() {
  Serial.flush();
}

void DiyBMSATTiny841::DisableSerial0TX() {
  UCSR0B &= ~_BV(TXEN0);  //disable transmitter (saves 6mA)
}

void DiyBMSATTiny841::EnableSerial0TX(){
  UCSR0B |=(1<<TXEN0); // enable TX Serial0
}



void DiyBMSATTiny841::ConfigurePorts()  {
  //PUEA – Port A Pull-Up Enable Control Register (All disabled)
  PUEA=0;
  //PUEB – Port B Pull-Up Enable Control Register (All disabled)
  PUEB=0;


  //DDRA – Port A Data Direction Register
  //When DDAn is set, the pin PAn is configured as an output. When DDAn is cleared, the pin is configured as an input
  DDRA |= _BV(DDA3) | _BV(DDA6)| _BV(DDA7);

  #ifndef DIYBMS_DEBUG
  //RED LED - output
  DDRA |=  _BV(DDA5);
  #endif

  //DDRB – Port B Data Direction Register
  //All inputs
  DDRB = 0;

  //Set pins to initial state
  DumpLoadOff();
  ReferenceVoltageOff();

  RedLedOff();
  GreenLedOff();
}

//void DiyBMSATTiny841::DiyBMSATTiny841() {}

//void DiyBMSATTiny841::~DiyBMSATTiny841() {}

void DiyBMSATTiny841::DumpLoadOn() {
  PORTA |= _BV(PORTA3);
}
void DiyBMSATTiny841::DumpLoadOff() {
  PORTA &= (~_BV(PORTA3));
}

void DiyBMSATTiny841::ReferenceVoltageOn() {
  //When to switch 2.048V regulator on or off
  //Connected to Pin 6, PA7
  PORTA |= _BV(PORTA7);
}
void DiyBMSATTiny841::ReferenceVoltageOff() {
  //When to switch 2.048V regulator on or off
  //Connected to Pin 6, PA7
  PORTA &= (~_BV(PORTA7));
}

void DiyBMSATTiny841::GreenLedOn() {
  //#define GREEN_LED_ON PORTA |= _BV(PORTA6);
  PORTA |= _BV(PORTA6);
}
void DiyBMSATTiny841::GreenLedOff() {
  //#define GREEN_LED_OFF PORTA &= (~_BV(PORTA6));
  PORTA &= (~_BV(PORTA6));
}

void DiyBMSATTiny841::RedLedOn() {
  #ifdef DIYBMS_DEBUG
  //Do nothing
  #else
  PORTA |= _BV(PORTA5);
  #endif
}
void DiyBMSATTiny841::RedLedOff() {
  #ifdef DIYBMS_DEBUG
  //Do nothing
  #else
  PORTA &= (~_BV(PORTA5));
  #endif
}

void DiyBMSATTiny841::FlushSerial0() {
  Serial.flush();
}

void DiyBMSATTiny841::DisableSerial0() {
  //Disable serial0
  UCSR0B &= ~_BV(RXEN0);  //disable receiver
  UCSR0B &= ~_BV(TXEN0);  //disable transmitter
}
void DiyBMSATTiny841::EnableSerial0(){
  UCSR0B |=(1<<RXEN0); // enable RX Serial0
  UCSR0B |=(1<<TXEN0); // enable TX Serial0
}
void DiyBMSATTiny841::DisableSerial1(){
  UCSR1B &= ~_BV(RXEN1);  //disable receiver
  UCSR1B &= ~_BV(TXEN1);  //disable transmitter
}
void DiyBMSATTiny841::EnableSerial1(){
  UCSR1B |=(1<<RXEN1); // enable RX Serial1
  UCSR1B |=(1<<TXEN1); // enable TX Serial1
}

void DiyBMSATTiny841::EnableStartFrameDetection()
{
  noInterrupts();
  // Enable Start Frame Detection
  UCSR0D = (1 << RXSIE0) | (1 << SFDE0);

  interrupts();
}

void DiyBMSATTiny841::EnablePinChangeInterrupt()
{
  //Fire pin change interrupt on RXD0 changing state
  noInterrupts();

  MCUCR |= (1 << ISC01);
  MCUCR |= (1 << ISC00);
  // GIFR – General Interrupt Flag Register
  // PCIF0: Pin Change Interrupt Flag 0
  GIFR |= (1 << PCIF0);

  // PCIE0: Pin Change Interrupt Enable 0
  GIMSK |= (1 << PCIE0);

  //PCMSK0 – Pin Change Mask Register 0
  //PCINT2 maps to PA2 RXD0 Serial data input of USART0
  PCMSK0 |= (1 << PCINT2);

  interrupts();
}

void DiyBMSATTiny841::DisablePinChangeInterrupt()
{
  GIMSK &= ~(1 << PCIE0);  // disable interrupt
}

void DiyBMSATTiny841::SetWatchdog8sec() {
  //Setup a watchdog timer for 8 seconds
  MCUSR = 0;
  //Enable watchdog (to reset)
  WDTCSR |=  bit(WDE);

  CCP = 0xD8;
  //WDTCSR – Watchdog Timer Control and Status Register
  // We INTERRUPT the chip after 8 seconds of sleeping (not reboot!)
  // WDE: Watchdog Enable
  // Bits 5, 2:0 – WDP[3:0]: Watchdog Timer Prescaler 3 - 0
  WDTCSR = bit (WDIE)  | bit (WDP3) | bit (WDP0);
  //| bit(WDE)

  wdt_reset();
}

uint16_t DiyBMSATTiny841::ReadADC() {
  // must read ADCL first
  uint8_t low  = ADCL;
  return (ADCH << 8) | low;
}

void DiyBMSATTiny841::BeginADCReading() {
  //ADMUXB – ADC Multiplexer Selection Register
  //Select external AREF pin (internal reference turned off)
  ADMUXB = _BV(REFS2);

  //ADCSRA – ADC Control and Status Register A
  //Consider ADC sleep conversion mode?
  //prescaler of 64 = 8MHz/64 = 125KHz.
  ADCSRA |= _BV(ADPS2) |_BV(ADPS1);// | _BV(ADPS0);

  //adc_enable();
  //Bit 4 – ADIF: ADC Interrupt Flag
  //Bit 7 – ADEN: ADC Enable
  ADCSRA |= _BV(ADEN) | _BV(ADIF);  // enable ADC, turn off any pending interrupt

  // wait for ADC to settle
  // The ADC must be enabled during the settling time.
  // ADC requires a settling time of 1ms before measurements are stable
  delay(2);

  noInterrupts ();
  set_sleep_mode (SLEEP_MODE_ADC);    // sleep during ADC sample
  sleep_enable();

  // start the conversion
  ADCSRA |= _BV(ADSC) | _BV(ADIE);
  interrupts ();
  sleep_cpu ();
  sleep_disable ();

  // awake again, reading should be done, better make sure maybe the timer interrupt fired
  while (bit_is_set (ADCSRA, ADSC)) { }

  //adc_disable
  ADCSRA&=(~(1<<ADEN));
}


void DiyBMSATTiny841::Sleep() {
    //ATTINY841 sleep mode
    byte old_ADCSRA = ADCSRA;
    //For low power applications, before entering sleep, remember to turn off the ADC
    //ADCSRA&=(~(1<<ADEN));
    // disable ADC
    ADCSRA = 0;
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);
    power_spi_disable();
    power_timer0_disable();
    power_timer1_disable();
    power_timer2_disable() ;
    power_twi_disable();
    power_adc_disable();

    power_usart1_disable();

    //Keep this alive
    //power_usart0_enable();

    sei();
    interrupts();
    sleep_enable();
    sleep_cpu();

    //Snoring can be heard at this point....

    sleep_disable();

    power_adc_enable();
    power_timer0_enable();
    power_timer1_enable();
    power_timer2_enable();

    //power_all_enable();

    ADCSRA = old_ADCSRA;
}

void DiyBMSATTiny841::SelectCellVoltageChannel() {
  //PB2 = ADC8 PIN 5 ARDUINO PIN 2/A8 = VOLTAGE reading
  //ADMUXA – ADC Multiplexer Selection Register A
  //ADC8 (single end) MUX[5:0] 00 1000
  ADMUXA = (0<<MUX5) |(0<<MUX4) |(1<<MUX3) | (0<<MUX2) | (0<<MUX1) | (0<<MUX0);
}

void DiyBMSATTiny841::SelectInternalTemperatureChannel(){
  //PA4
  //ADMUXA – ADC Multiplexer Selection Register A
  //ADC4 (single end) MUX[5:0] 00 0100
  ADMUXA = (0<<MUX5) |(0<<MUX4) |(0<<MUX3) | (1<<MUX2) | (0<<MUX1) | (0<<MUX0);
}

void DiyBMSATTiny841::SelectExternalTemperatureChannel(){
  //External sensor
  //ADMUXA – ADC Multiplexer Selection Register A
  //ADC11 (single end) MUX[5:0] 00 1011
  ADMUXA = (0<<MUX5) |(0<<MUX4) |(1<<MUX3) | (0<<MUX2) | (1<<MUX1) | (1<<MUX0);
}
