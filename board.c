/*
 * board.c
 *
 *  Created on: 24 Nov 2017
 *      Author: Administrator
 */

#include "board.h"


void gpioInit()
{
	// khai b�o ng� ra
	sbi(dDIR,DIR);
	cbi(pDIR, DIR);
	sbi(dLED,LED);
	cbi(pLED, LED);
	// khai b�o ng� v�o
	sbi(dDHT,DHT);
	sbi(pDHT,DHT);

	cbi(dRESET,RESET);
	sbi(pRESET,RESET);
}
