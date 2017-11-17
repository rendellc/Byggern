/*!@file
*  functions for PI regulator
*/

#ifndef PI_H_
#define PI_H_

#include <stdint.h>

#define SCALING_FACTOR 128

typedef struct pi_t_ {
    // setpoint
    //float setpoint;

    // parameters
    uint16_t Kp;
    uint16_t Ki;

    // error tracking
    int16_t errorSum;
} pi_t;


/** Initialise discrete PI regulator 
    @param select which PI regulator
	@param set Kp gain
	@param set Ki gain
*/
void pi_regulator_init(struct pi_t_* regulator_p, float Kp, float Ki);

/** returns the motor speed from the regulator
    @param select which PI regulator
	@param set setpoint
	@param measured value
	@return motor speed
*/
int16_t pi_regulator(struct pi_t_* regulator_p, float setpoint, float measurement);


#endif