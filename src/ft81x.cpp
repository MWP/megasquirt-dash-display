/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 FT81x EVE2 driver/library

 Mark Williams (2019-07-01)
 Distributed under the GNU GENERAL PUBLIC LICENSE v3.0 (see LICENCE file)

=========================================================================== */

#include <Arduino.h>
#include "ft81x_io.h"
#include "ft81x.h"
#include "proj_config.h"
#include "cf_gt911.h"

//globals
uint16_t FT81x_FIFOLocation = 0;

// init
void FT81x_Init(void)
{
	//hard reset of the eve2
	EVE_Reset();

	//wakeup
	DPRINT("FINDING EVE2 ID\n");
	FT81x_HostCommand(HCMD_ACTIVE);
	delay(100);
	while (!FT81x_ReadRegID());
	DPRINT("EVE2 ACTIVE\n");

	//turn off screen
	FT81x_W8(REG_GPIOX + RAM_REG, 0);
	FT81x_W8(REG_PCLK + RAM_REG, 0);

	// load parameters of the physical screen
	FT81x_W16(REG_HCYCLE + RAM_REG, EVE2_HCYCLE);
	FT81x_W16(REG_HOFFSET + RAM_REG, EVE2_HOFFSET);
	FT81x_W16(REG_HSYNC0 + RAM_REG, EVE2_HSYNC0);
	FT81x_W16(REG_HSYNC1 + RAM_REG, EVE2_HSYNC1);
	FT81x_W16(REG_VCYCLE + RAM_REG, EVE2_VCYCLE);
	FT81x_W16(REG_VOFFSET + RAM_REG, EVE2_VOFFSET);
	FT81x_W16(REG_VSYNC0 + RAM_REG, EVE2_VSYNC0);
	FT81x_W16(REG_VSYNC1 + RAM_REG, EVE2_VSYNC1);
	FT81x_W8(REG_SWIZZLE + RAM_REG, EVE2_SWIZZLE);
	FT81x_W8(REG_PCLK_POL + RAM_REG, EVE2_PCLK_POL);
	FT81x_W16(REG_HSIZE + RAM_REG, EVE2_DWIDTH);
	FT81x_W16(REG_VSIZE + RAM_REG, EVE2_DHEIGHT);
	FT81x_W8(REG_CSPREAD + RAM_REG, EVE2_CSPREAD);
	FT81x_W8(REG_DITHER + RAM_REG, EVE2_DITHER);

	//gt911 touch setup
#ifdef EVE2_GT911
	FT8xx_Init_Goodix_GT911();
#endif

	//configure touch & audio
	FT81x_W16(REG_TOUCH_RZTHRESH + RAM_REG, 1200);
	FT81x_W8(REG_TOUCH_MODE + RAM_REG, 0x02);
	FT81x_W8(REG_TOUCH_ADC_MODE + RAM_REG, 0x01);
	FT81x_W8(REG_TOUCH_OVERSAMPLE + RAM_REG, 15);

	//clear the lcd
	FT81x_W32(RAM_DL + 0, CLEAR_COLOR_RGB(0, 0, 0));
	FT81x_W32(RAM_DL + 4, CLEAR(0, 0, 0));
	FT81x_W32(RAM_DL + 8, DISPLAY());
	FT81x_W8(REG_DLSWAP + RAM_REG, DLSWAP_FRAME);
	FT81x_W8(REG_PCLK + RAM_REG, 5);

	//setup GPIO
	FT81x_W16(REG_GPIOX_DIR + RAM_REG, 0x8000);
	FT81x_W16(REG_GPIOX + RAM_REG, 0x8000);
	//setup backlight
	FT81x_W16(REG_PWM_HZ + RAM_REG, 250);
	FT81x_W8(REG_PWM_DUTY + RAM_REG, 0x00);

	//done
}

