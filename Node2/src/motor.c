/**@file
 * Implementation of motor controller and interface to 
 * motor box.
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


#define PIN_DIR	PH1 /*!< Motor direction select pin */
#define PIN_SEL PH3 /*!< Motor encoder mode select pin */
#define PIN_EN	PH4 /*!< Motor enable pin */
#define PIN_OE	PH5 /*!< Encoder output enable pin */
#define PIN_RST PH6 /*!< Encoder reset pin */

#define MOTOR_MAX_FORCE			50
#define MOTOR_CALIBRATE_SPEED	40 /*!< speed which motor will run with during calibration */
#define MOTOR_REG_THESHOLD		100 /*!< Regulator is stopped when abs(encoder-setpoint) < threshold */

// calibration
static BOOL is_calibrated = FALSE;
static int16_t calibrate_min = 0;
static int16_t calibrate_max = 0;

/// signal processing 
/// lowpass the raw encoder data with 1. order AR filter with coefficient lowpass_enable / lowpass_coeff
const int16_t lowpass_coeff = 4;
const int16_t lowpass_enable = 1; /*!< set to zero to disable low pass filter */

// regulator
pi_t regulator;

/// position set point
static volatile int16_t encoder_setpoint = 0;


static volatile int16_t measurement = 0; /*!< Raw encoder data. Index i means at time (n - i) */
//static volatile int16_t force[2] = {}; 		 /*!< Output sent to motor Index i means at time (n - i) */

/// Interrupt vector which controls the motor
ISR(TIMER4_OVF_vect){
	
	//fprintf(&uart_out, "encoder setpoint: %i\t%i\n", encoder_setpoint, calibrate_min);
	//fprintf(&uart_out, "encoder: %i\t%i\t%i\n", calibrate_min, encoder_setpoint, calibrate_max);
	measurement = motor_read_encoder();
	int8_t force = pi_regulator(&regulator, encoder_setpoint, measurement);
	
	if (force > MOTOR_MAX_FORCE){
		force = MOTOR_MAX_FORCE;
	} 
	else if (force < - MOTOR_MAX_FORCE){
		force = -MOTOR_MAX_FORCE;
	}
	
	motor_set_speed(force);
	fprintf(&uart_out, "force: %i\n", force);
	//fprintf(&uart_out, "pi_reg %i\t%i\t%i\n", encoder_setpoint, measurement, force);
	
}


/*!
 * Subroutine which runs the motor from wall to wall and sets the global calibration variables. 
 */
void motor_encoder_calibrate()
{	
	fprintf(&uart_out, "Motor calibration\n");
	
	motor_set_speed(-MOTOR_CALIBRATE_SPEED);
	int16_t old_value = motor_read_encoder();
	_delay_ms(100);
	int16_t new_value = motor_read_encoder();
	
	//find max value
	while(new_value > old_value)
	{
		old_value = motor_read_encoder();
		_delay_ms(100);
		new_value = motor_read_encoder();
		
	}
	calibrate_min = new_value;
	fprintf(&uart_out, "\tleft %i <---> ", calibrate_min);
	
	motor_set_speed(MOTOR_CALIBRATE_SPEED);
	old_value = motor_read_encoder();
	_delay_ms(100);
	new_value = motor_read_encoder();
	//find min value
	
	while(new_value < old_value)
	{
		old_value = motor_read_encoder();
		_delay_ms(100);
		new_value = motor_read_encoder();
	}
	calibrate_max = new_value;
	fprintf(&uart_out, "%i right\n", calibrate_max);
	
	motor_set_speed(0);
	
	is_calibrated = TRUE;
	

	
	motor_goto_center();
	
}

/*!
 * Convert data from encoder using the calibration constants.
 * Undefined behaviour if motor_encoder_calibrate has not been executed first.
 * \deprecated
 */
int8_t motor_encoder_convert_range(uint16_t raw_data){
	/// \test this need to be tested for overflow errors
	return (int8_t)((uint32_t)((raw_data - calibrate_min)*100L)/calibrate_max);
}

/**
 * Read encoder position by sending commands to the motor box. 
 * This takes 40-50 microseconds and is a blocking process.
 */
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

/**
 * Initialize DAC and set up IO pins to interface with motor.
 * Setup position regulator and enable motor. 
 */
void motor_init(void){

	DDRH |= ( 1 << PIN_DIR | 1 << PIN_SEL | 1 << PIN_EN | 1 << PIN_OE | 1 << PIN_RST );
	
	
	/// set sampling rate for encoder to 15 Hz with prescaler=4, and interrupt on overflow
	TCCR4B |= (1 << CS41); 
	
	// set regulator parameters
	pi_regulator_init(&regulator, 1, 1);
}

/**
 * Enable motor,
 */
void motor_enable(void){
	PORTH |= (1 << PIN_EN);
}


/**
 * Set speed of motor.
 * @param speed Integer which denotes speed and direction. Negative numbers give negative motion. 
 */
void motor_set_speed(int8_t speed){	
	sei(); // twi locks node if interrupts are disabled. \todo Find better solution
	
	if (speed < 0){
		PORTH &= ~(1 << PIN_DIR);
		dac_output(-speed); 		// -speed because speed is negative
	} else {
		PORTH |= (1 << PIN_DIR);
		dac_output(speed);
	}	
}

/**
 * Set setpoint for motor and enable position regulator. 
 * @param[in] position 0 is left and 255 is right. 
 */
void motor_set_position(uint8_t position){
	if (is_calibrated){
		
	
		/*
		#define MOVING_AVG_NUMBER 5
		static uint8_t position_log[MOVING_AVG_NUMBER] = {};
		static int16_t last_setpoint = 0;
		//static int32_t encoder_setpoint[3] = {};
		
		uint16_t position_filtered = position;
		for (uint8_t i = 0; i < MOVING_AVG_NUMBER-1; ++i){
			position_filtered += position_log[i];
		}
		position = (uint8_t)position_filtered / MOVING_AVG_NUMBER;
		*/

		// Add 1st order IIR filter make measurements more stable
	
		//fprintf(&uart_out, "position: %i\n", (int16_t)position);
	
		encoder_setpoint = calibrate_min + (calibrate_max - calibrate_min)/256 * (int16_t)position;
	
		/*
		if ( abs(encoder_setpoint - last_setpoint) > 35)
			fprintf(&uart_out, "difference %i\t%u\n", abs(encoder_setpoint - last_setpoint), position);
		*/

		//fprintf(&uart_out, "motor %u\n", (uint8_t)position);
		//fprintf(&uart_out, "encoder: %i\t%i\t%i\n", calibrate_min, encoder_setpoint, calibrate_max);
		
		//pi_regulator(&regulator, encoder_setpoint, measurement);
		
		// enable interrupts
		TIMSK4 |= (1 << TOIE4);
		TIFR4 |= (1 << TOV4);
	}
	
}


void motor_disable_position_control(void){
	TIMSK4 &= ~(1 << TOIE4);  // disable interrupts
}



void motor_goto_center(void){
	
	motor_set_position(128);
	
	while (abs(encoder_setpoint - measurement) > MOTOR_REG_THESHOLD);
	
	
	motor_disable_position_control();
}