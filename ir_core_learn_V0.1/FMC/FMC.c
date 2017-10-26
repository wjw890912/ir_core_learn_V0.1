// File Name:DrvFMC.C
// Author:СARM�˲�
// Date: 2012��
 //Q Q:925295580
#include "DrvSYS.h"
#include "DrvFMC.h"
#include "FMC.h"
 /******************************************************************************************
 ****
 ****
 ****
 ****	         DATA FLASH����
 ****
 ****
 ****
 ******************************************************************
 **********************************************************************************************/
 void initFMC()
 {
 	 //APIҪ��رռĴ�������
DrvSYS_UnlockProtectedReg();//�رռĴ�������
DrvFMC_EnableISP();//����ISP
DrvSYS_LockProtectedReg();//�����Ĵ�������
 
 }

 void write_ISP_DATA_FLASH(uint32_t  ADDR,uint32_t DATA )
 {
 
      DrvSYS_UnlockProtectedReg();//�رռĴ�������
	  DrvFMC_Write(ADDR, DATA);
	  DrvSYS_LockProtectedReg();//�����Ĵ�������
 
 }
 uint32_t read_ISP_DATA_FLASH(uint32_t ADDR)
 {
 		 uint32_t da;
 
 	  DrvSYS_UnlockProtectedReg();//�رռĴ�������
	  DrvFMC_Read( ADDR, &da);
	  DrvSYS_LockProtectedReg();//�����Ĵ�������
 		 return da;
 }
 uint32_t Read_UID_M0(uint32_t addr)
 {
 		  uint32_t da;
			  	addr=addr;
 	DrvSYS_UnlockProtectedReg();//�رռĴ�������
DrvFMC_ReadUID(addr, &da);

	  DrvSYS_LockProtectedReg();//�����Ĵ�������
	return da;


 }
 void Erase_ISP_DATA_FLASH(uint32_t ADDR)
 {
 
 	
 	  DrvSYS_UnlockProtectedReg();//�رռĴ�������
      DrvFMC_Erase(ADDR);
	  DrvSYS_LockProtectedReg();//�����Ĵ�������
 
 }

 unsigned char Lode_Authenticated_Add(uint32_t *padd_group)
 {
 			   uint32_t temp;
 
 	
		
		   
		       temp  = 	read_ISP_DATA_FLASH(ADD0) ;
			   if(temp!=0xFFFFFFFF){
			   *padd_group = temp;

			   }
			   temp  = 	read_ISP_DATA_FLASH(ADD1) ;
			   if(temp!=0xFFFFFFFF){
			   *(padd_group+1) = temp;
			   }
			   temp  = 	read_ISP_DATA_FLASH(ADD2) ;
			   if(temp!=0xFFFFFFFF){
			   *(padd_group+2) = temp;
			   }
			 /*  temp  = 	read_ISP_DATA_FLASH(ADD3) ;
			   if(temp!=0xFFFFFFFF){
			   *add_group = temp;
			   }
			   temp  = 	read_ISP_DATA_FLASH(ADD4) ;
			   if(temp!=0xFFFFFFFF){
			   *add_group = temp;
			   }
			   temp  = 	read_ISP_DATA_FLASH(ADD5) ;
			   if(temp!=0xFFFFFFFF){
			   *add_group = temp;
			   }
			   temp  = 	read_ISP_DATA_FLASH(ADD6) ;
			   if(temp!=0xFFFFFFFF){
			   *add_group = temp;
			   }
			   temp  = 	read_ISP_DATA_FLASH(ADD7) ;
			   if(temp!=0xFFFFFFFF){
			   *add_group = temp;
			   }
		
				  */
		
		
		
			


 	  return 1;
 
 } 
 unsigned char Save_Authentication_Add(uint32_t data)
 {
 
 	   if(read_ISP_DATA_FLASH(ADD0)==0xFFFFFFFF)
	   {
	   write_ISP_DATA_FLASH(ADD0,data);
	   	 return 1;
	   }
	   else
	   if(read_ISP_DATA_FLASH(ADD1)==0xFFFFFFFF)
	   {
	   write_ISP_DATA_FLASH(ADD1,data);
	   	   return 1;
	   }
	   else
	   if(read_ISP_DATA_FLASH(ADD2)==0xFFFFFFFF)
	   {
	   write_ISP_DATA_FLASH(ADD2,data);
	   		return 1;
	   }
 
 
 	return 0;
 

 
 }
