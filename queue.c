#include "queue.h"

 int MyQueueInit(MyQueue* q)
 {
 	return My402ListInit(&q->list);
 }
 int MyQueueEnqueue(MyQueue* q, void* obj)
 {
 	return My402ListAppend(&q->list, obj);
 }
 int MyQueueDequeue(MyQueue* q)
 {
 	My402ListUnlink(&q->list, My402ListFirst(&q->list));
 	return TRUE;
 }
 int MyQueueEmpty(MyQueue* q)
 {
 	return My402ListEmpty(&q->list);
 }

void* MyQueueTop(MyQueue* q)
{
	My402ListElem *first = My402ListFirst(&q->list);
	return first->obj;
}