/**@file
 * Interface for motor controller and motor box interface. 
 */

#ifndef MOTOR_H_
#define MOTOR_H_

/**
 * Initialize DAC and set up IO pins to interface with motor.
 * Setup position regulator and enable motor. 
 */
void motor_init();

/// Enable motor
void motor_enable(void);

/*!
 * Subroutine which runs the motor from wall to wall and sets the global calibration variables. 
 */
void motor_encoder_calibrate();

/// set speed and direction of of motor
void motor_set_speed(int8_t speed);

/** 
 * Set setpoint of regulator
 * @param[in] position 0 is left and 255 is right. 
 */
void motor_set_position(uint8_t position);


/**
 * Read encoder position by sending commands to the motor box. 
 * This takes 40-50 microseconds and is a blocking process.
 */
int16_t motor_read_encoder(void);

/**
 * Running this will center the motor on the game board. 
 */
void motor_goto_center(void);

/**
 * Set the parameters of the regulator
 */
void motor_tune_regulator(uint16_t Kp, uint16_t Ki);

#endif /* MOTOR_H_ */