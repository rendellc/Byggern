/*
 * solenoid.c
 *
 * Created: 18.11.2017 07:28:05
 *  Author: rendellc
 */ 

#include "global_declarations.h"
#include "solenoid.h"
#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(TIMER5_COMPA_vect){
	PORTE |= (1 << PE4);
	
	TIMSK5 &= ~(1 << OCIE5A);
}


void solenoid_init(){
	// set up interrupt
	TCCR5A |= (1 << COM5A1); //| (1 << FOC3A); // CTC, Top=OCR3A
		
	// prescaler=1024 and OCR3A=128 gives frequency of 60 Hz
	TCCR5B |= (1 << WGM52) | (1 << CS52) | (1 << CS50);
	OCR5A = 5000; //128; // 30 Hz
	
	
	
	DDRE |= (1 << PE4);
	PORTE |= (1 << PE4);
}


void solenoid_trigger(){
	PORTE &= ~(1 << PE4);
	
	TIMSK5 |= (1 << OCIE5A);
}


