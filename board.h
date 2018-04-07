/*
 * board.h
 *
 *  Created on: 24 Nov 2017
 *      Author: Administrator
 */

#ifndef BOARD_H_
#define BOARD_H_
#include <avr/io.h>
#include "global.h"
// DDRX
#define DIR		PD2
#define DHT		PB0
#define RESET	PD3
#define LED		PD4

// DDRX
#define dDIR	DDRD
#define dDHT	DDRB
#define dLED	DDRD
#define dRESET	DDRD
// PORTX
#define pDIR	PORTD
#define pDHT	PORTB
#define pLED	PORTD
#define pRESET	PORTD

// pin reset
#define iRESET	PIND
#define IN_RESET	(inb(iRESET)&(1<<RESET))

//PIN dht
#define iDHT	PINB
#define IN_DHT	(inb(iDHT) & (1<<DHT))

// khai  báo direction rs485
#define REV		cbi(pDIR, DIR)
#define SEND	sbi(pDIR, DIR)

// LED ON
#define LED_ON	sbi(pLED,LED)
#define LED_OFF cbi(pLED, LED)



#endif /* BOARD_H_ */
