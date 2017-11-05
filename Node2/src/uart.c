/*
 * uart.c
 *
 * Created: 01.09.2017 10:33:35
 *  Author: rendellc
 */ 
#include "global_declarations.h"
#include "uart.h"

#include <avr/io.h>
#include <avr/interrupt.h>

FILE uart_out = FDEV_SETUP_STREAM(uart_send, NULL, _FDEV_SETUP_WRITE);
FILE uart_in = FDEV_SETUP_STREAM(NULL, uart_recv, _FDEV_SETUP_READ);

#define BAUD 9600UL
#define BUFFER_MAX 128

volatile char recv_buffer[BUFFER_MAX];
volatile int recvhead = 0;
volatile int recvtail = 0;

ISR(USART0_RX_vect){
	cli();
	char input = UDR0;
	if((recvhead+1)%BUFFER_MAX != recvtail){		// Sjekk at bufferen ikke er full
		recv_buffer[recvhead] = input;				// If so, legg inn byte
		recvhead = (recvhead+1)%BUFFER_MAX;
	}
	sei();
}

ISR(USART0_TX_vect){
	
}


void uart_init(void){
	cli();
	
	// set baud rate
	int timerval = (F_CPU/(16*BAUD) - 1);
	UBRR0L = timerval&0xFF;
	UBRR0H = (timerval>>8)&0x0F;
	
	// enable interupts on Rx & Tx, and enable Rx & Tx
	UCSR0B |= (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	
	// enable async mode
	UCSR0C &= ~(1 << UMSEL01); // changed from UMSEL0
	
	// parity disable
	UCSR0C &= ~((1 << UPM00) | (1 << UPM10));
	
	// stop bit to 1
	UCSR0C &= ~(1 << USBS0);

	sei();
}

int uart_send(unsigned char msg){
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = msg;
	return 0;
}

unsigned char uart_recv(void){
	char returnval = 0;
	if(recvhead != recvtail)
	{
		returnval = recv_buffer[recvtail++];
		recvtail = recvtail%BUFFER_MAX;
	}
	return returnval;
}

#undef BAUD
