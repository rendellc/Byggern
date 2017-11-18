/*
 * game_slave.h
 *
 * Created: 10.11.2017 13:03:33
 *  Author: rendellc
 */ 

#ifndef GAME_SLAVE_H_
#define GAME_SLAVE_H_

void game_init();

void game_tick();
void game_tick_playing();

void game_handle_cmd(can_msg_t cmd_msg);

void game_update_ball_dropped();

#endif /* GAME_H_ */
