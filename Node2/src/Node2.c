/*
 * Node2.c
 *
 * Created: 20.10.2017 13:52:58
 *  Author: rendellc
 */ 

#include "global_declarations.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "spi_driver.h"
#include "uart.h"
#include "MCP2515.h"
#include "can.h"
#include "pwm.h"
#include "ir.h"

uint16_t negativescore = 0;
uint16_t adc_read;

int main(void)
{
	cli();
	spi_init();
	uart_init();
	fprintf(&uart_out, "can init starting...");
	can_init();
	fprintf(&uart_out, "done\n");
	fprintf(&uart_out, "pwm init starting...");
	pwm_init();
	fprintf(&uart_out, "...done\n");
	fprintf(&uart_out, "ir init starting...");
	ir_init();
	fprintf(&uart_out, "done\n");
	sei();
	
	
	int8_t joy_x = 0;
    while(1)
    {
		can_msg_t read = can_read_buffer(0);
		
		switch (read.sid)
		{
			case MSG_JOY:
				joy_x = read.data[0];
				break;
				
			case MSG_INVALID:
				fprintf(&uart_out, "Invalid\n");
		}
		

		//uint16_t adc_read = ir_read(); // changed to global variable instead
		adc_read = ir_read();
		//scorekeeping();
		fprintf(&uart_out, "adc value: %i\n", adc_read);
		fprintf(&uart_out, "pwm duty: %i\n",  32 + joy_x/2);
		pwm_set_duty(32 + joy_x/2);
		
    }
	
	
}

/*
void scorekeeping(){
	if (adc_read < 30)
	{
		negativescore++;
		fprintf(&uart_out, "LOSER: %i\n", negativescore);
		//adc_read = ir_read();
		while (adc_read < 30)
		{
			adc_read = ir_read();
		}
		
	}
	
}
*/