/*
 * Byggern.c
 *
 * Created: 01.09.2017 10:00:34
 *  Author: gruppe 43
 */


#include "global_declarations.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


#include "uart.h"
#include "adc.h"
#include "oled.h"
#include "sram_test.h"
#include "menu.h"
#include "sram_test.h"
#include "spi_driver.h"
#include "can.h"
#include "MCP2515.h"


void can_print_msg(can_msg_t msg)
{
	fprintf(&uart_out, "SID:%i\n", msg.sid);
	for (uint8_t i = 0; i < msg.length; ++i)
	{
		fprintf(&uart_out, "%x ", msg.data[i]);
	}
	fprintf(&uart_out, "\n");
}


int main(void) 
{
	// SRAM init
	MCUCR |= (1 << SRE);
	SFIOR |= (1 << XMM2);
	
	
	uart_init();
	
	// setup heap
	__malloc_heap_start = (char*)RAM_ADR;
	__malloc_heap_end   = (char*)(RAM_ADR + RAM_SIZE);
	
	adc_init(); // adc init (must be after SRAM init)
	oled_init();
	menu_init(); // after oled
	spi_init();
	can_init(); // after spi
	mcp_loopback_set();
	
	sei();
	
	// setup printf
	stdout = &uart_out; // printf defaults to oled
	stdin  = &uart_in;
	
	
	
	
	//char data = 'a';
	can_msg_t read;
	while(1) {
		//menu_print_current();
		//menu_update_subchoice();
		
		// enable clkout
		//mcp_bitmodify(0b01101010, CLKOUT_ENABLE,CLKOUT_ENABLE);
		
		can_msg_t send = {};
		
		send.length = 8;

		for (uint8_t i = 0; ; ++i)
		{
			send.data[0] = i;
			send.data[1] = 2*i;
			send.data[2] = 3*i;
			send.sid = i;
			

			
			can_send(send, 2);
			
			read = can_read_buffer(0);
			
			_delay_ms(500);
			
			fprintf(&uart_out, "send msg: "); can_print_msg(send);
			fprintf(&uart_out, "read msg: "); can_print_msg(read);
			
		}
		//fprintf(&uart_out, "Data write %c\t =>\t", data);
		//data = mcp_loopback(data);
		//fprintf(&uart_out, "Data read %c\n", data);
		//_delay_ms(100);
	}
	
	

	return 0;
}
