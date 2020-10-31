#include "display.h"
#include "st7735.h"
#include <stdio.h>

void display_init()
{
	ST7735_Init();

	ST7735_Clear(ST7735_WHITE);
}

void display_deinit()
{
	ST7735_deinit();
}
