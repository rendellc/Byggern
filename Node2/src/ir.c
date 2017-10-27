/*
 * ir.c
 *
 * Created: 27.10.2017 14:02:29
 *  Author: rendellc
 */ 
#include "global_declarations.h"
#include <avr/io.h>
#include "uart.h"

// make separate adc module?
void ir_init(void)
{
	
	ADCSRA |= (1 << ADPS0 | 1 << ADPS1 | 1 << ADPS2); 
	
	//DIDR0 |= (1 << ADC0D); // disable digital input on adc pin
	
	ADCSRA |= (1 << ADEN);
}

uint16_t ir_read(void)
{
	//ADMUX &= ~(1 << REFS1); // vref=1.1V internal
	ADMUX |= (1 << REFS1);
	ADCSRA |= (1 << ADSC);
	
	while (ADCSRA & (1 << ADSC));
	
	return ADC;
}