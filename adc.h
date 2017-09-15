/*
 * adc.h
 *
 * Created: 15.09.2017 11:34:32
 *  Author: rendellc
 */ 


#ifndef ADC_H_
#define ADC_H_

#include "global_declarations.h"

void adc_init(void);

joystick_t adc_analogread_joy();

direction_t adc_direction_joy();

touch_t adc_read_touch();

#endif /* ADC_H_ */