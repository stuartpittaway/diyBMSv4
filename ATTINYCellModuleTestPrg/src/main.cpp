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


//USING CLOCKWISE PIN MAPPINGS
#define dump_enable 3
#define green_led 6
#define blue_led 5
#define enable_2048 7

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <FastPID.h>

//Kp: Determines how aggressively the PID reacts to the current amount of error (Proportional)
//Ki: Determines how aggressively the PID reacts to error over time (Integral)
//Kd: Determines how aggressively the PID reacts to the change in error (Derivative)
//3Hz rate - number of times we call this code in Loop
FastPID myPID(180.0, 3.5, 8, 3,
              16, false);

  int16_t ThermistorToCelcius(uint16_t BCOEFFICIENT, uint16_t RawADC) {
  //The thermistor is connected in series with another 47k resistor
  //and across the 2.048V reference giving 50:50 weighting

  //We can calculate the  Steinhart-Hart Thermistor Equation based on the B Coefficient of the thermistor
  // at 25 degrees C rating
  #define NOMINAL_TEMPERATURE 25

  //If we get zero its likely the ADC is connected to ground
   if (RawADC>0){
      //47000 = resistor value
      //https://arduinodiy.wordpress.com/2015/11/10/measuring-temperature-with-ntc-the-steinhart-hart-formula/

      float steinhart= (1023.0F/(float)RawADC - 1.0);

      steinhart = log(steinhart); // ln(R/Ro)
      steinhart /= BCOEFFICIENT; // 1/B * ln(R/Ro)
      steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15); // + (1/To)
      steinhart = 1.0 / steinhart; // Invert
      steinhart -= 273.15; // convert to oC
      return (int16_t)round(steinhart);
   }

   return -273;
  }



  void BeginADCReading() {
    ADMUXB = _BV(REFS2);
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1); // | _BV(ADPS0);
    ADCSRA |= _BV(ADEN) | _BV(ADIF); // enable ADC, turn off any pending interrupt

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

  void SetTimer2Value(uint16_t value) {
    OCR2B = value;
  }

  void StopTimer2() {
    TOCPMCOE = 0;
    TCCR2B = 0;
    OCR2B = 0;
    //TIMSK2 = 0;
  }

  void StartTimer2() {
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

void setup() {

  Serial1.begin(38400, SERIAL_8N1);
  Serial1.println(F("DIYBMS Test Program"));

  if (myPID.err()) {
    //Report and halt here
    Serial1.println(F("PDF config error!"));
  }

  //The TIMER2 can vary between 0 and 10,000
  myPID.setOutputRange(0, 10000);

  // put your setup code here, to run once:
  pinMode(green_led,OUTPUT);
  //pinMode(blue_led,OUTPUT);
  pinMode(dump_enable,OUTPUT);
  pinMode(enable_2048,OUTPUT);

  pinMode(A4,INPUT);

  //Leave 2.048V reference on all the time
  digitalWrite(enable_2048,HIGH);
  digitalWrite(dump_enable,LOW);

  //PUEA – Port A Pull-Up Enable Control Register (All disabled)
  PUEA = 0;
  //PUEB – Port B Pull-Up Enable Control Register (All disabled)
  PUEB = 0;

  //DDRA – Port A Data Direction Register
  //When DDAn is set, the pin PAn is configured as an output. When DDAn is cleared, the pin is configured as an input
  DDRA |= _BV(DDA3);

  //Set the extra high sink capability of pin PA7 is enabled.
  PHDE |= _BV(PHDEA1);

  StartTimer2();
}


void SelectInternalTemperatureChannel() {
  //PA4
  //ADMUXA – ADC Multiplexer Selection Register A
  //ADC4 (single end) MUX[5:0] 00 0100
  ADMUXA = (0 << MUX5) | (0 << MUX4) | (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (0 << MUX0);
}


volatile uint16_t rawTemp;

uint16_t ReadADC() {
  // must read ADCL first
  uint8_t low = ADCL;
  return (ADCH << 8) | low;
}

ISR(ADC_vect) {
  // when ADC completed, take an interrupt and process result
  rawTemp=ReadADC();
}

void loop() {
  digitalWrite(green_led,HIGH);

  SelectInternalTemperatureChannel();
  BeginADCReading();

  int16_t temperature=ThermistorToCelcius(4150, rawTemp);
  Serial1.print(temperature);
  Serial1.print(" (");

  Serial1.print(millis());
  //We must not call this more than 3 times per second
  //Try and maintain 70 degrees
  uint16_t output = myPID.step(70, temperature);
  Serial1.print(" ");
  Serial1.print(millis());
  Serial1.print(") =");
  Serial1.println(output);

  SetTimer2Value(output);
  delay(150);
  digitalWrite(green_led,LOW);
  delay(160);
}
