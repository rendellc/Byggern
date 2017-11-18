#include "pi.h"
#include "uart.h"

void pi_regulator_init(pi_t* regulator_p, float Kp, float Ki){
    //regulator_p->setpoint = setpoint;
    regulator_p->Kp       = Kp;
    regulator_p->Ki       = Ki;
    regulator_p->error    = 0;
    regulator_p->errorSum = 0;
	
	#ifdef UART_H_
	fprintf(&uart_out, "pi...");
	#endif
}


float pi_regulator(pi_t* regulator_p, float setpoint, float measurement){
    regulator_p->error = (measurement - setpoint);
    regulator_p->errorSum += regulator_p->error;
	
	//fprintf(&uart_out, "error: %i\n", regulator_p->error);
	
	return regulator_p->Kp * regulator_p->error + regulator_p->Ki * regulator_p->errorSum;
}


void pi_reset_integrator(pi_t* regulator_p){
    regulator_p->errorSum = 0;
}