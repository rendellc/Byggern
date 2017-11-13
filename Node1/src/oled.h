/**@file
 * Interface of oled driver with compile-time
 * selectable fonts and autorefresh. 
 * \todo Remove most of these functions and reduce the 
         size of the interface. 
 */

#ifndef OLED_H_
#define OLED_H_
#include <stdint.h>
#include <stdio.h>

extern FILE oled_out;

#define OLED_C_ADR 0x1000       /*!< Oled command address */
#define OLED_D_ADR 0x1001       /*!< Oled data address */

#define OLED_REFRESH_RATE 60    /*!< Refresh rate in Hz. Unimplemented */


/// Initialize oled, clear screen, set cursor to home, and enable autorefresh. 
void oled_init();

/// Initialize autorefresh to 60 Hz
void oled_autorefresh_init();

/// Reset oled display by initializing, clearing screen and setting cursor to home.
void oled_reset();

/// Update display. 
void oled_update();

/// set cursor to top left
void oled_home();

/// Place cursor to start of line.
void oled_cartridgereturn();

/// Place cursor at start of nect line. 
void oled_newline();

/**
 * Move cursor to specified line. No input validation is performed.
 * @param[in] y The line to move cursor to. 
 */
void oled_goto_line(uint8_t y);

/**
 * Move cursor to specified column. No input validation is performed.
 * @param[in] x The column to move cursor to. 
 */
void oled_goto_column(uint8_t x);

/// Move cursor to next position. Wraps around at right and bottom of the screen.
void oled_goto_nextpos();

/** 
 * Place cursor at specied row and column.
 * @param[in] row The row to place cursor at.
 * @param[in] column The column to place cursor at. 
 */
void oled_pos(uint8_t row, uint8_t column);

/**
 * Emulate regular stdio printf with oled.
 */
void oled_printf(const char* msg);

/**
 * Emulate regular stdio putchar with oled.
 */
int  oled_putchar(char chr);

/// Place chr in buffer with correct numbering according to font indices
void oled_buffchar(char chr);

/**
 * Fill entire display with one value. 
 * @param[in] val Value to place fill screen with. 
 *
 * \example val = 0 will clear the entire screen.
 */
void oled_fill(uint8_t val);


#endif /* OLED_H_ */

