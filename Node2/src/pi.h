#ifndef PI_H_
#define PI_H_

#include <stdint.h>

typedef struct pi_t_ {
    // setpoint
    //float setpoint;

    // parameters
    float Kp;
    float Ki;

    // error tracking
    float error;
    float errorSum;
} pi_t;


void pi_regulator_init(struct pi_t_* regulator_p, float Kp, float Ki);


float pi_regulator(struct pi_t_* regulator_p, float setpoint, float measurement);


#endif