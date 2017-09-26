/*
 * oled.h
 *
 * Created: 22.09.2017 08:49:44
 *  Author: rendellc
 */ 


#ifndef OLED_H_
#define OLED_H_
#include <stdint.h>

#define OLED_WIDTH 128
#define OLED_PAGES 8
#define OLED_HEIGHT OLED_PAGES*8 // 8 pages tall

#define OLED_C_ADR 0x1000
#define OLED_D_ADR 0x1001

void oled_init();

void oled_reset();

void oled_home();
void oled_cartridgereturn();
void oled_newline();

void oled_goto_line(uint8_t);

void oled_goto_column(uint8_t);

void oled_clear_line(uint8_t);

void oled_pos(uint8_t, uint8_t);

void oled_fill_line(uint8_t, uint8_t);

void oled_write_dot(uint8_t, uint8_t, uint8_t);

void oled_update();

void oled_print(char*);


#endif /* OLED_H_ */