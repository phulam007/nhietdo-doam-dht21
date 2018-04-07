/*
 *
 * 28 - 10 -2017
 * - Doc adc cam bien ap suat nuoc
 * ---- ngo ra cam bien dang dong : tu 4 - 20mA (0 - 6 bar)
 * - Dieu khien ngo ra bien tan tu 0 - 10v thong qua leader resistor (dung hc595 + uln2803)
 * - Dung giai thuat fuzzy
 * - Dung threadRtos
 *
 */
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "atom.h"
#include "atomport-private.h"
#include "atomtimer.h"
#include "ioeasy.h"
#include "board.h"
#include "uart.h"

#define __DEBUG__					0
#define TIME_RESET					10
#define ATOM_STACK_CHECKING			1
#define IDLE_STACK_SIZE_BYTES       128
#define TEST_THREAD_PRIO			16
#define MAIN_STACK_SIZE_BYTES       250
#define NUMBER_THREAD				2

	/* Local data */
	/* Application threads' TCBs */
static ATOM_TCB main_tcb[NUMBER_THREAD];
static ATOM_TCB butt_tcb;
	/* Main thread's stack area */
static uint8_t main_thread_stack[NUMBER_THREAD][MAIN_STACK_SIZE_BYTES];
static uint8_t button_thread_stack[100];
	/* Idle thread's stack area */
static uint8_t idle_thread_stack[IDLE_STACK_SIZE_BYTES];

static FILE uart_stdout = FDEV_SETUP_STREAM(sendByte, NULL, _FDEV_SETUP_WRITE);
	/*Forward declarations */
static void main_thread_func (uint32_t data);// đọc nhiệt độ, độ ẩm về
static void printUartRingBuffer(); // kiểm tra uart -- xử lý lệnh
static void buttonDevice(); // nhấn nút search thiết bị hoặc reset thiết bị
/* STDIO stream */
//FILE uart_stdout;
volatile struct ring_buf
		{
			u08 data[255];
			u08 len;
			u08 head;
			u08 tail;
		} ring_buf;

float h,t;
volatile u08 ackTrans;
u08 strH[10];
u08 strT[10];
u08 thrTrigger[NUMBER_THR][4];
struct device device;
struct dataIn dataIn;
u08 process;
u08 getTimeout;
u08 searchEn;
u08 setDeviceIp(u08 i, u08 *dta);

int main ( void )
	{
		int8_t status;
	    // tạo ngăn chứ cho thread idle
	    SP = (int)&idle_thread_stack[(IDLE_STACK_SIZE_BYTES/2) - 1];
	    // khoi tao atomOS
	    status = atomOSInit(&idle_thread_stack[0], IDLE_STACK_SIZE_BYTES, FALSE);
	    if (status == ATOM_OK)
	    {
	        /* Enable the system tick timer */
	        avrInitSystemTickTimer();

	        /* Create an application thread */
	        status = atomThreadCreate(&main_tcb[0],
	                     TEST_THREAD_PRIO, main_thread_func, 0,
	                     &main_thread_stack[0][0],
	                     MAIN_STACK_SIZE_BYTES,
	                     TRUE);
	        status |= atomThreadCreate(&main_tcb[1],
	        			15, printUartRingBuffer, 0,
						&main_thread_stack[1][0],
						MAIN_STACK_SIZE_BYTES,
						TRUE);
	        status |= atomThreadCreate(&butt_tcb,
	        			17, buttonDevice, 0,
						&button_thread_stack[0],
						100,
						TRUE);
	        if (status == ATOM_OK)
	        {
	        	//archFirstThreadRestore();
	            atomOSStart();
	        }
	    }
	    while (1)
	        ;
	    /* There was an error starting the OS if we reach here */
	    return (0);
	}

