

#include  <stdio.h>
#include "queue.h"

 //存储BUFF   [MAXQSIZE ]：索引最大[MEM_POOL] ：消息最大长度
 //内存占用=	（MAXQSIZE*MEM_POOL）*sizeof(uint16_t)	= （MAXQSIZE*MEM_POOL）*2   BYTE	   
 uint16_t Queue_buf[MAXQSIZE ][MEM_POOL];

 	
 status InitQueue (SqQueue *Q)
 {
 
 	  Q->base = Queue_buf;

	  Q->front =0 ;

	  Q->rear =0;
 
 	  return OK;
 					
 }
int QueueLength(SqQueue *Q)
{



		return ((Q->rear-Q->front+MAXQSIZE)	% MAXQSIZE)	;


}


status EnQueue(SqQueue *Q,QElemtype *e,int length)
{


	   if(((Q->rear +1 )%MAXQSIZE)==Q->front )return ERROR;

		if(length> MEM_POOL)length=MEM_POOL-1;
		
	    Q->sizeofbuf[Q->rear]=length; 

		memcpy(*(Q->base+Q->rear),(const char*)e,Q->sizeofbuf[Q->rear]);

		Q->rear =(Q->rear+1) % MAXQSIZE	;

		return OK;
}

status DeQueue(SqQueue *Q,QElemtype *e)
{
		int  length;
		if( Q->front==Q->rear) return ERROR;

		 length=Q->sizeofbuf[Q->front];
	
		 memcpy((char*)e,*(Q->base+Q->front),Q->sizeofbuf[Q->front]);

		 Q->front =(Q->front+1)	%  MAXQSIZE;

		return OK;


}



