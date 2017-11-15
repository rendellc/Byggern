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
#include "mcp2515.h"
#include "can.h"
#include "pwm.h"
#include "ir.h"
#include "dac.h"
#include "motor.h"
#include "game.h"
#include "solenoid.h"

int main(void)
{
	cli();
	uart_init();
	fprintf(&uart_out, "uart initialized\n");
	fprintf(&uart_out, "can init starting...");
	spi_init();
	can_init();
	fprintf(&uart_out, "done\n");
	fprintf(&uart_out, "pwm init starting...");
	pwm_init();
	fprintf(&uart_out, "...done\n");
	fprintf(&uart_out, "ir init starting...");
	ir_init();
	fprintf(&uart_out, "done\n");
	fprintf(&uart_out, "motor init starting...");
	motor_init();
	fprintf(&uart_out, "done\n");
	fprintf(&uart_out, "solenoid init starting...");
	solenoid_init();
	fprintf(&uart_out, "done\n");

	sei();
	
	
	int8_t joy_x = 0;
	int8_t joy_y = 0;
	uint8_t joy_click = 0;
	uint8_t slider_pos = 0;
	
	
	
	
	
	motor_enable();
	motor_encoder_calibrate();
	
	
	
	
    while(1)
    {
		motor_enable();
		
		can_msg_t read = can_read_buffer(0);
		
		switch (read.sid)
		{
			
			case can_JOY:
				joy_x = read.data[0];
				joy_y = read.data[1];
				joy_click = read.data[3];
				/*
				if (joy_click){
					fprintf(&uart_out, "click\n");
					PORTE &= ~(1 << PE4);
				}
				else{
					fprintf(&uart_out, "no click\n");
					PORTE |= (1 << PE4);
				}
				*/
				break;
				
			case can_SLIDER:
				slider_pos = read.data[0];
				motor_set_position(slider_pos);
				//fprintf(&uart_out, "slider: %i\n", (slider_pos/5)*2);
				//motor_set_position(50);
				
				break;
				
			case can_INVALID:
				//fprintf(&uart_out, "Invalid\n");
				break;
		}
		
		//int16_t enc_read = motor_read_encoder();
		//fprintf(&uart_out, "encoder read %i\n", enc_read);
		//fprintf(&uart_out, "joy_y read %i\n", joy_y);
		//fprintf(&uart_out, "click: %i\n", joy_click);
		//motor_set_speed(joy_y/2);
		//uint16_t adc_read = ir_read(); // changed to global variable instead
		uint8_t adc_read = ir_read();
		
		//scorekeeping();
		
		fprintf(&uart_out, "adc value: %i\t\n", adc_read);
		//fprintf(&uart_out, "lives: %u\n", scorekeeping());
		
		//fprintf(&uart_out, "pwm duty: %i\n",  32 + joy_x/2);
		
		pwm_set_duty(34 + joy_x/2);
    }
}