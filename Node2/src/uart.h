/*!@file
 * UART implementation for atmega2560
 * Hardcoded to use 9600 buad
 * 8n1 = 8 data-bits, no parity, 1 stop bit
 * Uses interrupts for recieving and busy wait for sending. 
 * Recieved messages go into an internal buffer untill mcu request them.
 * \todo {change to use uint8_t where it makes sense}
 * \todo {implement "buffer full" function}
 */

#ifndef UART_H_
#define UART_H_
#include <stdio.h>


// fprintf and file stream related
extern FILE uart_out;
extern FILE uart_in;

/// Initialize uart 
void uart_init(void);

/**
 * Send data
 * @param[in] msg one byte of data to send
 * @retval 0 always.
 */
int uart_send(unsigned char msg);

/**
 * Read one byte from the Rx buffer. 
 * \retval data or zero if buffer is empty.
 * \todo {implement a better way to detect if buffer is empty}
 * \test {has this been tested yet?}
 */
unsigned char uart_recv(void);

#endif /* UART_H_ */