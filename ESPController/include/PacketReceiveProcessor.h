#ifndef PacketReceiveProcessor_H_
#define PacketReceiveProcessor_H_

#include <Arduino.h>
#include <defines.h>

//Tiny and cross-device compatible CCITT CRC16 calculator library - uCRC16Lib
//https://github.com/Naguissa/uCRC16Lib
#include <uCRC16Lib.h>

typedef union
{
 float number;
 uint8_t bytes[4];
 uint16_t word[2];
} FLOATUNION_t;



class PacketReceiveProcessor {
   public:
     PacketReceiveProcessor() {}
     ~PacketReceiveProcessor() {}
     bool ProcessReply(const uint8_t* receivebuffer, uint16_t sequenceToExpect);
     uint16_t totalMissedPacketCount=0;
     uint16_t totalCRCErrors=0;
     uint16_t totalNotProcessedErrors=0;
     uint8_t commsError=0;

  private:
    packet _packetbuffer;
    uint8_t ReplyFromBank() {return (_packetbuffer.address & B00110000) >> 4;}
    uint8_t ReplyLastAddress() {return _packetbuffer.address & 0x0F;}
    uint8_t ReplyForCommand() { return (_packetbuffer.command & 0x0F); }
    bool ReplyWasProcessedByAModule() {return (_packetbuffer.command & B10000000)>0;}

    void ProcessReplySettings();
    void ProcessReplyVoltage();
    void ProcessReplyTemperature();
    void ProcessReplyAddressByte();
};



#endif
