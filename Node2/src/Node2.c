/*
 * Node2.c
 *
 * Created: 20.10.2017 13:52:58
 *  Author: rendellc
 */ 

#include "global_declarations.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include "spi_driver.h"
#include "uart.h"
#include "MCP2515.h"
#include "can.h"


int main(void)
{
	cli();
	spi_init();
	uart_init();
	fprintf(&uart_out, "can init starting...");
	can_init();
	fprintf(&uart_out, "done\n");
	sei();
	
    while(1)
    {
		can_msg_t read = can_read_buffer(0);
		
		fprintf(&uart_out, "joy_x: %i\t",   (int8_t)read.data[0]);
		fprintf(&uart_out, "joy_y: %i\t",   (int8_t)read.data[1]);
		fprintf(&uart_out, "joy_dir: %i\n", (int8_t)read.data[2]);
    }
	
	
}