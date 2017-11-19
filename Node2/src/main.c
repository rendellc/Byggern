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
#include <avr/sleep.h>

#include "spi_driver.h"
#include "uart.h"
#include "mcp2515.h"
#include "can.h"
#include "pwm.h"
#include "ir.h"
#include "dac.h"
#include "motor.h"
#include "game_slave.h"
#include "solenoid.h"
#include "TWI_Master.h"

int main(void)
{
	cli();
	uart_init();
	fprintf(&uart_out, "\nuart initialized\n");
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
	
	TWI_Master_Initialise();
	
	dac_init();
	
	
	fprintf(&uart_out, "motor init starting...");
	motor_init();
	fprintf(&uart_out, "done\n");
	fprintf(&uart_out, "solenoid init starting...");
	solenoid_init();
	fprintf(&uart_out, "done\n");
	fprintf(&uart_out, "game init starting...");
	game_init();
	fprintf(&uart_out, "done\n");
	
	
	
	
	sei();
	motor_enable();
	motor_encoder_calibrate(); // after enabling interrupts
	
	motor_set_position(128);
	
	fprintf(&uart_out, "system init complete\n");
	
	
	
    while(1){
	//	uint16_t ir = ir_read();
		
	//	fprintf(&uart_out, "%u\n", ir);	
	}
	
	
	return 0;
}
