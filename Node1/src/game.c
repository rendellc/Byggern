/*
 * game.c
 *
 * Created: 10.11.2017 13:03:21
 *  Author: rendellc
 */ 

#include "../../Common/common.h"
#include "game.h"
#include "menu.h"
#include "joystick.h"
#include "uart.h"


#define JOY_Y_TRESHOLD 32

void game_start();




static game_state_t game_state = game_INVALID;


void game_init(){
	game_init_menu();
	
	game_state = game_IN_MENU;
}

void game_init_menu(){
	
	menu_move_cursor(0);
	
}

void game_start(){
	fprintf(&uart_out, "game starting!");
	
	//game_state = game_PLAYING;
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
		
		
			break;	
		default:
			break;
	}
}

void game_tick_playing(){
	
	// check if ball has been dropped
	
	
	
	
}

void game_tick_menu(){
	static BOOL joy_released = TRUE;
	
	menu_print_current();
	
	joystick_t joy = joy_get_state();
	
	direction_t joy_dir = joy_direction(joy.x, joy.y);
	
	switch (joy_dir){
		case UP:
			if (joy_released){
				joy_released = FALSE;
				menu_move_cursor(-1);
			}
			break;
		case DOWN:
			if (joy_released){
				joy_released = FALSE;
				menu_move_cursor(1);
			}
			break;
			
		case RIGHT:
			if (joy_released){
				joy_released = FALSE;
				menu_enter_current();
			}
			break;
		
		case LEFT:
			if (joy_released){
				joy_released = FALSE;
				menu_enter_parent();
			}
			break;
		
		case NEUTRAL:
			joy_released = TRUE;
			break;
	}
	
	/*
	
	if (joy.click && click_released){
		click_released = FALSE;
		menu_enter_current();
	} 
	else {
		 // joystick is pressed makes click_released FALSE
		
		//fprintf(&uart_out, "\tjoy: %u\tjoy_rel: %u\t", joy.y, joy_released)
		

		if ((joy.y < (128 - JOY_Y_TRESHOLD)) && joy_released){
			joy_released = FALSE;
			menu_move_cursor(1);
		} 
		else if ((joy.y > (128 + JOY_Y_TRESHOLD)) && joy_released){
			joy_released = FALSE;
			
		}
		
		if (128 - JOY_Y_TRESHOLD < joy.y && joy.y < 128 + JOY_Y_TRESHOLD){
			joy_released = TRUE;
		}
		

	}
	*/
}
