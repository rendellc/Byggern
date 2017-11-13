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
//#include "sram_test.h"
#include "spi_driver.h"
#include "can.h"
//#include "mcp2515.h"
#include "joystick.h"
#include "game.h"

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
	
	adc_init(); // adc init (must be after SRAM init)
	oled_init();
	menu_init(); // after oled
	spi_init();
	can_init(); // after spi
	
	sei();
	
	// setup printf, \note remove?
	stdout = &uart_out; 
	stdin  = &uart_in;
	
	oled_home();
	
	game_init(); // after menu_init
	
	SRAM_test();

	fprintf(&uart_out, "entering loop\n");
	
	uint8_t i = 0;
	
	while (1)
	{	
		if (++i == 0)
			fprintf(&uart_out, "tick x256\n");
		
		game_tick();

		_delay_ms(10);
	}
	
	
	return 0;
}
