

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
	if (!++interrupt_counter){
		//fprintf(&uart_out, ".");
	}
	
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
	
	// Handle all messages on the can buffer
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
					turn = PWM_TURN_MID + (int8_t)msg.data[game_setting_standard_turn]/2;
					
					//fprintf(&uart_out, "motor %u\n", (uint8_t)motor);
					
					motor_set_position((uint8_t)motor);
					
					
				break;
				case game_setting_ALTERNATIVE:
					motor = msg.data[game_setting_alternative_motor];
					fire = msg.data[game_setting_alternative_fire];
					turn = PWM_TURN_MID - (int8_t)(msg.data[game_setting_alternative_turn]-128)*50/128;
				break;
				}
				
				//fprintf(&uart_out, "%i\t%i\t%u\n", (int8_t)turn, (uint8_t)motor, fire);
				
				//fprintf(&uart_out, "motor: %i\n", motor);
				
				//motor_set_speed(motor);
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


void game_update_ball_dropped(){
	// check if ball has been dropped
	uint16_t light_level = ir_read();
	
    if (!ball_dropped && light_level < BALL_DROPPED_TRESHHOLD){
		fprintf(&uart_out, "ball dropped\n");
		ball_dropped = TRUE;
	}
}
