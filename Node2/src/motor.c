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
#include "pi.h"

#include "uart.h" /*!< \todo remove */


#define PIN_DIR	PH1
#define PIN_SEL PH3
#define PIN_EN	PH4
#define PIN_OE	PH5
#define PIN_RST PH6

/// velocity reference
static int8_t vel_setpoint = 0;

// calibration
static uint16_t calibrate_min = 0;
static uint16_t calibrate_max = 0;

// regulator
pi_t regulator = {};


static volatile uint8_t position[2] = {};
static volatile int8_t velocity[2] = {};


ISR(TIMER0_OVF_vect){
	cli();
	// sample encoder
	//fprintf(&uart_out, "encoder: %i\n", motor_read_encoder()/32);
	uint8_t  read_position = motor_read_encoder();
	
	velocity[1] = position[0]   - position[1];
	velocity[0] = read_position - position[0];
	position[1] = position[0];
	position[0] = read_position;


	// PI-regulator
	PI_regulator()
	
	// set motor speed
	
	
	TIFR0  |= (1 << TOV0); // clear overflow flag
	sei();
}


int8_t PI_regulator(int8_t setpoint){
	error = (velocity[0] - setpoint);
	errorSum += error;

	return Kp*error + Ki*errorSum;
}

/**
 * Manually calibrate the encoder.
 * 
 */
void motor_encoder_calibrate(){
	uint16_t encoder_raw = motor_read_encoder();
	uint16_t encoder_minvalue = encoder_raw;
	uint16_t encoder_maxvalue = encoder_raw;
	
	
	/// \todo this is heavily dependent on processor performance. Isolate more
	uint16_t i = -1;
	while (i--){
		encoder_raw = motor_read_encoder();

		encoder_minvalue = (encoder_raw < encoder_minvalue) ? encoder_raw : encoder_minvalue
		encoder_maxvalue = (encoder_raw > encoder_maxvalue) ? encoder_raw : encoder_maxvalue
	}

	// save
	calibrate_min = encoder_minvalue;
	calibrate_max = encoder_maxvalue;


}

/**
 * Convert data fra encoder using the calibration constants.
 * Undefined behviour if motor_encoder_calibrate has not been executed first.
 */
int8_t motor_encoder_convert_range(uint16_t raw_data){
	/// \test this need to be tested for overflow errors
	return (int8_t)((uint32_t)((raw_data - calibrate_min)*100L)/calibrate_max);
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

void PI_regulator_tune(float Kp_, float Ki_){
	Kp = Kp_;
	Ki = Ki_;
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