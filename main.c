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
#include "sram_test.h"

#include <stdint.h>


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
