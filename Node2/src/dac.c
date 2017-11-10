/*
 * dac.c
 *
 * Created: 03.11.2017 14:27:41
 *  Author: rendellc
 */ 

#include <avr/io.h>
#include "TWI_Master.h"
#include "dac.h"

#include "uart.h"



void dac_init(void){
	DDRD |= (1 << PD0 | 1 << PD1); // set up scl, sda as output pins
	
	TWI_Master_Initialise();
	
}


void dac_output(uint8_t voltagePercent){
	uint8_t msg[3] = {};
	
	msg[0] = 0b01010000;
	msg[1] = DAC_CMD_OUTPUT;
	msg[2] = (voltagePercent*5)/2; // convert range from 0-100 to 0-250 (almost 0-255)
	
	//fprintf(&uart_out, "speed: %i\n", msg[2]);
	
	TWI_Start_Transceiver_With_Data(msg, 3);
}