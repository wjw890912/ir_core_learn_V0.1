
#include <stdint.h>
#include <string.h>
#include "Flash_Memory.h"
#include "FMC.h"

 //char  Membuf[512];//�����û�����


 LNode Nbuf[4];	//4*1024=4k
 LinkList node=&Nbuf[0];

 Pool flash_onchip;
 LinkPool flash=&flash_onchip;
 //��ʼ���ڴ����FLASH��������ݡ�
 void  init_flash_pool()
 {			 
 			char i ;
 		 flash->tail=&Nbuf[0];	  //flash ��ַ
		 flash->size=4096;		  //FLASH �ܴ�С
		 flash->block_size=1024;	  //�ڲ��Ŀ��С

		for(i=0;i<4;i++)
		{
		 node->tail=(0x0001f000+(i*1024)); //���������ַ
		 node->Data=(char*)NULL;
		 node->lenth=1024;//512�ֽ�
		 node->CRC16=0;//0
		 node->used=UNUSED;//δʹ��
		 node->next=(0x0001f000+((i+1)*1024));//��һ��
		  node++;
		}
	 

 }

// ����Ԫ�ص�FLASH������
int ListInsert(LinkList elem ,uint32_t *pt,uint16_t lenth,uint8_t add)
{			 uint32_t stadd,endadd,temp;
			 uint16_t i;


		if(elem==(LinkList)NULL)return ERRO;
		if( elem->used==USED )return ERRO;
		 elem->Data=(char*)pt; //ָ��ָ��������
		 elem->lenth=lenth;//   ���ݳ���
		 elem->CRC16=0;//0 		У������
		 elem->used=USED;//ʹ��
		//����FLASH

		i=sizeof(LNode);
         memcpy ( (uint32_t *)(pt+(256-(i/4))),(uint32_t *)elem,i);  //���������뵽�ڴ���һ��д��FLASH
		 stadd=elem->tail;//��ʼ��ַ
		 if(stadd==0)
		 {
		  stadd=(add*1024)+0x0001f000;
		 
		 }
		 endadd=(elem->next);	//������ַ

		 Erase_ISP_DATA_FLASH(elem->tail);		 //��������0
		 Erase_ISP_DATA_FLASH((elem->next)-512);		 //��������1
		   stadd=elem->tail;//��ʼ��ַ
		   for(i=0;i<(flash->block_size/4);i++)
		 {
				temp=*(pt+i);
		  write_ISP_DATA_FLASH(stadd,temp );	   //д��
		   	 stadd+=4;
		 }
		


		

}

//��ָ���Ŀ��ж�ȡ���ݡ����س��ȡ�
int ListGet(LinkList elem ,uint32_t *pt,uint8_t add)
{		uint32_t stadd;
		uint16_t i;
	  if(elem==(LinkList)NULL)return ERRO;
	//  if( elem->used==UNUSED) return  ERRO;
	   stadd=elem->tail;//��ʼ��ַ
	   if(stadd==0)
	   {
	   	stadd=(add*1024)+0x0001f000;
	   }
		// elem->Data=(char*)pt; //ָ��ָ��������
//		 elem->lenth=lenth;//   ���ݳ���
		// elem->CRC16=0;//0 		У������
	   for(i=0;i<(flash->block_size/4);i++)
		 {
		   *(pt+i)=read_ISP_DATA_FLASH(stadd)	;//��ʼ��ȡͷ��ַ���ݵ�PT
		   	stadd+=4;
		 }			
		 	  i=sizeof(LNode);
	    memcpy ((uint32_t *)elem,(uint32_t *)(pt+(256-(i/4))),i); 
		return elem->lenth;//���ݳ���

} 

//ɾ��Ԫ����FLASH ����������
int ListDelete(LinkList elem)
{

		 if(elem==(LinkList)NULL)return ERRO;
	     elem->Data=(char*)NULL ;//������
		 elem->lenth=0;//�������С
		 elem->CRC16=0;//����У���
		 elem->used=UNUSED;//�Ƿ�ռ��
		 //����FLASH
		 Erase_ISP_DATA_FLASH(elem->tail);
		 Erase_ISP_DATA_FLASH((elem->next)-512);	
}


//˳����Ҷ��еĿ��п�,	
char FindEmptyBlock(void)
{
  		uint8_t i;
			flash->tail=&Nbuf[0]; //ȡ�õ�ַ�ص��׵�ַ׼������

		   for(i=0;i<4;i++)
	  {
	  
		if((flash->tail->used)==UNUSED)//���п�
		{
		
		return i;  //�ҵ��˿��п죬���ؿ��п�ĵ�ǰ�ڴ�λ��


		}
		 flash->tail++;		 //û�м�������һ����

	  }
	  
	  	  return ERRO;	//û�п��п�


			/*
		if(size>3) //ȫ���������ҿ���
		{
		
  	  //���ҿ��п�
	//	flash->tail=&Nbuf[0]; //ȡ�õ�ַ�ص��׵�ַ׼������
		 return (LinkList)NULL;//û���ҵ����еĿ�
		}
		else
		{
		 		flash->tail=&Nbuf[size]; //ȡ�õ�ַ�ص��׵�ַ׼������
				if((flash->tail->used)==UNUSED)//���п�
				{
				return flash->tail;  //�ҵ��˿��п죬���ؿ��п�ĵ�ǰ�ڴ�λ��
				}
				else
				{
				 return (LinkList)NULL;//û���ҵ����еĿ�
				}

		} */


	 /* for(i=0;i>8;i++)
	  {
	  
		if((flash->tail->used)==UNUSED)//���п�
		{
		return flash->tail;  //�ҵ��˿��п죬���ؿ��п�ĵ�ǰ�ڴ�λ��


		}
		 flash->tail++;		 //û�м�������һ����

	 }	 */
		 
	   

	
		



}


 //���Ҷ���ָ����ŵĿ��ַ
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
 //д��FLASH
void Save_Flash(uint32_t *pt)
{
         uint32_t i,temp,stadd=0x0001f000;

		 Erase_ISP_DATA_FLASH(0x0001f000);
		 Erase_ISP_DATA_FLASH(0x0001f000+512);
			for(i=0;i<256;i++)
		 {
				temp=*(pt+i);
		  write_ISP_DATA_FLASH(stadd,temp );	   //д��

		   	 stadd+=4;
		 }


	


}

void Get_Flash(uint32_t *pt)
{

		uint32_t i,stadd=0x0001f000;
	  for(i=0;i<256;i++)
		 {
		   *(pt+i)=read_ISP_DATA_FLASH(stadd)	;//��ʼ��ȡͷ��ַ���ݵ�PT
		   	stadd+=4;
		 }

}
	  */



