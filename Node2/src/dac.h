/*
 * dac.h
 *
 * Created: 03.11.2017 14:27:27
 *  Author: rendellc
 */ 


#ifndef DAC_H_
#define DAC_H_

#define DAC_RESET		0x10
#define DAC_POWERDOWN	0x08
#define DAC_CMD_OUTPUT	0x00

void dac_init(void);


void dac_output(uint8_t voltagePercent);




#endif /* DAC_H_ */