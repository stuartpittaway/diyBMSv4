#include <Arduino.h>

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

#define green_led 4
#define blue_led 5
#define dump_enable 7
#define enable_2048 3

void setup() {
  // put your setup code here, to run once:
  pinMode(green_led,OUTPUT);
  pinMode(blue_led,OUTPUT);
  pinMode(dump_enable,OUTPUT);
  pinMode(enable_2048,OUTPUT);

  //Leave 2.048V reference on all the time
  digitalWrite(enable_2048,HIGH);
}

int counter=0;

void loop() {
  counter++;
  digitalWrite(blue_led,LOW);
  digitalWrite(green_led,HIGH);
  delay(500);
  digitalWrite(blue_led,HIGH);
  digitalWrite(green_led,LOW);
  delay(500);

  if (counter % 10==0) {
    digitalWrite(dump_enable,HIGH);
    delay(2000);
    digitalWrite(dump_enable,LOW);
  }
}
