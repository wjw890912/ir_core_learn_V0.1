 /******************************************************************
 *	时间：2012年
 *	文件名："GPIO.c"
 *  备注：All rights reserved！
  Q Q:925295580
 ******************************************************************/
#include "DrvGPIO.h"
#include "GPIO.h"


/******************************************************************
 *	时间：2012年
 *	功能：初始化GPIO
 *	输入：	
 *	输出：	
 ***************************************************************c ***/
void initGPIO()
 {
 		 
DrvGPIO_Open(E_PORT3, E_PIN2 , E_IO_INPUT);//IR INPUT 
DrvGPIO_Open(E_PORT4, E_PIN0 , E_IO_OUTPUT);	//PWM0 OUTPUT
		
			//si4432
	DrvGPIO_Open(E_PORT2,E_PIN3, E_IO_INPUT);//input miso
  	DrvGPIO_Open(E_PORT3,E_PIN3, E_IO_INPUT);//input nirq
	DrvGPIO_Open(E_PORT2,E_PIN4, E_IO_OUTPUT); //mosi
   	DrvGPIO_Open(E_PORT2,E_PIN5, E_IO_OUTPUT); //sck
	DrvGPIO_Open(E_PORT2,E_PIN6, E_IO_OUTPUT); //nsel
    DrvGPIO_Open(E_PORT2,E_PIN7, E_IO_OUTPUT); //sdn


  
DrvGPIO_Open(E_PORT0, E_PIN0 , E_IO_INPUT);	//s3 inPUT  
DrvGPIO_Open(E_PORT0, E_PIN2 , E_IO_INPUT);	//s1 inPUT 
DrvGPIO_Open(E_PORT1, E_PIN1 , E_IO_OUTPUT);	//led2 OUTPUT
DrvGPIO_Open(E_PORT1, E_PIN0 , E_IO_OUTPUT);	//led1 OUTPUT


 }
