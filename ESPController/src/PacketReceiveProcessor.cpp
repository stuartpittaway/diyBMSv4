#include "PacketReceiveProcessor.h"

bool PacketReceiveProcessor::ProcessReply(const uint8_t* receivebuffer,
                                          uint16_t sequenceToExpect) {

  // Copy to our buffer
  memcpy(&_packetbuffer, receivebuffer, sizeof(_packetbuffer));

  // Calculate the CRC and compare to received
  uint16_t validateCRC = uCRC16Lib::calculate((char*)&_packetbuffer, sizeof(_packetbuffer) - 2);

  if (validateCRC == _packetbuffer.crc) {
    if (_packetbuffer.sequence == sequenceToExpect) {

      if (ReplyWasProcessedByAModule()) {
        switch (ReplyForCommand()) {
          case COMMAND::SetBankIdentity:
            break;  // Ignore reply
          case COMMAND::ReadVoltageAndStatus:
            ProcessReplyVoltage();
            break;
          case COMMAND::Identify:
            break;  // Ignore reply
          case COMMAND::ReadTemperature:
            ProcessReplyTemperature();
            break;
          case COMMAND::ReadBadPacketCounter:
            break;
          case COMMAND::ReadSettings:
            ProcessReplySettings();
            break;
        }

        //Clear any error counter - we have a good packet
        commsError=0;

        return true;
      } else {
        //Error count for a request that was not processed by any module
        totalNotProcessedErrors++;
      }
    } else {
      // Increase the error count of out of sequence packets
      totalMissedPacketCount++;
    }
  } else {
    //crc error
    totalCRCErrors++;
  }

  return false;
}

void PacketReceiveProcessor::ProcessReplyAddressByte() {
  // address byte
  // B X KK AAAA
  // B    = 1 bit broadcast to all modules (in the bank)
  // X    = 1 bit unused
  // KK   = 2 bits module bank select (4 possible banks of 16 modules) -
  // reserved and not used
  // AAAA = 4 bits for address (module id 0 to 15)

  uint8_t broadcast = (_packetbuffer.address & B10000000) >> 7;
  // uint8_t bank=(_packetbuffer.address & B00110000) >> 4;
  // uint8_t lastAddress=_packetbuffer.address & 0x0F;

  // Only set if it was a reply from a broadcast message
  if (broadcast > 0) {
    if (numberOfModules[ReplyFromBank()] != ReplyLastAddress()) {
      numberOfModules[ReplyFromBank()] = ReplyLastAddress();

      // if we have a different number of modules in this bank
      // we should clear all the cached config flags from the modules
      // as they have probably moved address
      for (size_t i = 0; i < maximum_cell_modules; i++) {
        cmi[ReplyFromBank()][i].settingsCached = false;
        cmi[ReplyFromBank()][i].voltagemVMin = 6000;
        cmi[ReplyFromBank()][i].voltagemVMax = 0;
      }
    }
  }
}

void PacketReceiveProcessor::ProcessReplyTemperature() {
  // Called when a decoded packet has arrived in buffer for command 3

  ProcessReplyAddressByte();
  // 40 offset for below zero temps
  for (size_t i = 0; i < maximum_cell_modules; i++) {
    cmi[ReplyFromBank()][i].internalTemp = ((_packetbuffer.moduledata[i] & 0xFF00) >> 8) - 40;
    cmi[ReplyFromBank()][i].externalTemp = (_packetbuffer.moduledata[i] & 0x00FF) - 40;
  }
}

void PacketReceiveProcessor::ProcessReplyVoltage() {
  // Called when a decoded packet has arrived in _packetbuffer for command 1

  ProcessReplyAddressByte();

  uint8_t b = ReplyFromBank();

  for (size_t i = 0; i < maximum_cell_modules; i++) {
    // 3 top bits remaining
    // X = In bypass
    // Y = Bypass over temperature
    // Z = Not used

    cmi[b][i].voltagemV = _packetbuffer.moduledata[i] & 0x1FFF;
    cmi[b][i].inBypass = (_packetbuffer.moduledata[i] & 0x8000) > 0;
    cmi[b][i].bypassOverTemp = (_packetbuffer.moduledata[i] & 0x4000) > 0;

    if (cmi[b][i].voltagemV > cmi[b][i].voltagemVMax) {
      cmi[b][i].voltagemVMax = cmi[b][i].voltagemV;
    }

    if (cmi[b][i].voltagemV < cmi[b][i].voltagemVMin) {
      cmi[b][i].voltagemVMin = cmi[b][i].voltagemV;
    }
  }

}

void PacketReceiveProcessor::ProcessReplySettings() {
  uint8_t b = ReplyFromBank();
  uint8_t m = ReplyLastAddress();

  // TODO Validate b and m here to prevent array overflow
  cmi[b][m].settingsCached = true;

  FLOATUNION_t myFloat;

  myFloat.word[0] = _packetbuffer.moduledata[0];
  myFloat.word[1] = _packetbuffer.moduledata[1];

  // Arduino float (4 byte)
  cmi[b][m].LoadResistance = myFloat.number;
  // Arduino float(4 byte)
  myFloat.word[0] = _packetbuffer.moduledata[2];
  myFloat.word[1] = _packetbuffer.moduledata[3];
  cmi[b][m].Calibration = myFloat.number;

  // Arduino float(4 byte)
  myFloat.word[0] = _packetbuffer.moduledata[4];
  myFloat.word[1] = _packetbuffer.moduledata[5];
  cmi[b][m].mVPerADC = myFloat.number;
  // uint8_t
  cmi[b][m].BypassOverTempShutdown = _packetbuffer.moduledata[6] & 0x00FF;
  // uint16_t
  cmi[b][m].BypassThresholdmV = _packetbuffer.moduledata[7];
  // uint16_t
  cmi[b][m].Internal_BCoefficient = _packetbuffer.moduledata[8];
  // uint16_t
  cmi[b][m].External_BCoefficient = _packetbuffer.moduledata[9];
}
