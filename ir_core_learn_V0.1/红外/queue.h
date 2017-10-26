


#ifndef  QUEUE_H
#define  QUEUE_H
 #include  <stdint.h>

   #define MAXQSIZE 2	//队列长度
   #define MEM_POOL 256	//缓存长度
   #define OK 		(1)
   #define ERROR 	(0)
   #define QElemtype  uint16_t
   #define status     char 

  typedef struct {
	   
     
	  QElemtype (*base)[MEM_POOL];	//分配的内存指向

	  int front ;//头指针

	  int rear;//尾指针

	  int sizeofbuf[MAXQSIZE];

 }SqQueue;





 status InitQueue (SqQueue *Q);	 //初始化队列
 int QueueLength(SqQueue *Q) ;	 //获取队列长度
 status EnQueue(SqQueue *Q,QElemtype *e,int length);  //入队
 status DeQueue(SqQueue *Q,QElemtype *e);			  //出队







#endif	