// reset mac
static void buttonDevice()
{
	static u16 cnt;
	u08 i;
	while(1)
	{
		atomTimerDelay(100);
		if(IN_RESET==0)
		{
			cnt++;
		}else{
			if((cnt>=2)&&(cnt<=20))
			{
				if(searchEn==1)
				{
					LED_OFF;
					searchEn = 0;
				}
				else
				{
					LED_ON;
					searchEn = 1;
				}
			}
			cnt = 0;
		}
		if(cnt>=TIME_RESET*1000/100)
		{
			for(i=0;i<10;i++)
			{
				LED_ON;
				atomTimerDelay(300);
				LED_OFF;
				atomTimerDelay(300);
			}
			cnt = 0;
			deleteEeprom();
			initDevice();
		}
	}
}

static void main_thread_func (uint32_t data)
	{
	    /* Initialise UART (9600bps) */
	    uartInit();
	    gpioInit();
	    initDevice();
	    sei();
	    stdout = &uart_stdout;
	    u08 stt=0;
	    u08 stt1 = 0;
	    u16 timeout = 0;
	    u08 i;
	    u08 res[5] = {0,0,0,0,0};
	    u16 hm = 0;
	    u16 tm = 0;
	    u08 checksum = 0;
		#ifdef __DEBUG__
	    	for(i=0;i<NUMBER_DEVICE;i+=1)
	    	{
	    		rs485("macDev:%s-%d-%s\r\n",device.myMac[i],device.myIp[i],device.host);
	    		atomTimerDelay(100);
	    	}
		#endif
	    while (1)
	    {
	    	// low 10ms
	    	cbi(pDHT,DHT);
	    	_delay_ms(10);
	    	// high 20 - 40us
	    	sbi(pDHT,DHT);
	    	_delay_us(40);
	    	// cho dht keo xuong trong vong 80us
	    	_delay_us(40);
	    	cbi(dDHT,DHT);
	    	cbi(pDHT,DHT);
	    	if(IN_DHT==0)
	    	{
	    		stt = 1;
	    	}
	    	_delay_us(80);
	    	if(IN_DHT)
	    	{
	    		stt1= 1;
	    		while(IN_DHT && (timeout<1000))
	    		{
	    			_delay_us(2);
	    			timeout++;
	    		}
	    		if(timeout>=1000)
	    		{
	    			timeout = 0;
	    			//rs485("fail\r\n");
	    		}
	    		else
	    		{
	    			for(i=0;i<40;i++)
	    			{
	    				while(!IN_DHT && (timeout<100))
	    				{
	    					_delay_us(2);
	    					timeout++;
	    				}
	    				if(timeout<1000)
	    				{
	    					timeout = 0;
	    					while(IN_DHT && (timeout<100))
	    					{
	    						_delay_us(2);
	    						timeout++;
	    					}
	    					if(timeout<1000)
	    					{
	    						if(timeout < 15)
	    						{
	    							timeout=0;
	    							res[i/8]=(res[i/8]<<1);
	    						}
	    						else
	    						{
	    							timeout=0;
	    							res[i/8]=(res[i/8]<<1)|0x01;
	    						}
	    					} else
	    					{
	    						timeout = 0;
	    						//rs485("fail 2 \r\n");
	    					}
	    				} else
	    				{
	    					timeout=0;
	    					//rs485("fail 1 \r\n");
	    				}
	    			}
	    		}
	    	}
	    	checksum = res[0]+res[1]+res[2]+res[3];
	    	if((checksum == res[4])&&(checksum!=0))
	    	{
		    	hm = res[0];
		    	hm = (hm<<8)|res[1];
		    	tm = res[2];
		    	tm = (tm<<8)|res[3];
		    	checkThr(tm,0);
		    	checkThr(hm,1);
		    	h=hm/10.0;
		    	t=tm/10.0;
		    	//đổi số thập phân thành chuỗi
		    	dtostrf((u08)h,3,1,strH);
		    	dtostrf((u08)t,3,1,strT);
	    	}
	    	sbi(dDHT,DHT);
	    	sbi(pDHT,DHT);
	    	atomTimerDelay(5000);
	    }
	}



