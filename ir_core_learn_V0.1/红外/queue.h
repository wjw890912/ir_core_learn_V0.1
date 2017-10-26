


#ifndef  QUEUE_H
#define  QUEUE_H
 #include  <stdint.h>

   #define MAXQSIZE 2	//���г���
   #define MEM_POOL 256	//���泤��
   #define OK 		(1)
   #define ERROR 	(0)
   #define QElemtype  uint16_t
   #define status     char 

  typedef struct {
	   
     
	  QElemtype (*base)[MEM_POOL];	//������ڴ�ָ��

	  int front ;//ͷָ��

	  int rear;//βָ��

	  int sizeofbuf[MAXQSIZE];

 }SqQueue;





 status InitQueue (SqQueue *Q);	 //��ʼ������
 int QueueLength(SqQueue *Q) ;	 //��ȡ���г���
 status EnQueue(SqQueue *Q,QElemtype *e,int length);  //���
 status DeQueue(SqQueue *Q,QElemtype *e);			  //����







#endif	
