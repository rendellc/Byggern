/**@file
 * Interface for spi driver. 
 */


#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_
#include <stdio.h>

//extern FILE spi_out; // not implemented

/// Set slave select pin low
void spi_ss_low();

/// Set slave select pin high
void spi_ss_high();

/**Send and recieve data. Blocks processor while transmitting. 
 * @param[in] data One byte of data to transmit
 * @returns Data recieved.
 */
uint8_t spi_transmit(uint8_t data);

/// Initialize Atmega162 as SPI master
void spi_init(void);

#endif /* SPI_DRIVER_H_ */