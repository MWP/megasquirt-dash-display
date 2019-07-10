/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
  FT81x IO interface.

 Mark Williams (2019-07-01)
 Distributed under the MIT License (see licence.txt)

=========================================================================== */

#include <Arduino.h>
#include "proj_config.h"

void SPI_Setup(void);
void SPI_FullSpeed(void);

void SPI_WriteByte(uint8_t data);
void SPI_WriteBuffer(uint8_t *Buffer, uint32_t Length);
void SPI_Write(uint8_t data);
void SPI_ReadBuffer(uint8_t *Buffer, uint32_t Length);
void SPI_Enable(void);
void SPI_Disable(void);
void EVE_Reset(void);
