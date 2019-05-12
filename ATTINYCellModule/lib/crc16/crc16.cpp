#include "crc16.h"

// Calculate XMODEM 16 crc code on data array
uint16_t CRC16::CalculateArray(uint8_t data[], uint16_t length)
{
  //Calculates XMODEM CRC16
  uint8_t reflectIn=false;
  uint8_t reflectOut=false;
  uint16_t polynomial=0x1021;
  uint16_t xorIn=0x0000;
  uint16_t xorOut=0x0000;
  uint16_t msbMask=0x8000;
  uint16_t mask=0xffff;

	uint16_t crc = xorIn;

	int j;
	uint8_t c;
	uint16_t bit;

	if (length == 0) return crc;

	for (uint16_t i = 0; i < length; i++)
	{
		c = data[i];

		if (reflectIn != 0)
			c = (uint8_t) reflect(c, 8);

		j = 0x80;

		while (j > 0)
		{
			bit = (uint16_t)(crc & msbMask);
			crc <<= 1;

			if ((c & j) != 0)
			{
				bit = (uint16_t)(bit ^ msbMask);
			}

			if (bit != 0)
			{
				crc ^= polynomial;
			}

			j >>= 1;
		}
	}

	if (reflectOut != 0)
		crc = (uint16_t)((reflect(crc) ^ xorOut) & mask);

	return crc;
}

uint8_t CRC16::reflect(uint8_t data, uint8_t bits)
{
	unsigned long reflection = 0x00000000;

	for (uint8_t bit = 0; bit < bits; bit++)
	{
		if ((data & 0x01) != 0)
		{
			reflection |= (unsigned long)(1 << ((bits - 1) - bit));
		}

		data = (uint8_t)(data >> 1);
	}

	return reflection;
}
