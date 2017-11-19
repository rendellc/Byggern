/*
 * Byggern.c
 *
 * Created: 01.09.2017 10:00:34
 *  Author: gruppe 43
 */


#include "global_declarations.h"
#include <avr/interrupt.h>

#include <util/delay.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


#include "uart.h"

#include "adc.h"
#include "oled.h"
//#include "sram_test.h"
#include "menu.h"
#include "spi_driver.h"
#include "can.h"
#include "input.h"
#include "game.h"

#include "mcp2515.h"


int main()
{
	cli();
	
	// SRAM init
	MCUCR |= (1 << SRE);
	SFIOR |= (1 << XMM2);
	
	uart_init();
	// setup heap
	__malloc_heap_start = (char*)RAM_ADR;
	__malloc_heap_end   = (char*)(RAM_ADR + RAM_SIZE);
	
	adc_init(); // adc init (must be after sram init)
	oled_init();
	oled_home();
	menu_init(); // after oled
	spi_init();
	can_init(); // after spi
	game_init(); // after menu_init
	
	fprintf(&uart_out, "system init complete\n");
	sei();
	
	// NOTE(rendellc): required for sram test
	stdout = &uart_out; 
	stdin  = &uart_in;
	
	//sram_test();
	
	while (1);
	
	return 0;
}
