/*
 * joy_com.c
 *
 * Created: 20.10.2017 16:00:26
 *  Author: rendellc
 */ 

#include "can.h"
#include "adc.h"
#include "uart.h"

#define CH_JOY_Y	0
#define CH_JOY_X	1
#define CH_SLIDER	2
#define CH_TOUCH	3
#define ANALOG_THRESH 25 // 18 before

int8_t convert_range(uint8_t data){ // convert from 0->255 to -100->100
	return (float)data*0.784314 - 100;
}

direction_t joy_direction(uint8_t joy_x, uint8_t joy_y){
	if (abs(joy_x-128) < ANALOG_THRESH && abs(joy_y-128) < ANALOG_THRESH){
		return NEUTRAL;
	}
	
	if (joy_x - 128 > abs(joy_y - 128))
		return RIGHT;
	else if (joy_x - 128 < - abs(joy_y - 128))
		return LEFT;
	else if (joy_y - 128 > abs(joy_x - 128))
		return UP;
	else
		return DOWN;
	
}

void send_joy(void)
{
	uint8_t joy_x = adc_read_channel(CH_JOY_X);
	uint8_t joy_y = adc_read_channel(CH_JOY_Y);
	direction_t joy_dir = joy_direction(joy_x, joy_y);
	
	
	int8_t joy_xs = convert_range(joy_x);
	int8_t joy_ys = convert_range(joy_y);

	can_msg_t msg = {};
	msg.sid = MSG_JOY;
	msg.data[0] = joy_xs;
	msg.data[1] = joy_ys;
	msg.data[2] = joy_dir; // casting?
	msg.length = 3;
	
	fprintf(&uart_out, "joy_x: %i\t",   joy_xs);
	fprintf(&uart_out, "joy_y: %i\t",   joy_ys);
	fprintf(&uart_out, "joy_dir: %i\n", joy_dir);
	
	can_send(msg, 0);
}

