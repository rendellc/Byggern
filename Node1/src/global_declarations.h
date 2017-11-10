/*!@file
 * Declrations and includes that are usesful in most of Node1.
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
#define ADC_ADR         0x1100 /*!< IO-memory for analog to digital converter*/
#define OLED_BUFFER_ADR 0x1400 /*!< Memory address for OLED buffer*/
#define RAM_ADR         0x1800 /*!< Address of external RAM*/

#define RAM_SIZE        0x800  /*!< Size of available external RAM */

#endif /* GLOBAL_DECLARATIONS_H_ */