//write a command
void FT81x_HostCommand(uint8_t command)
{
	SPI_Enable();
	SPI_Write(command);
	SPI_Write(0x00);
	SPI_Write(0x00);
	SPI_Disable();
}

//write 32bits to address
void FT81x_W32(uint32_t address, uint32_t parameter)
{
	SPI_Enable();
	SPI_Write((uint8_t)((address >> 16) | 0x80));
	SPI_Write((uint8_t)(address >> 8));
	SPI_Write((uint8_t) address);
	SPI_Write((uint8_t)(parameter & 0xff));
	SPI_Write((uint8_t)((parameter >> 8) & 0xff));
	SPI_Write((uint8_t)((parameter >> 16) & 0xff));
	SPI_Write((uint8_t)((parameter >> 24) & 0xff));
	SPI_Disable();
}

//write 16bits to address
void FT81x_W16(uint32_t address, uint16_t parameter)
{
	SPI_Enable();
	SPI_Write((uint8_t)((address >> 16) | 0x80));
	SPI_Write((uint8_t)(address >> 8));
	SPI_Write((uint8_t) address);
	SPI_Write((uint8_t)(parameter & 0xff));
	SPI_Write((uint8_t)(parameter >> 8));
	SPI_Disable();
}

//write 8bits to address
void FT81x_W8(uint32_t address, uint8_t parameter)
{
	SPI_Enable();
	SPI_Write((uint8_t)((address >> 16) | 0x80));
	SPI_Write((uint8_t)(address >> 8));
	SPI_Write((uint8_t)(address));
	SPI_Write(parameter);
	SPI_Disable();
}

//read 32bits from address
uint32_t FT81x_R32(uint32_t address)
{
	uint8_t buf[4];
	SPI_Enable();
	SPI_Write((address >> 16) & 0x3F);
	SPI_Write((address >> 8) & 0xff);
	SPI_Write(address & 0xff);
	SPI_ReadBuffer(buf, 4);
	SPI_Disable();
	uint32_t data = buf[0] + ((uint32_t) buf[1] << 8) + ((uint32_t) buf[2] << 16) + ((uint32_t) buf[3] << 24);
	return data;
}

//read 16bits from address
uint16_t FT81x_R16(uint32_t address)
{
	uint8_t buf[2];
	SPI_Enable();
	SPI_Write((address >> 16) & 0x3F);
	SPI_Write((address >> 8) & 0xff);
	SPI_Write(address & 0xff);
	SPI_ReadBuffer(buf, 2);
	SPI_Disable();
	uint16_t data = buf[0] + ((uint16_t) buf[1] << 8);
	return data;
}

//read 8bits from address
uint8_t FT81x_R8(uint32_t address)
{
	uint8_t buf;
	SPI_Enable();
	SPI_Write((address >> 16) & 0x3F);
	SPI_Write((address >> 8) & 0xff);
	SPI_Write(address & 0xff);
	SPI_ReadBuffer(&buf, 1);
	SPI_Disable();
	return buf;
}

//send a command & update fifo location
void FT81x_SendCommand(uint32_t data)
{
	FT81x_W32(FT81x_FIFOLocation + RAM_CMD, data);
	FT81x_FIFOLocation += FT_CMD_SIZE;
	FT81x_FIFOLocation %= FT_CMD_FIFO_SIZE;
}

//write the fifo location
//causes the FT81x to process the command list
void FT81x_UpdateFIFO(void)
{
	FT81x_W16(REG_CMD_WRITE + RAM_REG, FT81x_FIFOLocation);
}

//read chip ID, looking for 0x7C
uint8_t FT81x_ReadRegID(void)
{
	uint8_t data;
	SPI_Enable();
	SPI_Write(0x30);
	SPI_Write(0x20);
	SPI_Write(REG_ID);
	SPI_ReadBuffer(&data, 1);
	SPI_Disable();

	//found
	if (data == 0x7C)
		return 1;

	//not found
	return 0;
}

