/*!@file
 * Implementation of external digital to analog interface using I2C.
 */

#include <avr/io.h>
#include "TWI_Master.h"
#include "dac.h"

/// \todo Remove uart.h after testing
#include "uart.h"


/**
 * Initialize DAC and I2C on PortD pin 0 and 1. 
 */
void dac_init(void){
	DDRD |= (1 << PD0 | 1 << PD1); // set up scl, sda as output pins
	
	TWI_Master_Initialise();
	
}

/**
 * Command DAC to output specified voltage. 
 */
void dac_output(uint8_t voltage){
	uint8_t msg[3] = {};
	
	msg[0] = 0b01010000;
	msg[1] = DAC_CMD_OUTPUT;
	
	// handle overflow / invalid input
	if (voltage < 0){
		msg[2] = 0;		
	} else if (voltage >= 128){
		msg[2] = 255;
	} else{
		msg[2] = 2*voltage; // convert range from 0-127 to 0-254 (almost 0-255)	
	}

	#ifdef UART_H_
	fprintf(&uart_out, "voltage out [0-255]: %i\n", msg[2]);
	#endif

	TWI_Start_Transceiver_With_Data(msg, 3);
}