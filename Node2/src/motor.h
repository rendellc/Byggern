/*
 * motor.h
 *
 * Created: 03.11.2017 15:27:23
 *  Author: rendellc
 */ 


#ifndef MOTOR_H_
#define MOTOR_H_

void motor_PIreg_tune(float Kp, float Ti);

void motor_init(void);

void motor_enable(void);

void motor_set_speed(int8_t speed);

void motor_set_position(uint8_t position);

int16_t motor_read_encoder(void);

#endif /* MOTOR_H_ */