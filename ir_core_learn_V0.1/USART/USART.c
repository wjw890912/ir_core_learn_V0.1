 /******************************************************************
 *	���ߣ�СARM�˲�
 *	ʱ�䣺2012��
 *	�ļ�����"usart.c"
 *  ��ע��All rights reserved��
 *   QQ:925295580
 ******************************************************************/


#include <stdio.h>
#include <string.h> 
#include "M051Series.h"
#include "DrvGPIO.h"
#include "DrvSYS.h"
#include "DrvUART.h"
#include "DrvFMC.h"
#include "USART.h"
#include "DrvTIMER.h"
#include "WDT.h"
#include "FMC.h"
#include "GPIO.h"

STR_UART_T  config;	//��������
/******************************************************************
 *	ʱ�䣺2012��
 *	���ܣ�����1����
 *	���룺���ڱ�־״̬
 *	�������
 ******************************************************************/
void usart1_funcallback (uint32_t uartstart)
 {		 	
          

	


		if((uartstart&1==1))
		{
		         	 		 
		}
}
/******************************************************************
 *	ʱ�䣺2012��
 *	���ܣ�����0������
 *
 *
 ******************************************************************/
void funcallback (uint32_t uartstart)
 {		   

 		  if(uartstart!=NULL)
		  {
	   
		   

		  } 				
 }
/******************************************************************
 *	ʱ�䣺2012��
 *	���ܣ����ڳ�ʼ��
 ******************************************************************/
 void inituart()
 {


       DrvSYS_SetIPClock(E_SYS_UART0_CLK,1);//ʹ��UART0ʱ��

      while(DrvSYS_SetClockDivider(E_SYS_HCLK_DIV, 0)!=FALSE); //1��ƵHCLK
      while(DrvSYS_SetClockDivider(E_SYS_UART_DIV, 0)!=FALSE); //1��ƵUART
	  DrvSYS_SelectIPClockSource(E_SYS_UART_CLKSRC, 0);//UARTѡ���ⲿPLL��ʱ��Դ
	  DrvGPIO_InitFunction(E_FUNC_UART0);//����IO���UARTģʽ
		   /*�ر����д����ж�*/
      DrvUART_DisableInt (UART_PORT0,DRVUART_WAKEUPINT|
                                        DRVUART_RLSINT|
								        DRVUART_MOSINT|
								       DRVUART_THREINT| 
								       DRVUART_TOUTINT);	 
				/*���ô��ڲ���*/
		config.u32BaudRate=9600;//������38400
		config.u8cDataBits=DRVUART_DATABITS_8 ;//����λ8λ
		config.u8cStopBits=DRVUART_STOPBITS_1; //ֹͣλ1λ
		config.u8cParity=DRVUART_PARITY_NONE ; //NONEУ��
		config.u8cRxTriggerLevel=DRVUART_FIFO_1BYTES;//����FIFO 8�ֽ�

    DrvUART_Open(UART_PORT0, &config);//����UART�˿�

	/*��װ�ص��������������������жϣ���ʵ֤�����˺�������֮ǰ����ʹ�ÿ����˿�����*/
    DrvUART_EnableInt(	UART_PORT0,DRVUART_RDAINT,funcallback);



	  DrvSYS_SetIPClock(E_SYS_UART1_CLK,1);//ʹ��UART1ʱ��
	 
	  DrvGPIO_InitFunction(E_FUNC_UART1);//����IO���UARTģʽ
		   /*�ر����д����ж�*/
      DrvUART_DisableInt (UART_PORT1,DRVUART_WAKEUPINT|
                                        DRVUART_RLSINT|
								        DRVUART_MOSINT|
								       DRVUART_THREINT| 
								       DRVUART_TOUTINT);	 
				/*���ô��ڲ���*/
		config.u32BaudRate=9600;//������38400
		config.u8cDataBits=DRVUART_DATABITS_8 ;//����λ8λ
		config.u8cStopBits=DRVUART_STOPBITS_1; //ֹͣλ1λ
		config.u8cParity=DRVUART_PARITY_NONE ; //NONEУ��
		config.u8cRxTriggerLevel=DRVUART_FIFO_1BYTES;//����FIFO 8�ֽ�

    DrvUART_Open(UART_PORT1, &config);//����UART�˿�

			/*��װ�ص��������������������жϣ���ʵ֤�����˺�������֮ǰ����ʹ�ÿ����˿�����*/
  DrvUART_EnableInt(UART_PORT1,DRVUART_RDAINT,usart1_funcallback);
}

