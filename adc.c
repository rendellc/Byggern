/*
 * adc.c
 *
 * Created: 15.09.2017 11:11:38
 *  Author: rendellc
 */ 
#include "adc.h"
#include "global_declarations.h"

#define ADC_CH1 0b0100
#define ADC_CH2 0b0101
#define ADC_CH3 0b0110
#define ADC_CH4 0b0111

#define ADC_JOY_X ADC_CH2
#define ADC_JOY_Y ADC_CH1
#define ADC_SLIDER ADC_CH3
#define ADC_BUTTON ADC_CH4

#define ANALOG_MID 128
#define ANALOG_THRESH 18

volatile char* adc_adr = ADC_ADR;

volatile unsigned char x_read		= 0;
volatile unsigned char y_read		= 0;
volatile unsigned char slider_read	= 0;
volatile unsigned char btn_read		= 0;

volatile unsigned char read_state = ADC_JOY_X; 



ISR(INT0_vect){
	switch(read_state){
		case ADC_JOY_X:
			x_read = *adc_adr;
			*adc_adr = ADC_JOY_Y;
			read_state = ADC_JOY_Y;
			break;
		case ADC_JOY_Y:
			y_read = *adc_adr;
			*adc_adr = ADC_SLIDER;
			read_state = ADC_SLIDER;
			break;
		case ADC_SLIDER:
			slider_read = *adc_adr;
			//printf("Slider: %i\n", slider_read);
			*adc_adr = ADC_BUTTON;
			read_state = ADC_BUTTON;
			break;
		case ADC_BUTTON:
			btn_read = *adc_adr;
			*adc_adr = ADC_JOY_X;
			read_state = ADC_JOY_X;
			break;
		default:
			printf("Error in INT0_vect\n");
	}
}


void adc_init(void){
	
	GICR = (1 << INT0); // enable interupt
	EMCUCR |= (1 << SRW00);
	
	*adc_adr = ADC_JOY_X;
}

inline int convert_range(unsigned char data){ // convert from 0->255 to -100->100
	return (float)data*0.784314 - 100;
}

joystick_t adc_analogread_joy(){
	joystick_t a = {convert_range(x_read), convert_range(y_read)};
	return a;
}

direction_t adc_direction_joy(){
	if (abs(x_read-128) < ANALOG_THRESH && abs(y_read-128) < ANALOG_THRESH){
		return NEUTRAL;
	}
	
	if (x_read - 128 > abs(y_read - 128))
		return RIGHT;
	else if (x_read - 128 < - abs(y_read - 128))
		return LEFT;
	else if (y_read - 128 > abs(x_read - 128))
		return UP;
	else
		return DOWN;
	
}

touch_t adc_read_touch(){
	touch_t a = {convert_range(slider_read), btn_read/128};
	return a;
}



