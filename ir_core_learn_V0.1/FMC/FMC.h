// File Name:DrvFMC.H
// Author:BY СARM�˲�
// Date: 2012��
// Q Q:925295580

#ifndef _FMC_H
#define _FMC_H
	 //�洢
#define   ADD0          0x1F000
#define   ADD1          0x1F004
#define   ADD2          0x1F008
#define   ADD3          0x1F00C

#define   ADD4          0x1F010
#define   ADD5          0x1F014
#define   ADD6          0x1F018
#define   ADD7          0x1F01C

#define   ADD8          0x1F020
#define   ADD9          0x1F024
#define   ADD10         0x1F028
#define   SUM           0x1F02C
#define   BOOT          0x1F030






void initFMC(void);
void write_ISP_DATA_FLASH(uint32_t  ADDR,uint32_t DATA );//дDATAFLASH
uint32_t read_ISP_DATA_FLASH(uint32_t ADDR); //��DATA FLASH
void Erase_ISP_DATA_FLASH(uint32_t ADDR);//��������512�ֽ�
 uint32_t Read_UID_M0(uint32_t addr);

 unsigned char Save_Authentication_Add(uint32_t data);
  unsigned char Lode_Authenticated_Add(uint32_t *padd_group);
#endif

