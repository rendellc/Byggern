#include "pi.h"
//#include "uart.h"

void pi_regulator_init(pi_t* regulator_p, uint16_t Kp, uint16_t Ki){
	//regulator_p->setpoint = setpoint;
	regulator_p->Kp       = Kp;
	regulator_p->Ki       = Ki;
	regulator_p->error_sum = 0;
	
	#ifdef UART_H_
	fprintf(&uart_out, "pi...");
	#endif
}

int16_t pi_regulator(pi_t* reg_p, int16_t setpoint, int16_t measurement){

	// calculate error
	int16_t error = measurement - setpoint;
	reg_p->error_sum += error;
	
	//calculate p_term
	int16_t p_term = reg_p->Kp * error;
	
	//calculate i_term
	int32_t i_term = reg_p->Ki * reg_p->error_sum / 16;
	
	#ifdef UART_H_
	fprintf(&uart_out, "setpoint: %i\n", setpoint);
	#endif
	
	return (int16_t)((p_term + i_term) / SCALING_FACTOR);
}


void pi_reset_integrator(pi_t* regulator_p){
    regulator_p->error_sum = 0;
}