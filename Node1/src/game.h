/*
 * game.h
 *
 * Created: 10.11.2017 13:03:33
 *  Author: rendellc
 */ 


#ifndef GAME_H_
#define GAME_H_




//uint8_t scorekeeping(void);

//uint8_t average_ir_read(void);

void game_init();

void game_tick();

void game_start();

void game_set_state(game_state_t state);

void game_tick_menu();
void game_tick_playing();
void game_tick_paused();


void game_setting_standard();
void game_setting_alternative();

#endif /* GAME_H_ */