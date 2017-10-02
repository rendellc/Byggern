/*
 * uart.h
 *
 * Created: 01.09.2017 10:33:50
 *  Author: rendellc
 */ 


#ifndef UART_H_
#define UART_H_
#include <stdio.h>

extern FILE uart_out;
extern FILE uart_in;

void uart_init();

int uart_send(unsigned char msg);

unsigned char uart_recv();

#endif /* UART_H_ */