// chương trình kiểm tra dữ liệu đầu vào....
static void printUartRingBuffer()
{
	struct command a;
	u08 i;
	u08 res;
	while(1)
	{
		while((ring_buf.len>ring_buf.tail)&&(!process))
			{
				checkCmd(ring_buf.data[ring_buf.tail]);
				ring_buf.tail++;
			}
		ring_buf.head= 0;
		ring_buf.len = 0;
		ring_buf.tail= 0;
		if(dataIn.type == ACK)
		{
			ackTrans = dataIn.trans;
		}
		if(process)
		{
			// kiểm tra thiết bị đã có host trước đó chưa.
			if(checkDeviceHost())
			{
				// kiểm tra host có đúng kg?
				if(strcmp(dataIn.host,device.host)==0)
				{
					// kiểm tra loại thiết bị có tương ứng kg..?
					for(i=0;i<NUMBER_DEVICE;i++)
					{
						// đúng loại đúng IP -- kiểm tra lệnh gửi xuống.
						if((dataIn.type==INPUT)&&(dataIn.ip==device.myIp[i]))
						{
							// gửi lệnh đọc mac thiết bị
							if(checkDataIn(dataIn.data,"m",1))
							{
							  if(device.myIp[i]!=0xff)
								sendMac(i);
							  if((device.myIp[i]==0xff)&&(searchEn==1))
								  sendMac(i);
							}else
							// gửi trạng thái thiết bị
							if(checkDataIn(dataIn.data,"s",1))
							{
								sendStatus(i);
							}else
							// xóa thiết bị
							if(checkDataIn(dataIn.data,"D",1))
							{
								deleteDevice(i);
							}else
							// đọc ngưỡng 1 thiết bị
							if(checkDataIn(dataIn.data,"THR1",4))
							{
								sendThr(i,0);
							}else
							// đọc ngưỡng 2 thiết bị
							if(checkDataIn(dataIn.data,"THR2",4))
							{
								sendThr(i,1);
							}else
							// đọc ngưỡng 3 thiết bị
							if(checkDataIn(dataIn.data,"THR3",4))
							{
								sendThr(i,2);
							}else
							// đọc ngưỡng 4 thiết bị
							if(checkDataIn(dataIn.data,"THR4",4))
							{
								sendThr(i,3);
							}else
							// thiết lập ngưỡng của thiết bị
							{
								str2param(dataIn.data,"=",&a);
								if(checkDataIn(a.cmd,"THR1",4))
									{
										if(strlen(a.param)>1)
										{
											setThr(i,0,a.param);
										}else
										{
											if(strlen(a.param)==0)
											{
												clearThr(i,0);
											}
										}
									}else
									if(checkDataIn(a.cmd,"THR2",4))
									{
										if(strlen(a.param)>1)
										{
											setThr(i,1,a.param);
										}else
										{
											if(strlen(a.param)==0)
											{
												clearThr(i,1);
											}
										}
									}else
									if(checkDataIn(a.cmd,"THR3",4))
									{
										if(strlen(a.param)>1)
										{
											setThr(i,2,a.param);
										}else
										{
											if(strlen(a.param)==0)
											{
												clearThr(i,2);
											}
										}
									}else
									if(checkDataIn(a.cmd,"THR4",4))
									{
										if(strlen(a.param)>1)
										{
											setThr(i,3,a.param);
										}else
										{
											if(strlen(a.param)==0)
											{
												clearThr(i,3);
											}
										}
									}else
									if(checkDataIn(a.cmd,"OFFSET",6)){
										setOffset(i, a.param);
									}else
										{
											sendErr();
										}
							}
						}
					}
				}
			}
			else {
				// thiết bị chưa có host -- kiểm tra lệnh gửi tới -- nếu lệnh tìm tb thì trả về
				if((dataIn.type==INPUT)&&checkDataIn(dataIn.data,"m",1)&&(dataIn.ip==0xff)&&(searchEn==1))
				{
					u08 tmp;
					tmp = getDeviceIp();
					if (tmp!=0xff)
						sendMac(tmp);
				}
			}
			if(dataIn.type==SETMAC)
			{
				if(checkDeviceHost())
				{
					if(strcmp(dataIn.host,device.host)==0)
					{
						for(i=0;i<NUMBER_DEVICE;i++)
							{
								if((strcmp(dataIn.deviceMac,device.myMac[i])==0)&&(searchEn==1))
									{
									// thiết lập địa chỉ IP cho thiết bị....
										setDeviceIp(i,dataIn.data);
									}
							}
					}
				}
				// nếu chưa có host thì không cần kiểm tra host
				else{
					for(i=0;i<NUMBER_DEVICE;i++)
						{
							if((strcmp(dataIn.deviceMac,device.myMac[i])==0)&&(searchEn==1))
								{
									setDeviceIp(i,dataIn.data);
								}
						}
				}
			}
	endProcess:
			process = 0;
			memset(&dataIn.host[0], 0, sizeof(dataIn.host));
			memset(&dataIn.data[0], 0, sizeof(dataIn.data));
		}
		atomTimerDelay(100);
		if(ring_buf.len==0)
		{
			getTimeout = 1;
			process = 0;
		}
	}
}

