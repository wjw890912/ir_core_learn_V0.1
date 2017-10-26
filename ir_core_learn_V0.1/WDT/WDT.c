#include <stdio.h>
#include <string.h>
#include "M051Series.h"
#include "DrvSYS.h"
#include "DrvTIMER.h"
#include "WDT.h"
/*狗狗饿了，就每人喂他，他就叫唤！系统异常，马上准备复位硬件*/
void WDT_ISR_CALLBACK(uint32_t wakup_falge)
{
	 //dangerous!
	 //dangerous!
	 //dangerous!
	 //dangerous!

}
void init_WDT()
{
  DrvSYS_UnlockProtectedReg();
  DrvSYS_SetIPClock(E_SYS_WDT_CLK,1); //使能始终
  DrvSYS_SelectIPClockSource(E_SYS_WDT_CLKSRC,3 );//10K晶振
  while(E_DRVWDT_OPEN==DrvWDT_Open(E_WDT_LEVEL6));//安装时间间隔=6.5-6.6s
  DrvWDT_Ioctl(E_WDT_IOC_ENABLE_INT,0);	  
  DrvWDT_Ioctl(E_WDT_IOC_ENABLE_RESET_FUNC,0);	
  DrvWDT_InstallISR((WDT_CALLBACK)WDT_ISR_CALLBACK);//安装
  DrvWDT_Ioctl(E_WDT_IOC_START_TIMER,0); 
  DrvSYS_LockProtectedReg();
}
