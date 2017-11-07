#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "oled.h"
#include "fonts.h"
#include "global_declarations.h"
#include "oled_cmds.h"
#include <util/delay.h>

#define OLED_WIDTH 128
#define OLED_PAGES 8
#define OLED_HEIGHT OLED_PAGES*8 // 8 pages tall
#define SPACING 1

const uint8_t* font			= (const uint8_t*) font4;
const uint8_t  fontheight	= 6;		// buffchr will have bugs if fontwidth changes during execution
const uint8_t  fontwidth	= 4;

volatile uint8_t* buffer	= (uint8_t*)OLED_BUFFER_ADR;

uint8_t xpos = 0;
uint8_t ypos = 0;

FILE oled_out = FDEV_SETUP_STREAM(oled_putchar, NULL, _FDEV_SETUP_WRITE);

ISR(TIMER0_OVF_vect){
	oled_update();
}

void oled_autorefresh_init(){
	// set up interrupt timer
	TIMSK |= (1<<TOIE0);;
	
	// set frequency to ca 60 Hz
	TCCR0 |= (1<<CS02 | 1<<CS00) & ~(1 << CS01);
	OCR0 = 39;
}

void write_c(uint8_t cmd){
	_delay_us(1);
	*((uint8_t *)OLED_C_ADR) = cmd;
}

void write_d(uint8_t data){
	_delay_us(1); // delay may be increased to 1000 to make debug easier, works without any delay
	*((uint8_t *)OLED_D_ADR) = data;
}

void oled_init(){
	cli();
	
	write_c(SET_DISP_OFF);
	write_c(SET_SEG_REMAP);
	write_c(SET_COM_PINS_HW_CONFIG);
	write_c(0x12);
	write_c(SET_COM_SCAN_REMAP);
	write_c(SET_MULTPLX_RATIO);
	write_c(0x3f);
	write_c(SET_DISP_CLK_DIV_FOSC);
	write_c(0x80);
	write_c(SET_CONTRAST_CTRL);
	write_c(0x50);
	write_c(SET_PRE_CHRG_PERIOD);
	write_c(0x21);
	write_c(SET_MEM_ADR_MODE);
	write_c(0x00);
	write_c(SET_VCOMH_DES_LEVEL);
	write_c(0x30);
	write_c(SET_INTERNAL_IREF);
	write_c(0x00);
	write_c(SET_FOLLOW_RAM);
	write_c(SET_NORMAL_DISP);
	write_c(SET_DISP_ON);

	
	oled_fill(0);
	oled_home();
	
	oled_autorefresh_init();
}

void oled_reset(){
	oled_init();
	oled_fill(0);
	oled_home();
}

void oled_home(){
	xpos = 0;
	ypos = 0;
}

void oled_cartridgereturn(){
	xpos = 0;
}

void oled_newline(){
	oled_cartridgereturn();
	oled_goto_line((ypos+1) % OLED_PAGES);
}

void oled_goto_line(uint8_t y){
	ypos = y;
}

void oled_goto_column(uint8_t x){
	xpos = x;
}

void oled_goto_nextpos(){
	// wrap around is applied to both xpos and ypos
	
	// increment xpos and ypos
	xpos = (xpos + 1) % OLED_WIDTH;
	
	ypos = (ypos + (xpos==0)) % OLED_PAGES; // increments ypos if xpos wrapped around to zero
	
}

void oled_pos(uint8_t row, uint8_t column){
	oled_goto_line(row);
	oled_goto_column(column);
}


// *** TEXT OUTPUT RELATED ***

// Place chr in buffer with correct numbering according to font indices
void oled_buffchar(char chr){
	if(chr >= ASCII_OFFSET && xpos < OLED_WIDTH && ypos < OLED_PAGES) {
		
		if (xpos + fontwidth >= OLED_WIDTH){
			oled_newline();	
			
			/*
			// --- works for font4 but not font8 ---
			// - purpose: prevent word crossover between lines
			// - how: writes last word to next line before placing chr in buffer
			// - bugs: extra gets added
			
			int8_t x = (int8_t)xpos - SPACING - (int8_t)fontwidth; // need signed values for error checking

			uint8_t zeroCounter = 0;
			while (zeroCounter != fontwidth && x >= 0){
				for (uint8_t i = 0; i<fontwidth; ++i){
					zeroCounter = zeroCounter + (buffer[ypos*OLED_WIDTH + x + i] == 0x00);
				}
				
				if (zeroCounter == fontwidth){ // chr consisting only of zeroes, ie a space ' '
					// push contents from this line of the buffer to the next line
					for (int seg = x+1, i=-fontwidth; seg < OLED_WIDTH; ++seg, ++i){
						buffer[((ypos + 1)%OLED_PAGES)*OLED_WIDTH + i] = buffer[ypos*OLED_WIDTH + seg];
						buffer[ypos*OLED_WIDTH + seg] = 0x00;
						xpos = i;
					}
					ypos = (ypos + 1)%OLED_PAGES;
					++xpos;
				} else {
					zeroCounter = 0;
					x =  x - SPACING - fontwidth;
				}
			}
			if (x < 0){
				oled_cartridgereturn();
			}
			*/		
		}

		// write char
		for (uint8_t seg = 0; seg < fontwidth; ++seg){
			buffer[ypos*OLED_WIDTH + xpos] = pgm_read_word(&font[(chr - ASCII_OFFSET)*fontwidth + seg]);
			oled_goto_nextpos();
		}
		
		// write spacing
		for (uint8_t _ = 0; _ < SPACING; ++_){
			buffer[ypos*OLED_WIDTH + xpos] = 0x00;
			oled_goto_nextpos();
		}
	} else if (chr == '\n'){
		oled_newline();
	} else if (chr == '\r'){
		oled_cartridgereturn();
	}
}

void oled_printf(const char* msg){
	for(int i=0; msg[i] != '\0'; i++){
		oled_buffchar(msg[i]);
	}
}

int oled_putchar(char chr){
	oled_buffchar(chr);
	return 0;
}

// *** DRAWING RELATED ***
void oled_fill(uint8_t val){
	write_c(SET_COL_ADR);  write_c(0x00); write_c(OLED_WIDTH-1);
	write_c(SET_PAGE_ADR); write_c(0x00); write_c(OLED_PAGES-1);
	
	for (int page = 0; page<OLED_PAGES; ++page){
		for (int seg = 0; seg<OLED_WIDTH; ++seg){
			buffer[page*OLED_WIDTH + seg] = val;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void oled_update(){
	// write content of buffer to oled
	
	write_c(SET_COL_ADR); write_c(0x00); write_c(OLED_WIDTH-1);
	write_c(SET_PAGE_ADR);write_c(0x00); write_c(OLED_PAGES-1);
	
	write_c(SET_LOW_COL_ADR  | 0x00);
	write_c(SET_HIGH_COL_ADR | 0x00);
	
	for (int page = 0; page<OLED_PAGES; ++page){
		for (int seg = 0; seg<OLED_WIDTH; ++seg){
			write_d(buffer[page*OLED_WIDTH + seg]);
		}
	}
}
