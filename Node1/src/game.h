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

void game_init(void);

void game_tick(void);

void game_start(void);

void game_set_state(game_state_t state);

void game_tick_menu(void);
void game_tick_playing(void);
void game_tick_playing_oled(void);
void game_tick_paused(void);
void game_tick_freeplay(void);
void game_tick_game_over(void);

void game_setting_standard(void);
void game_setting_alternative(void);

void game_difficulty_standard(void);
void game_difficulty_impossible(void);

void game_state_freeplay(void);
#endif /* GAME_H_ */