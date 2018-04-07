/*
 * uart.h
 *
 *  Created on: Jun 3, 2016
 *      Author: Administrator
 */


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include "global.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>

#define BAUD 	9600
#define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)
#define DELAY_485	atomTimerDelay(3)
void uartInit();
void sendByte(unsigned char c);
//static FILE uart_ = FDEV_SETUP_STREAM(sendByte, NULL, _FDEV_SETUP_WRITE);
#define rs485(...)	SEND;\
					DELAY_485;\
					printf(__VA_ARGS__);\
					DELAY_485;\
					REV;\

#endif /* UART_H_ */
