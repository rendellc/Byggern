/*
 * spi_driver.c
 *
 * Created: 06.10.2017 10:23:30
 *  Author: rendellc
 */ 
#include "global_declarations.h"
#include "spi_driver.h"
#include "uart.h"
#include "avr/io.h"
#include <stdio.h>

//#define SPI_DEBUG

/*
ISR(SPI_STC_vect){
}
*/

void spi_ss_low(){
	PORTB &= ~(1 << PB7);
	
}

void spi_ss_high(){
	PORTB |= (1 << PB7);
}

/// Initialize Atmega162 as SPI master
void spi_init(void)
{
	// setup IO pins
	DDRB |= (1<<DDB1 | 1<<DDB2 | 1<<DDB7 | 1<<DDB0);
	DDRB &= ~(1<<DDB3); // not necessary
	
	// setup SPI
	SPCR = (1<<SPE | 1<<MSTR | 1<<SPR1 | 1<<SPR0 | 1<<CPOL | 1<<CPHA ) & ~(/*1<<CPOL | 1<<CPHA |*/ 1<<DORD);
	// spi mode 0 in 162s datasheet
	// MSB transmitted first and LSB last
	
	
	#ifdef SPI_DEBUG
	fprintf(&uart_out, "SPCR: %x\n", SPCR);
	#endif
	
}


uint8_t spi_transmit(uint8_t data)
{
	SPDR = data;
	while (!(SPSR & (1<<SPIF)));
	
	return SPDR;
}