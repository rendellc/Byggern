/*
 * Byggern.c
 *
 * Created: 01.09.2017 10:00:34
 *  Author: gruppe 43
 */


#include "global_declarations.h"
#include <avr/interrupt.h>

#include <util/delay.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


#include "uart.h"

#include "adc.h"
#include "oled.h"
#include "sram_test.h"
#include "menu.h"
#include "spi_driver.h"
#include "can.h"
#include "joystick.h"
#include "game.h"

#include "mcp2515.h"


int main()
{
	cli();
	
	// SRAM init
	MCUCR |= (1 << SRE);
	SFIOR |= (1 << XMM2);
	
	uart_init();
	
	// setup heap
	__malloc_heap_start = (char*)RAM_ADR;
	__malloc_heap_end   = (char*)(RAM_ADR + RAM_SIZE);
	
	adc_init(); // adc init (must be after sram init)
	oled_init();
	menu_init(); // after oled
	
	spi_init();
	can_init(); // after spi
	
	sei();
	
	// NOTE(rendellc): required for sram test
	stdout = &uart_out; 
	stdin  = &uart_in;
	
	oled_home();
	
	game_init(); // after menu_init
	
	uint8_t i = 0;
	
	//mcp_loopback_set();
	
	can_msg_t msg = {};
	msg.length = 1;
	msg.data[0] = 'a';
	msg.sid = can_GAME_INFO;
	
	fprintf(&uart_out, "Sent message\n");
	can_print_msg(msg);
	
	fprintf(&uart_out, "entering loop\n");
	while (1)
	{
		//oled_fill(0xFF);
				
		can_send(msg,0);
		
		
		fprintf(&uart_out, "canctrl status %u\n", mcp_readstatus());

		can_msg_t recieved_msg = can_read_buffer(0);
		
		
		if (recieved_msg.sid != can_INVALID){
			fprintf(&uart_out, "Recieved message\n");
			can_print_msg(recieved_msg);
		}
	
		
		game_tick();
		
		_delay_ms(100);
	}
	
	
	return 0;
}
