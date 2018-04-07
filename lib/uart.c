#include "uart.h"

void uartInit()
{
	outb(UBRR0H, BAUD_PRESCALE>>8);
	outb(UBRR0L, BAUD_PRESCALE);
	UCSR0B |= ((1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0));
	//UCSRC |= (1<<UCSZ1) | (1<<UCSZ0);
}


void sendByte(unsigned char c)
{
	 while(!(UCSR0A & (1 << UDRE0)));
	    UDR0 = c;    // send character
}
