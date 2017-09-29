/*
 * global_declarations.h
 *
 * Created: 01.09.2017 12:53:09
 *  Author: gr 43
 */ 


#ifndef GLOBAL_DECLARATIONS_H_
#define GLOBAL_DECLARATIONS_H_

#define F_CPU 4915200UL

const void* NULL_PTR;

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define OLED_BUFFER_ADR 0x1400
#define RAM_ADR 0x1800
#define RAM_SIZE 0x800


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