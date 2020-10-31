#ifndef SPI_SPI_H_
#define SPI_SPI_H_

#include <stdint.h>

void spi_open(const char* cPL_filename, uint32_t u32L_speed);

void spi_write(uint8_t* u8PL_data, uint32_t u32L_numberOfBytes);

void spi_close(void);

#endif /* SPI_SPI_H_ */