// THIẾT LẬP OFFSET
void setOffset(u08 i, u08 *chr)
{
	u16 tmp;
	u08 str[10];
	if(strlen(chr)==0)
	{
		ltoa(device.offset[i],str,10);
		rs485(SEND_REPLY,dataIn.trans,strlen(str),str);
	}else
	{
		tmp = atol(chr);
		device.offset[i] = tmp;
		eeprom_write_word((u08)OFFSET_+i*2,tmp);
		rs485(SEND_REPLY,dataIn.trans,2,"OK");
	}
}
// kiểm tra thr của thiết bị
void checkThr(u16 v1,u08 k)
{
	u08 j, trans,cnt;
		for(j=0;j<4;j++){
			if(device.thrE[k][j] == 1)
			{
				switch(device.thrCmp[k][j])
				{
				case 1: if(v1 >= device.thr[k][j])
						{
							if(thrTrigger[k][j]==0)
							{
								trans = atomTimeGet()%250;
								cnt = 0;
								while((ackTrans!=trans)&&(cnt<2)) // resend 2 lần
								{
									rs485(SEND_EVENT,trans,strlen(device.host),device.host,device.myIp[k],j+1,2,"OK");
									thrTrigger[k][j]=1;
									cnt+=1;
									atomTimerDelay(500);
								}
							}
						}
						if(v1 < (device.thr[k][j]-device.offset[k]))
						{
							thrTrigger[k][j] = 0;
						}
						break;
				case 2: if(v1 <= device.thr[k][j])
						{
							if(thrTrigger[k][j]==0)
							{
								trans = atomTimeGet()%250;
								cnt = 0;
								while((ackTrans!=trans)&&(cnt<2)) // resend 2 lần
								{
									rs485(SEND_EVENT,trans,strlen(device.host),device.host,device.myIp[k],j+1,2,"OK");
									thrTrigger[k][j]=1;
									cnt+=1;
									atomTimerDelay(500);
								}
							}
						}
						if(v1 > (device.thr[k][j]+device.offset[k]))
						{
							thrTrigger[k][j] = 0;
						}
						break;
				}
		}
	}
}


