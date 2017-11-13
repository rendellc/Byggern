/**@file
 * Interface for SPI driver for atmega162
 * with atmega162 as spi master.
 */ 


#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_
#include <stdio.h>

//extern FILE spi_out;

/// Set slave select line low
void spi_ss_low();

/// Set slave select line high
void spi_ss_high();

/**
 * Send and recieve one byte of data. Caller must first
 * call spi_ss_low() and spi_ss_high() after transmission.
 * This is done so that multiple bytes can be transferred
 * in the same transmission. 
 * @param[in] data The byte to transmit.
 * @return Byte read in from slave.
 */
uint8_t spi_transmit(uint8_t data);

void spi_init(void);

#endif /* SPI_DRIVER_H_ */