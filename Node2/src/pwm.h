/*!@file
*  functions for PWN signal generation
*/
#ifndef PWM_H_
#define PWM_H_
#include <stdint.h>

///initialise register for PWM signal generation
void pwm_init(void);

/// set the "duty" cycle for. min value is 1ms, max value is 2ms, linear relation for values between min and max
void pwm_set_duty(int8_t duty);

#endif /* PWM_H_ */