void FT81x_Slider(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t options, uint16_t val, uint16_t range)
{
	FT81x_SendCommand(CMD_SLIDER);
	FT81x_SendCommand(((uint32_t) y << 16) | x);
	FT81x_SendCommand(((uint32_t) h << 16) | w);
	FT81x_SendCommand(((uint32_t) val << 16) | options);
	FT81x_SendCommand((uint32_t) range);
}

void FT81x_Spinner(uint16_t x, uint16_t y, uint16_t style, uint16_t scale)
{
	FT81x_SendCommand(CMD_SPINNER);
	FT81x_SendCommand(((uint32_t) y << 16) | x);
	FT81x_SendCommand(((uint32_t) scale << 16) | style);
}

void FT81x_Gauge(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range)
{
	FT81x_SendCommand(CMD_GAUGE);
	FT81x_SendCommand(((uint32_t) y << 16) | x);
	FT81x_SendCommand(((uint32_t) options << 16) | r);
	FT81x_SendCommand(((uint32_t) minor << 16) | major);
	FT81x_SendCommand(((uint32_t) range << 16) | val);
}

void FT81x_Dial(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t val)
{
	FT81x_SendCommand(CMD_DIAL);
	FT81x_SendCommand(((uint32_t) y << 16) | x);
	FT81x_SendCommand(((uint32_t) options << 16) | r);
	FT81x_SendCommand((uint32_t) val);
}

void FT81x_Track(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t tag)
{
	FT81x_SendCommand(CMD_TRACK);
	FT81x_SendCommand(((uint32_t) y << 16) | x);
	FT81x_SendCommand(((uint32_t) h << 16) | w);
	FT81x_SendCommand((uint32_t) tag);
}

void Cmd_Number(uint16_t x, uint16_t y, uint16_t font, uint16_t options, uint32_t num)
{
	FT81x_SendCommand(CMD_NUMBER);
	FT81x_SendCommand(((uint32_t) y << 16) | x);
	FT81x_SendCommand(((uint32_t) options << 16) | font);
	FT81x_SendCommand(num);
}

void Cmd_Gradient(uint16_t x0, uint16_t y0, uint32_t rgb0, uint16_t x1, uint16_t y1, uint32_t rgb1)
{
	FT81x_SendCommand(CMD_GRADIENT);
	FT81x_SendCommand(((uint32_t) y0 << 16) | x0);
	FT81x_SendCommand(rgb0);
	FT81x_SendCommand(((uint32_t) y1 << 16) | x1);
	FT81x_SendCommand(rgb1);
}

/*
void FT81x_Button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t font, uint16_t options, const char* str)
{
	uint16_t ptr, i, sptr;

	uint16_t length = strlen(str);
	if (!length)
		return;

	uint32_t* data = (uint32_t*)calloc((length / 4) + 1, sizeof(uint32_t));

	sptr = 0;
	for (ptr = 0; ptr < (length / 4); ptr++, sptr += 4)
		data[ptr] = (uint32_t) str[sptr + 3] << 24
			| (uint32_t) str[sptr + 2] << 16 | (uint32_t) str[sptr + 1] << 8
			| (uint32_t) str[sptr];

	for (i = 0; i < (length % 4); i++, sptr++)
		data[ptr] |= (uint32_t) str[sptr] << (i * 8);

	FT81x_SendCommand(CMD_BUTTON);
	FT81x_SendCommand(((uint32_t) y << 16) | x);
	FT81x_SendCommand(((uint32_t) h << 16) | w);
	FT81x_SendCommand(((uint32_t) options << 16) | font);

	for (i = 0; i <= length / 4; i++)
		FT81x_SendCommand(data[i]);

	free(data);
}
*/

