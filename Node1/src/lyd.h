
#ifndef LYD_H_
#define LYD_H_

typedef enum {
	C,
	D,
	E,
	F,
	G,
	A,
	B,
} lyd_TONENAVN;

typedef enum {
	NONE,
	SHARP,
	FLAT
} lyd_FORTEGN;

void initlyd();
void lyd_tikk();
void spilltone(lyd_TONENAVN, uint8_t, lyd_FORTEGN, uint8_t);	// tonenavn, oktav (1-7), fortegn, id (0-5)
void stopptone(uint8_t);										// id (0-5)
void spillstoy(uint8_t, int8_t, uint8_t);					// tone (0-31), glissando (+/- 64), id (0-1)
void stoppstoy(uint8_t);									// id (0-1)

#endif /* LYD_H_ */
