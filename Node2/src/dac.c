/*!@file
 * Implementation of external digital to analog interface using I2C.
 */
#include "global_declarations.h"
#include <avr/io.h>
#include "TWI_Master.h"
#include "dac.h"

// \todo Remove uart.h after testing
#include "uart.h"


/**
 * Initialize DAC and I2C on PortD pin 0 and 1. 
 */
void dac_init(void){
	
	DDRD |= (1 << PD0 | 1 << PD1); // set up scl, sda as output pins
	
	TWI_Master_Initialise();
	
	#ifdef UART_H_
	fprintf(&uart_out, "i2c...dac...");
	#endif
}

/**
 * Command DAC to output specified voltage. 
 */
void dac_output(uint8_t voltage){
	//fprintf(&uart_out, "success");
	uint8_t msg[3] = {};
	
	msg[0] = 0b01010000; //
	msg[1] = DAC_CMD_OUTPUT;
	
	// handle overflow / invalid input 
	//if (voltage >= 128){
	//	msg[2] = 255;
	//} 
	//else{
		msg[2] = 2*voltage; // convert range from 0-127 to 0-255 (almost 0-255)	
	//}

	#ifdef UART_H_
	//fprintf(&uart_out, "voltage out [0-255]: %u\n", msg[2]);
	#endif
	
	TWI_Start_Transceiver_With_Data(msg, 3);
}