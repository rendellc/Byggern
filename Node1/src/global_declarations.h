/*! @file
 * global_declarations.h
 *
 * Created: 01.09.2017 12:53:09
 *  Author: gr 43
 */ 


#ifndef GLOBAL_DECLARATIONS_H_
#define GLOBAL_DECLARATIONS_H_

#define F_CPU 4915200UL

//const void* NULL_PTR;
#include <avr/io.h>
//#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../Common/common.h"

/// ---- MEMORY ----
#define ADC_ADR         0x1100
#define OLED_BUFFER_ADR 0x1400
#define RAM_ADR         0x1800

#define RAM_SIZE        0x800


#endif /* GLOBAL_DECLARATIONS_H_ */