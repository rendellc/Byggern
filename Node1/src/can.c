/*! @file
 * High level CAN bus controller.
 */
#include "global_declarations.h"
#include <avr/interrupt.h>
#include "can.h"
#include "mcp2515.h"
#include "spi_driver.h"
#include "uart.h"

#define RX_BUFFER_MAX 8 ///\todo determine good size

static volatile can_msg_t rx_buffer[2][RX_BUFFER_MAX] = {};
static volatile uint8_t rx_head[2] = {};
static volatile uint8_t rx_tail[2] = {};

/*!
 * Interrupt vector for can message reception. \n
 * Takes the message, and places it in the corresponding recieve buffer, and clears
 * the interrupt. \n
 * If there is no room in the buffer, then the interrupt is cleared and message
 * is ignored. 
 */
ISR(INT1_vect){
	cli();	
	///\todo implement overflow such that new data replaces old?
	// pick correct buffer, RXB1 has highest priority now
	volatile uint8_t n			  = 0;
	volatile uint8_t MCP_RXBn	  = MCP_RXB0;
	volatile uint8_t MCP_READ_RXn = MCP_READ_RX0;
	volatile uint8_t MCP_RXnIF    = MCP_RX0IF;
	if (mcp_readstatus() & 0x02)
	{	
		n = 1;
		MCP_RXBn	 = MCP_RXB1;
		MCP_READ_RXn = MCP_READ_RX1;
		MCP_RXnIF     = MCP_RX1IF;
			
	}
	
	// if there is room in buffer first
	if ((rx_head[n]+1)%RX_BUFFER_MAX != rx_tail[n]){
    	volatile can_msg_t msg = {};
    	
    	msg.length = mcp_read(MCP_RXBn | MCP_RXBnDLC) & MCP_DLC_MASK;
    	
    	spi_ss_low();
    	spi_transmit(MCP_READ_RXn | 0x00); // sid
    	msg.sid = spi_transmit(0);
    	spi_ss_high();
    	
    	spi_ss_low();
    	spi_transmit(MCP_READ_RXn | 0x02);
    	
    	for (uint8_t i = 0; i < msg.length; ++i)
    	{
    		msg.data[i] = spi_transmit(0);
    	}
    	spi_ss_high();
        
    	rx_buffer[n][rx_head[n]] = msg;
    	rx_head[n] = (rx_head[n]+1) % RX_BUFFER_MAX;
	}

    mcp_bitmodify(MCP_CANINTF, MCP_RXnIF, 0);
	sei();
}

/*!
 * Initialize can controller and mcp. 
 */
void can_init()
{
	mcp_init();
	
	// enable interupt
	mcp_bitmodify(MCP_CANINTE, MCP_RX_INT, MCP_RX_INT);
	GICR |= (1 << INT1); 
	//EMCUCR |= (1 << SRW01);
}


#ifdef UART_H_
/*!
 * Print msg to uart_out stream 
 * Requires inclusion of "uart.h"
 */
void can_print_msg(can_msg_t msg)
{
	fprintf(&uart_out, "SID:%i\n", msg.sid);
	for (uint8_t i = 0; i < msg.length; ++i)
	{
		fprintf(&uart_out, "%x ", msg.data[i]);
	}
	fprintf(&uart_out, "\n");
}
#endif


/*!
 * Send message on can bus using the specified tx hardware buffer.
 */
void can_send(can_msg_t msg, uint8_t tx_buffer_select)
{
	/// \todo check for buffer full
	
	
	uint8_t MCP_TXBn = MCP_TXB0;
	uint8_t MCP_LOAD_TXn = MCP_LOAD_TX0;
	switch (tx_buffer_select)
	{
	case 0:
		MCP_TXBn = MCP_TXB0;
		MCP_LOAD_TXn = MCP_LOAD_TX0;
		break;
	case 1:
		MCP_TXBn = MCP_TXB1;
		MCP_LOAD_TXn = MCP_LOAD_TX1;
		break;
	case 2:
		MCP_TXBn = MCP_TXB2;
		MCP_LOAD_TXn = MCP_LOAD_TX2;
		break;
	}
	//fprintf(&uart_out, "MCP_TXBn=%x\tMCP_LOAD_TXn=%x\n", MCP_TXBn, MCP_LOAD_TXn);
	
	// set tx length
	mcp_write(MCP_TXBn | MCP_TXBnDLC, msg.length);
	
	// write sid
	mcp_write(MCP_TXBn | MCP_TXBnSIDL, 0x00);		// use only SIDH
	mcp_write(MCP_TXBn | MCP_TXBnSIDH , msg.sid);
	
	// write data
	spi_ss_low();
	spi_transmit(MCP_LOAD_TXn | 0x01); ///\todo add define to explain
	for (uint8_t i = 0; i < msg.length; ++i)
	{
		spi_transmit(msg.data[i]);
	}
	spi_ss_high();
	
	mcp_rts(tx_buffer_select);

}

/*!
 * Read message from specified  Rx buffer.
 * @param rx_buffer_select 0 or 1, which buffer too read from. Undefined behaviour for other values.
 * @return return the oldest message in the buffer. If buffer is empty, then returned
 * 		   message will have SID = MSG_INVALID and length=0. 
 */
can_msg_t can_read_buffer(uint8_t rx_buffer_select)
{
	const uint8_t n = rx_buffer_select; // not different but just a shorter variable name
	
	can_msg_t msg = {};
	
	if (rx_head[n] != rx_tail[n])
	{
		msg = rx_buffer[n][rx_tail[n]];
		rx_tail[n] = (rx_tail[n] + 1)%RX_BUFFER_MAX;
	}
		
	return msg;
}

/*
can_msg_t can_read_buffer_backup(uint8_t rx_buffer_select)
{
	can_msg_t msg = {};
	
	const uint8_t MCP_RXBn	   = (rx_buffer_select==0) ? MCP_RXB0 : MCP_RXB1;
	const uint8_t MCP_READ_RXn = (rx_buffer_select==0) ? MCP_READ_RX0 : MCP_READ_RX1;
	const uint8_t MCP_RXnIF    = (rx_buffer_select==0) ? MCP_RX0IF : MCP_RX1IF;
	
	msg.length = mcp_read(MCP_RXBn | MCP_RXBnDLC) & MCP_DLC_MASK;  
	
	spi_ss_low();
	spi_transmit(MCP_READ_RXn | 0x00); // sid
	msg.sid = spi_transmit(0);
	spi_ss_high();
	
	spi_ss_low();
	spi_transmit(MCP_READ_RXn | 0x02);
	
	for (uint8_t i = 0; i < msg.length; ++i)
	{
		msg.data[i] = spi_transmit(0);
	}
	spi_ss_high();
	
	mcp_bitmodify(MCP_CANINTF, MCP_RXnIF, 0);
	
	return msg;
}
*/
