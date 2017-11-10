#ifndef PI_H_
#define PI_H_

#include <stdint.h>

typedef struct pi_t_ {
    // setpoint
    //int16_t setpoint;

    // parameters
    int16_t Kp;
    int16_t Ki;

    // error tracking
    int16_t error;
    int16_t errorSum;
} pi_t;


void pi_regulator_init(pi_t* regulator_p, int16_t Kp, int16_t Ki, int16_t);


int16_t pi_regulator(pi_t* regulator_p, int16_t setpoint, int16_t measurement);


#endif