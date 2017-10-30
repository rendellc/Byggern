/*!@file
 * Implementation of ADC converter interface
 */

#include "global_declarations.h"
#include "adc.h"
//#include <stdlib.h> ///\todo why was this inlcuded?
#include <stdint.h>


volatile uint8_t * const adc_adr = (uint8_t*)ADC_ADR; /*!< address of ADC on bus as const pointer*/

/*!
 * Initialize atmega162 to interface with the interrupt signal
 * of the ADC through Port D pin 2
 */
void adc_init(void){
	DDRD  &= ~(1 << DDD2);
	PORTD |= (1 << PD2);
}


/*!
 * Perform analog conversion. 
 * @param[in] ch analog channel to perform conversion on
 * @return digital read of analog signal
 */
uint8_t adc_read_channel(uint8_t ch)
{	
	*adc_adr = (ch | 0b0100);
	
	while ((PIND & (1<<PIND2))); // wait until pin is high, adc read complete when pin is high
	
	return *adc_adr;
}

