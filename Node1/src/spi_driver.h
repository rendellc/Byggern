/*
 * spi_driver_h.h
 *
 * Created: 06.10.2017 09:07:20
 *  Author: rendellc
 */ 


#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_
#include <stdio.h>

//extern FILE spi_out;

void spi_ss_low();

void spi_ss_high();

uint8_t spi_transmit(uint8_t data);

void spi_init(void);

#endif /* SPI_DRIVER_H_ */