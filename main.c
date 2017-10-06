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
#include <stdint.h>


#include "uart.h"
#include "adc.h"
#include "oled.h"
#include "sram_test.h"
#include "menu.h"
#include "sram_test.h"
#include "spi_driver.h"
#include "MCP2515.h"

int main(void) 
{
	// SRAM init
	MCUCR |= (1 << SRE);
	SFIOR |= (1 << XMM2);
	
	
	uart_init();
	
	// setup heap
	__malloc_heap_start = RAM_ADR;
	__malloc_heap_end   = RAM_ADR + RAM_SIZE;
	
	adc_init(); // adc init (must be after SRAM init)
	oled_init();
	menu_init(); // after oled
	spi_init();
	cancon_init(); // after spi
	
	// setup printf
	stdout = &uart_out; // printf defaults to oled
	stdin  = &uart_in;
	
	// enable clkout
	spi_ss_low();
	cancon_bitmodify(MCP_CANCTRL, CLKOUT_ENABLE,CLKOUT_ENABLE);
	spi_ss_high();
	
	uint8_t data;
	
	while(1) {
		menu_update_subchoice();
		
		//data = cancon_loopback(0xC5);
			
		fprintf(&uart_out, "Data read %x\n", data);
		_delay_ms(1000);
	}
	
	

	return 0;
}
