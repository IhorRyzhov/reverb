#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include "gpio.h"

void gpio_init(uint8_t u8L_gpio, bool bL_direction)
{
	int32_t s32L_fd;
	char cPL_buff[64];

	// Export pin

	s32L_fd = open("/sys/class/gpio/export", O_WRONLY);

	sprintf(cPL_buff, "%u", u8L_gpio);

	write(s32L_fd, cPL_buff, strlen(cPL_buff) + 1);

	close(s32L_fd);

	// Set direction

	sprintf(cPL_buff, "/sys/class/gpio/gpio%u/direction", u8L_gpio);

	s32L_fd = open(cPL_buff, O_RDWR);

	if (false == bL_direction)
	{
		write(s32L_fd, "in", 3);
	}
	else
	{
		write(s32L_fd, "out", 4);
	}

	close(s32L_fd);
}

void gpio_deinit(uint8_t u8L_gpio)
{
	int32_t s32L_fd;
	char cPL_buff[64];

	// Export pin

	s32L_fd = open("/sys/class/gpio/unexport", O_WRONLY);

	sprintf(cPL_buff, "%u", u8L_gpio);

	write(s32L_fd, cPL_buff, strlen(cPL_buff) + 1);

	close(s32L_fd);
}

void gpio_write(uint8_t u8L_gpio, bool bL_value)
{
	uint8_t u8L_value;
	int32_t s32L_desc;
	char cPL_buff[64];

	sprintf(cPL_buff, "/sys/class/gpio/gpio%u/value", u8L_gpio);

	s32L_desc = open(cPL_buff, O_RDWR);

	if (true == bL_value)
	{
		write(s32L_desc, "1", 2);
	}
	else
	{
		write(s32L_desc, "0", 2);
	}

	close(s32L_desc);
}

bool gpio_read(uint8_t u8L_gpio)
{
	uint8_t u8L_value;
	int32_t s32L_desc;
	char cPL_buff[64];

	sprintf(cPL_buff, "/sys/class/gpio/gpio%u/value", u8L_gpio);

	s32L_desc = open(cPL_buff, O_RDWR);

	read(s32L_desc, &u8L_value, 2);

	close(s32L_desc);

	if (u8L_value == '1')
	{
		return true;
	}
	else
	{
		return false;
	}
}



