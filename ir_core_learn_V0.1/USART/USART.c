 /******************************************************************
 *	作者：小ARM菜菜
 *	时间：2012年
 *	文件名："usart.c"
 *  备注：All rights reserved！
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

STR_UART_T  config;	//串口配置
/******************************************************************
 *	时间：2012年
 *	功能：串口1处理
 *	输入：串口标志状态
 *	输出：无
 ******************************************************************/
void usart1_funcallback (uint32_t uartstart)
 {		 	
          

	


		if((uartstart&1==1))
		{
		         	 		 
		}
}
/******************************************************************
 *	时间：2012年
 *	功能：串口0处理函数
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
 *	时间：2012年
 *	功能：串口初始化
 ******************************************************************/
 void inituart()
 {


       DrvSYS_SetIPClock(E_SYS_UART0_CLK,1);//使能UART0时钟

      while(DrvSYS_SetClockDivider(E_SYS_HCLK_DIV, 0)!=FALSE); //1分频HCLK
      while(DrvSYS_SetClockDivider(E_SYS_UART_DIV, 0)!=FALSE); //1分频UART
	  DrvSYS_SelectIPClockSource(E_SYS_UART_CLKSRC, 0);//UART选择外部PLL的时钟源
	  DrvGPIO_InitFunction(E_FUNC_UART0);//复用IO编程UART模式
		   /*关闭所有串口中断*/
      DrvUART_DisableInt (UART_PORT0,DRVUART_WAKEUPINT|
                                        DRVUART_RLSINT|
								        DRVUART_MOSINT|
								       DRVUART_THREINT| 
								       DRVUART_TOUTINT);	 
				/*设置串口参数*/
		config.u32BaudRate=9600;//波特率38400
		config.u8cDataBits=DRVUART_DATABITS_8 ;//数据位8位
		config.u8cStopBits=DRVUART_STOPBITS_1; //停止位1位
		config.u8cParity=DRVUART_PARITY_NONE ; //NONE校验
		config.u8cRxTriggerLevel=DRVUART_FIFO_1BYTES;//接受FIFO 8字节

    DrvUART_Open(UART_PORT0, &config);//开启UART端口

	/*安装回调函数，开启串口数据中断，事实证明，此函数调用之前必须使用开启端口命令*/
    DrvUART_EnableInt(	UART_PORT0,DRVUART_RDAINT,funcallback);



	  DrvSYS_SetIPClock(E_SYS_UART1_CLK,1);//使能UART1时钟
	 
	  DrvGPIO_InitFunction(E_FUNC_UART1);//复用IO编程UART模式
		   /*关闭所有串口中断*/
      DrvUART_DisableInt (UART_PORT1,DRVUART_WAKEUPINT|
                                        DRVUART_RLSINT|
								        DRVUART_MOSINT|
								       DRVUART_THREINT| 
								       DRVUART_TOUTINT);	 
				/*设置串口参数*/
		config.u32BaudRate=9600;//波特率38400
		config.u8cDataBits=DRVUART_DATABITS_8 ;//数据位8位
		config.u8cStopBits=DRVUART_STOPBITS_1; //停止位1位
		config.u8cParity=DRVUART_PARITY_NONE ; //NONE校验
		config.u8cRxTriggerLevel=DRVUART_FIFO_1BYTES;//接受FIFO 8字节

    DrvUART_Open(UART_PORT1, &config);//开启UART端口

			/*安装回调函数，开启串口数据中断，事实证明，此函数调用之前必须使用开启端口命令*/
  DrvUART_EnableInt(UART_PORT1,DRVUART_RDAINT,usart1_funcallback);
}

