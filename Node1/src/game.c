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
#include <avr/eeprom.h>
#include "can.h"
#include "game.h"
#include "menu.h"
#include "input.h"
#include "uart.h"
#include "oled.h"
#include "lyd.h"

#define JOY_Y_TRESHOLD 32
#define BUTTON_PRESS_THRESHOLD 240
#define HIGHSCORE_SIZE 5

static game_state_t game_state = game_INVALID;
static game_setting_t game_setting = game_setting_STANDARD;

static int8_t health_left = 100; 

/// game "loop" interrupt
ISR(TIMER3_COMPA_vect){
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
	lyd_tikk();
	
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
		case game_GAME_OVER:
			game_tick_game_over();
			
		break;
		case game_FREE_PLAY: 
			game_tick_freeplay();
		
		break;
		default:
			break;
	}
}

void game_tick_paused(){
	static BOOL do_once = TRUE;
	if (do_once){
		//fprintf(&uart_out, "ball dropped\n");
		oled_home();
		oled_fill(0x0);
		fprintf(&oled_out, "Ball dropped!\n---\nPress button to retry\n");
	
		oled_pos(OLED_PAGES-1,0);
		fprintf(&oled_out, "Lives left: %u\n", health_left);
		--health_left;
	
		if (health_left <= 0){
			game_state = game_GAME_OVER;
			do_once = TRUE;
		}
	}
	
	touch_t touch = touch_get_state();
	
	if (touch.button > BUTTON_PRESS_THRESHOLD){
		oled_fill(0x0);
		
		// notify node 2
		can_msg_t msg = {};
		msg.sid = can_GAME_CMD;
		msg.cmd_specifier = game_cmd_RESET_GAME;
		msg.length = 1;
		can_send(msg,0);
		
		_delay_ms(10);
		
		game_state = game_PLAYING;
		do_once = TRUE;
	}
}


void game_tick_game_over(void){
	fprintf(&uart_out, "game over\n");
	oled_fill(0x0);
	
	oled_pos(OLED_PAGES/2 , OLED_WIDTH/8);
	fprintf(&oled_out, "GAME OVER");
	
	_delay_ms(5000);
	game_state = game_IN_MENU;
}

void game_tick_playing(){
	//static BOOL joy_released = FALSE;
	
	game_tick_playing_oled();
	
	// check if ball has been dropped every 256th tick
	//static uint8_t i = 0;
	//if (++i){
		can_msg_t request = {};
		request.sid = can_GAME_CMD;
		request.data[0] = game_cmd_CHECK_BALL_DROP;
		request.length = 1;
		can_send(request,0);
	//}
	
	//	handle input
	joystick_t joy = joy_get_state();
	touch_t  touch = touch_get_state();
	
	can_msg_t action_cmd = {};
	action_cmd.sid = can_GAME_CMD;
	action_cmd.length = 5;
	action_cmd.cmd_specifier = game_cmd_ACTION;
	action_cmd.cmd_data[game_joy_x_index] = convert_range(joy.x);
	action_cmd.cmd_data[game_joy_y_index] = convert_range(joy.y);
	action_cmd.cmd_data[game_slider_index] = touch.slider;
	action_cmd.cmd_data[game_button_index] = (touch.button > BUTTON_PRESS_THRESHOLD);	
	
	// \todo Find out why messages wont be sent if there is no delay between creation of action_cmd and sending
	_delay_us(100);
	can_send(action_cmd, 0);
	
	// handle bus
	can_msg_t recieved = can_read_buffer(0);
	
	while (recieved.sid != can_INVALID){
		switch (recieved.sid){
		case can_GAME_DATA:
			//can_print_msg(recieved);
			switch (recieved.data[0]){
			case game_cmd_CHECK_BALL_DROP:
				if (recieved.data[1]){
					game_state = game_PAUSED;
				}
			}
		break;
		default:
			fprintf(&uart_out, "ERROR: unhandled message\n");
		break;
		}
		
		recieved = can_read_buffer(0);
	}	
}


void game_tick_freeplay(void){
	oled_fill(0x0);
	
	
	//	handle input
	joystick_t joy = joy_get_state();
	touch_t touch = touch_get_state();
	
	can_msg_t action_cmd = {};
	action_cmd.sid = can_GAME_CMD;
	action_cmd.length = 5;
	action_cmd.cmd_specifier = game_cmd_ACTION;
	action_cmd.cmd_data[game_joy_x_index] = convert_range(joy.x);
	action_cmd.cmd_data[game_joy_y_index] = convert_range(joy.y);
	action_cmd.cmd_data[game_slider_index] = touch.slider;
	action_cmd.cmd_data[game_button_index] = (touch.button > BUTTON_PRESS_THRESHOLD);
	
	// \todo Find out why messages wont be sent if there is no delay between creation of action_cmd and sending
	_delay_us(100);
	can_send(action_cmd, 0);
	
	// ignore all bus activite
	can_msg_t recieved = can_read_buffer(0);
	while (recieved.sid != can_INVALID){
		recieved = can_read_buffer(0);
	}
}


void game_tick_playing_oled(void){
	oled_fill(0x0);
	//joystick_t joy = joy_get_state()
	
	oled_pos(OLED_PAGES-1, 0);
	fprintf(&oled_out, "HP : %i",  health_left);

}

void game_tick_menu(void){
	
	menu_print_current();
        
	menu_handle_input();
}


//////////////////////////////////////////////////////////////////////////
void game_start(void){
	
	can_msg_t msg = {};
	msg.sid = can_GAME_CMD;
	msg.data[0] = game_cmd_RESET_GAME;
	msg.length = 1;
	can_send(msg,0);
	
	// wait for node2 to prepare
	_delay_ms(10);
	
	game_state = game_PLAYING; // \note dont use game_set_state here since state will be set by the above can message
	
	fprintf(&uart_out, "game starting\n");
}


void game_setting_standard(void){
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


void game_setting_alternative(void){
	oled_pos(OLED_PAGES-1, 0);
	fprintf(&oled_out, "active: alternative");
	can_msg_t setting_msg;
	setting_msg.sid = can_GAME_CMD;
	setting_msg.length = 2;
	setting_msg.data[0] = game_cmd_CHANGE_SETTING;
	setting_msg.data[1] = game_cmd_setting_ALTERNATIVE;
	can_send(setting_msg,0);
	
	game_setting = game_setting_ALTERNATIVE;
}


void game_difficulty_standard(void){
	//fprintf(&uart_out, "standard\n");
	
	can_msg_t diff_msg;
	diff_msg.sid = can_GAME_CMD;
	diff_msg.length = 2;
	diff_msg.cmd_specifier =  game_cmd_CHANGE_DIFFICULTY;
	diff_msg.cmd_data[0] = game_cmd_difficulty_STANDARD;
	
	can_send(diff_msg,0);
}
void game_difficulty_impossible(void){
	//fprintf(&uart_out, "impossible\n");
	
	can_msg_t diff_msg;
	diff_msg.sid = can_GAME_CMD;
	diff_msg.length = 2;
	diff_msg.cmd_specifier =  game_cmd_CHANGE_DIFFICULTY;
	diff_msg.cmd_data[0] = game_cmd_difficulty_IMPOSSIBLE;
	
	can_send(diff_msg,0);
}

void game_state_freeplay(void){
	game_state = game_FREE_PLAY;
}