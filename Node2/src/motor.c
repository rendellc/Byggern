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


// calibration
static int16_t calibrate_min = 0;
static int16_t calibrate_max = 0;

/// signal processing 
/// lowpass the raw encoder data with 1. order AR filter with coefficient lowpass_enable / lowpass_coeff
const int16_t lowpass_coeff = 4;
const int16_t lowpass_enable = 1; /*!< set to zero to disable low pass filter */

// regulator
static uint8_t position_regulator_enabled = 0;


pi_t regulator = {};

/// position set point
static volatile int16_t encoder_setpoint = 0;

static volatile int16_t raw_encoder[2] = {};
static volatile int16_t force[2] = {}; 


ISR(TIMER0_OVF_vect){
	cli();
	// sample encoder
	raw_encoder[1] = raw_encoder[0];
	raw_encoder[0] = lowpass_enable * raw_encoder[1]/lowpass_coeff + motor_read_encoder();
	
	
	if (position_regulator_enabled){
		motor_set_speed(pi_regulator(&regulator, encoder_setpoint, raw_encoder[0]));
		//fprintf(&uart_out, "output %i\n", force[0]/128);
		
	}
	
	
	// clear overflow flag
	TIFR0  |= (1 << TOV0);
	sei();
}


/*!
 * Calibrate motor encoder
 */
void motor_encoder_calibrate()
{
	motor_set_speed(40);
	int16_t old_value = motor_read_encoder();
	_delay_ms(100);
	int16_t new_value = motor_read_encoder();
	//find min value
	
	while(new_value < old_value)
	{
		old_value = motor_read_encoder();
		_delay_ms(100);
		new_value = motor_read_encoder();
	}
	calibrate_max = new_value;
	fprintf(&uart_out, "encoder min %i\n", calibrate_min);
	
	
	motor_set_speed(-40);
	old_value = motor_read_encoder();
	_delay_ms(100);
	new_value = motor_read_encoder();
	//find max value
	
	while(new_value > old_value)
	{
		old_value = motor_read_encoder();
		_delay_ms(100);
		new_value = motor_read_encoder();
	}
	calibrate_min = new_value;
	fprintf(&uart_out, "encoder max %i\n", calibrate_max);
}


/*!
 * Convert data fra encoder using the calibration constants.
 * Undefined behaviour if motor_encoder_calibrate has not been executed first.
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
	
	// toggle !RST comment out PORTH &= ~(1 << PIN_RST); if you want position encoder
	//PORTH &= ~(1 << PIN_RST);
	_delay_us(10);
	PORTH |= (1 << PIN_RST);
	
	
	// set !OE high
	PORTH |= (1 << PIN_OE);
	
	// store data
	return read;
}


void motor_init(void){
	dac_init();
	
	DDRH |= ( 1 << PIN_DIR | 1 << PIN_SEL | 1 << PIN_EN | 1 << PIN_OE | 1 << PIN_RST );
	
	
	/// set sampling rate for encoder, \todo change to 16-bit timer to get slower refresh rate
	TCCR0B |= (1 << CS02 | 1 << CS00); // prescaler 1024 here  but prescaler = 256 => crash? \test more
	TIFR0  |= (1 << TOV0);   // clear overflow flag
	TIMSK0 |= (1 << TOIE0);  // enable
	
	// set regulator parameters
	pi_regulator_init(&regulator, 1, 0.1);
	
	motor_enable();
}

void motor_enable(void){
	PORTH |= (1 << PIN_EN);
}

void motor_set_speed(int8_t speed){	
	if (speed < 0){
		PORTH &= ~(1 << PIN_DIR);
		dac_output(-speed); 		// -speed because speed is negative
	} else {
		PORTH |= (1 << PIN_DIR);
		dac_output(speed);
	}	
}


void motor_set_position(uint8_t position){
	/*
	if (position > 100){
		position = 100;
	}
	*/
	
	encoder_setpoint = calibrate_min + ((calibrate_max - calibrate_min)*(int32_t)position)/256;
	
	//pi_regulator(&regulator, encoder_setpoint, raw_encoder[0]);
	
	position_regulator_enabled = 1;
}