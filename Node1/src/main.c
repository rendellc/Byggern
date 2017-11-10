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
//#include "sram_test.h"
#include "menu.h"
//#include "sram_test.h"
#include "spi_driver.h"
#include "can.h"
//#include "mcp2515.h"
#include "joystick.h"


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
	
	adc_init(); // adc init (must be after SRAM init)
	oled_init();
	menu_init(); // after oled
	spi_init();
	can_init(); // after spi
	
	sei();
	
	// setup printf
	stdout = &uart_out; // printf defaults to oled
	stdin  = &uart_in;
	can_msg_t click_msg = {};
	can_msg_t noclick_msg = {};
		
	click_msg.sid = MSG_JOY;
	click_msg.data[3] = 1;
	click_msg.length = 4;
	
	noclick_msg.sid = MSG_JOY;
	noclick_msg.data[3] = 0;
	click_msg.length = 4;
	
	while (1)
	{	
		send_joy();
		
		/*
		if (!(PINE & (1 << PE0)))
		{
			can_send(click_msg,0);
			fprintf(&uart_out, "click sent\n");
			_delay_ms(10);
		}
		
		
		else
		{
			can_send(noclick_msg,0);
			fprintf(&uart_out, "no click sent\n");
		}
		*/
		
		//send_joy();
		//can_send(msg,0);
		//can_print_msg(msg);

		//fprintf(&uart_out, ".");
	}

	return 0;
}
