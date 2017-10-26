
#include "M051Series.h"
#include "DrvSYS.h"
#include "DrvGPIO.h"
#include "main.h"
#include "GPIO.h"
#include "DrvTIMER.h"
#include "DrvPWM.h"
#include "ir.h"
#include "queue.h"
#include "Flash_Memory.h"
uint8_t StateLearn=0;   //ѧϰ״̬
uint8_t StartLeve=0;   //��ʼ��ƽ״̬
uint16_t DataLenth=0;    //���ͳ��� 
uint16_t  Learn_Finsh=0; //ѧϰ���
uint16_t ir_raw[512];    //������
uint32_t  ir_number;     //������

/*
 ���ͺ������
 */
 
 void Send_Ir_Code(uint8_t *pt ,uint16_t Lenth )
{	
	uint16_t i;	
	uint32_t da,da1;	 
   	for(i=1;i<Lenth+1;i+=2)
   	{
		 	 if(!StartLeve)
			 {
					 	//��������ƽΪ�͵�ƽ����ӵ͵�ƽ��ʼ���͡�
					   	DrvPWM_Enable(DRVPWM_TIMER0, 1);
						da = ir_raw[i];
						DrvTIMER_Delay( E_TMR0, da) ;
					 	DrvPWM_Enable(DRVPWM_TIMER0, 0);
						da1 = ir_raw[i+1]; 
						DrvTIMER_Delay( E_TMR0, da1) ;
			 }
			 else
			 {			//��������ƽΪ�ߵ�ƽ����Ӹߵ�ƽ��ʼ���͡�
			 		 	DrvPWM_Enable(DRVPWM_TIMER0, 0);
						da = ir_raw[i];
						DrvTIMER_Delay( E_TMR0, da) ;
					 	DrvPWM_Enable(DRVPWM_TIMER0, 1);
						da1 = ir_raw[i+1]; 
						DrvTIMER_Delay( E_TMR0, da1) ;
	 		 }		  

   	}

		DrvPWM_Enable(DRVPWM_TIMER0, 0);	//�رշ���
}


void EINT0callback()
{

	  	
  if((Learn_Finsh==0)&&(StateLearn))
  {
			
			 ir_raw[DataLenth]=ir_number;//���¼�����
				 ir_number=0; 			 //�������
				if(DataLenth==0) 			 //��ȡ��ʼ��ƽ���Ժ���������������Ϊ�ߵ͸ߵ�.
				{
				   if(DrvGPIO_GetBit(E_PORT3,E_PIN2)==0)
				   {
				   StartLeve = 0;
				   }
				   else					     //ȡ����ʼ��ƽ
				   {
				   StartLeve = 1;
				   }
				}
		
			   	DataLenth++; 			//��һ��������


   }
}


 /*
 
  checking  learn  ircode  need ping
 
 */

uint16_t Check_Learn_Finsh(void)
{

	  	//100MS	��Ϊ����������ı�־��������û�д���100MS�ĺ�������
			//����Ҫע����ѧϰ��ʱ��ѧ��һ��֮����ڰ�һ�£�����֮��
			 //��ʱ�Ѿ�ѧ�������ݲ����Է������ݵĳ�������	
			if((ir_number>10000)&&(DataLenth!=0)) 
			{								
			//ѧ����� ����0֮���ʾ�����Ѿ���¼����������
			//	Learn_Finsh = DataLenth-1
			//RAW���ݱ���¼��raw[1]-raw[Learn_Finsh]֮��
			//					
				Learn_Finsh= DataLenth-1;//	
				DataLenth=0;  			//׼����һ��ѧϰ��

				return 	Learn_Finsh; //����ѧϰ��ϵ����ݳ�������
			}
			else
			{
			
			    return 0;  //û��ѧϰ�������ѧϰ....
			
			}	


}


/*
	 ��λѧϰ
*/
void Reset_Finsh(void)
{
	 Learn_Finsh=0;
	 DataLenth=0;

}
																	

 /*

 �������ƿ���
 
 */
void Triget_Learn_state(uint8_t state)
{
	   if(state)
	   {
	   	 StateLearn=1;
		 init_IR_INTTRUPUT();
	   }
	   else
	   {
	   
	   	StateLearn=0;
		DrvGPIO_DisableEINT(E_EINT0_PIN);
	   }

}


void pfP2P3P4Callback(uint32_t u32P2Status, uint32_t u32P3Status, uint32_t u32P4Status)
{
	
}

void pfP0P1Callback	(uint32_t u32P0Status, uint32_t u32P1Status)
{
			
		if(u32P0Status&(1<<0))  //p0.0
		{
		
		
				
		}
	

}
  
void init_IR_INTTRUPUT(void)
{
	

 DrvGPIO_EnableEINT(E_EINT0_PIN,E_IO_BOTH_EDGE, E_MODE_EDGE, EINT0callback);
 
}


void init_PWM(void)
{

  

	S_DRVPWM_TIME_DATA_T sP_PWM; 

  /*Initialize PWM*/
    DrvPWM_Open();
	/* Select PWM engine clock */
   DrvPWM_SelectClockSource(DRVPWM_TIMER0, DRVPWM_INTERNAL_22M);
    /* PWM Timer property */
    sP_PWM.u8Mode = DRVPWM_AUTO_RELOAD_MODE;
    sP_PWM.u8HighPulseRatio = 50;   /* High Pulse peroid : Total Pulse peroid = 1 : 100 */
    sP_PWM.i32Inverter = 1;
    sP_PWM.u32Frequency =38000;
    DrvPWM_SetTimerClk(DRVPWM_TIMER0, &sP_PWM);
    /* Enable Output for PWM Timer */
    DrvPWM_SetTimerIO(DRVPWM_TIMER0, 1);

   	DrvGPIO_InitFunction(E_FUNC_PWM01_P4);
		
	DrvPWM_Enable(DRVPWM_TIMER0, 0);
	
	
	}
  extern  void initTMR0(void);
int  SaveIrCode2Flash(uint16_t *pt,uint16_t lenth)
{
		int k ;
   	//	DrvTIMER_Close(E_TMR0);	
k=	Malloc_Flash(FindEmptyBlock(),(uint32_t*)pt,lenth);
		initTMR0();
		 return k;

}

uint16_t  Send_IrCodeFlash(uint8_t size,uint16_t *p)
{	  uint16_t code_lenth;
		DrvTIMER_Close(E_TMR0);
	code_lenth=Mem_Flash_Get(size,(uint32_t*)p);
	   initTMR0();
	   if(code_lenth!=0xffff)
	   {
  Send_Ir_Code((uint8_t *)p,code_lenth );
  		}
	      	LEARN_DOWN;
			DrvTIMER_Delay( E_TMR0, 1000); 
			LEARN_UP;
	        DrvTIMER_Delay( E_TMR0, 3000); 
  //DrvTIMER_Close(E_TMR0);
      return code_lenth;

}

int GetIrCode2Flash(uint8_t size,uint16_t *p)
{
  
  return Mem_Flash_Get(size,(uint32_t*)p);

}




