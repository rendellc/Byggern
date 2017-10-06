/*
 * MCP2515.c
 *
 * Created: 06.10.2017 11:52:12
 *  Author: rendellc
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

#include "MCP2515.h"
#include "spi_driver.h"
#include "uart.h"

//#define MCP_DEBUG


void cancon_init()
{
	uint8_t intrpt_status = GICR & (1<<IVSEL);
	cli();
	
	cancon_reset();
	
	// enable tx
	cancon_bitmodify(MCP_TXB0SIDL, MCP_SIDLMASK, 0);
	cancon_bitmodify(MCP_TXB0SIDH, MCP_SIDHMASK, 0);
	cancon_bitmodify(MCP_TXB0DLC, MCP_DLC_MASK , 8);
	
	// enable rx
	cancon_bitmodify(MCP_RXB0CTRL, MCP_RXM1, MCP_RXM1); 
	
	
	if (intrpt_status == 1<<IVSEL)
		sei();
	else
		cli();
}



void cancon_reset()
{
	spi_ss_low();	
	spi_transmit(MCP_RESET);
	spi_ss_high();
	
	
	#ifdef MCP_DEBUG
	fprintf(&uart_out, "MCP_RESET\n");
	#endif // MCP_DEBUG
}

uint8_t cancon_read(uint8_t adr)
{
	spi_ss_low();	
	
	spi_transmit(MCP_READ);
	spi_transmit(adr);
	uint8_t read = spi_transmit(0);
	spi_ss_high();
	
	#ifdef MCP_DEBUG
	fprintf(&uart_out, "MCP_READ %x %x\n", adr, read);
	#endif // MCP_DEBUG
	
	return read;
}

void cancon_write(uint8_t adr, uint8_t data)
{
	spi_ss_low();
	
	spi_transmit(MCP_WRITE);
	spi_transmit(adr);
	spi_transmit(data);
	
	spi_ss_high();
	
	#ifdef MCP_DEBUG
	fprintf(&uart_out, "MCP_WRITE\t%x\t%x\n", adr, data);
	#endif // MCP_DEBUG
}

void cancon_rts()
{
	spi_ss_low();
	uint8_t retval = spi_transmit(MCP_RTS_ALL); // NB: MÅ NOK ENDRES
	
	
	spi_ss_high();
	
	#ifdef MCP_DEBUG
	fprintf(&uart_out, "MCP_RTS_ALL\t%x\n", retval);
	#endif // MCP_DEBUG
}

uint8_t cancon_readstatus()
{
	spi_ss_low();
	spi_transmit(MCP_READ_STATUS);
	uint8_t read = spi_transmit(0);
	spi_ss_high();
	return read;
}

void cancon_bitmodify(uint8_t adr, uint8_t mask, uint8_t data)
{
	spi_ss_low();
	
	spi_transmit(MCP_BITMOD);
	spi_transmit(adr);
	spi_transmit(mask);
	spi_transmit(data);
	
	spi_ss_high();
	
	#ifdef MCP_DEBUG
	fprintf(&uart_out, "MCP_BITMOD\t%x\t%x\t%x\n", adr, mask, data);
	#endif // MCP_DEBUG
	
}


uint8_t cancon_loopback(uint8_t data)
{
	cancon_bitmodify(MCP_CANCTRL, MODE_MASK, MODE_LOOPBACK);
	
	cancon_rts();
	
	
	cancon_write(MCP_LOAD_TX0, data);
	
	cancon_bitmodify(MCP_TXRTSCTRL, 0x3F, 0x08); // send
	
	
	
	return cancon_read(MCP_READ_RX0);
}



void cancon_send(uint8_t data)
{
	cancon_rts();
	
	spi_ss_low();
	spi_transmit(MCP_LOAD_TX0);
	spi_transmit(data);
	spi_ss_high();
	
	
	
	
	#ifdef MCP_DEBUG
	fprintf(&uart_out, "MCP_LOAD_TX0\t%x\n", data);
	#endif // MCP_DEBUG
}