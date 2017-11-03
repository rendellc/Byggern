/*
 * joystick.h
 *
 * Created: 20.10.2017 16:46:26
 *  Author: rendellc
 */ 


#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include "global_declarations.h"

int8_t convert_range(uint8_t data);

direction_t joy_direction(uint8_t joy_x, uint8_t joy_y);

void send_joy(void);

#endif /* JOYSTICK_H_ */