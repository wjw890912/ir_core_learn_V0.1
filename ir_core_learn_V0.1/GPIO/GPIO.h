#ifndef _GPIO_H
#define _GPIO_H



#define IRSEND_UP    DrvGPIO_SetBit(E_PORT4, E_PIN0) 
#define IRSEND_DOWN  DrvGPIO_ClrBit(E_PORT4, E_PIN0)

#define S38K_UP    DrvGPIO_SetBit(E_PORT1, E_PIN1) 
#define S38K_DOWN  DrvGPIO_ClrBit(E_PORT1, E_PIN1)


				 //修改
#define get_button DrvGPIO_GetBit(E_PORT0,E_PIN0)//发送红外码  S3

#define send_id   DrvGPIO_GetBit(E_PORT0,E_PIN2)//发送红外码学习  S2

						   //学习灯
 #define  LEARN_UP	  DrvGPIO_SetBit(E_PORT1, E_PIN1)
 #define  LEARN_DOWN  DrvGPIO_ClrBit(E_PORT1, E_PIN1)
 #define  LEARN_UP1	  DrvGPIO_SetBit(E_PORT1, E_PIN0)
 #define  LEARN_DOWN1  DrvGPIO_ClrBit(E_PORT1, E_PIN0)



#define GET_SDO   DrvGPIO_GetBit(E_PORT2, E_PIN3)

#define GET_NIRQ   DrvGPIO_GetBit(E_PORT3, E_PIN3)

#define SDI_UP    DrvGPIO_SetBit(E_PORT2, E_PIN4) 
#define SDI_DOWN  DrvGPIO_ClrBit(E_PORT2, E_PIN4)

#define SCK_UP    DrvGPIO_SetBit(E_PORT2, E_PIN5) 
#define SCK_DOWN  DrvGPIO_ClrBit(E_PORT2, E_PIN5)

#define nSEL_UP   DrvGPIO_SetBit(E_PORT2, E_PIN6) 
#define nSEL_DOWN DrvGPIO_ClrBit(E_PORT2, E_PIN6)

#define SDN_UP    DrvGPIO_SetBit(E_PORT2, E_PIN7) 
#define SDN_DOWN  DrvGPIO_ClrBit(E_PORT2, E_PIN7)



void initGPIO(void);



#endif

