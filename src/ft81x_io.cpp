/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
  FT81x IO interface.

 Mark Williams (2019-07-01)
 Distributed under the MIT License (see licence.txt)

=========================================================================== */

#include <Arduino.h>
#include <SPI.h>
#include "proj_config.h"
#include "ft81x_io.h"

void SPI_Setup(void)
{
	pinMode(EVE2_PIN_PD, OUTPUT);
	digitalWrite(EVE2_PIN_PD, HIGH);
	pinMode(EVE2_PIN_CS, OUTPUT);
	digitalWrite(EVE2_PIN_CS, HIGH);
	SPI.begin();
	SPI.setClockDivider(spiFrequencyToClockDiv(8000000));
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
}

void SPI_FullSpeed(void)
{
	SPI.begin();
	SPI.setClockDivider(spiFrequencyToClockDiv(SPI_SPEED));
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
}

void SPI_WriteByte(uint8_t data)
{
	//SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
	digitalWrite(EVE2_PIN_CS, LOW);
	SPI.transfer(data);
	digitalWrite(EVE2_PIN_CS, HIGH);
	//SPI.endTransaction();
}

// Send a series of bytes (contents of a buffer) through SPI
void SPI_WriteBuffer(uint8_t *Buffer, uint32_t Length)
{
	//SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
	digitalWrite(EVE2_PIN_CS, LOW);
	SPI.transferBytes(Buffer, NULL, Length);
	digitalWrite(EVE2_PIN_CS, HIGH);
	//SPI.endTransaction();
}

// Send a byte through SPI as part of a larger transmission.  Does not enable/disable SPI CS
void SPI_Write(uint8_t data)
{
	SPI.transfer(data);
}

// Read a series of bytes from SPI and store them in a buffer
void SPI_ReadBuffer(uint8_t *Buffer, uint32_t Length)
{
	volatile uint8_t a = SPI.transfer(0x00); // dummy read
	(void)(a);

	while (Length--)
	{
		*(Buffer++) = SPI.transfer(0x00);
	}
}

// Enable SPI by activating chip select line
void SPI_Enable(void)
{
	//SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
	digitalWrite(EVE2_PIN_CS, LOW);
}

// Disable SPI by deasserting the chip select line
void SPI_Disable(void)
{
	digitalWrite(EVE2_PIN_CS, HIGH);
	//SPI.endTransaction();
}

//EVE hardware reset
void EVE_Reset(void)
{
	digitalWrite(EVE2_PIN_PD, 0);
	delay(50);
	digitalWrite(EVE2_PIN_PD, 1);
	delay(150);
}
