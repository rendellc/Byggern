/*
 * adc.c
 *
 * Created: 15.09.2017 11:11:38
 *  Author: rendellc
 */ 
#include "global_declarations.h"
#include "adc.h"
#include "uart.h"
#include <stdlib.h>

volatile uint8_t* adc_adr = (uint8_t*)ADC_ADR;

void adc_init(void){
	DDRD  &= ~(1 << DDD2);
	PORTD |= (1 << PD2);
}

uint8_t adc_read_channel(uint8_t ch)
{	
	*adc_adr = (ch | 0b0100);
	
	while ((PIND & (1<<PIND2))); // wait until pin is high, adc read complete when pin is high
	
	return *adc_adr;
}