void FT81x_Text(uint16_t x, uint16_t y, uint16_t font, uint16_t options, const char* str)
{
	uint16_t ptr, i, sptr;
	uint16_t length = strlen(str);

	if (!length)
		return;

	uint32_t* data = (uint32_t*)calloc((length / 4) + 1, sizeof(uint32_t));

	sptr = 0;
	for (ptr = 0; ptr < (length / 4); ++ptr, sptr = sptr + 4)
		data[ptr] = (uint32_t) str[sptr + 3] << 24
			| (uint32_t) str[sptr + 2] << 16 | (uint32_t) str[sptr + 1] << 8
			| (uint32_t) str[sptr];

	for (i = 0; i < (length % 4); ++i, ++sptr)
		data[ptr] |= (uint32_t) str[sptr] << (i * 8);

	FT81x_SendCommand(CMD_TEXT);
	FT81x_SendCommand(((uint32_t) y << 16) | x);
	FT81x_SendCommand(((uint32_t) options << 16) | font);

	for (i = 0; i <= length / 4; i++)
		FT81x_SendCommand(data[i]);

	free(data);
}

void FT81x_SetBitmap(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height)
{
	FT81x_SendCommand( CMD_SETBITMAP);
	FT81x_SendCommand(addr);
	FT81x_SendCommand(((uint32_t) width << 16) | fmt);
	FT81x_SendCommand((uint32_t) height);
}

void FT81x_Memcpy(uint32_t dest, uint32_t src, uint32_t num)
{
	FT81x_SendCommand(CMD_MEMCPY);
	FT81x_SendCommand(dest);
	FT81x_SendCommand(src);
	FT81x_SendCommand(num);
}

void Cmd_GetPtr(void)
{
	FT81x_SendCommand(CMD_GETPTR);
	FT81x_SendCommand(0);
}

void Cmd_GradientColor(uint32_t c)
{
	FT81x_SendCommand(CMD_GRADCOLOR);
	FT81x_SendCommand(c);
}

void FT81x_FGcolor(uint32_t c)
{
	FT81x_SendCommand(CMD_FGCOLOR);
	FT81x_SendCommand(c);
}

void Cmd_BGcolor(uint32_t c)
{
	FT81x_SendCommand(CMD_BGCOLOR);
	FT81x_SendCommand(c);
}

void FT81x_Translate(uint32_t tx, uint32_t ty)
{
	FT81x_SendCommand(CMD_TRANSLATE);
	FT81x_SendCommand(tx);
	FT81x_SendCommand(ty);
}

void Cmd_Rotate(uint32_t a)
{
	FT81x_SendCommand(CMD_ROTATE);
	FT81x_SendCommand(a);
}

void FT81x_SetRotate(uint32_t rotation)
{
	FT81x_SendCommand(CMD_SETROTATE);
	FT81x_SendCommand(rotation);
}

void FT81x_Scale(uint32_t sx, uint32_t sy)
{
	FT81x_SendCommand(CMD_SCALE);
	FT81x_SendCommand(sx);
	FT81x_SendCommand(sy);
}


void Cmd_Calibrate(uint32_t result)
{
	FT81x_SendCommand(CMD_CALIBRATE);
	FT81x_SendCommand(result);
}

