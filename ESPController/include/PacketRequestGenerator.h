#ifndef PacketRequestGenerator_H_
#define PacketRequestGenerator_H_

#include <Arduino.h>
#include <cppQueue.h>
#include <defines.h>

//Tiny and cross-device compatible CCITT CRC16 calculator library - uCRC16Lib
//https://github.com/Naguissa/uCRC16Lib
#include <uCRC16Lib.h>


class PacketRequestGenerator {
   public:
     PacketRequestGenerator(Queue* requestQ) {_requestq=requestQ;}
     ~PacketRequestGenerator() {}
     void sendGetSettingsRequest(uint8_t b,uint8_t m);
     void sendIdentifyModuleRequest(uint8_t b,uint8_t m);
     void sendCellVoltageRequest();
     void sendCellTemperatureRequest();

  private:
    Queue* _requestq;
    packet _packetbuffer;
    void pushPacketToQueue();
    void setPacketAddress(bool broadcast,uint8_t bank,uint8_t module);
    void clearmoduledata();
};



#endif
