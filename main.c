/*
 * Byggern.c
 *
 * Created: 01.09.2017 10:00:34
 *  Author: gruppe 43
 */ 


#include "global_declarations.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "uart.h"

int main(void) {
	cli();
	uart_init();
	
	sei();
	
	
	
    while(1) {
		//char in = uart_recv();
		//if(in != 0)
			//uart_send(in);
		printf("heyhey! 12345");
		_delay_ms(1000);
    }
	return(0);
}


