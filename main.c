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
	__malloc_heap_start = (char*)RAM_ADR;
	__malloc_heap_end   = (char*)(RAM_ADR + RAM_SIZE);
	
	adc_init(); // adc init (must be after SRAM init)
	oled_init();
	menu_init(); // after oled
	spi_init();
	mcp_init(); // after spi
	
	// setup printf
	stdout = &uart_out; // printf defaults to oled
	stdin  = &uart_in;
	
	
	
	//char data = 'a';
	
	while(1) {
		menu_update_subchoice();
		
		// enable clkout
		//mcp_bitmodify(0b01101010, CLKOUT_ENABLE,CLKOUT_ENABLE);
		
		
		for (uint8_t i = 0; ; ++i)
		{
			spi_ss_low();
			spi_transmit(i);
			spi_ss_high();
			_delay_ms(250);
		}
		
		//fprintf(&uart_out, "Data write %c\t =>\t", data);
		//data = mcp_loopback(data);
		//fprintf(&uart_out, "Data read %c\n", data);
		//_delay_ms(100);
	}
	
	

	return 0;
}
