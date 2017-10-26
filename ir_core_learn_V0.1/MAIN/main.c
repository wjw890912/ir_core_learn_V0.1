 /******************************************************************
 *	���ߣ�СARM�˲�
 *	ʱ�䣺2012��
 *	�ص㣺ɽ������
 *	�ļ�����"main.c"
 *  ��ע��All rights reserved��
 *   Q Q:925295580
 ******************************************************************/
#include "M051Series.h"
#include "DrvSYS.h"
#include "DrvGPIO.h"
#include "main.h"
#include "GPIO.h"
#include "DrvTIMER.h"
#include "DrvPWM.h"
#include "ir.h"
#include "Flash_Memory.h"
#include "FMC.h"
uint8_t RX_BUF[100];
uint8_t TX_buf[5]={0};
uint32_t UID;
extern LNode Nbuf[4];	
extern void Send_Ir_Code(uint8_t *pt ,uint16_t Lenth );
extern uint32_t ir_number;//����ʱ��
extern uint16_t ir_raw[512];    //������
extern uint16_t  Learn_Finsh; //ѧϰ���
 extern void  init_flash_pool();
 extern unsigned char RecvCmdFromSI4432(uint8_t *pBuf,uint8_t leg);

int main()
{
	 int j=0,m;
	uint8_t leg;
 initsys();
 initGPIO();
 init_PWM();
 initFMC();//FLASH
 UID=Read_UID_M0(2);
UID>>=16;
TX_buf[0]= UID>>24;
TX_buf[1]= UID>>16;
TX_buf[2]= UID>>8;
TX_buf[3]= UID>>0; //save id
initsi4432();
rx_data();
 init_flash_pool();	//��ʼ��FLASH �͹�������
initTMR0();
 init_IR_INTTRUPUT();
  
   TX_buf[0]= UID>>8;
   TX_buf[1]= UID;
   TX_buf[2]=0x04; //�յ�����
   TX_buf[3]=0xaa;
   TX_buf[4]=0xaa;	   //�������
 tx_data(TX_buf, 5);  //��������
  while(1)
  {
					
		 	if( Check_si4432(0)==1)	//rf online ?
				{

				 		leg = save_rec_data(RX_BUF);
			
							if(leg>0)
							{
						  	   //leg=0;
							   DrvTIMER_Close(E_TMR0);
 							RecvCmdFromSI4432(RX_BUF,leg);//
					 	//	   initTMR0();
							 leg=0;
				 	        }   		 
	 	       }

		if(!send_id)
		{
		  DrvTIMER_Close(E_TMR0);
		LEARN_DOWN;
   TX_buf[0]= UID>>8;
   TX_buf[1]= UID;
   TX_buf[2]=0x04; //�յ�����
   TX_buf[3]=0xaa;
   TX_buf[4]=0xaa;	   //�������

   Send_IrCodeFlash(1,ir_raw);
 tx_data(TX_buf, 5);  //��������
	LEARN_UP;	
		}
		if((!get_button))
		{	
		   
			LEARN_DOWN1;
			 Triget_Learn_state(1);		
			  	if((Check_Learn_Finsh())>0)
			  	  {		

				    // here you can do something ...
						DrvTIMER_Close(E_TMR0);
		
						m =	SaveIrCode2Flash(ir_raw,Learn_Finsh); //save code to flash 

				 			 	if(m!=0xff)
								{


										for(j=0;j<m+1;j++)
										{	   
										
											LEARN_DOWN;
											DrvTIMER_Delay( E_TMR0, 50000); 
											LEARN_UP;
											DrvTIMER_Delay( E_TMR0, 50000); 
									   //   Send_IrCodeFlash(0,ir_raw);
			   	
										}


								}
								else
								{
											LEARN_DOWN;
											DrvTIMER_Delay( E_TMR0, 5000); 
											LEARN_UP;
											DrvTIMER_Delay( E_TMR0, 10000); 
								}
				  
				  Reset_Finsh(); 

				  }


		}
		else
		{
			LEARN_UP1;
			Triget_Learn_state(0);
		}



  }
	

 } 
/******************************************************************
 *	���ߣ�СARM�˲�
 *	ʱ�䣺2012��
 *	�ص㣺ɽ������
 *	���ܣ�ϵͳ�ϵ��ʼ��
 *
 *
 ******************************************************************/
  void initsys(void) 
 {
uint32_t number=0,hz;

hz=hz;
DrvSYS_UnlockProtectedReg();//�رռĴ�������
if(DrvSYS_SetOscCtrl(E_SYS_XTL12M, 1) ==0)
{
// DrvSYS_SelectPLLSource(E_SYS_INTERNAL_22M);
 //DrvSYS_SetPLLMode(0);//pll����

// DrvSYS_Open(50000000);//sys=50MHZ
 DrvSYS_SelectHCLKSource(0);//pll 
DrvSYS_SetClockDivider(E_SYS_HCLK_DIV , 1);//HCLK1��Ƶ

hz= DrvSYS_GetHCLKFreq();


	while(DrvSYS_SelectSysTickSource(3)!=FALSE)
	{
	   number++;
	    if(number==30000)break;
	
	}
}
DrvSYS_LockProtectedReg();	
 
 }



 void initTMR0(void)
 {
				
  DrvSYS_SetIPClock(E_SYS_TMR0_CLK,1); //ʹ��ʼ��
  DrvSYS_SelectIPClockSource(E_SYS_TMR0_CLKSRC,0 );//extern xtal 12mhz
  DrvTIMER_Init();
  DrvTIMER_Open(E_TMR0,100000,E_PERIODIC_MODE);	  // 100khz
  DrvTIMER_EnableInt(E_TMR0);  
  DrvTIMER_Start(E_TMR0);
	
 }
 























