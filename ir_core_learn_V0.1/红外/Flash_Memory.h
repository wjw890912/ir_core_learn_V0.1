#define NULL   (0)
#define ERRO   (-1)
#define USED   (0x7A)
#define UNUSED (0x5C)
typedef struct LNode{
					   uint32_t  tail; //节点的自身地址
			           char  *Data ;//数据域
					   uint32_t lenth;//数据域大小
					   uint32_t CRC16;//数据校验块
					   uint32_t used;//是否被占用
					   uint32_t next;//下一个
	
}LNode,*LinkList;  //建立节点



typedef struct Pool{


					   LNode    *tail;		//头结点的指针
					   uint32_t size;		//flash 大小
					   uint16_t block_size; //块大小


}Pool,*LinkPool;//FLASH 池管理


int Malloc_Flash(int  size,uint32_t *pt,uint16_t lenth);
void Free_Flash(uint8_t size);
int  Mem_Flash_Get(uint8_t size,uint32_t *pt);
char FindEmptyBlock(void);
