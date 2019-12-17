/*
____  ____  _  _  ____  __  __  ___
(  _ \(_  _)( \/ )(  _ \(  \/  )/ __)
)(_) )_)(_  \  /  ) _ < )    ( \__ \
(____/(____) (__) (____/(_/\/\_)(___/

DIYBMS V4.0
CELL MODULE FOR ATTINY841

(c)2019 Stuart Pittaway

COMPILE THIS CODE USING PLATFORM.IO

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
*/
/*

HARDWARE ABSTRACTION CODE FOR ATTINY841

  PIN MAPPINGS
  Diagram
  https://github.com/SpenceKonde/ATTinyCore/blob/master/avr/extras/ATtiny_x41.md

  PA1 = PIN 12 SERIAL TRANSMIT (TXD0)
  PA2 = PIN 11 SERIAL RECEIVE (RXD0)

  PA3 = DUMP LOAD ENABLE / PIN 10 /  ARDUINO PIN 7/A3 / TOCC2
  PA4 = ADC4 PIN 9 ARDUINO PIN 6/A4 = ON BOARD TEMP sensor
  PA5 = SERIAL PORT 1 TXD1 - NOT USED
  PA6 = GREEN_LED / PIN 7 / ARDUINO PIN 4/A6
  PA7 = ADC7 = PIN 6 = ARDUINO PIN 3/A7 = 2.048V REFERENCE ENABLE

  PB2 = ADC8 PIN 5 ARDUINO PIN 2/A8 = VOLTAGE reading
  PB0 = ADC11 PIN 2 ARDUINO PIN 0/A11 = REMOTE TEMP sensor
  PB1 = ADC10 PIN 3 ARDUINO PIN 1/A10 = SPARE INPUT/OUTPUT

  ATTiny841 data sheet
  http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8495-8-bit-AVR-Microcontrollers-ATtiny441-ATtiny841_Datasheet.pdf
*/

#include "diybms_attiny841.h"

void DiyBMSATTiny841::SetTimer2Value(uint16_t value) {
  OCR2B = value;
}

void DiyBMSATTiny841::StopTimer2() {
  TOCPMCOE = 0;
  TCCR2B = 0;
  OCR2B = 0;
  //TIMSK2 = 0;
}

void DiyBMSATTiny841::StartTimer2() {
  //Dump resistor is on PA3 which maps to TOCC2
  //Before this is called, the DDR register has already been set

  //Enable OC2B for TOCC2
  TOCPMSA0 = (1 << TOCC2S1);

  // Timer/Counter Output Compare Pin Mux Channel Output Enable
  TOCPMCOE = (1 << TOCC2OE);

  // Fast PWM, mode 14, non inverting, presc 1:8
  //COM2b1= Clear OCnA/OCnB on Compare Match (Set output to low level)
  TCCR2A = (1 << COM2B1) | 1 << WGM21;

  //Clock div 64 prescaler
  TCCR2B = 1 << CS21 | 1 << CS20 | 1 << WGM23 | 1 << WGM22;

  //Maximum of 10000 and low of zero
  ICR2 = 10000 - 1;

  //OFF
  SetTimer2Value(0);
}


void DiyBMSATTiny841::double_tap_green_led() {
  GreenLedOn();
  delay(50);
  GreenLedOff();
  delay(50);
  GreenLedOn();
  delay(50);
  GreenLedOff();
}

void DiyBMSATTiny841::ConfigurePorts() {
  //PUEA – Port A Pull-Up Enable Control Register (All disabled)
  PUEA = 0;
  //PUEB – Port B Pull-Up Enable Control Register (All disabled)
  PUEB = 0;

  //DDRA – Port A Data Direction Register
  //When DDAn is set, the pin PAn is configured as an output. When DDAn is cleared, the pin is configured as an input
  DDRA |= _BV(DDA3) | _BV(DDA6) | _BV(DDA7) | _BV(DDA5);

  //DDRB – Port B Data Direction Register
  //Spare pin is output
  DDRB |= _BV(DDB1);

  //Set the extra high sink capability of pin PA7 is enabled.
  PHDE |= _BV(PHDEA1);

  //Set pins to initial state
  DumpLoadOff();
  ReferenceVoltageOff();

  BlueLedOff();
  GreenLedOff();
}

void DiyBMSATTiny841::DumpLoadOn() {
  PORTA |= _BV(PORTA3);
}

void DiyBMSATTiny841::DumpLoadOff() {
  PORTA &= (~_BV(PORTA3));
}

void DiyBMSATTiny841::ReferenceVoltageOn() {
  //When to switch 2.048V regulator on or off. Connected to Pin 6, PA7
  PORTA |= _BV(PORTA7);
}

