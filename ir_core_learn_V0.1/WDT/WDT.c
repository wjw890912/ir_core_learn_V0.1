#include <stdio.h>
#include <string.h>
#include "M051Series.h"
#include "DrvSYS.h"
#include "DrvTIMER.h"
#include "WDT.h"
/*�������ˣ���ÿ��ι�������ͽл���ϵͳ�쳣������׼����λӲ��*/
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
  DrvSYS_SetIPClock(E_SYS_WDT_CLK,1); //ʹ��ʼ��
  DrvSYS_SelectIPClockSource(E_SYS_WDT_CLKSRC,3 );//10K����
  while(E_DRVWDT_OPEN==DrvWDT_Open(E_WDT_LEVEL6));//��װʱ����=6.5-6.6s
  DrvWDT_Ioctl(E_WDT_IOC_ENABLE_INT,0);	  
  DrvWDT_Ioctl(E_WDT_IOC_ENABLE_RESET_FUNC,0);	
  DrvWDT_InstallISR((WDT_CALLBACK)WDT_ISR_CALLBACK);//��װ
  DrvWDT_Ioctl(E_WDT_IOC_START_TIMER,0); 
  DrvSYS_LockProtectedReg();
}
