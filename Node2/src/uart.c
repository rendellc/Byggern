/*!@file
 * Implement uart on atmega2560. \n
 * Uses interrupt for recieving and busy wait for sending. 
 * \todo Convert to use uint8_t
 */ 

#include "global_declarations.h"

#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/// Enable use of fprintf(&uart_out,...)
FILE uart_out = FDEV_SETUP_STREAM(uart_send, NULL, _FDEV_SETUP_WRITE);
/// Create input stream for uart. Unused. 
FILE uart_in = FDEV_SETUP_STREAM(NULL, uart_recv, _FDEV_SETUP_READ);

#define BAUD 9600UL 	/*!< BAUD rate of UART*/
#define BUFFER_MAX 128	/*!< Size of Rx buffer*/

volatile char recv_buffer[BUFFER_MAX]; 	/*!< Buffer for storing recieved data*/
volatile int recvhead = 0;				/*!< Head of buffer. Where next recieved byte will be placed. */
volatile int recvtail = 0;				/*!< Tail of buffer. Where next read will occour. */

/// Interrupt vector for Rx. Place recieved data into buffer.
ISR(USART0_RX_vect){
	cli();
	char input = UDR0;
	if((recvhead+1)%BUFFER_MAX != recvtail){		// Sjekk at bufferen ikke er full
		recv_buffer[recvhead] = input;				// If so, legg inn byte
		recvhead = (recvhead+1)%BUFFER_MAX;
	}
	sei();
}

///\todo remove interrupt from Tx
ISR(USART0_TX_vect){
	
}

///Initialize uart
void uart_init(void){
	
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
}

/// Busy wait transmission of msg
int uart_send(unsigned char msg){
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = msg;
	return 0;
}

/// Read data from buffer
unsigned char uart_recv(void){
	char returnval = 0;
	if(recvhead != recvtail)
	{
		returnval = recv_buffer[recvtail++];
		recvtail = recvtail%BUFFER_MAX;
	}
	return returnval;
}

