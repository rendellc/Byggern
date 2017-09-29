/*
 * oled.h
 *
 * Created: 22.09.2017 08:49:44
 *  Author: rendellc
 */ 


#ifndef OLED_H_
#define OLED_H_
#include <stdint.h>
#include <stdio.h>

extern FILE oled_out;

#define OLED_C_ADR 0x1000
#define OLED_D_ADR 0x1001

#define OLED_REFRESH_RATE 60 // Hz

void oled_init();
void oled_reset();

void oled_home();
void oled_cartridgereturn();
void oled_newline();

void oled_goto_nextpos();
void oled_goto_line(uint8_t line);
void oled_goto_column(uint8_t col);
void oled_clear_line(uint8_t line);

void oled_update();

////
void oled_init();
void oled_reset();

void oled_home();
void oled_cartridgereturn();
void oled_newline();

void oled_goto_line(uint8_t y);
void oled_goto_column(uint8_t x);
void oled_goto_nextpos();

void oled_pos(uint8_t row, uint8_t column);

void oled_printf(const char* msg);
int  oled_putchar(char chr);
void oled_buffchar(char chr);

void oled_clear_line(uint8_t line);
void oled_fill(uint8_t val);
void oled_write_dot(uint8_t line, uint8_t column, uint8_t val);



void oled_update();


#endif /* OLED_H_ */