void FT81x_CalibrateManual(uint16_t width, uint16_t height, uint16_t voffset, int16_t hoffset)
{
	uint32_t displayX[3], displayY[3];
	uint32_t touchX[3], touchY[3];
	uint32_t touchValue = 0;            //, storedValue = 0;
	int32_t tmp, k;
	int32_t TransMatrix[6];
	uint8_t count = 0;
	char num[2];

	// These values determine where your calibration points will be drawn on your display
	displayX[0] = (width * 0.15) + hoffset;
	displayY[0] = (height * 0.15) + voffset;

	displayX[1] = (width * 0.85) + hoffset;
	displayY[1] = (height / 2) + voffset;

	displayX[2] = (width / 2) + hoffset;
	displayY[2] = (height * 0.85) + voffset;

	while (count < 3)
	{
		FT81x_SendCommand(CMD_DLSTART);
		FT81x_SendCommand(CLEAR_COLOR_RGB(64, 64, 64));
		FT81x_SendCommand(CLEAR(1, 1, 1));

		// Draw Calibration Point on screen
		FT81x_SendCommand(COLOR_RGB(255, 0, 0));
		FT81x_SendCommand(POINT_SIZE(20 * 16));
		FT81x_SendCommand(BEGIN(POINTS));
		FT81x_SendCommand(
			VERTEX2F((uint32_t)(displayX[count]) * 16,
				(uint32_t)((displayY[count])) * 16));
		FT81x_SendCommand(END());
		FT81x_SendCommand(COLOR_RGB(255, 255, 255));
		FT81x_Text((width / 2) + hoffset, (height / 3) + voffset, 27, OPT_CENTER, "Calibrating");
		FT81x_Text((width / 2) + hoffset, (height / 2) + voffset, 27, OPT_CENTER, "Please tap the dots");
		num[0] = count + 0x31;
		num[1] = 0;                   // null terminated string of one character
		FT81x_Text(displayX[count], displayY[count], 27, OPT_CENTER, num);

		FT81x_SendCommand(DISPLAY());
		FT81x_SendCommand(CMD_SWAP);
		FT81x_UpdateFIFO();
		FT81x_FIFO_WaitUntilEmpty();
		delay(300);

		touchValue = FT81x_R32(REG_TOUCH_DIRECT_XY + RAM_REG);
		if (!(touchValue & 0x80000000))
		{
			touchX[count] = (touchValue >> 16) & 0x03FF;
			touchY[count] = touchValue & 0x03FF;
			count++;
		}
	}

	k = ((touchX[0] - touchX[2]) * (touchY[1] - touchY[2])) - ((touchX[1] - touchX[2]) * (touchY[0] - touchY[2]));

	tmp = (((displayX[0] - displayX[2]) * (touchY[1] - touchY[2])) - ((displayX[1] - displayX[2]) * (touchY[0] - touchY[2])));
	TransMatrix[0] = FT81x_CalcCoef(tmp, k);

	tmp = (((touchX[0] - touchX[2]) * (displayX[1] - displayX[2])) - ((displayX[0] - displayX[2]) * (touchX[1] - touchX[2])));
	TransMatrix[1] = FT81x_CalcCoef(tmp, k);

	tmp = ((touchY[0] * (((touchX[2] * displayX[1]) - (touchX[1] * displayX[2]))))
			+ (touchY[1] * (((touchX[0] * displayX[2]) - (touchX[2] * displayX[0]))))
			+ (touchY[2] * (((touchX[1] * displayX[0]) - (touchX[0] * displayX[1])))));
	TransMatrix[2] = FT81x_CalcCoef(tmp, k);

	tmp = (((displayY[0] - displayY[2]) * (touchY[1] - touchY[2])) - ((displayY[1] - displayY[2]) * (touchY[0] - touchY[2])));
	TransMatrix[3] = FT81x_CalcCoef(tmp, k);

	tmp = (((touchX[0] - touchX[2]) * (displayY[1] - displayY[2])) - ((displayY[0] - displayY[2]) * (touchX[1] - touchX[2])));
	TransMatrix[4] = FT81x_CalcCoef(tmp, k);

	tmp = ((touchY[0] * (((touchX[2] * displayY[1]) - (touchX[1] * displayY[2]))))
			+ (touchY[1] * (((touchX[0] * displayY[2]) - (touchX[2] * displayY[0]))))
			+ (touchY[2] * (((touchX[1] * displayY[0]) - (touchX[0] * displayY[1])))));
	TransMatrix[5] = FT81x_CalcCoef(tmp, k);

	count = 0;
	do
	{
		FT81x_W32(REG_TOUCH_TRANSFORM_A + RAM_REG + (count * 4), TransMatrix[count]);
		count++;
	} while (count < 6);
}

