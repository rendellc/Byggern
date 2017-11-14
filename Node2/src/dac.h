/*!@file
 * Interface of external digital to analog interface using I2C.
 */

#ifndef DAC_H_
#define DAC_H_

#define DAC_RESET		0x10
#define DAC_POWERDOWN	0x08
#define DAC_CMD_OUTPUT	0x00

/**
 * Initialize DAC and I2C on PortD pin 0 and 1. 
 */
void dac_init(void);

/**
 * Command DAC to output specified voltage. 
 * @param[in] voltage 
 */
void dac_output(uint8_t voltage);


#endif /* DAC_H_ */