/*!@file
 * Declarations and includes that are usesful in most of Node1.
 * Defines F_CPU and should be the first include.
 */ 


#ifndef GLOBAL_DECLARATIONS_H_
#define GLOBAL_DECLARATIONS_H_

#define F_CPU 4915200UL


#include <avr/io.h>
//#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../Common/common.h"

/// ---- MEMORY ----
#define ADC_ADR         0x1100
#define OLED_BUFFER_ADR 0x1400
#define RAM_ADR         0x1800

#define RAM_SIZE        0x800


#endif /* GLOBAL_DECLARATIONS_H_ */