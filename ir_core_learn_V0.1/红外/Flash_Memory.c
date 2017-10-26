
#include <stdint.h>
#include <string.h>
#include "Flash_Memory.h"
#include "FMC.h"

 //char  Membuf[512];//用于置换扇区


 LNode Nbuf[4];	//4*1024=4k
 LinkList node=&Nbuf[0];

 Pool flash_onchip;
 LinkPool flash=&flash_onchip;
 //初始化内存管理FLASH的相关数据。
 void  init_flash_pool()
 {			 
 			char i ;
 		 flash->tail=&Nbuf[0];	  //flash 地址
		 flash->size=4096;		  //FLASH 总大小
		 flash->block_size=1024;	  //内部的块大小

		for(i=0;i<4;i++)
		{
		 node->tail=(0x0001f000+(i*1024)); //加载自身地址
		 node->Data=(char*)NULL;
		 node->lenth=1024;//512字节
		 node->CRC16=0;//0
		 node->used=UNUSED;//未使用
		 node->next=(0x0001f000+((i+1)*1024));//下一个
		  node++;
		}
	 

 }

// 插入元素到FLASH链表中
int ListInsert(LinkList elem ,uint32_t *pt,uint16_t lenth,uint8_t add)
{			 uint32_t stadd,endadd,temp;
			 uint16_t i;


		if(elem==(LinkList)NULL)return ERRO;
		if( elem->used==USED )return ERRO;
		 elem->Data=(char*)pt; //指针指向数据域
		 elem->lenth=lenth;//   数据长度
		 elem->CRC16=0;//0 		校验数据
		 elem->used=USED;//使用
		//操作FLASH

		i=sizeof(LNode);
         memcpy ( (uint32_t *)(pt+(256-(i/4))),(uint32_t *)elem,i);  //将参数导入到内存中一并写入FLASH
		 stadd=elem->tail;//开始地址
		 if(stadd==0)
		 {
		  stadd=(add*1024)+0x0001f000;
		 
		 }
		 endadd=(elem->next);	//结束地址

		 Erase_ISP_DATA_FLASH(elem->tail);		 //擦除扇区0
		 Erase_ISP_DATA_FLASH((elem->next)-512);		 //擦除扇区1
		   stadd=elem->tail;//开始地址
		   for(i=0;i<(flash->block_size/4);i++)
		 {
				temp=*(pt+i);
		  write_ISP_DATA_FLASH(stadd,temp );	   //写入
		   	 stadd+=4;
		 }
		


		

}

//从指定的块中读取数据。返回长度。
int ListGet(LinkList elem ,uint32_t *pt,uint8_t add)
{		uint32_t stadd;
		uint16_t i;
	  if(elem==(LinkList)NULL)return ERRO;
	//  if( elem->used==UNUSED) return  ERRO;
	   stadd=elem->tail;//开始地址
	   if(stadd==0)
	   {
	   	stadd=(add*1024)+0x0001f000;
	   }
		// elem->Data=(char*)pt; //指针指向数据域
//		 elem->lenth=lenth;//   数据长度
		// elem->CRC16=0;//0 		校验数据
	   for(i=0;i<(flash->block_size/4);i++)
		 {
		   *(pt+i)=read_ISP_DATA_FLASH(stadd)	;//开始读取头地址数据到PT
		   	stadd+=4;
		 }			
		 	  i=sizeof(LNode);
	    memcpy ((uint32_t *)elem,(uint32_t *)(pt+(256-(i/4))),i); 
		return elem->lenth;//数据长度

} 

//删除元素在FLASH 管理链表中
int ListDelete(LinkList elem)
{

		 if(elem==(LinkList)NULL)return ERRO;
	     elem->Data=(char*)NULL ;//数据域
		 elem->lenth=0;//数据域大小
		 elem->CRC16=0;//数据校验块
		 elem->used=UNUSED;//是否被占用
		 //操作FLASH
		 Erase_ISP_DATA_FLASH(elem->tail);
		 Erase_ISP_DATA_FLASH((elem->next)-512);	
}


//顺序查找堆中的空闲块,	
char FindEmptyBlock(void)
{
  		uint8_t i;
			flash->tail=&Nbuf[0]; //取得地址池的首地址准备遍历

		   for(i=0;i<4;i++)
	  {
	  
		if((flash->tail->used)==UNUSED)//空闲块
		{
		
		return i;  //找到了空闲快，返回空闲快的当前内存位置


		}
		 flash->tail++;		 //没有继续找下一个块

	  }
	  
	  	  return ERRO;	//没有空闲块


			/*
		if(size>3) //全部遍历查找空闲
		{
		
  	  //查找空闲块
	//	flash->tail=&Nbuf[0]; //取得地址池的首地址准备遍历
		 return (LinkList)NULL;//没有找到空闲的块
		}
		else
		{
		 		flash->tail=&Nbuf[size]; //取得地址池的首地址准备遍历
				if((flash->tail->used)==UNUSED)//空闲块
				{
				return flash->tail;  //找到了空闲快，返回空闲快的当前内存位置
				}
				else
				{
				 return (LinkList)NULL;//没有找到空闲的块
				}

		} */


	 /* for(i=0;i>8;i++)
	  {
	  
		if((flash->tail->used)==UNUSED)//空闲块
		{
		return flash->tail;  //找到了空闲快，返回空闲快的当前内存位置


		}
		 flash->tail++;		 //没有继续找下一个块

	 }	 */
		 
	   

	
		



}


 //查找堆中指定序号的块地址
LinkList FindSpecifiedBlock(uint8_t size)
{

	  
	   	 if(size>3)return NULL;
		 return  &Nbuf[size];
	   


}



int Malloc_Flash(int size,uint32_t *pt,uint16_t lenth)
{
   if(size!=-1){
ListInsert( FindSpecifiedBlock(size),pt,lenth,size);
                }

		return size;

}
void Free_Flash(uint8_t size)
{

ListDelete(FindSpecifiedBlock(size)); 

}

int  Mem_Flash_Get(uint8_t size,uint32_t *pt)
{
	return ListGet(FindSpecifiedBlock(size),pt,size);

}


void Clearn(uint16_t *p)
{	 uint8_t j;
	 for(j=0;j<48;j++)
	 {
	   *p=0;
	   p++;
	   

	// memcpy((LNode *)&ir_raw[512-48],&Nbuf[0],4);
	 }
}
 /*
 //写入FLASH
void Save_Flash(uint32_t *pt)
{
         uint32_t i,temp,stadd=0x0001f000;

		 Erase_ISP_DATA_FLASH(0x0001f000);
		 Erase_ISP_DATA_FLASH(0x0001f000+512);
			for(i=0;i<256;i++)
		 {
				temp=*(pt+i);
		  write_ISP_DATA_FLASH(stadd,temp );	   //写入

		   	 stadd+=4;
		 }


	


}

void Get_Flash(uint32_t *pt)
{

		uint32_t i,stadd=0x0001f000;
	  for(i=0;i<256;i++)
		 {
		   *(pt+i)=read_ISP_DATA_FLASH(stadd)	;//开始读取头地址数据到PT
		   	stadd+=4;
		 }

}
	  */



