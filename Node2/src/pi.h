/*!@file
*  functions for PI regulator
*/

#ifndef PI_H_
#define PI_H_

#include <stdint.h>

#define SCALING_FACTOR 128

/// PI regulator data structure
typedef struct pi_t_ {
    uint16_t Kp;
    uint16_t Ki;

    // error tracking
    int32_t error_sum;
} pi_t;


/** Initialise discrete PI regulator 
 *  @param[in] regulator_p select which PI regulator
 *	@param Kp proportional gain
 *	@param Ki integral gain
 */
void pi_regulator_init(struct pi_t_* regulator_p, uint16_t Kp, uint16_t Ki);


/** returns the motor speed from the regulator
 *  @param select which PI regulator
 *	@param set setpoint
 *	@param measured value
 *	@return motor speed
 */
int16_t pi_regulator(struct pi_t_* regulator_p, int16_t setpoint, int16_t measurement);


#endif