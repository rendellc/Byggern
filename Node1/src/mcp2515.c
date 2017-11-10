/*
 * MCP2515.c
 *
 * Created: 06.10.2017 11:52:12
 *  Author: rendellc
 */ 
#include "global_declarations.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "mcp2515.h"
#include "spi_driver.h"
#include "uart.h"

/*!
 * Initialize MCP2515 and set to normal operating mode. 
 */
void mcp_init()
{
	mcp_reset();
	
/*	--- Set physical layer config ---
	Fosc		= 16 MHz
	Bus Speed	= 1/(Total # of TQ)*TQ
*/
	const uint8_t BRP		= 1;	// TQ = 2*Tosc*(BRP+1)  = 250 ns
	const uint8_t PrSeg		= 1;	// tPropSeg = (PrSeg + 1)
	const uint8_t PhSeg1	= 2;	// tPS1 = (PhSeg1 + 1)*TQ
	const uint8_t PhSeg2	= 2;	// tPS2 = (PhSeg2 + 1)*TQ
	
	mcp_write(MCP_CNF1, BRP);
	mcp_write(MCP_CNF2, PrSeg | (PhSeg1 << 3));
	mcp_write(MCP_CNF3, PhSeg2);
	
	
	// Setup Tx registers
	//mcp_write(MCP_TXB0SIDL, 0);	  // use only SID 
	//mcp_write(MCP_TXB0DLC, 0x01); // transmit 1 byte, transmit data frame


	// Setup Rx registers
	//mcp_write(MCP_RXM0SIDH, 0);
	//mcp_write(MCP_RXM0SIDL, 0);
	mcp_bitmodify(MCP_RXB0CTRL, 0x60, 0x60);
	mcp_bitmodify(MCP_RXB1CTRL, 0x60, 0x60);	
	
	// go to normal mode
	mcp_bitmodify(MCP_CANCTRL, MODE_MASK, MODE_NORMAL);
	
}


/*!
 * Reset MCP2515.
 * Sends reset signal and waits for atleast 128 clock cycles to give MCP2515 time to reset. 
 */
void mcp_reset()
{
	spi_ss_low();
	spi_transmit(MCP_RESET);
	spi_ss_high();
	
	for (uint8_t _ = 0; _ < 128; ++_);	// Wait for 128 cycles
	
	#ifdef MCP_DEBUG
	fprintf(&uart_out, "MCP_RESET\n");
	#endif // MCP_DEBUG
}

/*!
 * Reads data from the register at the specified address
 * @param adr Address to read from
 * @return Data in the specified register
 */
uint8_t mcp_read(uint8_t adr)
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

/*!
 * Read data in buffer
 * @param rx_select Which buffer to read from. Values can be 0 or 1. \todo not implemented
 * @param data_select Read  data register if 1. Read  SIDH if 0. \todo not implemented
 * @return Data read from buffer
 */
uint8_t mcp_read_buffer(uint8_t rx_select, uint8_t data_select)
{
	spi_ss_low();
	spi_transmit(MCP_READ_RX0 | 0x02);
	uint8_t data = spi_transmit(0);
	spi_ss_high();
	
	mcp_bitmodify(MCP_CANINTF, MCP_RX0IF, 0);
	
	
	#ifdef MCP_DEBUG
	fprintf(&uart_out, "MCP_READ_RX0 %x\n", data);
	#endif // MCP_DEBUG
	
	return data;
}

/*!
 * Write data to specified buffer.
 * @param adr Register address to write data to
 * @param data Data to write
 */
void mcp_write(uint8_t adr, uint8_t data)
{
	spi_ss_low();
	spi_transmit(MCP_WRITE);
	spi_transmit(adr);
	spi_transmit(data);
	spi_ss_high();
	
	
	#ifdef MCP_DEBUG
	fprintf(&uart_out, "MCP_WRITE\t%x\t%i\n", adr, data);
	#endif // MCP_DEBUG
}

/*!
 * Request to send
 * @param tx_buffer_select The buffer to request transmission on
 */
void mcp_rts(uint8_t tx_buffer_select)
{
	spi_ss_low();
	spi_transmit(MCP_RTS | (1<<tx_buffer_select));
	spi_ss_high();
	
	#ifdef MCP_DEBUG
	fprintf(&uart_out, "MCP_RTS\t%i\n", tx_buffer);
	#endif // MCP_DEBUG
}

/*!
 * Read status.
 * @return 8 flags that indicate the current status of the controller. 
 */
uint8_t mcp_readstatus()
{
	spi_ss_low();
	spi_transmit(MCP_READ_STATUS);
	uint8_t status = spi_transmit(0);
	spi_ss_high();
	
	
	#ifdef MCP_DEBUG
	fprintf(&uart_out, "MCP_READ_STATUS\t%x\n", status);
	#endif // MCP_DEBUG
	
	return status;
}

/*!
 * Modify single bit in register. Note that this doesn't work on all registers.
 * @param adr Address to modify
 * @param mask Mask: 1 on all the bits that should be affacted.
 * @param data Value that masked bits will be updated to.
 */
void mcp_bitmodify(uint8_t adr, uint8_t mask, uint8_t data)
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

/*!
 * Set to controller into loopback mode. Used for debugging.
 */
void mcp_loopback_set()
{
	mcp_write(MCP_CANCTRL, MODE_CONFIG);
		
	/*	--- Initialize in loopback mode */
	#ifdef MCP_LOOPBACK_INIT
	mcp_write(MCP_CANCTRL, MODE_LOOPBACK);
	// verify
	if (!((mcp_read(MCP_CANSTAT) & MODE_MASK)  == MODE_LOOPBACK))
		fprintf(&uart_out, "Loopback mode set failed\n\tMCP_CANSTAT: 0x%x\n", mcp_read(MCP_CANSTAT));
	#endif // MCP_LOOPBACK_INIT
	
}
