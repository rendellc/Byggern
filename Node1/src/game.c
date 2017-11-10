/*
 * game.c
 *
 * Created: 10.11.2017 13:03:21
 *  Author: rendellc
 */ 

#include "game.h"
#include "menu.h"
#include "joystick.h"
#include "uart.h"


#define TRUE 1
#define FALSE (!TRUE)
typedef uint8_t BOOL;

#define JOY_Y_TRESHOLD 32

void game_start();


typedef enum {
	INVALID,
	IN_MENU,
	PLAYING,
	PAUSED,
	
} game_state_t;

static game_state_t game_state = INVALID;


void game_init(){
	game_init_menu();
	
	game_state = IN_MENU;
}

void game_init_menu(){
	
	menu_move_cursor(0);
	
}

void game_start(){
	fprintf(&uart_out, "game starting!");
}


void game_tick(){
	
	switch (game_state){
		case INVALID:
		
		
		
			break;
		case IN_MENU:
			game_tick_menu();
			
			break;
		case PLAYING:
		
			break;
		case PAUSED:
		
		
			break;	
		default:
			break;
	}
}

void game_tick_menu(){
	static BOOL click_released = TRUE;
	static BOOL joy_released = TRUE;
	
	menu_print_current();
	
	joystick_t joy = joy_get_state();
	
	
	if (joy.click && !!click_released){
		click_released = FALSE;
		menu_enter_current();
	} 
	else {
		click_released = TRUE;
		
		fprintf(&uart_out, "\tjoy: %u\tjoy_rel: %u\t", joy.y, joy_released);
		if ((joy.y < (128 - JOY_Y_TRESHOLD)) && joy_released){
			joy_released = FALSE;
			menu_move_cursor(1);
		} 
		else if ((joy.y > (128 + JOY_Y_TRESHOLD)) && joy_released){
			joy_released = FALSE;
			menu_move_cursor(-1);
		}
		
		if (128 - JOY_Y_TRESHOLD < joy.y && joy.y < 128 + JOY_Y_TRESHOLD){
			joy_released = TRUE;
		}
		
	}
	

}
