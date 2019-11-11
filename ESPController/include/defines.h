#ifndef DIYBMS_DEFINES_H_
#define DIYBMS_DEFINES_H_

#define GREEN_LED D0

#define GREEN_LED_ON digitalWrite(GREEN_LED,HIGH)
#define GREEN_LED_OFF digitalWrite(GREEN_LED,LOW)

#define EEPROM_SETTINGS_START_ADDRESS 256

#define RELAY_ON 0xFF
#define RELAY_OFF 0x99
#define RELAY_X 0x00

#define RELAY_RULES 7
//Number of relays on board (4)
#define RELAY_TOTAL 4


#define SHOW_TIME_PERIOD 5000
#define NTP_TIMEOUT 1500


struct diybms_eeprom_settings {
  bool combinationParallel;
  uint8_t totalNumberOfBanks;

  uint16_t rulevalue[RELAY_RULES];

  //Use a bit pattern to indicate the relay states
  uint8_t rulerelaystate[RELAY_RULES][RELAY_TOTAL];
  //Default starting state
  uint8_t rulerelaydefault[RELAY_TOTAL];

  int8_t timeZone;// = 0;
  int8_t minutesTimeZone;// = 0;
  bool daylight;//=false;
  char ntpServer[64+1];// = "time.google.com";


  //NOTE this array is subject to buffer overflow vulnerabilities!
  bool mqtt_enabled;
  uint16_t mqtt_port;
  char mqtt_server[64+1];
  char mqtt_username[32+1];
  char mqtt_password[32+1];

  bool influxdb_enabled;
  uint16_t influxdb_httpPort;
  char influxdb_host[64 +1];
  char influxdb_database[32 + 1];
  char influxdb_user[32 + 1];
  char influxdb_password[32 + 1];
};


typedef union
{
 float number;
 uint8_t bytes[4];
 uint16_t word[2];
} FLOATUNION_t;

enum COMMAND: uint8_t
{
    SetBankIdentity=B00000000,
    ReadVoltageAndStatus=B00000001,
    Identify=B00000010,
    ReadTemperature=B00000011,
    ReadBadPacketCounter=B00000100,
    ReadSettings=B00000101,
    WriteSettings=B00000110

    // 0000 0000  = set bank identity
    // 0000 0001  = read voltage and status
    // 0000 0010  = identify module (flash leds)
    // 0000 0011  = Read temperature
    // 0000 0100  = Report number of bad packets
    // 0000 0101  = Report settings/configuration
};

//Maximum of 16 cell modules (dont change this!)
#define maximum_cell_modules 16

//NOTE THIS MUST BE EVEN IN SIZE (BYTES) ESP8266 IS 32 BIT AND WILL ALIGN AS SUCH!
struct packet {
  uint8_t address;
  uint8_t command;
  uint16_t sequence;
  uint16_t moduledata[maximum_cell_modules];
  uint16_t crc;
}  __attribute__((packed));

struct CellModuleInfo {
  //Used as part of the enquiry functions
  bool settingsCached;

  uint16_t voltagemV;
  uint16_t voltagemVMin;
  uint16_t voltagemVMax;
  //Signed integer - should these be byte?
  int8_t internalTemp;
  int8_t externalTemp;

  bool inBypass;
  bool bypassOverTemp;

  uint8_t BypassOverTempShutdown;
  uint16_t BypassThresholdmV;

  uint16_t badPacketCount;

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
extern uint8_t numberOfModules[4];


#endif
