/*
 * Byggern.c
 *
 * Created: 01.09.2017 10:00:34
 *  Author: gruppe 43
 */ 


#include "global_declarations.h"


#include "uart.h"
#include "adc.h"

#include <stdint.h>

void SRAM_test(void){	
	volatile char *ext_ram = (char *) RAM_ADR; // Start address for the SRAM
	uint16_t ext_ram_size = RAM_SIZE;
	uint16_t write_errors = 0;
	uint16_t retrieval_errors = 0;
	printf("Starting SRAM test...\n");
	
	// rand() stores some internal state, so calling this function in a loop will
	// yield different seeds each time (unless srand() is called before this function)
	uint16_t seed = rand();
	// Write phase: Immediately check that the correct value was stored
	srand(seed);
	
	for (uint16_t i = 0; i < ext_ram_size; i++) {
		uint8_t some_value = rand();
		ext_ram[i] = some_value;
		uint8_t retreived_value = ext_ram[i];
		if (retreived_value != some_value) {
			printf("Write phase error: ext_ram[%4d] = %02X (should be %02X)\n", i,retreived_value, some_value);
			write_errors++;
		}
	}
	
	// Retrieval phase: Check that no values were changed during or after the write phase
	srand(seed);
	// reset the PRNG to the state it had before the write phase
	for (uint16_t i = 0; i < ext_ram_size; i++) {
		uint8_t some_value = rand();
		uint8_t retreived_value = ext_ram[i];
		if (retreived_value != some_value) {
			printf("Retrieval phase error: ext_ram[%4d] = %02X (should be %02X)\n",i, retreived_value, some_value);
			retrieval_errors++;
		}
	}
	printf("SRAM test completed with\n%4d errors in write phase and\n%4d errors in retrieval phase\n\n", write_errors, retrieval_errors);
}


int main(void) {
	cli();
	uart_init();
	
	// SRAM init
	MCUCR |= (1 << SRE);
	SFIOR |= (1 << XMM2);
	
	// ADC init (must be after SRAM init)
	adc_init();
	
	sei();
	
	printf("Entering loop\n");
	joystick_t joystick = {0,0};
	touch_t touch = {0,0};
	while(1){
		//joystick = adc_read_joy();
		//touch = adc_read_touch();
		//printf("Joystick x: %i\tJoystick y: %i\n", joystick.x, joystick.y);
		//printf("Slider: %i\tButton: %i\n", touch.slider, touch.button);
		printf("%i\n", adc_direction_joy());
		
	}
	
	//SRAM_test();
	
	return 0;
}