/*
 * Byggern.c
 *
 * Created: 01.09.2017 10:00:34
 *  Author: gruppe 43
 */ 


#include "global_declarations.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "uart.h"
#include "adc.h"
#include "oled.h"
#include "menu.h"

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
	
	uart_init();
	
	// SRAM init
	MCUCR |= (1 << SRE);
	SFIOR |= (1 << XMM2);
	
	// setup heap
	__malloc_heap_start = RAM_ADR;
	__malloc_heap_end   = RAM_ADR + RAM_SIZE;
	
	
	
	adc_init(); // ADC init (must be after SRAM init)
	
	oled_reset();

	// setup printf
	stdout = &uart_out; // printf defaults to oled
	stdin  = &uart_in;
	
	
	menu_init();
	
	printf("Running\n");
	
	
	printf("Entering loop\n");
	joystick_t joystick = {0,0};
	touch_t touch = {0,0};
	stdout = &uart_out;
	
	while(1) {
		menu_update_subchoice();
	}

	/*
	stdout = &oled_out;
	printf("stdout: printf stdout\n");
	
	fprintf(stdout, "uart: fprintf with stdout\n");  printf("\n");
	
	fprintf(&uart_out, "uart: fprintf with &uart_out\n"); printf("\n");
	*/
	

	//SRAM_test();
	
	return 0;
}