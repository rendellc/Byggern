
#ifndef LYD_H_
#define LYD_H_

#include <stdlib.h>
#include <stdio.h>

typedef enum {
	tone_C,
	tone_D,
	tone_E,
	tone_F,
	tone_G,
	tone_A,
	tone_B,
} lyd_TONENAVN;

typedef enum {
	tone_NONE,
	tone_SHARP,
	tone_FLAT
} lyd_FORTEGN;

void lyd_init();
void lyd_tikk();
void spilltone(lyd_TONENAVN tonenavn, uint8_t oktav, lyd_FORTEGN fortegn, uint8_t id);	// tonenavn, oktav (1-7), fortegn, id (0-5)
void stopptone(uint8_t id);										// id (0-5)
void spillstoy(uint8_t tone, int8_t glissando, uint8_t id);					// tone (0-31), glissando (+/- 64), id (0-1)
void stoppstoy(uint8_t id);									// id (0-1)

#endif /* LYD_H_ */
