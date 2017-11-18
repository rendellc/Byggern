/*
 * game.c
 *
 * Created: 10.11.2017 13:03:21
 *  Author: rendellc
 */ 

#include "../../Common/common.h"
#include "global_declarations.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "can.h"
#include "game.h"
#include "menu.h"
#include "input.h"
#include "uart.h"
#include "oled.h"

#define JOY_Y_TRESHOLD 32
#define BUTTON_PRESS_THRESHOLD 240

static game_state_t game_state = game_INVALID;
static game_setting_t game_setting = game_setting_STANDARD;

// settings


uint8_t interrupt_counter = 0;
ISR(TIMER3_COMPA_vect){
	++interrupt_counter;
	if (!interrupt_counter)
		fprintf(&uart_out, ":");
	
	
	game_tick();
}


void game_init(){
	game_state = game_IN_MENU;
	
	// set up interrupt
	TCCR3A |= (1 << COM3A1); //| (1 << FOC3A); // CTC, Top=OCR3A
	
	// prescaler=1024 and OCR3A=128 gives frequency of 60 Hz
	TCCR3B |= (1 << WGM32) | (1 << CS32);
	OCR3A = 130;
	
	ETIMSK |= (1 << OCIE3A);
}


void game_tick(){
	
	switch (game_state){
		case game_INVALID:
			
			break;
		case game_IN_MENU:
			game_tick_menu();
			
			break;
		case game_PLAYING:
			game_tick_playing();
		
			break;
		case game_PAUSED:
			game_tick_paused();
			break;	
		default:
			break;
	}
}

void game_tick_paused(){
	static BOOL do_once = FALSE;
	if (!do_once){
		do_once = TRUE;
		fprintf(&uart_out, "Press button to retry\n");
	}
	
	touch_t touch = touch_get_state();
	
	if (touch.button > BUTTON_PRESS_THRESHOLD){
		// notify node 2
		can_msg_t msg = {};
		msg.sid = can_GAME_CMD;
		msg.data[0] = game_cmd_RESET_GAME;
		msg.length = 1;
		can_send(msg,0);
		
		game_state = game_PLAYING;
	}
}


void game_tick_playing(){
	//static BOOL joy_released = FALSE;
	
	// check if ball has been dropped every 256th tick
	static uint8_t i = 0;
	if (++i){
		can_msg_t request = {};
		request.sid = can_GAME_CMD;
		request.data[0] = game_cmd_CHECK_BALL_DROP;
		request.length = 1;
		can_send(request,0);
	}
	
	
	//	handle input
	joystick_t joy = joy_get_state();
	touch_t touch = touch_get_state();
	
	fprintf(&uart_out, "%u\n", joy.x);
	
	can_msg_t action_cmd = {};
	action_cmd.sid = can_GAME_CMD;
	action_cmd.length = 4;
	action_cmd.data[0] = game_cmd_ACTION;
	
	switch (game_setting){
		case game_setting_STANDARD:
			action_cmd.data[game_setting_standard_motor] = touch.slider;
			action_cmd.data[game_setting_standard_turn]  = convert_range(joy.x);
			action_cmd.data[game_setting_standard_fire]  = (touch.button > BUTTON_PRESS_THRESHOLD);
		break;
		case game_setting_ALTERNATIVE:
			action_cmd.data[game_setting_alternative_motor] = convert_range(joy.x);
			action_cmd.data[game_setting_alternative_turn]  = touch.slider;
			action_cmd.data[game_setting_alternative_fire]  = (touch.button > BUTTON_PRESS_THRESHOLD);
		break;
	}
	can_send(action_cmd, 0);
	
	// handle bus
	can_msg_t recieved = can_read_buffer(0);
	
	while (recieved.sid != can_INVALID){
		
		switch (recieved.sid){
		case can_GAME_INFO:
			//can_print_msg(recieved);
			switch (recieved.data[0]){
			case game_cmd_CHECK_BALL_DROP:
				if (recieved.data[1]){
					fprintf(&uart_out, "ball dropped\n");
					game_set_state(game_PAUSED);
				}
				
			}
		break;
		case can_GAME_DATA:
			
			
		break;
		case can_GAME_CMD:
			
		break;
		default:
			fprintf(&uart_out, "ERROR: Node 1 recieved non-game related can message\n");
		break;
		}
		
		recieved = can_read_buffer(0);
	}
}

void game_tick_menu(){
	
	menu_print_current();
        
	menu_handle_input();
}


void game_set_state(game_state_t state){
	game_state = state;
	
	// update node 2
	can_msg_t msg = {};
	msg.sid = can_GAME_CMD;
	msg.data[0] = game_cmd_SET_STATE;
	msg.data[1] = state;
	//can_send(msg, 0);
	
	
}

//////////////////////////////////////////////////////////////////////////
void game_start(){
	
	can_msg_t msg = {};
	msg.sid = can_GAME_CMD;
	msg.data[0] = game_cmd_RESET_GAME;
	msg.length = 1;
	can_send(msg,0);
	
	// wait for node2 to acknowledge
	_delay_ms(1000);
	
	game_state = game_PLAYING; // \note dont use game_set_state here since state will be set by the above can message
	
	fprintf(&uart_out, "game starting\n");
}


void game_setting_standard(){
	oled_pos(7, 0);
	fprintf(&oled_out, "active: standard");
	
	//fprintf(&uart_out, "Setting set to Standard\n");
	// notify node 2
	can_msg_t setting_msg;
	setting_msg.sid = can_GAME_CMD;
	setting_msg.length = 2;
	setting_msg.data[0] = game_cmd_CHANGE_SETTING;
	setting_msg.data[1] = game_setting_STANDARD;
	can_send(setting_msg,0);
	
	game_setting = game_setting_STANDARD;
}


void game_setting_alternative(){
	oled_pos(7, 0);
	fprintf(&oled_out, "active: alternative");
	can_msg_t setting_msg;
	setting_msg.sid = can_GAME_CMD;
	setting_msg.length = 2;
	setting_msg.data[0] = game_cmd_CHANGE_SETTING;
	setting_msg.data[1] = game_setting_ALTERNATIVE;
	can_send(setting_msg,0);
	
	
	game_setting = game_setting_ALTERNATIVE;
}

