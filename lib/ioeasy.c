/*
 * ioeasy.c
 *
 *  Created on: 28 Nov 2017
 *      Author: Administrator
 */

#include "ioeasy.h"
#include "avr/eeprom.h"
#include "string.h"
#include "uart.h"

unsigned char default_mac[NUMBER_DEVICE][15]={"mI2302-1","mI2302-2"};
u08 default_type[NUMBER_DEVICE]={1,1};
// cat chuoi lay 2 thong so la cmd va param
void str2param(unsigned char *des,const char *c,struct command *s)
{
    unsigned char *token;
    memset(s->cmd,0,sizeof(s->cmd));
    memset(s->param,0,sizeof(s->param));
    token = strtok(des,c);
    strcpy(s->cmd,token);
    token = strtok(NULL,c);
    if(token)
    {
    	strcpy(s->param,token);
    }
}

// xóa dữ liệu thiết bị -- dùng cho nút reset ..
void deleteEeprom()
{
	u08 i,j;
	eeprom_write_byte((u08)LENHOST,0);
	for(i=0;i<NUMBER_DEVICE;i++)
	{
		eeprom_write_byte((u08)IPADDR+i,0xff);
	}
#ifdef __THR__
	for(i=0;i<NUMBER_DEVICE;i++)
	{
		for(j=0;j<4;j++)
		{
			eeprom_write_byte((u08)THRE_+((i*4)+j),0);
		}
	}
#endif
}

// lay thong tin thiet bi tu bo nho eeprom
void initDevice()
{
	u08 lenDevice;
	u08 lenHost;
	u08 i,j;

	eeprom_busy_wait();
	// doc thong tin cua thiet bi
	lenHost = eeprom_read_byte((u08)LENHOST);
	if(lenHost < 20)
		eeprom_read_block(device.host,MACHOST,lenHost);
	else
	{
		lenHost = 0;
		eeprom_write_byte(LENHOST,0);
	}
	for(i=0;i<NUMBER_DEVICE;i++)
	{
		if(lenHost>0)
			device.myIp[i] = eeprom_read_byte((u08)IPADDR+i);
		else
			device.myIp[i] = 0xff;
		memcpy(device.myMac[i],default_mac[i],sizeof(default_mac[i]));
		device.myType[i]=eeprom_read_byte((u08)TYPE+i);
	}
	// đọc ngưỡng... sau này nên sửa lại
	// những thiết bị có ngưỡng mới đọc ngưỡng về.
#ifdef __THR__
	for(i=0;i<NUMBER_DEVICE;i++)
	{
		for(j=0;j<4;j++)
		{
			device.thr[i][j] = eeprom_read_word((u08)THR_+((i*8)+j*2));
			device.offset[i] = eeprom_read_word((u08)OFFSET_+(i*2));
			device.thrCmp[i][j] = eeprom_read_byte((u08)THRCMP_+((i*4)+j));
			device.thrE[i][j] = eeprom_read_byte((u08)THRE_+((i*4)+j));
		}
	}
#endif
}


// kiem tra lenh tu IO gui xuong
void checkCmd(char c)
{
	static u08 cnt;
	static u08 len;
	static u08 tmp;
	if(getTimeout)
	{
		cnt = 0;
		getTimeout=0;
	}
	switch(cnt){
	// detect
	case 0: if(c=='V')
			{
				cnt = 1;
			}
			break;
	case 1: if(c=='S')
			{
				cnt = 2;
			} else cnt = 0;
			break;
	// kiem tra kieu thiet bi dieu khien
	case 2: if(c=='O')
			{
				// kieu thiet bi la output
				dataIn.type = OUTPUT;
				cnt = 30;
			}
			else if(c=='I')
			{
				// kieu thiet bi la input
				dataIn.type = INPUT;
				cnt = 30;
			}
			else if(c=='M')
			{
				// lenh gan thiet bi
				dataIn.type = SETMAC;
				cnt = 40;
			}
			else if(c=='a')
			{
				// gui ack tu host
				dataIn.type = ACK;
				cnt = 50;
			}
			else {
				cnt = 0;
			}
			break;
	// lay ma trans tu host gui qua...
	case 30: 	dataIn.trans=c;
				cnt = 31;
				break;
	case 31:	len = c;
				cnt = 32;
				tmp = 0;
				break;
	case 32:	dataIn.host[tmp]=c;
				tmp++;
				if(tmp==len)
				{
					cnt = 33;
					tmp = 0;
					len = 0;
				}
				break;
	case 33:    dataIn.ip = c;
				cnt = 34;
				break;
	case 34:	len = c;
				cnt = 35;
				break;
	case 35: 	dataIn.data[tmp]=c;
				tmp++;
				if(tmp==len)
				{
					tmp = 0;
					len = 0;
					cnt = 0;
					process  = 1;
				}
				break;
	case 40:	dataIn.trans = c;
				cnt = 41;
				break;
	case 41: 	len = c;
				cnt = 42;
				tmp=0;
				break;
	case 42:	dataIn.host[tmp]=c;
				tmp++;
				if(tmp==len)
				{
					tmp=0;
					len=0;
					cnt = 43;
				}
				break;
	case 43:	len = c;
				cnt = 44;
				break;
	case 44:	dataIn.deviceMac[tmp]=c;
				tmp++;
				if(tmp==len)
				{
					tmp=0;
					len=0;
					cnt=45;
				}
				break;
	case 45:	len = c;
				cnt = 46;
				break;
	case 46:    dataIn.data[tmp]=c;
				tmp++;
				if(tmp==len)
				{
					tmp=0;
					len=0;
					cnt=0;
					process  =1;
				}
				break;
	case 50:	dataIn.trans = c;
				cnt = 0;
				process = 1;
				break;
	}
}


u08 checkDataIn(char data[],char str[],u08 len)
{
	if((strlen(data)==len) && strncmp(data,str,len)==0)
		return 1;
	else
		return 0;
}

u08 checkDeviceHost()
{
	u08 i;
	for(i=0;i<NUMBER_DEVICE;i++)
	{
		if(device.myIp[i]!=0xff)
		{
			return 1;
		}
	}
	return 0;
}

u08 getDeviceIp()
{
	u08 i;
	for(i=0;i<NUMBER_DEVICE;i++)
	{
		if(device.myIp[i]==0xff)
			return i;
	}
	return 0xff;
}

