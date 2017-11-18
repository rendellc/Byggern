

/*
 * game.c
 *
 * Created: 10.11.2017 13:03:21
 *  Author: rendellc
 */ 

#include "common.h"
#include "global_declarations.h"
#include "solenoid.h"
#include "game_slave.h"
#include "motor.h"
#include "can.h"
#include "ir.h"
#include "pwm.h"
#include "uart.h"

#include <avr/interrupt.h>

#define BALL_DROPPED_TRESHHOLD		20	// needs adjustment
#define BALL_PICKED_UP_THRESHHOLD	100 // needs adjustment

#define PWM_TURN_MID 50

/// variable for keeping track of ball
static BOOL ball_dropped = FALSE;

/// game state
static game_setting_t game_setting = game_setting_STANDARD;

/// Declare reset acknowledgment message, initialized in init
static const can_msg_t msg_reset_ack = {can_GAME_INFO, 2, {game_cmd_SLAVE_ACK, game_cmd_RESET_GAME}};
static const can_msg_t msg_start_ack = {can_GAME_INFO, 2, {game_cmd_SLAVE_ACK, game_cmd_RESET_GAME}};

static uint8_t interrupt_counter = 0;
ISR(TIMER3_COMPA_vect){
	if (!++interrupt_counter)
		fprintf(&uart_out, ".");
	
	game_tick();

}


void game_init(){
	
	// set up interrupt
	TCCR3A |= (1 << COM3A1); //| (1 << FOC3A); // CTC, Top=OCR3A
	
	// prescaler=1024 and OCR3A=128 gives frequency of 60 Hz
	TCCR3B |= (1 << WGM32) | (1 << CS32) | (1 << CS30);
	OCR3A = 1000; //128; // 30 Hz
	
	TIMSK3 |= (1 << OCIE3A);
}


/** 
 * Run one iteration of game loop. 
 */
void game_tick(){
	
	game_update_ball_dropped();
	
	can_msg_t msg = can_read_buffer(0);
	can_msg_t ack_msg = {0,0,{}};
	
	while (msg.sid != can_INVALID){
		if (msg.sid == can_GAME_CMD){
			switch (msg.data[0]){
			case game_cmd_CHECK_BALL_DROP:
				//fprintf(&uart_out, "check ball\n");
				
				ack_msg.sid = can_GAME_DATA;
				ack_msg.data[0] = game_cmd_CHECK_BALL_DROP;
				ack_msg.data[1] = ball_dropped;
				ack_msg.length = 2;
				
				can_send(ack_msg,0);
			
			break;
			case game_cmd_CHANGE_SETTING:
				fprintf(&uart_out, "change setting\n");
				game_setting = msg.data[1];
			
			break;
			case game_cmd_ACTION:{
				//fprintf(&uart_out, "action %i\n", (int8_t)msg.data[1]);
				static BOOL fire = FALSE;
				static int8_t turn = PWM_TURN_MID;
				static int8_t motor = 0;
				
				switch (game_setting){
				case game_setting_STANDARD:
					motor = msg.data[game_setting_standard_motor];
					fire = msg.data[game_setting_standard_fire];
					turn = PWM_TURN_MID - (int8_t)msg.data[game_setting_standard_turn]/2;	
				break;
				case game_setting_ALTERNATIVE:
					motor = msg.data[game_setting_alternative_motor];
					fire = msg.data[game_setting_alternative_fire];
					turn = PWM_TURN_MID - (int8_t)msg.data[game_setting_alternative_turn]/2;
					fprintf(&uart_out, "turn: %u \n", (uint8_t)turn);
				break;
				}
				
				//fprintf(&uart_out, "(m,f,t) = (%u, %u, %i)\n", (uint8_t)motor, fire, turn);
				//motor_set_position(motor);
				
				
				pwm_set_duty(turn);
				
				if (fire){
					solenoid_trigger();
				}
				
				
			} break;
			default:
				fprintf(&uart_out, "ignored cmd %u\n", msg.data[0]);
			}
		} 
		else {
			fprintf(&uart_out, "non-command received %u\n", msg.sid);
		}
		
		msg = can_read_buffer(0);
	}
}

/*
void game_tick_playing(){
	// things that should be done every iteration
	game_update_ball_dropped();
	
	// communication with game_master on node1
	can_msg_t msg = can_read_buffer(0);
	
	while (msg.sid != can_INVALID){
		//fprintf(&uart_out, "...");
		
		switch (msg.sid){
		case can_GAME_CMD:
			//fprintf(&uart_out, "cmd...");
			game_handle_cmd(msg);
			
		break;
		case can_GAME_INFO:
			fprintf(&uart_out, "info\n");
			
		break;
		case can_GAME_DATA:
			fprintf(&uart_out, "data\n");
			
		break;
		case can_INVALID:
			fprintf(&uart_out, "invalid message");
		break;
		default:
			fprintf(&uart_out, "ERROR: invalid sid %u\n", msg.sid);
			
		break;
		}
		
		msg = can_read_buffer(0); // get new message untill buffer is empty
	}
	fprintf(&uart_out, ":");	
	
}
*/

void game_update_ball_dropped(){
	// check if ball has been dropped
	uint16_t light_level = ir_read();
	
    if (!ball_dropped && light_level < BALL_DROPPED_TRESHHOLD){
		fprintf(&uart_out, "ball dropped\n");
		ball_dropped = TRUE;
	}
}

/*
void game_handle_cmd(can_msg_t cmd_msg){
	
	can_msg_t msg = {0,0,{}};
	
	
	switch (cmd_msg.data[0]){
	case game_cmd_CHECK_BALL_DROP:
		//fprintf(&uart_out, "dropped: %u\n", ball_dropped);
		msg.sid = can_GAME_INFO;
		msg.length = 2;
		msg.data[0] = game_cmd_CHECK_BALL_DROP;
		msg.data[1] = ball_dropped;
		
		can_send(msg,0);
		
	break;
	case game_cmd_RESET_GAME:
		fprintf(&uart_out, "reset game\n");
		ball_dropped = FALSE;
		
		game_state = game_PLAYING;
		can_send(msg_reset_ack, 0);
		
	break;
	case game_cmd_CHANGE_SETTING:
		game_setting = cmd_msg.data[1];
		
	break;
	case game_cmd_ACTION:{
		BOOL fire = FALSE;
		int8_t turn = 50;
		int8_t motor = 0;
		
		fprintf(&uart_out, "action\n");
		switch (game_setting){
		case game_setting_STANDARD:
			fire  = cmd_msg.data[game_setting_standard_fire];
			turn  = cmd_msg.data[game_setting_standard_turn];
			motor = cmd_msg.data[game_setting_standard_motor];
		
		break;
		case game_setting_ALTERNATIVE:
			fire  = cmd_msg.data[game_setting_alternative_fire];
			turn  = cmd_msg.data[game_setting_alternative_turn];
			motor = cmd_msg.data[game_setting_alternative_motor];
			
		break;
		default:
			fprintf(&uart_out, "invalid input setting\n");
		break;
		}
		
		fprintf(&uart_out, "%u\t%i\t%u\n", fire, motor, turn);
		
	}
	break;
	default:
		fprintf(&uart_out, "unhandled command: %u\n", cmd_msg.data[0]);
	break;
	}
}

*/
