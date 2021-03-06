/**@file
 * Implementation of oled driver with compile-time
 * selectable fonts and autorefresh. 
 */

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


#define OLED_HEIGHT OLED_PAGES*8 /*!< Screen is 8 pages tall */
#define SPACING 1				 /*!< Spacing between characters when writing to display */

/// \note buffchr will have bugs if FONT_WIDTH changes during execution

const uint8_t* FONT			= (const uint8_t*) font4; /*!< Font selection */

/// Backbuffer for the oled display
volatile uint8_t* buffer	= (uint8_t*)OLED_BUFFER_ADR;


uint8_t xpos = 0; /*!< Horisontal cursor */
uint8_t ypos = 0; /*!< Vertical cursor */

/// Make oled into a output file stream
FILE oled_out = FDEV_SETUP_STREAM(oled_putchar, NULL, _FDEV_SETUP_WRITE);


/// Interrupt vector which refreshes the display
ISR(TIMER0_OVF_vect){
	oled_update();
}

/// Initialize autorefresh by setting up interrupt.
void oled_autorefresh_init(){
	// set up interrupt timer
	TIMSK |= (1<<TOIE0);
	
	/// \todo Now 60 Hz is hardcoded and dependent on F_CPU. Change to use the define in header file.

	// set frequency to ca 60 Hz
	TCCR0 |= (1<<CS02 | 1<<CS00) & ~(1 << CS01);
	OCR0 = 39;
}

/// Write command to oled
void write_c(uint8_t cmd){
	_delay_us(1);
	*((uint8_t *)OLED_C_ADR) = cmd;
}

/// Write data to oled
void write_d(uint8_t data){
	_delay_us(1); // delay may be increased to 1000 to make debug easier, works without any delay
	*((uint8_t *)OLED_D_ADR) = data;
}

/// Initialize oled, clear screen, set cursor to home, and enable autorefresh. 
void oled_init(){
	
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

/// Reset oled display by initializing, clearing screen and setting cursor to home.
void oled_reset(){
	/// \note This is exexctly the same as oled_init except oled_fill and oled_home are called twice
	oled_init(); 
	oled_fill(0);
	oled_home();
}

/// Set cursor to top left of display.
void oled_home(){
	xpos = 0;
	ypos = 0;
}

/// Place cursor to start of line.
void oled_cartridgereturn(){
	xpos = 0;
}

/// Place cursor at start of nect line. 
void oled_newline(){
	oled_cartridgereturn();
	oled_goto_line((ypos+1) % OLED_PAGES);
}

/**
 * Move cursor to specified line. No input validation is performed.
 * @param[in] y The line to move cursor to. 
 */
void oled_goto_line(uint8_t y){
	ypos = y;
}

/**
 * Move cursor to specified column. No input validation is performed.
 * @param[in] x The column to move cursor to. 
 */
void oled_goto_column(uint8_t x){
	xpos = x;
}

/// Move cursor to next position. Wraps around at right and bottom of the screen.
void oled_goto_nextpos(){
	// wrap around is applied to both xpos and ypos
	
	// increment xpos and ypos
	xpos = (xpos + 1) % OLED_WIDTH;
	
	ypos = (ypos + (xpos==0)) % OLED_PAGES; // increments ypos if xpos wrapped around to zero
	
}

/** Place cursor at specied row and column.
 * @param[in] row The row to place cursor at.
 * @param[in] column The column to place cursor at. 
 */
void oled_pos(uint8_t row, uint8_t column){
	oled_goto_line(row);
	oled_goto_column(column);
}


// *** TEXT OUTPUT RELATED ***

/// Place chr in buffer with correct numbering according to font indices
void oled_buffchar(char chr){
	if(chr >= ASCII_OFFSET && xpos < OLED_WIDTH && ypos < OLED_PAGES) {
		
		if (xpos + FONT_WIDTH >= OLED_WIDTH){
			oled_newline();	
			
			/*
			// --- works for font4 but not font8 ---
			// - purpose: prevent word crossover between lines
			// - how: writes last word to next line before placing chr in buffer
			// - bugs: extra gets added
			
			int8_t x = (int8_t)xpos - SPACING - (int8_t)FONT_WIDTH; // need signed values for error checking

			uint8_t zeroCounter = 0;
			while (zeroCounter != FONT_WIDTH && x >= 0){
				for (uint8_t i = 0; i<FONT_WIDTH; ++i){
					zeroCounter = zeroCounter + (buffer[ypos*OLED_WIDTH + x + i] == 0x00);
				}
				
				if (zeroCounter == FONT_WIDTH){ // chr consisting only of zeroes, ie a space ' '
					// push contents from this line of the buffer to the next line
					for (int seg = x+1, i=-FONT_WIDTH; seg < OLED_WIDTH; ++seg, ++i){
						buffer[((ypos + 1)%OLED_PAGES)*OLED_WIDTH + i] = buffer[ypos*OLED_WIDTH + seg];
						buffer[ypos*OLED_WIDTH + seg] = 0x00;
						xpos = i;
					}
					ypos = (ypos + 1)%OLED_PAGES;
					++xpos;
				} else {
					zeroCounter = 0;
					x =  x - SPACING - FONT_WIDTH;
				}
			}
			if (x < 0){
				oled_cartridgereturn();
			}
			*/		
		}

		// write char
		for (uint8_t seg = 0; seg < FONT_WIDTH; ++seg){
			buffer[ypos*OLED_WIDTH + xpos] = pgm_read_word(&FONT[(chr - ASCII_OFFSET)*FONT_WIDTH + seg]);
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


/**
 * Emulate regular stdio printf with oled.
 */
void oled_printf(const char* msg){
	for(int i=0; msg[i] != '\0'; i++){
		oled_buffchar(msg[i]);
	}
}

/**
 * Emulate regular stdio putchar with oled.
 */
int oled_putchar(char chr){
	oled_buffchar(chr);
	return 0;
}

// *** DRAWING RELATED ***
/**
 * Fill entire display with one value. 
 * @param[in] val Value to place fill screen with. 
 *
 * Example: val = 0 will clear the entire screen.
 */
void oled_fill(uint8_t val){    
	write_c(SET_COL_ADR);  write_c(0x00); write_c(OLED_WIDTH-1);
	write_c(SET_PAGE_ADR); write_c(0x00); write_c(OLED_PAGES-1);
	
	for (int page = 0; page<OLED_PAGES; ++page){
		for (int seg = 0; seg<OLED_WIDTH; ++seg){
			buffer[page*OLED_WIDTH + seg] = val;
		}
	}
}

/**
 * Place raw data on screen at cursor position. Doesn't increment cursor.
 */
void oled_putraw(uint8_t raw, uint8_t x, uint8_t y){
	buffer[y*OLED_WIDTH + x] = raw;
}


//////////////////////////////////////////////////////////////////////////

/**
 * Go sequentially through the buffer and write data to the display. 
 */
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
