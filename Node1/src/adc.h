/*
 * adc.h
 *
 * Created: 15.09.2017 11:34:32
 *  Author: rendellc
 */ 


#ifndef ADC_H_
#define ADC_H_

#include "global_declarations.h"

#define ADC_ADR 0x1100

void adc_init(void);

uint8_t adc_read_channel(uint8_t);

//touch_t adc_read_touch();

#endif /* ADC_H_ */