#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "spi.h"

static struct spi_ioc_transfer obj_transfer;

static int32_t s32_fd;

void spi_open(const char* cPL_filename, uint32_t u32L_speed)
{
	uint8_t u8L_mode = SPI_MODE_0;
	uint8_t u8L_bitsOrder = 0;
	uint8_t u8L_bitsPerWord = 8;

	if ((s32_fd = open(cPL_filename, O_RDWR)) < 0)
	{
		printf("Failed to open the bus.");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}

	if (ioctl(s32_fd, SPI_IOC_WR_MODE, &u8L_mode) < 0)
	{
		perror("SPI wr_mode");
		return;
	}
	if (ioctl(s32_fd, SPI_IOC_WR_LSB_FIRST, &u8L_bitsOrder) < 0)
	{
		perror("SPI wr_lsb_fist");
		return;
	}

	if (ioctl(s32_fd, SPI_IOC_WR_BITS_PER_WORD, &u8L_bitsPerWord) < 0)
	{
		perror("SPI bits_per_word");
		return;
	}

	if (ioctl(s32_fd, SPI_IOC_WR_MAX_SPEED_HZ, &u32L_speed) < 0)
	{
		perror("SPI max_speed_hz");
		return;
	}

	printf("File: %d, spi mode: %u, bits per word: %u, order of bits: %s, speed: %uHz \n",
		s32_fd,
		u8L_mode,
		u8L_bitsPerWord,
		u8L_bitsOrder ? "LSB first" : "MSB first",
		u32L_speed);

	obj_transfer.cs_change = 0;						// Keep CS activated
	obj_transfer.delay_usecs = 0;					//delay in us
	obj_transfer.speed_hz = u32L_speed;				//speed
	obj_transfer.bits_per_word = u8L_bitsPerWord;	// bites per word 8
}

void spi_write(uint8_t* u8PL_data, uint32_t u32L_numberOfBytes)
{
	int32_t s32L_status;

	obj_transfer.tx_buf = (unsigned long)u8PL_data;
	obj_transfer.len = u32L_numberOfBytes;

	s32L_status = ioctl(s32_fd, SPI_IOC_MESSAGE(1), &obj_transfer);

	if (s32L_status < 0)
	{
		perror("SPI_IOC_MESSAGE");
		return;
	}
}

void spi_close(void)
{
	close(s32_fd);
}
