/*!@file
 * Implement ADC usage on atmega2560. \n
 */ 

#include "global_declarations.h"
#include <avr/io.h>

/// \todo make separate adc module?

/// Initialising of ADC for reading ir diode
void ir_init(void)
{
	
	ADCSRA |= (1 << ADPS0 | 1 << ADPS1 | 1 << ADPS2); 
	
	//DIDR0 |= (1 << ADC0D); // disable digital input on adc pin
	
	ADCSRA |= (1 << ADEN);
}


///  Read the ADC for ir value
uint16_t ir_read(void)
{
	//ADMUX &= ~(1 << REFS1); // vref=1.1V internal
	ADMUX |= (1 << REFS1);
	ADCSRA |= (1 << ADSC);
	
	while (ADCSRA & (1 << ADSC));
	
	return ADC;
}