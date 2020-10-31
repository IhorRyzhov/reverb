#include <unistd.h>
#include <string.h>
#include "st7735.h"
#include "../gpio/gpio.h"
#include "../spi/spi.h"

#define LCD_SPIDEV_PATH "/dev/spidev1.1"
#define LCD_SPI_SPEED 40000000

#define LCD_GPIO_RST 2 // PA02
#define LCD_GPIO_DC 13 // PA13

static uint8_t u8P_graphicMemory[16384 * 2];


static void ST7735_ioInit(void)
{
	spi_open(LCD_SPIDEV_PATH, LCD_SPI_SPEED);

	gpio_init(LCD_GPIO_RST, true);

	gpio_init(LCD_GPIO_DC, true);

	gpio_write(LCD_GPIO_RST, true);

	gpio_write(LCD_GPIO_DC, true);
}

static void ST7735_ioDeinit(void)
{
	spi_close();

	gpio_deinit(LCD_GPIO_RST);

	gpio_deinit(LCD_GPIO_DC);
}


void ST7735_SendDataArray(uint8_t* data, uint16_t len)
{
	spi_write(data, len);
}

void ST7735_SendData(uint8_t data)
{
	spi_write(&data, 1);
}

void ST7735_SendCmd(uint8_t cmd)
{
	gpio_write(LCD_GPIO_DC, false);

	spi_write(&cmd, 1);

	gpio_write(LCD_GPIO_DC, true);
}



void ST7735_Init(void)
{
	uint8_t data[3];

	ST7735_ioInit();

	usleep(5000);
	gpio_write(LCD_GPIO_RST, false);
	usleep(5000);
	gpio_write(LCD_GPIO_RST, true);


	//ST7735_SendCmd(ST7735_SWRESET); // software reset
	//usleep(125000);

	ST7735_SendCmd(ST7735_SLPOUT); // turn off sleep mode
	usleep(125000);

	data[0] = 0x05;
	ST7735_SendCmd(ST7735_COLMOD); // Interface Pixel Format
	ST7735_SendDataArray(data, 1); // 16 bit pixel

	data[0] = 0x01;
	data[1] = 0x2C;
	data[2] = 0x2D;
	ST7735_SendCmd(ST7735_FRMCTR1);
	ST7735_SendDataArray(data, 3);

	data[0] = 0x08;
	ST7735_SendCmd(ST7735_MADCTL);
	ST7735_SendDataArray(data, 3);

	ST7735_SendCmd(ST7735_DISPON);
	usleep(125000);
}

void ST7735_deinit()
{
	ST7735_Clear(ST7735_WHITE);

	ST7735_ioDeinit();
}

void ST7735_AddrSet(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	ST7735_SendCmd(ST7735_CASET); // Column address set
	ST7735_SendData(0x00);
	ST7735_SendData(x1);
	ST7735_SendData(0x00);
	ST7735_SendData(x2);

	ST7735_SendCmd(ST7735_RASET); // Row address set
	ST7735_SendData(0x00);
	ST7735_SendData(y1);
	ST7735_SendData(0x00);
	ST7735_SendData(y2);

	ST7735_SendCmd(ST7735_RAMWR); // Memory write
}

void ST7735_Clear(uint16_t Color)
{
	uint8_t color[2];

	color[0] = (Color >> 8) & 0xFF;
	color[1] = Color & 0x00FF;

	ST7735_AddrSet(2, 1, 129, 128);

	for (uint16_t i = 0; i < 32768; i++)
	{
		u8P_graphicMemory[i++] = color[0];
		u8P_graphicMemory[i] = color[1];
	}

	for(uint8_t i = 0; i < 8; i++)
	{
		ST7735_SendDataArray(&u8P_graphicMemory[i * 4096], 4096);
	}
}

void ST7735_DrawChar(uint8_t x, uint8_t y, uint8_t chr, uint16_t Color, uint16_t BackgroundColor)
{
	uint8_t i, j;
	uint8_t CH, CL, BCH, BCL;
	uint8_t buffer[5];

	uint8_t mem[84];
	uint8_t p = 0;

	CL = Color & 0xFF;
	CH = (Color >> 8) & 0xFF;

	BCL = BackgroundColor & 0xFF;
	BCH = (BackgroundColor >> 8) & 0xFF;

	memcpy(buffer, &Font5x7[(chr - 32) * 5], 5);

	ST7735_AddrSet(x, y, x + 5, y + 7);

	for(i = 0; i < 7; i++)
	{
		for(j = 0; j < 5; j++)
		{
			if((buffer[j] & (1 << i)) == 0)
			{
				mem[p++] = BCH;
				mem[p++] = BCL;
			}
			else
			{
				mem[p++] = CH;
				mem[p++] = CL;
			}
		}
		mem[p++] = BCH;
		mem[p++] = BCL;
	}

	ST7735_SendDataArray(mem, p);
}

void ST7735_DrawChar7x11(uint8_t x, uint8_t y, uint8_t chr, uint16_t Color, uint16_t BackgroundColor)
{
	uint8_t i, j;
	uint8_t CH, CL, BCH, BCL;
	uint8_t buffer[11];

	uint8_t mem[176];
	uint8_t p = 0;

	CL = Color & 0xFF;
	CH = (Color >> 8) & 0xFF;

	BCL = BackgroundColor & 0xFF;
	BCH = (BackgroundColor >> 8) & 0xFF;

	memcpy(buffer, &Font7x11[(chr - 32) * 11], 11);
	ST7735_AddrSet(x, y, x + 7, y + 11);
	for(j = 0; j < 11; j++)
	{
		for(i = 0; i < 7; i++)
		{
			if((buffer[j] & (1 << i)) == 0)
			{
				mem[p++] = BCH;
				mem[p++] = BCL;
			}
			else
			{
				mem[p++] = CH;
				mem[p++] = CL;
			}
		}
		mem[p++] = BCH;
		mem[p++] = BCL;
	}

	ST7735_SendDataArray(mem, p);
}

void ST7735_DrawString(uint8_t x, uint8_t y, const char* chr, uint16_t Color, uint16_t BackgroundColor)
{
	uint8_t i = 0;
	while(chr[i] != '\0')
	{
		ST7735_DrawChar(x + (6 * i), y, chr[i], Color, BackgroundColor);
		i++;
	}
}

void ST7735_DrawString7x11(uint8_t x, uint8_t y, const char* chr, uint16_t Color, uint16_t BackgroundColor)
{
	uint8_t i = 0;
	while(chr[i] != '\0')
	{
		ST7735_DrawChar7x11(x + (8 * i), y, chr[i], Color, BackgroundColor);
		i++;
	}
}
