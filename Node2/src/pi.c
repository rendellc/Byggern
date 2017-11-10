#include "pi.h"


void pi_regulator_init(pi_t* regulator_p, int16_t setpoint, int16_t Kp, int16_t Ki){
    //regulator_p->setpoint = setpoint;
    regulator_p->Kp       = Kp;
    regulator_p->Ki       = Ki;
    regulator_p->error    = 0;
    regulator_p->errorSum = 0;
}


int16_t pi_regulator(pi_t* regulator_p, int16_t setpoint, int16_t measurement){
    regulator_p->error = (setpoint - measurement);
    regulator_p->errorSum += regulator_p->error;    
}