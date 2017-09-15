/*
 * global_declarations.h
 *
 * Created: 01.09.2017 12:53:09
 *  Author: gr 43
 */ 


#ifndef GLOBAL_DECLARATIONS_H_
#define GLOBAL_DECLARATIONS_H_

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>

#define F_CPU 4915200UL

#define RAM_ADR 0x1400
#define RAM_SIZE 0xC00

#define ADC_ADR 0x1100
#define ADC_CH1 0b0100
#define ADC_CH2 0b0101
#define ADC_CH3 0b0110
#define ADC_CH4 0b0111

#define ADC_JOY_X ADC_CH2
#define ADC_JOY_Y ADC_CH1
#define ADC_SLIDER ADC_CH3
#define ADC_BUTTON ADC_CH4

typedef struct {
	int x;
	int y;
} joystick_t;

typedef struct {
	int slider;
	int button;
} touch_t;

typedef enum {	LEFT = 0, 
				RIGHT = 1, 
				UP = 2, 
				DOWN = 3, 
				NEUTRAL = 4
} direction_t;

#endif /* GLOBAL_DECLARATIONS_H_ */