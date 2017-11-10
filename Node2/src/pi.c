#include "pi.h"
#include "uart.h"

void pi_regulator_init(pi_t* regulator_p, int16_t Kp, int16_t Ki){
    //regulator_p->setpoint = setpoint;
    regulator_p->Kp       = Kp;
    regulator_p->Ki       = Ki;
    regulator_p->error    = 0;
    regulator_p->errorSum = 0;
	
	
	fprintf(&uart_out, "pi initialized\n");
}


int16_t pi_regulator(pi_t* regulator_p, int16_t setpoint, int16_t measurement){
    regulator_p->error = (measurement - setpoint);
    regulator_p->errorSum += regulator_p->error;
	
	//fprintf(&uart_out, "error: %i\n", regulator_p->error);
	
	return regulator_p->Kp * regulator_p->error + regulator_p->Ki * regulator_p->errorSum / 16;
}