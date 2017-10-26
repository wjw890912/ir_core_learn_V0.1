// File Name:SI4432_proto.C
// Author:小ARM菜菜&王德壮
// Date: 2012年
 //Q Q:925295580

#include "SI4432_proto.h"
#include "string.h"
#include "SI4432.H"
#include <stdio.h>
#include "M051Series.h"
#include "DrvSYS.h"
#include "DrvGPIO.h"
#include "GPIO.h"
#include "stdint.h"
#include "FMC.H"




extern uint32_t UID;
 extern uint8_t TX_buf[5]; 
 extern uint16_t ir_raw[512];    //数据码
/*解析无线协议帧函数*/
extern uint16_t  Send_IrCodeFlash(uint8_t size,uint16_t *p);
unsigned char RecvCmdFromSI4432(uint8_t *pBuf,uint8_t leg)
{		 
	 	 uint32_t ID_RX=0;
		  uint8_t temp;


		if(leg!=5)return;	 //数据包个数不正确
		
		  ID_RX=pBuf[0];
		  ID_RX<<=8;
		  ID_RX+= pBuf[1];

		if(ID_RX!=UID)return;	  //id acess
		
		
				
							if((pBuf[2]==0x04)&&(pBuf[3]==0x00)) //空调
							{
								 temp= pBuf[4];	  //get number...
								 if(temp>3)
								 {
								 	TX_buf[3]=0x44;
								    TX_buf[4]=0x44;	   //不合法
								 }
								 else
								 {
								temp =    Send_IrCodeFlash(temp,ir_raw);
									TX_buf[3]=temp;
								    TX_buf[4]=temp>>8;	   //发送完毕
								 }
								   TX_buf[0]= UID>>8;
								   TX_buf[1]= UID;
								   TX_buf[2]=0x04; //空调类型
								   
								 tx_data(TX_buf, 5);  //返回数据
				
							
							}
						
}
		