void FT81x_SetBacklight(uint8_t brightness)
{
	FT81x_W8(REG_PWM_DUTY + RAM_REG, brightness);            // Backlight PWM duty (brightness)
}

uint16_t FT81x_FIFO_FreeSpace(void)
{
	uint16_t cmdBufferDiff, cmdBufferRd, cmdBufferWr, retval;

	cmdBufferRd = FT81x_R16(REG_CMD_READ + RAM_REG);
	cmdBufferWr = FT81x_R16(REG_CMD_WRITE + RAM_REG);

	cmdBufferDiff = (cmdBufferWr - cmdBufferRd) % FT_CMD_FIFO_SIZE;
	retval = (FT_CMD_FIFO_SIZE - 4) - cmdBufferDiff;
	return (retval);
}

void FT81x_FIFO_Wait(uint32_t room)
{
	uint16_t getfreespace;
	do
	{
		getfreespace = FT81x_FIFO_FreeSpace();
	} while (getfreespace < room);
}

uint8_t FT81x_FIFO_IsEmpty(void)
{
	return FT81x_R16(REG_CMD_READ + RAM_REG) == FT81x_R16(REG_CMD_WRITE + RAM_REG);
}

void FT81x_FIFO_WaitUntilEmpty(void)
{
	while (FT81x_R16(REG_CMD_READ + RAM_REG) != FT81x_R16(REG_CMD_WRITE + RAM_REG));
}

void FT81x_StartTransfer(uint32_t address, uint8_t reading)
{
	SPI_Enable();
	if (reading)
	{
		SPI_Write(address >> 16);
		SPI_Write(address >> 8);
		SPI_Write(address);
		SPI_Write(0);
	}
	else
	{
		SPI_Write((address >> 16) | 0x80);
		SPI_Write(address >> 8);
		SPI_Write(address);
	}
}

void FT81x_WriteCommandBuffer(uint8_t *buff, uint32_t count)
{
	uint32_t TransferSize = 0;
	int32_t Remaining = count;

	do
	{
		FT81x_FIFO_Wait(EVE2_WORKBUFSIZE);
		if (Remaining > EVE2_WORKBUFSIZE)
			TransferSize = EVE2_WORKBUFSIZE;
		else
		{
			TransferSize = Remaining;
			TransferSize = (TransferSize + 3) & 0xFFC;
		}

		DPRINTF("TransferSize=%d\n", TransferSize);
		DPRINTF("Remaining=%d\n", Remaining);
		DPRINTF("FifoWriteLocation=%d\n", FT81x_FIFOLocation);

		FT81x_StartTransfer(FT81x_FIFOLocation + RAM_CMD, false);

		SPI_WriteBuffer(buff, TransferSize);
		buff += TransferSize;

		FT81x_FIFOLocation = (FT81x_FIFOLocation + TransferSize) % FT_CMD_FIFO_SIZE;
		SPI_Disable();
		FT81x_W16(REG_CMD_WRITE + RAM_REG, FT81x_FIFOLocation);
		Remaining -= TransferSize;
	} while (Remaining > 0);
}

uint32_t FT81x_WriteBlockRAM(uint32_t address, const uint8_t *buff, uint32_t count)
{
	uint8_t index;
	uint32_t waddr = address;
	for (index = 0; index < count; index++)
		FT81x_W8(waddr++, buff[index]);
	return waddr;
}

int32_t CalcCoef(int32_t Q, int32_t K)
{
	int8_t sn = 0;
	if (Q < 0)
	{
		Q *= -1;
		sn++;
	}
	if (K < 0)
	{
		K *= -1;
		sn++;
	}
	uint32_t I = ((uint32_t) Q / (uint32_t) K) << 16;
	uint32_t R = Q % K;
	R = R << 14;
	R = R / K;
	R = R << 2;
	int32_t returnValue = I + R;
	if (sn & 0x01) returnValue *= -1;
	return returnValue;
}