// set IP cho thiết bị
u08 setDeviceIp(u08 i, u08 *dta)
{
	struct command e;
	u08 res,j;
	str2param(dta,"=",&e);
	if((strcmp(e.cmd,"IP")==0)&&(strlen(e.param)>0))
	{
		res = atoi(e.param);
		for(j=0;j<NUMBER_DEVICE;j++)
		{
			if(device.myIp[j]==res)
			{
				rs485(SEND_REPLY,dataIn.trans,3,"err");
				return 0;
			}
		}
		if(!checkDeviceHost())
		{
			eeprom_write_byte((u08)LENHOST,strlen(dataIn.host));
			for(j=0;j<strlen(dataIn.host);j++)
			{
				eeprom_write_byte((u08)MACHOST+j,dataIn.host[j]);
			}
			strcpy(device.host,dataIn.host);
		}
		eeprom_write_byte((u08)IPADDR+i,res);
		device.myIp[i]=res;
		rs485(SEND_REPLY,dataIn.trans,2,"OK");
		return 1;
	}
	return 0;
}
// xóa ngưỡng
void clearThr(u08 i,u08 j)
{
	eeprom_write_byte((u08)THRE_+i*4+j,0);
	eeprom_write_byte((u08)THRCMP_+i*4+j,0);
	device.thrE[i][j] = 0;
	device.thrCmp[i][j] = 0;
	rs485(SEND_REPLY,dataIn.trans,2,"OK");
}
// set ngưỡng thiết bị
void setThr(u08 i,u08 j,char* thr)
{
	u08 k;
	u16 tmp;
	k = strlen(thr);
	if(thr[k-1]=='u')
	{
		// so sánh lớn hơn
		thr[k-1] = 0;
		tmp = atol(thr);
		rs485(SEND_REPLY,dataIn.trans,strlen(thr),thr);
		eeprom_write_word((u08)THR_+(i*8)+j*2,tmp);
		eeprom_write_byte((u08)THRE_+i*4+j,1);
		eeprom_write_byte((u08)THRCMP_+i*4+j,1);
		device.thrE[i][j] = 1;
		device.thrCmp[i][j]=1;
		device.thr[i][j]=tmp;
		//rs485("%u",device.thr[i][j]);
	}else
		if(thr[k-1]=='d')
		{
			// so sánh nhỏ hơn
			thr[k-1] = 0;
			tmp = atol(thr);
			rs485(SEND_REPLY,dataIn.trans,strlen(thr),thr);
			eeprom_write_word((u08)THR_+(i*8)+j*2,tmp);
			eeprom_write_byte((u08)THRE_+i*4+j,1);
			eeprom_write_byte((u08)THRCMP_+i*4+j,2);
			device.thrE[i][j] = 1;
			device.thrCmp[i][j]=2;
			device.thr[i][j]=tmp;

		}else
		{
			rs485(SEND_REPLY,dataIn.trans,3,"Err");
		}
}

// xóa thiết bị
void deleteDevice(u08 i)
{
	eeprom_write_byte((u08)IPADDR+i,0xff);
	device.myIp[i]=0xff;
	if(!checkDeviceHost())
	{
		deleteEeprom();
	}
	rs485(SEND_REPLY,dataIn.trans,2,"OK");
}

// gửi ngưỡng về
void sendThr(u08 i, u08 j)
{
	u08 s[10];
	if(device.thrE[i][j]==1)
	{
		ltoa(device.thr[i][j],s,10);
		if(device.thrCmp[i][j] == 1)
			s[strlen(s)] = 'u';
		else if(device.thrCmp[i][j]==2)
		{
			s[strlen(s)] = 'd';
		}
		rs485(SEND_REPLY,dataIn.trans,strlen(s),s);
	}
	else
	{
		rs485(SEND_REPLY,dataIn.trans,4,"null");
	}
}
// gửi trạng thái về io
void sendStatus(u08 i)
{
	switch(i)
	{
	case 0: rs485(SEND_REPLY,dataIn.trans,strlen(strT),strT); break;
	case 1: rs485(SEND_REPLY,dataIn.trans,strlen(strH),strH); break;
	}
}
// gửi lỗi về io
void sendErr(u08 i)
{
	rs485(SEND_REPLY,dataIn.trans,3,"Err");
}
// gửi địa chỉ mac của thiết bị
void sendMac(u08 i)
{
	rs485(SEND_REPLY,dataIn.trans,strlen(device.myMac[i]),device.myMac[i]);
}

// đọc về ngắt UART
ISR(USART_RX_vect)
{
	atomIntEnter();
	unsigned char c;
	c = UDR0;
	ring_buf.data[ring_buf.head+ring_buf.len]=c;
	ring_buf.data[ring_buf.head+ring_buf.len+1]='\0';
	ring_buf.len++;
	atomIntExit(TRUE);
}
