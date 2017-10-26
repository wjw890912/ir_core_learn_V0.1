
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
uint8_t StateLearn=0;   //学习状态
uint8_t StartLeve=0;   //起始电平状态
uint16_t DataLenth=0;    //码型长度 
uint16_t  Learn_Finsh=0; //学习完毕
uint16_t ir_raw[512];    //数据码
uint32_t  ir_number;     //计数器

/*
 发送红外调制
 */
 
 void Send_Ir_Code(uint8_t *pt ,uint16_t Lenth )
{	
	uint16_t i;	
	uint32_t da,da1;	 
   	for(i=1;i<Lenth+1;i+=2)
   	{
		 	 if(!StartLeve)
			 {
					 	//如果捕获电平为低电平，则从低电平开始发送。
					   	DrvPWM_Enable(DRVPWM_TIMER0, 1);
						da = ir_raw[i];
						DrvTIMER_Delay( E_TMR0, da) ;
					 	DrvPWM_Enable(DRVPWM_TIMER0, 0);
						da1 = ir_raw[i+1]; 
						DrvTIMER_Delay( E_TMR0, da1) ;
			 }
			 else
			 {			//如果捕获电平为高电平，则从高电平开始发送。
			 		 	DrvPWM_Enable(DRVPWM_TIMER0, 0);
						da = ir_raw[i];
						DrvTIMER_Delay( E_TMR0, da) ;
					 	DrvPWM_Enable(DRVPWM_TIMER0, 1);
						da1 = ir_raw[i+1]; 
						DrvTIMER_Delay( E_TMR0, da1) ;
	 		 }		  

   	}

		DrvPWM_Enable(DRVPWM_TIMER0, 0);	//关闭发送
}


void EINT0callback()
{

	  	
  if((Learn_Finsh==0)&&(StateLearn))
  {
			
			 ir_raw[DataLenth]=ir_number;//更新计数器
				 ir_number=0; 			 //清计数器
				if(DataLenth==0) 			 //获取起始电平，以后按照数组依次排列为高低高低.
				{
				   if(DrvGPIO_GetBit(E_PORT3,E_PIN2)==0)
				   {
				   StartLeve = 0;
				   }
				   else					     //取得起始电平
				   {
				   StartLeve = 1;
				   }
				}
		
			   	DataLenth++; 			//下一个数据码


   }
}


 /*
 
  checking  learn  ircode  need ping
 
 */

uint16_t Check_Learn_Finsh(void)
{

	  	//100MS	作为数据码结束的标志，理论上没有大于100MS的红外间隔。
			//但是要注意再学习的时候学完一次之后得在按一下，触发之。
			 //此时已经学到了数据并可以返回数据的长度啦。	
			if((ir_number>10000)&&(DataLenth!=0)) 
			{								
			//学码完成 大于0之后表示数据已经记录进缓存中了
			//	Learn_Finsh = DataLenth-1
			//RAW数据被记录在raw[1]-raw[Learn_Finsh]之间
			//					
				Learn_Finsh= DataLenth-1;//	
				DataLenth=0;  			//准备下一次学习。

				return 	Learn_Finsh; //返回学习完毕的数据长度序数
			}
			else
			{
			
			    return 0;  //没有学习完毕正在学习....
			
			}	


}


/*
	 复位学习
*/
void Reset_Finsh(void)
{
	 Learn_Finsh=0;
	 DataLenth=0;

}
																	

 /*

 触发控制开关
 
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




