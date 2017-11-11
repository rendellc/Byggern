/*
 * game.c
 *
 * Created: 10.11.2017 13:03:21
 *  Author: rendellc
 */ 

#include "common.h"

game_state_t game_state = INVALID;

void game_tick(){
	
	
	switch (game_state){
		case PLAYING:
			game_tick_playing();
			break;
		
		default:
			break;
	}
	
	
	
	
	
}


void game_tick_playing(){
	static BOOL ball_dropped = FALSE;
	
	// check if ball has been
	
	
	
	
	
}