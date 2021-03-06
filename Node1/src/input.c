/**@file
 * Implementation of joystick module for node 1.
 */

#include "can.h"
#include "adc.h"
#include "uart.h"

#define CH_JOY_Y	0 /*!< ADC channel for joystick y axis */
#define CH_JOY_X	1 /*!< ADC channel for joystick x axis */
#define CH_SLIDER	2 /*!< ADC channel for slider */
#define CH_TOUCH	3 /*!< ADC channel for touch button */
#define ANALOG_THRESH 25 // 18 before

#define CLICK_PIN PE0 /*!< Pin on atmega162 which joy click is connected to */

/// Convert range from 0-to-255 to -100-to-100
int8_t convert_range(uint8_t data){
	return (int8_t)((200*data) / 255 - 100);
}

/**
 * Calculate direction of joystick based on coordinates
 * @param[in] joy_x X axis of joystick
 * @param[in] joy_y Y axis of joystick
 */
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

/// get current state of joystick
joystick_t joy_get_state(){
	joystick_t joy_state = {};
	
	joy_state.x = adc_read_channel(CH_JOY_X);
	joy_state.y = adc_read_channel(CH_JOY_Y);
	joy_state.click = !(PINE & (1 << CLICK_PIN));
	
	return joy_state;
}

// Send slider position on can bus. Used for testing. \deprecated
/*
void send_slider(void)
{
	uint8_t slider_pos = adc_read_channel(CH_SLIDER);
	
	can_msg_t msg = {};
	msg.sid = can_SLIDER;
	msg.data[0] = slider_pos;
	msg.length = 1;
	
	fprintf(&uart_out, "slider sent: %i\n",  msg.data[0]);
	
	can_send(msg, 0);
		
}
*/

touch_t touch_get_state(){
	uint8_t slider = adc_read_channel(CH_SLIDER);
	uint8_t button = adc_read_channel(CH_TOUCH);
	
	touch_t touch = {};
	touch.button = button;
	touch.slider = slider;
	
	
	return touch;
}



/*
 * Read joystick and send it as a message on the can bus.
 * Always sends on can channel 0. 
 \deprecated
 */
/*
void send_joy(void)
{
	uint8_t joy_x = adc_read_channel(CH_JOY_X);
	uint8_t joy_y = adc_read_channel(CH_JOY_Y);
	direction_t joy_dir = joy_direction(joy_x, joy_y);
	
	int8_t joy_xs = convert_range(joy_x);
	int8_t joy_ys = convert_range(joy_y);
	
	uint8_t joy_click = !(PINE & (1 << CLICK_PIN));
	
	can_msg_t msg = {};
	msg.sid = can_JOY;
	msg.data[0] = joy_xs;
	msg.data[1] = joy_ys;
	msg.data[2] = joy_dir; // casting?
	msg.data[3] = (uint8_t)joy_click;
	msg.length = 4;
	
	fprintf(&uart_out, "joy sent: %i\t%i\t%i\t%i\n",   joy_xs, joy_ys, joy_dir, joy_click);
	
	can_send(msg, 0);
}
*/
