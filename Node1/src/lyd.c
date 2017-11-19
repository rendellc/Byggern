v/*
 * lyd.c
 *
 * Created: 19.11.2017 21.22.06
 *  Author: frode
 */ 
#include "lyd.h"

#define CHIP1R         0x1200
#define CHIP1D         0x1201
#define CHIP2R         0x1300
#define CHIP2D         0x1301

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
int8_t volumretning[] = {0, 0, 0, 0, 0, 0};
int8_t volumteller[] = {0, 0, 0, 0, 0, 0};

int8_t stoy[] = {-1, -1};	
uint8_t stoytoner[] = {0, 0};
int8_t stoyteller[] = {0, 0};
int8_t stoyretning[] = {0, 0};

void lyd_init(){
	CHIP1R = 7;
	CHIP1D = 0xFF;
	CHIP2R = 7;
	CHIP2D = 0xFF;
}
void lyd_tikk(){
	//
	// Sjekk for endring i volum
	//
	for(int i=0; i<6; i++)
	{
		volumteller[i]++;
		if(volumretning[i] < 0 && 0-volumteller[i] <= volumretning[i] && volum[i]>0)
		{
			volum[i]--;
			volumteller[i] = 0;
		}
		//
		//	ikke en case.
		/*
		else if(volumretning[i] > 0 && volumteller[i] >= volumretning[i] && volum[i]<15)
		{
			volum[i]++;
			volumteller[i] = 0;
		}
		*/
	}
	
	// Sjekk for endringer i støy
	for(int i=0; i<2; i++)
	{
		stoyteller[i]++;
		if(stoyretning[i] > 0 && stoyteller[i] >= stoyretning[i] && stoy[i]<31)
		{
			stoy[i]++;
			stoyteller[i] = 0;
		}
		else if(stoyretning[i] < 0 && 0-stoyteller[i] <= stoyretning[i] && stoy[i]>0)
		{
			stoy[i]--;
			stoyteller[i] = 0;
		}
	}
	
	
	//
	// Oppdater lydbrikken
	//
	//    Register i AY-3-8912 brikkene settes først, så settes data på dataporten.
	//    Alle lydrelaterte register oppdateres hvert tikk.
	//
	uint8_t kontroll1 = 0xFF;
	uint8_t kontroll2 = 0xFF;
	for(int i=0; i<3; i++)
	{
		if(toner[i] != -1)				// Sett toner
		{
			CHIP1R = i*2;
			CHIP1D = (frekvenstabell[toner[i]])&0xFF;
			CHIP1R = 1+(i*2);
			CHIP1D = (frekvenstabell[toner[i]]>>8)&0x0F;
			kontroll1 &= ~(1<<i);
		}
		if(toner[i+3] != -1)
		{
			CHIP2R = i*2;
			CHIP2D = (frekvenstabell[toner[i+3]])&0xFF;
			CHIP2R = 1+(i*2);
			CHIP2D = (frekvenstabell[toner[i+3]]>>8)&0x0F;
			kontroll2 &= ~(1<<i);
		}
		CHIP1R = 8+i;					// Sett volum
		CHIP1D = volum[i];
		CHIP2R = 8+i;
		CHIP2D = volum[i+3];
	}
	if(stoy[0] != -1)					// Sett støygenerator
	{
		CHIP1R = 6;
		CHIP1D = (stoytoner[0])&0x1F;
		kontroll1 &= ~(1<<(3+stoy[0]));
	}
	if(stoy[1] != -1)
	{
		CHIP2R = 6
		CHIP2D = (stoytoner[1])&0x1F;
		kontroll2 &= ~(1<<stoy[1]);		// (på chip to er kanal ID allerede lagt til 3 fra før)
	}
	CHIP1R = 7;							// Sett kontrollbits
	CHIP1D = kontroll1;
	CHIP2R = 7;
	CHIP2D = kontroll2;
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
	volumretning[id] = 0;
	volumteller[id] = 0;
}
	
void stopptone(uint8_t id){
	toner[id] = -1;
	volumretning[id] = -5;
	volumteller[id] = 0;
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
	volumretning[id] = 0;
	volumteller[ch] = 0;
	stoy[id] = ch;
	stoytoner[id] = tone;
	stoyretning[id] = glissando;
	stoyteller[id] = 0;
}

void stoppstoy(uint8_t id){
	if(id != -1 && toner[stoy[id]] == -1)
		{
			volum[stoy[id]] = 0;
			volumretning[id] = 0;
			volumteller[stoy[id]] = 0;
			stoy[id] = -1;
			stoytoner[id] = 0;
			stoyretning[id] = 0;
			stoyteller[id] = 0;
		}
}