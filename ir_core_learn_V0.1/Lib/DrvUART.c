 /******************************************************************
 *	作者：小ARM菜菜
 *	时间：2012年
 *	地点：山东日照
 *  备注：All rights reserved！
 *   Q Q:925295580
 ******************************************************************/
 #include <stdio.h>
#include "M051Series.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "DrvUART.h"
#include "DrvSYS.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static PFN_DRVUART_CALLBACK  *g_pfnUART0callback = NULL;
static PFN_DRVUART_CALLBACK  *g_pfnUART1callback = NULL;

extern uint8_t UARTDATA;
 
/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt Handler                                                                                 	   */
/*---------------------------------------------------------------------------------------------------------*/
void UART0_IRQHandler(void)
{
    uint32_t u32uart0IntStatus;
	// UART_T * tUART;

//	tUART = (UART_T *)((uint32_t)UART0 ); 
	//	if(tUART->FSR.RX_EMPTY ==0)
	//	{
		
		//	 UARTDATA = tUART->DATA;



		//}
    u32uart0IntStatus = inpw(&UART0->ISR) ;

    if(g_pfnUART0callback != NULL)
	  {
        g_pfnUART0callback(u32uart0IntStatus);
 

      }
}


void UART1_IRQHandler(void)
{
    uint32_t u32IntStatus;

	u32IntStatus = inpw(&UART1->ISR) ;

    if(g_pfnUART1callback != NULL)
    {
       g_pfnUART1callback(u32IntStatus);
    }

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     BaudRateCalculator                                                                        */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             clk          		-[in] Uart Source Clock                                                */
/*	             baudRate     		-[in] User seting BaudRate                                             */
/*				 u32Port            -[in] UART Channel:  UART_PORT0 / UART_PORT1                           */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to set Baud Rate register according user's settings                  */
/*---------------------------------------------------------------------------------------------------------*/
/*-----BaudRate Configure----------------------------------------------------------------------------------*/

/*

Mode	DIV_X_EN	DIV_X_ONE	Divider X	BRD	(Baud rate equation)
-----------------------------------------------------------------------
0	    Disable	0	B	        A	        UART_CLK / [16 * (A+2)]
1	    Enable	0	B	        A	        UART_CLK / [(B+1) * (A+2)] , B must >= 8
2	    Enable	1	Don't care	A	        UART_CLK / (A+2), A must >=3

*/

static void BaudRateCalculator(uint32_t clk, uint32_t baudRate, E_UART_PORT u32Port)
{
  	int32_t tmp;
	int32_t div;

  	UART_T * tUART;
	tUART = (UART_T *)((uint32_t)UART0 + u32Port);  

    if ((((clk / baudRate)%16) < 2)  &&  (clk>(baudRate*16*2)))  /* Source Clock mod 16 < 2 => Using Divider X =16 (MODE#0) */ 
	{								  
		tUART->BAUD.DIV_X_EN = 0;
	    tUART->BAUD.DIV_X_ONE   = 0;
		tmp = clk / baudRate/16  -2;
	}
	else							  /* Source Clock mod 16 >2 => Up 5% Error BaudRate */
	{			 
	    tUART->BAUD.DIV_X_EN = 1;			  /* Try to Set Divider X = 1 (MODE#2)*/
	    tUART->BAUD.DIV_X_ONE   = 1;
		tmp = clk / baudRate  -2;

		if(tmp > 0xFFFF)			  /* If Divider > Range  */
		{
			tUART->BAUD.DIV_X_EN = 1;		  /* Try to Set Divider X up 10 (MODE#1) */
			tUART->BAUD.DIV_X_ONE   = 0;

			for(div = 8; div <16;div++)
			{
				if(((clk / baudRate)%(div+1))<3)
				{
					tUART->BAUD.DIVIDER_X   = div;
					tmp = clk / baudRate / (div+1) -2;
					break;
				}
			}
		}
	}

	tUART->BAUD.BRD = tmp; 

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     GetUartClk                                                                       		   */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             clk          		-[in] Uart Source Clock                                                */
/*	             baudRate     		-[in] User seting BaudRate                                             */
/*	             UART_BAUD_T *baud  -[in] Get User Settings                                                */
/* Returns:                                                                                                */
/*               Current Uart Clock  (Hz)                                                                  */
/* Description:                                                                                            */
/*               The function is used to get Uart clock                                                    */
/*---------------------------------------------------------------------------------------------------------*/

static uint32_t GetUartClock(void)
{
    uint32_t clk =0 , div;

    div = SYSCLK->CLKDIV.UART_N;                    /* According PLL Clock and UART_Divider to get clock */                                                 
                                                    
                                                    /* Check UART Clock Source Setting */
    if(SYSCLK->CLKSEL1.UART_S == 0)         
    {
        clk = DrvSYS_GetExtClockFreq()/ (div+1);;   /* Get External Clock From DrvSYS Setting */
    }
    else if(SYSCLK->CLKSEL1.UART_S == 1)
    {
        clk = DrvSYS_GetPLLClockFreq()/ (div+1);
    }
    else
        clk = __IRC22M/ (div+1);                    /* Clock 22Mhz  */

    return clk;

}

 

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_SetRTS				                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 u32Port 		-[in]   UART Channel:  UART_PORT0 / UART_PORT1                             */
/*               uint8_t	   	-[in]   RTS Value 					          	         				   */
/*                           Set 0: Drive RTS pin to logic 1 (If the LEV_RTS set to low level triggered).  */
/*                                  Drive RTS pin to logic 0 (If the LEV_RTS set to high level triggered)  */
/*                           Set 1: Drive RTS pin to logic 0 (If the LEV_RTS set to low level triggered)   */
/*                                   Drive RTS pin to logic 1 (If the LEV_RTS set to high level triggered) */
/*                           Note. Lev_RTS is RTS Trigger Level. 0 is low level and 1 is high level        */
/*                                                                                                         */
/*               u16TriggerLevel - [in]   RTS Trigger Level :DRVUART_FIFO_1BYTES to DRVUART_FIFO_62BYTES   */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is used to set RTS information	                                           */
/*---------------------------------------------------------------------------------------------------------*/
void DrvUART_SetRTS(E_UART_PORT u32Port,uint8_t u8Value,uint16_t u16TriggerLevel)
{
	
	UART_T * tUART;

	tUART = (UART_T *)((uint32_t)UART0 + u32Port);  

	tUART->MCR.RTS = u8Value;

	tUART->FCR.RTS_TRI_LEV = u16TriggerLevel;
}	
	



/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_Open                                                                        	   */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 u32Port -[in] UART Channel:  UART_PORT0 / UART_PORT1                           		   */
/*               sParam  -[in] the struct parameter to configure UART                                      */
/*                         include of                                                                      */
/*                           * u32BaudRate - Baud rate 													   */									        
/*                           * u8cParity   - DRVUART_PARITY_NONE / DRVUART_PARITY_EVEN / DRVUART_PARITY_ODD*/
/*                           * u8cDataBits - DRVUART_DATA_BITS_5 / DRVUART_DATA_BITS_6 					   */
/*									  	 	 DRVUART_DATA_BITS_7 / DRVUART_DATA_BITS_8        			   */
/*                           * u8cStopBits - DRVUART_STOPBITS_1 / STOPBITS_1_5 / STOPBITS_2   			   */
/*                           * u8cRxTriggerLevel   - LEVEL_1_BYTE to LEVEL_62_BYTES                        */
/*                           * u8TimeOut - Time out value	                                     		   */
/*                                                                                                         */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               E_DRVUART_ERR_PORT_INVALID         												       */
/*               E_DRVUART_ERR_PARITY_INVALID                    										   */
/*               E_DRVUART_ERR_DATA_BITS_INVALID               											   */
/*               E_DRVUART_ERR_STOP_BITS_INVALID                										   */
/*               E_DRVUART_ERR_TRIGGERLEVEL_INVALID      												   */
/*               E_DRVUART_ERR_ALLOC_MEMORY_FAIL                 										   */
/*               E_SUCCESS                                                                          	   */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is used to initialize UART. It consists of baud-rate, parity, data-bits,     */
/*               stop-bits, rx-trigger-level and timeout interval settings.                                */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvUART_Open(E_UART_PORT u32Port, STR_UART_T *sParam)
{

	UART_T * tUART;
   
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check UART port                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((u32Port != UART_PORT0) &&
        (u32Port != UART_PORT1))
    {

		return E_DRVUART_ERR_PORT_INVALID;
    }


    /*-----------------------------------------------------------------------------------------------------*/
    /* Check the supplied parity                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((sParam->u8cParity != DRVUART_PARITY_NONE) &&
        (sParam->u8cParity != DRVUART_PARITY_EVEN) &&
        (sParam->u8cParity != DRVUART_PARITY_ODD)  &&
        (sParam->u8cParity != DRVUART_PARITY_MARK) &&
        (sParam->u8cParity != DRVUART_PARITY_SPACE))
    {
 		return E_DRVUART_ERR_PARITY_INVALID;		
    }
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check the supplied number of data bits                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    else if ((sParam->u8cDataBits != DRVUART_DATABITS_5) &&
             (sParam->u8cDataBits != DRVUART_DATABITS_6) &&
             (sParam->u8cDataBits != DRVUART_DATABITS_7) &&
             (sParam->u8cDataBits != DRVUART_DATABITS_8))
    {
 		return E_DRVUART_ERR_DATA_BITS_INVALID;	 
    }
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check the supplied number of stop bits                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    else if ((sParam->u8cStopBits != DRVUART_STOPBITS_1) &&
             (sParam->u8cStopBits != DRVUART_STOPBITS_2) &&
             (sParam->u8cStopBits != DRVUART_STOPBITS_1_5)
             )
    {
      	return E_DRVUART_ERR_STOP_BITS_INVALID;	 
    }


    /*-----------------------------------------------------------------------------------------------------*/
    /* Check the supplied nember of trigger level bytes                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    else if ((sParam->u8cRxTriggerLevel != DRVUART_FIFO_1BYTES) &&
             (sParam->u8cRxTriggerLevel != DRVUART_FIFO_4BYTES) &&
             (sParam->u8cRxTriggerLevel != DRVUART_FIFO_8BYTES) &&
             (sParam->u8cRxTriggerLevel != DRVUART_FIFO_14BYTES)&&
             (sParam->u8cRxTriggerLevel != DRVUART_FIFO_30BYTES)&&
             (sParam->u8cRxTriggerLevel != DRVUART_FIFO_46BYTES)&&
             (sParam->u8cRxTriggerLevel != DRVUART_FIFO_62BYTES))
    {
		return E_DRVUART_ERR_TRIGGERLEVEL_INVALID;		 
    }				

	if(u32Port == UART_PORT0)
	{
		/* Reset IP */
		SYS->IPRSTC2.UART0_RST = 1;
		SYS->IPRSTC2.UART0_RST = 0;
		
		/* Enable UART clock */
	    SYSCLK->APBCLK.UART0_EN = 1;
	}
	else 
	{
		/* Reset IP */
		SYS->IPRSTC2.UART1_RST = 1;
		SYS->IPRSTC2.UART1_RST = 0;
		
		/* Enable UART clock */
	    SYSCLK->APBCLK.UART1_EN = 1;
	}
	tUART = (UART_T *)((uint32_t)UART0 + u32Port); 


	/* Tx FIFO Reset & Rx FIFO Reset & FIFO Mode Enable */
  	tUART->FCR.TFR =1;
  	tUART->FCR.RFR =1;

	/* Set Rx Trigger Level */
	tUART->FCR.RFITL = sParam->u8cRxTriggerLevel;  
	
	/* Set Parity & Data bits & Stop bits */
	tUART->LCR.SPE	=((sParam->u8cParity)&0x4)?1:0;
	tUART->LCR.EPE	=((sParam->u8cParity)&0x2)?1:0;
	tUART->LCR.PBE	=((sParam->u8cParity)&0x1)?1:0;
		
	tUART->LCR.WLS	=sParam->u8cDataBits;
	tUART->LCR.NSB	=sParam->u8cStopBits;
		
	/* Set Time-Out */
	tUART->TOR.TOIC	=sParam->u8TimeOut;

    /* Check Clock Source */
    if(SYSCLK->PWRCON.XTL12M_EN)
        SYSCLK->CLKSEL1.UART_S = 0;
    else
    {
        if(SYSCLK->PLLCON.PD==0)
            SYSCLK->CLKSEL1.UART_S = 1;
        else
            if(SYSCLK->PWRCON.OSC22M_EN)
                SYSCLK->CLKSEL1.UART_S = 3;
            else
                return E_DRVUART_ERR_OPEN;
    }

	/* Set BaudRate */
	BaudRateCalculator(GetUartClock(), sParam->u32BaudRate, u32Port);

	return E_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DrvUART_Close                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*				 	u32Port 	 -[in] UART Channel:  UART_PORT0 / UART_PORT1                              */
/* RETURN                                                                                                  */
/*                  None                                                                                   */
/* Description:                                                                                            */
/*                  The function is used to disable UART clock, disable ISR and clear callback  		   */
/*                  function pointer after checking the TX empty.                                          */
/*---------------------------------------------------------------------------------------------------------*/

void DrvUART_Close(E_UART_PORT u32Port)
{
	if(u32Port == UART_PORT1)	
	{
		while(!UART1->FSR.TE_FLAG);
		SYSCLK->APBCLK.UART1_EN = 0;
		g_pfnUART1callback = NULL;
	        NVIC_DisableIRQ (UART1_IRQn); 
	}
	else
	{
		while(!UART0->FSR.TE_FLAG);
		SYSCLK->APBCLK.UART0_EN = 0;
		g_pfnUART0callback = NULL;
        NVIC_DisableIRQ(UART0_IRQn);      
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_EnableInt                                                                     	   */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u32Port		  -[in] UART Channel:  UART_PORT0 / UART_PORT1                             */
/*               u32InterruptFlag -[in]	DRVUART_WAKEUPINT/DRVUART_RLSINT/DRVUART_BUFERRINT                 */
/*										DRVUART_MOSINT/DRVUART_THREINT/DRVUART_RDAINT/DRVUART_TOUTINT      */
/*               pfncallback      -[in] A function pointer for callback function                           */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to enable specified UART interrupt, install the callback             */
/*               function and enable NVIC UART IRQ                                                         */
/* Remark:                                                                                                 */
/*               When enable DRVUART_THREINT interrupt source, the interrup will happen continouly if      */
/*               TX_FIFO is empty. Remind you thant be careful to use it.                                  */
/*               Please reference TRM to get more information.                                             */
/*---------------------------------------------------------------------------------------------------------*/
void
DrvUART_EnableInt(
	E_UART_PORT   u32Port,
	uint32_t  u32InterruptFlag,
	PFN_DRVUART_CALLBACK pfncallback
)
{
	UART_T * tUART;
    
	tUART = (UART_T *)((uint32_t)UART0 + u32Port); 

	tUART->IER.RDA_IEN		=(u32InterruptFlag & DRVUART_RDAINT)?1:0 ;
	tUART->IER.THRE_IEN		=(u32InterruptFlag & DRVUART_THREINT)?1:0;
	tUART->IER.RLS_IEN		=(u32InterruptFlag & DRVUART_RLSINT)?1:0;
	tUART->IER.MODEM_IEN	=(u32InterruptFlag & DRVUART_MOSINT)?1:0;
		
	tUART->IER.TIME_OUT_EN	=(u32InterruptFlag & DRVUART_TOUTINT)?1:0;   	/* Time-out count enable */
	tUART->IER.RTO_IEN		=(u32InterruptFlag & DRVUART_TOUTINT)?1:0;		/* Time-out INT enable */
	tUART->IER.BUF_ERR_IEN	=(u32InterruptFlag & DRVUART_BUFERRINT)?1:0;
	tUART->IER.WAKE_EN		=(u32InterruptFlag & DRVUART_WAKEUPINT)?1:0;

    if(u32Port == UART_PORT0)											   	/* Install Callback function */
    {
		g_pfnUART0callback = pfncallback;        
		NVIC_SetPriority(UART0_IRQn, (1<<__NVIC_PRIO_BITS) - 2);
		NVIC_EnableIRQ(UART0_IRQn); 
    }
	else 
	{
		g_pfnUART1callback = pfncallback;
		NVIC_SetPriority(UART1_IRQn, (1<<__NVIC_PRIO_BITS) - 2);
		NVIC_EnableIRQ(UART1_IRQn); 
	}
}

																  
/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_IsIntEnabled                                                                 	   */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u32Port	   	  -[in] UART Channel:  UART_PORT0 / UART_PORT1                             */
/*               u32InterruptFlag -[in]	DRVUART_WAKEUPINT/DRVUART_RLSINT/DRVUART_BUFERRINT                 */
/*										DRVUART_MOSINT/DRVUART_THREINT/DRVUART_RDAINT/DRVUART_TOUTINT      */
/* Returns:                                                                                                */
/*               Specified Interrupt Flag Set or clear                                                     */
/* Description:                                                                                            */
/*               The function is used to get the "interrupt enable" status								   */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvUART_IsIntEnabled(E_UART_PORT	u32Port,uint32_t u32InterruptFlag)
{

	if(u32Port == UART_PORT0)					/* Read IER Register and check specified flag is enable */
		return ((inpw(&UART0->IER) &
		 (u32InterruptFlag & (DRVUART_WAKEUPINT |DRVUART_BUFERRINT| 
		 		DRVUART_TOUTINT | DRVUART_MOSINT | DRVUART_RLSINT| DRVUART_THREINT | DRVUART_RDAINT))))?1:0;
	else 
		return ((inpw(&UART1->IER) & 
		(u32InterruptFlag & ( DRVUART_WAKEUPINT |DRVUART_BUFERRINT|
		 		DRVUART_TOUTINT | DRVUART_MOSINT | DRVUART_RLSINT | DRVUART_THREINT | DRVUART_RDAINT))))?1:0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_DisableInt                                                                   	   */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u32Port	   	  -[in] UART Channel:  UART_PORT0 / UART_PORT1                             */
/*               u32InterruptFlag -[in]	DRVUART_WAKEUPINT/DRVUART_RLSINT/DRVUART_BUFERRINT                 */
/*										DRVUART_MOSINT/DRVUART_THREINT/DRVUART_RDAINT/DRVUART_TOUTINT      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to disable UART specified interrupt, uninstall the call back         */
/*               function and disable NVIC UART IRQ                                                        */
/*---------------------------------------------------------------------------------------------------------*/
void DrvUART_DisableInt(E_UART_PORT u32Port,uint32_t u32InterruptFlag)
{
	if(u32Port == UART_PORT0)			   
	{
		outpw(&UART0->IER + u32Port,inpw(&UART0->IER + u32Port) &~u32InterruptFlag);	   /* Disable INT  */
	}
	
	if(u32InterruptFlag & DRVUART_TOUTINT)			  							 /* Disable Counter Enable */
	{
		if(u32Port == UART_PORT0)
			UART0->IER.RTO_IEN = 0;
		else 
			UART1->IER.RTO_IEN = 0;
	}
	
	switch (u32Port)									   			 /* Disable Callback function and NVIC */
    {
        case UART_PORT0:
			g_pfnUART0callback = NULL;
            NVIC_DisableIRQ(UART0_IRQn);   
            break;

        case UART_PORT1:
			g_pfnUART1callback = NULL;
           	NVIC_DisableIRQ(UART1_IRQn);    
            break;
        
        default:
            break;
    }

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_ClearIntFlag                                                                      */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               u32Port              -[in]   UART Channel:  UART_PORT0 / UART_PORT1                       */
/*               u32InterruptFlag     -[in]   Interrupt Flag : DRVUART_RLSINT_FLAG/DRVUART_MOSINT_FLAG     */
/*                                                             DRVUART_BUFERRINT_FLAG                      */
/* Returns:                                                                                                */
/*               E_SUCCESS      Successful                                                                 */
/* Description:                                                                                            */
/*               The function is used to clear UART specified interrupt flag                               */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvUART_ClearIntFlag(E_UART_PORT u32Port,uint32_t u32InterruptFlag)
{
    UART_T * tUART;

    tUART = (UART_T *)((uint32_t)UART0 + u32Port); 

    if((u32InterruptFlag & DRVUART_RDAINT_FLAG)  || 
       (u32InterruptFlag & DRVUART_THREINT_FLAG) ||
       (u32InterruptFlag & DRVUART_TOUTINT_FLAG) )
    {
        return E_DRVUART_ARGUMENT;
    }

    if(u32InterruptFlag & DRVUART_RLSINT_FLAG)          /* clear Receive Line Status Interrupt */      
    {
        tUART->FSR.BIF = 1;
        tUART->FSR.FEF = 1;
        tUART->FSR.PEF = 1;
    }

    if(u32InterruptFlag & DRVUART_MOSINT_FLAG)          /* clear Modem Interrupt */    
        tUART->MSR.DCTSF = 1;


    if(u32InterruptFlag & DRVUART_BUFERRINT_FLAG)       /* clear Buffer ErrorInterrupt */      
    {
        tUART->FSR.TX_OVER_IF = 1;
        tUART->FSR.RX_OVER_IF = 1;
    }
    return E_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_GetIntStatus                                                                 	   */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u32Port	   	  	  -[in]   UART Channel:  UART_PORT0 / UART_PORT1                       */
/*               u32InterruptFlag     -[in]   DRVUART_LININT_FLAG / DRVUART_BUFERRINT_FLAG  		       */
/*											  DRVUART_TOUTINT_FLAG/DRVUART_MOSINT_FLAG                     */
/*                                            DRVUART_RLSINT_FLAG/DRVUART_THREINT_FLAG/DRVUART_RDAINT_FLAG */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               0: The specified interrupt is not happened.                                               */
/*               1: The specified interrupt is happened.                                                   */
/*               E_DRVUART_ARGUMENT: Error Parameter                                                       */
/* Description:                                                                                            */
/*               The function is used to get the interrupt status										   */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvUART_GetIntStatus(E_UART_PORT u32Port,uint32_t u32InterruptFlag)
{

	UART_T * tUART;

	tUART = (UART_T *)((uint32_t)UART0 + u32Port); 
   
    return (tUART->u32ISR & u32InterruptFlag)>>u32InterruptFlag ;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_GetCTSInfo				                                                           */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 u32Port	  	      -[in]   UART Channel:  UART_PORT0 / UART_PORT1                       */
/*               pu8CTSValue   		  -[out]   Buffer to store the CTS Value           	         		   */
/*                                             return current CTS pin state                                */
/*				 pu8CTSChangeState	  -[out]   Buffer to store the CTS Change State                        */
/*                                             return CTS pin status is changed or not 1:Changed 0:Not yet */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to get CTS pin value and detect CTS change state                     */
/*---------------------------------------------------------------------------------------------------------*/

void DrvUART_GetCTS(E_UART_PORT u32Port,uint8_t *pu8CTSValue,	uint8_t	*pu8CTSChangeState)
{
	UART_T * tUART;

	tUART = (UART_T *)((uint32_t)UART0 + u32Port);  

 	*pu8CTSValue 		= tUART->MSR.CTS_ST;
	*pu8CTSChangeState 	= tUART->MSR.DCTSF;
}	

 
/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_Read                                                                              */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u32Port   	   	  	  -[in]   UART Channel:  UART_PORT0 / UART_PORT1                       */
/*               pu8RxBuf   		  -[in]   Specify the buffer to receive the data of receive FIFO       */
/*               u32ReadBytes  		  -[in]   Specify the bytes number of data.     		               */
/* Returns:                                                                                                */
/*               E_SUCCESS                                                                          	   */
/* Description:                                                                                            */
/*               The function is used to read Rx data from RX FIFO and the data will be stored in pu8RxBuf */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvUART_Read(E_UART_PORT	u32Port,uint8_t	*pu8RxBuf, uint32_t	u32ReadBytes)
{
    uint32_t  u32Count, u32delayno;

	UART_T * tUART;

	tUART = (UART_T *)((uint32_t)UART0 + u32Port);  

    for (u32Count=0; u32Count < u32ReadBytes; u32Count++)
    {
		 u32delayno=0;
	     while (tUART->FSR.RX_EMPTY ==1)					    /* Check RX empty => failed */	   
	     {
	     	u32delayno++;        
	        if ( u32delayno >= 0x40000000 )        
	        	return E_DRVUART_ERR_TIMEOUT;               
         }
         pu8RxBuf[u32Count] = tUART->DATA;					    /* Get Data from UART RX  */
    }

    return E_SUCCESS;
	
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_Write	                                                                           */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u32Port   	   	  	  -[in]   UART Channel:  UART_PORT0 / UART_PORT1                       */
/*               pu8RxBuf   		  -[in]   Specify the buffer to send the data to transmission FIFO.    */
/*               u32ReadBytes  		  -[in]   Specify the byte number of data.         		               */
/* Returns:                                                                                                */
/*               E_SUCCESS                                                                          	   */
/*                                                                                                         */
/* Description:                                                                                            */
/*               TThe function is to write data to TX buffer to transmit data by UART                      */
/* Note:                                                                                                   */
/*               In IrDA Mode, the BAUD RATE configure MUST to use MODE # 0                                */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvUART_Write(E_UART_PORT	u32Port,uint8_t	*pu8TxBuf, 	uint32_t u32WriteBytes)
{
    uint32_t  u32Count, u32delayno;

	UART_T * tUART;

	tUART = (UART_T *)((uint32_t)UART0 + u32Port);  

    for (u32Count=0; u32Count<u32WriteBytes; u32Count++)
    {
       u32delayno = 0;
	   while (tUART->FSR.TE_FLAG !=1)						    	/* Wait Tx empty and Time-out manner */
       {
       	    u32delayno++;
       	    if ( u32delayno >= 0x40000000 )     	    
       	       return E_DRVUART_ERR_TIMEOUT;     	       
				   
       }
	   tUART->DATA =pu8TxBuf[u32Count];						/* Send UART Data from buffer */
    }

    return E_SUCCESS;
	
  
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_SetFnIRDA                                                                         */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u32Port	   	  	  -[in]   UART Channel:  UART_PORT0 / UART_PORT1                       */
/*               STR_IRCR_T   		  -[in]   Ther stucture of IRCR  	  	         		               */
/*                                            It includes of                                               */
/*                                               u8cTXSelect: 1: Enable Irda transmit function.(TX mode)   */
/*                                                            0: Disable Irda transmit function.(RX mode)  */
/*                                               u8cInvTX: Inverse TX signal                               */
/*                                               u8cInvRX: Inverse RX signal                               */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               None                                                                                	   */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is used to configure IRDA relative settings.                                 */
/*               It consists of TX or RX mode and Inverse TX or RX signals.                                */
/*---------------------------------------------------------------------------------------------------------*/
void DrvUART_SetFnIRDA(E_UART_PORT u32Port,STR_IRCR_T *str_IRCR )
{
	UART_T * tUART;

	tUART = (UART_T *)((uint32_t)UART0 + u32Port);  

	tUART->FUNSEL.FUN_SEL 	= FUN_IRCR;							   	/* Enable IrDA function and configure */
	tUART->IRCR.TX_SELECT	= (str_IRCR->u8cTXSelect) ?1:0;
	tUART->IRCR.INV_TX		= str_IRCR->u8cInvTX ;
	tUART->IRCR.INV_RX		= str_IRCR->u8cInvRX ;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_SetFnRS485                                                                        */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u32Port   	   	  	  -[in]   UART Channel:  UART_PORT0 / UART_PORT1                       */
/*               STR_RS485_T   		  -[in]   Ther stucture of RS485  	  	         		               */
/*                                            It includes of                                               */
/*                                               u8cModeSelect: MODE_RS485_AUD / MODE_RS485_AAD            */
/*                                                              MODE_RS485_NMM                             */
/*                                               u8cAddrEnable: Enable or Disable RS-485 Address Detection */
/*                                               u8cAddrValue:  Match Address Value                        */
/*                                               u8cDelayTime:  Set transmit delay time value              */
/*                                               u8cRxDisable:  Enable or Disable receiver function        */
/* Returns:                                                                                                */
/*               None                                                                               	   */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is to Set RS485 Control Register                                             */
/*---------------------------------------------------------------------------------------------------------*/

void DrvUART_SetFnRS485(E_UART_PORT u32Port,STR_RS485_T *str_RS485)
{
	
	UART_T * tUART;

	tUART = (UART_T *)((uint32_t)UART0 + u32Port);  

	tUART->FUNSEL.FUN_SEL 			= FUN_RS485;					/* Enable RS485 function and configure */
	tUART->ALTCON.RS485_ADD_EN	    =  str_RS485-> u8cAddrEnable ?1:0;
	tUART->ALTCON.ADDR_MATCH		=  str_RS485-> u8cAddrValue ;
	tUART->ALTCON.RS485_NMM		    = (str_RS485-> u8cModeSelect & MODE_RS485_NMM)?1:0;	
	tUART->ALTCON.RS485_AAD		    = (str_RS485-> u8cModeSelect & MODE_RS485_AAD)?1:0;	
	tUART->ALTCON.RS485_AUD		    = (str_RS485-> u8cModeSelect & MODE_RS485_AUD)?1:0;	
	tUART->TOR.DLY                  =  str_RS485-> u8cDelayTime;
	tUART->FCR.RX_DIS               =  str_RS485-> u8cRxDisable;
	tUART->MCR.LEV_RTS = 0;
	

} 
/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_GetVersion                                                                        */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             None                                                                                 	   */
/* Returns:                                                                                                */
/*               Version Number                                                                            */
/* Side effects:                                                                                           */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is used to get  DrvUART Version Number                                       */
/*---------------------------------------------------------------------------------------------------------*/
int32_t	DrvUART_GetVersion(void)
{
	return DRVUART_VERSION_NUM;
	
}






