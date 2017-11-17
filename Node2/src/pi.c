#include "pi.h"
#include "uart.h" //debugging purposes

void pi_regulator_init(pi_t* regulator_p, uint16_t Kp, uint16_t Ki){
    //regulator_p->setpoint = setpoint;
    regulator_p->Kp       = Kp;
    regulator_p->Ki       = Ki;
	
    regulator_p->errorSum = 0;
	
	
	fprintf(&uart_out, "pi initialized\n");
}


int16_t pi_regulator(pi_t* regulator_p, int16_t setpoint, int16_t measurement){
	
	int16_t errors = setpoint - measurement;
	int16_t p_term, i_term;
	int32_t ret;
	
	//calculate p_term
	p_term = pi_t->Kp * errors;
	
	//calculate i_term
	pi_t->errorSum = pi_t->errorSum + errors;
	i_term = pi_t->Ki * pi_t->errorSum;
	
	//fprintf(&uart_out, "error: %i\n", regulator_p->error);
	
	ret = (p_term + i_term) / SCALING_FACTOR;
	
	return ((int16_t)ret);
}


void pi_reset_integrator(pi_t* regulator_p){
    regulator_p->errorSum = 0;
}