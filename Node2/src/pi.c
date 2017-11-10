#include "pi.h"


void pi_regulator_init(pi_t* regulator_p, int16_t setpoint, int16_t Kp, int16_t Ki, int16_t){
    //regulator_p->setpoint = setpoint;
    regulator_p->Kp       = Kp;
    regulator_p->Ki       = Ki;
    regulator_p->error    = 0;
    regulator_p->errorSum = 0;
}


int16_t pi_regulator(pi_t* regulator_p, int16_t setpoint, int16_t encoder){
    regulator_p->error = (setpoint - encoder);
    regulator_p->errorSum += error;

    
}