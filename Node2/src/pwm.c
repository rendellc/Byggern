/*
 * pwm.c
 *
 * Created: 27.10.2017 09:37:52
 *  Author: rendellc
 */ 
#include "global_declarations.h"
#include "uart.h"
#include "pwm.h"
#include <avr/interrupt.h>

#define PWM_TOP 40000
#define PWM_MAX 3900
#define PWM_MIN 2000
#define PWM_MID ((PWM_MAX+PWM_MIN)/2)
#define PWM_RES ((PWM_MAX-PWM_MIN)/100)

/// Set up 16-bit timers for pwm
/// 50 Hertz pwm period
void pwm_init(void)
{
	// PB5
	DDRB |= (1 << DDB5);
	
	// Select clock source
	ICR1 = PWM_TOP;
	//ICR1L = 40000	& 0xFF; 
	//ICR1H = (40000<<8) & 0xFF; // TOP = 310
	TCCR1B |= (1 << CS11) & ~(1 << CS10 | 1 << CS12);     //((1 << CS12) | (1 << CS10)) & ~(1 << CS11); // prescaler = 8
	TCCR1B |= (1 << WGM12 | 1 << WGM13);
	TCCR1A |= (~(1 << COM1A0) & (1 << COM1A1)) | ((1 << WGM11) & ~(1 << WGM10));	// non-inverting fast pwm
	
	OCR1A = PWM_MID;
}


void pwm_set_duty(int8_t duty){
	if (duty > 100){
		duty = 100;
	} else if (duty < 0){
		duty = 0;
	}
	
	//fprintf(&uart_out, "duty: %i\n", duty);
	
	OCR1A = PWM_MIN + PWM_RES*duty;
	
}

