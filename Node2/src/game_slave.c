

/*
 * game.c
 *
 * Created: 10.11.2017 13:03:21
 *  Author: rendellc
 */ 

#include "common.h"
#include "game_slave.h"
#include "ir.h"

#define BALL_DROPPED_TRESHHOLD 40 // needs adjustment



game_state_t game_state = game_INVALID;

/** 
 * Run one iteration of game loop. 
 */
void game_tick(){
	 can_msg_t msg = can_read_buffer(0);


	switch (msg.sid){
	case can_GAME_CMD:
        
        
	break;
	case can_GAME_INFO:
        
        
	break;
	case can_GAME_DATA:
        
	break;
	default:
	break;
	}
}


void game_check_ball_dropped(){
	static BOOL ball_dropped = FALSE;
	
	// check if ball has been dropped
         
	
	
	
	
}