void DiyBMSATTiny841::ReferenceVoltageOff() {
  //When to switch 2.048V regulator on or off. Connected to Pin 6, PA7
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

void DiyBMSATTiny841::SparePinOn() {
  PORTB |= _BV(PORTB1);
}

void DiyBMSATTiny841::SparePinOff() {
  PORTB &= (~_BV(PORTB1));
}

void DiyBMSATTiny841::BlueLedOn() {
  PORTA |= _BV(PORTA5);
}

void DiyBMSATTiny841::BlueLedOff() {
  PORTA &= (~_BV(PORTA5));
}

void DiyBMSATTiny841::FlushSerial0() {
  Serial.flush();
}

void DiyBMSATTiny841::DisableSerial0TX() {
  UCSR0B &= ~_BV(TXEN0); //disable transmitter (saves 6mA)
}

void DiyBMSATTiny841::EnableSerial0TX() {
  UCSR0B |= (1 << TXEN0); // enable transmitter
}

void DiyBMSATTiny841::DisableSerial0() {
  //Disable serial0
  UCSR0B &= ~_BV(RXEN0); //disable receiver
  UCSR0B &= ~_BV(TXEN0); //disable transmitter
}

void DiyBMSATTiny841::EnableSerial0() {
  UCSR0B |= (1 << RXEN0); // enable RX Serial0
  UCSR0B |= (1 << TXEN0); // enable TX Serial0
}

void DiyBMSATTiny841::DisableSerial1() {
  UCSR1B &= ~_BV(RXEN1); //disable receiver
  UCSR1B &= ~_BV(TXEN1); //disable transmitter
}

void DiyBMSATTiny841::EnableSerial1() {
  UCSR1B |= (1 << RXEN1); // enable RX Serial1
  UCSR1B |= (1 << TXEN1); // enable TX Serial1
}

void DiyBMSATTiny841::EnableStartFrameDetection() {
  noInterrupts();
  // Enable Start Frame Detection
  UCSR0D = (1 << RXSIE0) | (1 << SFDE0);

  interrupts();
}

void DiyBMSATTiny841::EnablePinChangeInterrupt() {
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

void DiyBMSATTiny841::DisablePinChangeInterrupt() {
  GIMSK &= ~(1 << PCIE0); // disable interrupt
}

void DiyBMSATTiny841::SetWatchdog8sec() {
  //Setup a watchdog timer for 8 seconds
  MCUSR = 0;
  //Enable watchdog (to reset)
  WDTCSR |= bit(WDE);

  CCP = 0xD8;
  //WDTCSR – Watchdog Timer Control and Status Register
  // We INTERRUPT the chip after 8 seconds of sleeping (not reboot!)
  // WDE: Watchdog Enable
  // Bits 5, 2:0 – WDP[3:0]: Watchdog Timer Prescaler 3 - 0
  WDTCSR = bit(WDIE) | bit(WDP3) | bit(WDP0);
  //| bit(WDE)

  wdt_reset();
}

uint16_t DiyBMSATTiny841::ReadADC() {

  //BlueLedOff();


  // must read ADCL first
  uint8_t low = ADCL;
  return (ADCH << 8) | low;
}

void DiyBMSATTiny841::BeginADCReading() {

  //BlueLedOn();

  //ADMUXB – ADC Multiplexer Selection Register
  //Select external AREF pin (internal reference turned off)
  ADMUXB = _BV(REFS2);

  //ADCSRA – ADC Control and Status Register A
  //Consider ADC sleep conversion mode?
  //prescaler of 64 = 8MHz/64 = 125KHz.
  ADCSRA |= _BV(ADPS2) | _BV(ADPS1); // | _BV(ADPS0);

  //adc_enable();
  //Bit 4 – ADIF: ADC Interrupt Flag
  //Bit 7 – ADEN: ADC Enable
  ADCSRA |= _BV(ADEN) | _BV(ADIF); // enable ADC, turn off any pending interrupt

  // wait for ADC to settle
  // The ADC must be enabled during the settling time.
  // ADC requires a settling time of 1ms before measurements are stable
  delay(2);

  noInterrupts();
  set_sleep_mode(SLEEP_MODE_ADC); // sleep during ADC sample
  sleep_enable();

  // start the conversion
  ADCSRA |= _BV(ADSC) | _BV(ADIE);
  interrupts();
  sleep_cpu();
  sleep_disable();

  // awake again, reading should be done, better make sure maybe the timer interrupt fired
  while (bit_is_set(ADCSRA, ADSC)) {}

  //adc_disable
  ADCSRA &= (~(1 << ADEN));
}

void DiyBMSATTiny841::Sleep() {
  //ATTINY841 sleep mode
  byte old_ADCSRA = ADCSRA;
  //For low power applications, before entering sleep, remember to turn off the ADC
  //ADCSRA&=(~(1<<ADEN));
  // disable ADC
  ADCSRA = 0;
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
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
  ADMUXA = (0 << MUX5) | (0 << MUX4) | (1 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);
}

void DiyBMSATTiny841::SelectInternalTemperatureChannel() {
  //PA4
  //ADMUXA – ADC Multiplexer Selection Register A
  //ADC4 (single end) MUX[5:0] 00 0100
  ADMUXA = (0 << MUX5) | (0 << MUX4) | (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (0 << MUX0);
}

void DiyBMSATTiny841::SelectExternalTemperatureChannel() {
  //External sensor
  //ADMUXA – ADC Multiplexer Selection Register A
  //ADC11 (single end) MUX[5:0] 00 1011
  ADMUXA = (0 << MUX5) | (0 << MUX4) | (1 << MUX3) | (0 << MUX2) | (1 << MUX1) | (1 << MUX0);
}
