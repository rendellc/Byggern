/*
 * lyd.c
 *
 * Created: 19.11.2017 21.22.06
 *  Author: frode
 */ 
#include "lyd.h"

// Fra B1 til og med C8
uint16_t frekvenstabell[] = {4050,
	3822,3608,3405,3214,3034,2863,2703,2551,2408,2273,2145,2025,
	1911,1804,1703,1607,1517,1432,1351,1276,1204,1136,1073,1012,
	956,902,851,804,758,716,676,638,602,568,536,506,
	478,451,426,402,379,358,338,319,301,284,268,253,
	239,225,213,201,190,179,169,159,150,142,134,127,
	119,113,106,100,95,89,84,80,75,71,67,63,
	60};

int8_t toner[] = {-1, -1, -1, -1, -1, -1};
uint8_t volum[] = {0, 0, 0, 0, 0, 0};
int8_t volumteller[] = {0, 0, 0, 0, 0, 0};

int8_t stoy[] = {-1, -1};	
int8_t stoytoner[] = {-1, -1};
int8_t stoyteller[] = {0, 0};

void lyd_init(){
	// Sett opp lydbrikkene
}
void lyd_tikk(){
	// oppdater alle registere i lydbrikken
}

void spilltone(lyd_TONENAVN tonenavn, uint8_t oktav, lyd_FORTEGN fortegn, uint8_t id){
	int8_t tone = 1;		// Første tonen i lista er B, så C (oktavreferanse) er en semitone over.
	switch (tonenavn){		// Pek på riktig tone innad i oktav
		case tone_B:
			tone += 2;
		case tone_A:
			tone += 2;
		case tone_G:
			tone += 2;
		case tone_F:
			tone += 1;
		case tone_E:
			tone += 2;
		case tone_D:
			tone += 2;
		case tone_C:
		break;
		default:
			break;
	}
	tone += (oktav-2)*12;	// Pek på riktig oktav
	switch (fortegn){		// Juster for fortegn
		case tone_FLAT:
			tone--;
			break;
		case tone_SHARP:
			tone++;
			break;
		default:
			break;
	}
	while(tone<0){tone += 12;}		// Juster oktav til å være innafor tabellen
	while(tone>73){tone -= 12;}
	
	toner[id] = tone;				// Husk at tonen spilles
	volum[id] = 15;
	volumteller[id] = 0;
}
	
void stopptone(uint8_t id){
	toner[id] = -1;
	volumteller[id] = -5;
}

void spillstoy(uint8_t tone, int8_t glissando, uint8_t id){
	int ch = id*3;
	for(int i=ch; i<ch+3; i++)
	{
		if(toner[i] == -1)
		{
			ch = i;
			break;		
		}
	}
	toner[ch] = -1;
	volum[ch] = 15;
	volumteller[ch] = 0;
	stoy[id] = ch;
	stoytoner[id] = tone;
	stoyteller[id] = glissando;
}

void stoppstoy(uint8_t id){
	if(id != -1 && toner[stoy[id]] == -1)
		{
			volum[stoy[id]] = 0;
			volumteller[stoy[id]] = 0;
			stoy[id] = -1;
			stoytoner[id] = -1;
			stoyteller[id] = 0;
		}
}