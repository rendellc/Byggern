/*
 * ir.h
 *
 * Created: 27.10.2017 14:02:37
 *  Author: rendellc
 */ 


#ifndef IR_H_
#define IR_H_

#include <stdint.h>

/// Initialising of ADC for reading ir diode
void ir_init(void);

///  Read the ADC for ir value
uint16_t ir_read(void);

#endif /* IR_H_ */