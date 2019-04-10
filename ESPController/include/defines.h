#ifndef DIYBMS_DEFINES_H_
#define DIYBMS_DEFINES_H_

#define GREEN_LED D6

#define GREEN_LED_ON digitalWrite(GREEN_LED,HIGH)
#define GREEN_LED_OFF digitalWrite(GREEN_LED,LOW)

//Maximum of 16 cell modules (dont change this!)
#define maximum_cell_modules 16
//NOTE THIS MUST BE EVEN IN SIZE (BYTES) ESP8266 IS 32 BIT AND WILL ALIGN AS SUCH!
struct packet {
  uint8_t address;
  uint8_t command;
  uint16_t moduledata[maximum_cell_modules];
  uint16_t crc;
}  __attribute__((packed));

extern packet buffer;

struct CellModuleInfo {
  //Used as part of the enquiry functions
  bool settingsCached;
  bool settingsRequested;
  bool identifyModule;


  uint16_t voltagemV;
  uint16_t voltagemVMin;
  uint16_t voltagemVMax;
  //Signed integer - should these be byte?
  int16_t internalTemp;
  int16_t externalTemp;

  bool inBypass;
  bool bypassOverTemp;

  uint8_t BypassOverTempShutdown;
  uint16_t BypassThresholdmV;

  // Resistance of bypass load
  float LoadResistance;
  //Voltage Calibration
  float Calibration;
  //Reference voltage (millivolt) normally 2.00mV
  float mVPerADC;
  //Internal Thermistor settings
  uint16_t Internal_BCoefficient;
  //External Thermistor settings
  uint16_t External_BCoefficient;

};

//This holds all the cell information in a large array 2D array (4x16)
extern CellModuleInfo cmi[4][maximum_cell_modules];

extern int numberOfModules[4];


#endif
