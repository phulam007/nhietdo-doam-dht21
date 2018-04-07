/*
 * ioeasy.h
 *
 *  Created on: 28 Nov 2017
 *      Author: Administrator
 */

#ifndef LIB_IOEASY_H_
#define LIB_IOEASY_H_

#include "global.h"
#define __THR__			1
#define __FLOAT__		1
#define NUMBER_DEVICE	2
#define NUMBER_THR		2
#define SEND_REPLY		"VSr%c%c%s"
#define SEND_ACK		"VSa%c"
#define SEND_EVENT		"VSe%c%c%s%c%c%c%s"
#define IPADDR			1	// địa chỉ đọc ip thiết bị
#define LENHOST			10	// độ dài mã host
#define MACHOST			11	// mã host
#define TYPE			40	// loại thiết bị
#ifdef __THR__
	#define THR_			50	// ngưỡng
	#define THRCMP_			70	// loại ngưỡng (thấp hơn, lớn hơn)
	#define THRE_			80	// ngưỡng enable hay disable
	#define OFFSET_			90	// phần offset cộng thêm
#endif


enum typeDevice {OUTPUT,INPUT,ACK,SETMAC};
struct device{
			unsigned char host[15];
			unsigned char myMac[NUMBER_DEVICE][15];
			u08 myIp[NUMBER_DEVICE];
			u08 myType[NUMBER_DEVICE];
			u08 trans;
#ifdef __THR__
			u16 thr[NUMBER_THR][4];
			u16 offset[NUMBER_THR];
			u08 thrE[NUMBER_THR][4];
			u08 thrCmp[NUMBER_THR][4];
#endif
		};



extern struct device device;

struct command{
	unsigned char cmd[8];
	unsigned char param[10];
};

struct dataIn
{
			u08 trans;
			unsigned char host[15];
			unsigned char deviceMac[15];
			u08 ip;
			u08 type;
			u08 data[100];
};
extern struct dataIn dataIn;
extern u08 process = 0;
extern u08 getTimeout=0;

#endif /* LIB_IOEASY_H_ */
