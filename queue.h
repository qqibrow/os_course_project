#ifndef QUEUE_H
#define QUEUE_H

#include "my402list.h"

typedef struct tagMyQueue
{
	My402List list;

}MyQueue;

extern int MyQueueInit(MyQueue* q);
extern int MyQueueEnqueue(MyQueue* q, void* obj);
extern int MyQueueDequeue(MyQueue* q);
extern int MyQueueEmpty(MyQueue* q);
extern void* MyQueueTop(MyQueue* q);

#endif