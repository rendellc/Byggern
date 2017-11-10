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

#define CLICK_PIN PE0

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

joystick_t joy_get_state(){
	joystick_t joy_state = {};
	
	joy_state.x = adc_read_channel(CH_JOY_X);
	joy_state.y = adc_read_channel(CH_JOY_Y);
	joy_state.click = !(PINE & (1 << CLICK_PIN));
	
	return joy_state;
}


void send_slider(void)
{
	uint8_t slider_pos = adc_read_channel(CH_SLIDER);
	
	can_msg_t msg = {};
	msg.sid = MSG_SLIDER;
	msg.data[0] = slider_pos;
	msg.length = 1;
	
	fprintf(&uart_out, "slider sent: %i\n",  msg.data[0]);
	
	can_send(msg, 0);
		
}

void send_joy(void)
{
	uint8_t joy_x = adc_read_channel(CH_JOY_X);
	uint8_t joy_y = adc_read_channel(CH_JOY_Y);
	direction_t joy_dir = joy_direction(joy_x, joy_y);
	
	int8_t joy_xs = convert_range(joy_x);
	int8_t joy_ys = convert_range(joy_y);
	
	uint8_t joy_click = !(PINE & (1 << CLICK_PIN));
	
	can_msg_t msg = {};
	msg.sid = MSG_JOY;
	msg.data[0] = joy_xs;
	msg.data[1] = joy_ys;
	msg.data[2] = joy_dir; // casting?
	msg.data[3] = (uint8_t)joy_click;
	msg.length = 4;
	
	fprintf(&uart_out, "joy sent: %i\t%i\t%i\t%i\n",   joy_xs, joy_ys, joy_dir, joy_click);
	
	can_send(msg, 0);
}


void joystick_init(void){
	PORTE  |= 1 << CLICK_PIN;    // + PUD=0 gives pull-up
	DDRE   &= ~(1 << CLICK_PIN); // input
}

