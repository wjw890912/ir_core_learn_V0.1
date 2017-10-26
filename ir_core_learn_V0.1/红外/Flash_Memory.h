#define NULL   (0)
#define ERRO   (-1)
#define USED   (0x7A)
#define UNUSED (0x5C)
typedef struct LNode{
					   uint32_t  tail; //�ڵ�������ַ
			           char  *Data ;//������
					   uint32_t lenth;//�������С
					   uint32_t CRC16;//����У���
					   uint32_t used;//�Ƿ�ռ��
					   uint32_t next;//��һ��
	
}LNode,*LinkList;  //�����ڵ�



typedef struct Pool{


					   LNode    *tail;		//ͷ����ָ��
					   uint32_t size;		//flash ��С
					   uint16_t block_size; //���С


}Pool,*LinkPool;//FLASH �ع���


int Malloc_Flash(int  size,uint32_t *pt,uint16_t lenth);
void Free_Flash(uint8_t size);
int  Mem_Flash_Get(uint8_t size,uint32_t *pt);
char FindEmptyBlock(void);
