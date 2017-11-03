/*
 * motor.c
 *
 * Created: 03.11.2017 15:29:06
 *  Author: rendellc
 */
#include "global_declarations.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "dac.h"
#include "motor.h"

#include "uart.h" /*!< \todo remove */


#define PIN_DIR	PH1
#define PIN_SEL PH3
#define PIN_EN	PH4
#define PIN_OE	PH5
#define PIN_RST PH6

/// position reference 0-100 from left-to-right
uint8_t vel_reference = 50;
uint8_t PIreg_force = 0;

// 


// calibration

uint8_t Ki;
uint8_t Kp;

volatile uint16_t oldvalue = 0;
volatile uint16_t currentvalue = 0;



ISR(TIMER0_OVF_vect){
	cli();
	// sample encoder
	fprintf(&uart_out, "encoder: %i\n", motor_read_encoder()/32);
	
	
	
	// PI-regulator
	
	
	// set motor speed
	
	
	TIFR0  |= (1 << TOV0); // clear overflow flag
	sei();
}

int8_t PI_regulator(){
	
	
	
}

int16_t motor_read_encoder(){
	
	// set !OE low
	PORTH &= ~(1 << PIN_OE);
	
	// set SEL low
	PORTH &= ~(1 << PIN_SEL);
	
	// wait 20 us
	_delay_us(20);
	
	// read msb
	volatile int16_t read = (PINK << 8);
	
	// set SEL high
	PORTH |= (1 << PIN_SEL);
	
	// wait 20 us
	_delay_us(20);
	
	// read lsb
	read |= PINK;
	
	// toggle !RST
	PORTH &= (1 << PIN_RST);
	_delay_us(1);
	PORTH |= (1 << PIN_RST);
	
	
	// set !OE high
	PORTH |= (1 << PIN_OE);
	
	// store data
	return read;
}

void PI_regulator_tune(float Kp, float Ki){
	
	
}

void PI_regulator_set_setpoint(uint8_t new_vel_reference){
	if (new_vel_reference <= 100)
		vel_reference = new_vel_reference;
}

void motor_init(void){
	dac_init();
	
	DDRH |= ( 1 << PIN_DIR | 1 << PIN_SEL | 1 << PIN_EN | 1 << PIN_OE | 1 << PIN_RST );
	
	// set sampling rate for encoder 
	TCCR0B |= (1 << CS02 | 1 << CS00); // prescaler = 256 = crash? \test more
	TIFR0  |= (1 << TOV0); // clear overflow flag
	TIMSK0 |= (1<<TOIE0);  // enable
	
	motor_enable();
}

void motor_enable(void){
	PORTH |= (1 << PIN_EN);
}

void motor_set_speed(int8_t speed){
	
	if (speed < 0){
		PORTH &= ~(1 << PIN_DIR);	
	} else {
		PORTH |= (1 << PIN_DIR);
	}

	
	dac_output(abs(speed));
}