/*
 * pwm.h
 *
 * Created: 27.10.2017 09:38:05
 *  Author: rendellc
 */ 


#ifndef PWM_H_
#define PWM_H_
#include <stdint.h>

void pwm_init(void);

void pwm_set_duty(uint8_t duty);

#endif /* PWM_H_ */