

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


#define BALL_PICKED_UP_THRESHHOLD	110 // needs adjustment

#define TURN_MID 50 // value to center servo

/// variable for keeping track of ball
static BOOL ball_dropped = FALSE;

/// game state
static game_setting_t game_setting = game_setting_STANDARD;

/// calibration of ball drop detection. set to proper values by game_init
static uint8_t ball_dropped_threshold = 40;
static uint8_t ball_picked_up_threshold = 90;

// Declare reset acknowledgment message, initialized in init
//static const can_msg_t msg_reset_ack = {can_GAME_DATA, 2, {game_cmd_SLAVE_ACK, game_cmd_RESET_GAME}};
//static const can_msg_t msg_start_ack = {can_GAME_DATA, 2, {game_cmd_SLAVE_ACK, game_cmd_RESET_GAME}};

static uint8_t interrupt_counter = 0;
ISR(TIMER3_COMPA_vect){
	if (!++interrupt_counter){
		fprintf(&uart_out, ".");
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
	
	
	// calibrate ir
	uint16_t ir_sum = 0;
	for (uint8_t i = -1; i; --i){
		ir_sum += ir_read();
	}
	uint16_t ir_avg = ir_sum/256;
	
	ball_dropped_threshold = 4*ir_avg/8;
	//ball_picked_up_threshold = 3*ir_avg/4;
	
	fprintf(&uart_out, "drop(%u)\n", ball_dropped_threshold);//, ball_picked_up_threshold);
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
			switch (msg.cmd_specifier){
			case game_cmd_CHECK_BALL_DROP:		
				ack_msg.sid = can_GAME_DATA;
				ack_msg.length = 2;
				ack_msg.cmd_specifier = game_cmd_CHECK_BALL_DROP;
				ack_msg.cmd_data[0] = ball_dropped;
				
				can_send(ack_msg,0);
			
			break;
			case game_cmd_CHANGE_SETTING:
				fprintf(&uart_out, "change setting\n");
				game_setting = msg.cmd_data[0];
			
			break;
			case game_cmd_ACTION:{
				//fprintf(&uart_out, "action %i\n", (int8_t)msg.data[1]);
				uint8_t turn = 0;
				int8_t motor = 0;
				if (game_setting == game_setting_STANDARD){
					motor = msg.cmd_data[game_slider_index];
					turn = TURN_MID + (int8_t)msg.cmd_data[game_joy_x_index]/2;
					motor_set_position((uint8_t)motor);
				}
				else if (game_setting == game_setting_ALTERNATIVE){	
					motor = msg.cmd_data[game_slider_index];
					turn = TURN_MID + (int8_t)msg.cmd_data[game_joy_x_index]/2;
					motor_set_position((uint8_t)motor);
					/*
					//fprintf(&uart_out, "turn: %i\n", ((int8_t)msg.cmd_data[game_slider_index]+128)/2);
					fprintf(&uart_out, "motor: %i\n", (int8_t)msg.cmd_data[game_joy_x_index]/2);
					motor = (int8_t)msg.cmd_data[game_joy_x_index]/2;
					turn = (uint8_t)msg.cmd_data[game_slider_index];
					motor_set_speed(motor);
					*/
				}
				
				BOOL fire = msg.cmd_data[game_button_index];
				
				//fprintf(&uart_out, "%i\t%i\t%u\n", (int8_t)turn, (uint8_t)motor, fire);
				
				motor_set_position((uint8_t)motor);
				pwm_set_duty(turn);
				if (fire){
					solenoid_trigger();
				}
				
			} break;
			case game_cmd_RESET_GAME:
				fprintf(&uart_out, "game reset\n");
				ball_dropped = FALSE;
				_delay_us(100);
			break;
			case game_cmd_CHANGE_DIFFICULTY:
				
				switch (msg.cmd_data[0]){
				case game_cmd_difficulty_STANDARD:
					motor_tune_regulator(2,1); 
					fprintf(&uart_out, "standard difficulty\n");  
				break;
				case game_cmd_difficulty_IMPOSSIBLE: 
					motor_tune_regulator(1,2); 
					fprintf(&uart_out, "impossible difficulty\n");
				break;
				}			
				
			break;
			default:
				fprintf(&uart_out, "ignored cmd %u\n", msg.cmd_specifier);
			}
		} 
		else {
			fprintf(&uart_out, "non-command received %u\n", msg.sid);
		}
		
		msg = can_read_buffer(0);
	}
	
	//fprintf(&uart_out, "buffer cleared\n");
}


void game_update_ball_dropped(){
	// check if ball has been dropped
	uint16_t light_level = ir_read();
	
    if (!ball_dropped && light_level < ball_dropped_threshold){
		fprintf(&uart_out, "ball dropped\n");
		ball_dropped = TRUE;
	}
}

