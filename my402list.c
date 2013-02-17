#include "my402list.h"
#include <stdlib.h>

 int  My402ListLength(My402List* list)
 {
	return list->num_members;
 }
 int  My402ListEmpty(My402List* list)
 {
	 return list->num_members == 0;
 }

 int  My402ListAppend(My402List* list, void* obj)
 {
	My402ListElem* last,*elem;
	last = My402ListLast(list);

	if(last == NULL)
		last = &list->anchor;

    	elem = (My402ListElem*)malloc(sizeof(My402ListElem));
	if( elem == NULL)
	{
		// print error???
		return FALSE;
	}
	elem->obj = obj;

	elem->next = &list->anchor;
	list->anchor.prev = elem;

	elem->prev = last;
	last->next = elem;
	++list->num_members;
	return TRUE;

 }
 int  My402ListPrepend(My402List* list, void* obj)
 {
	 My402ListElem* first, *elem;
	 first = My402ListFirst(list);
	 if(first == NULL)
		first = &list->anchor;
	 
	elem = (My402ListElem*)malloc(sizeof(My402ListElem));
	 if(NULL == elem)
	 {
		 //error??
		 return FALSE;
	 }
	 elem->obj = obj;

	 list->anchor.next = elem;
	 elem->prev = &list->anchor;

	 elem->next = first;
	 first->prev = elem;
	 ++list->num_members;
	 return TRUE;
	 
	 return FALSE;
 }
 void My402ListUnlink(My402List* list, My402ListElem* elem)
 {
	 // do I need to use list?
	if( My402ListLength(list) == 0)
		return;

	 elem->prev->next = elem->next;
	 elem->next->prev = elem->prev;
	 --list->num_members;
	 free(elem); elem = NULL;
 }
 void My402ListUnlinkAll(My402List* list)
 {
	 My402ListElem *elem, *prev;
	 elem = prev = NULL;

	 for(elem=My402ListFirst(list);elem != NULL;) 
	 {
		prev = elem;
		elem = My402ListNext(list, elem);
		free(prev);
		prev = NULL;
	 }
	 list->anchor.next = list->anchor.prev = NULL;
	 list->num_members = 0;
 }
 int  My402ListInsertAfter(My402List* list, void* obj, My402ListElem* elem)
 {
	 My402ListElem* newItem = NULL;
	 if(NULL == elem || My402ListLast(list) == elem)
		 return My402ListAppend(list,obj);
	 else
	 {
	 newItem = (My402ListElem*)malloc(sizeof(My402ListElem));
		if(NULL == newItem) 
		{
			//print error
			return FALSE;
		}
		newItem->obj = obj;

		newItem->next = elem->next;
		elem->next->prev = newItem;

		newItem->prev = elem;
		elem->next = newItem;
		++list->num_members;
		return TRUE;
	 }
	return FALSE;	 
 }
 int  My402ListInsertBefore(My402List* list, void* obj, My402ListElem* elem)
 {
	 My402ListElem* newItem = NULL;
	 if(NULL == elem)
		 return My402ListPrepend(list,obj);
	 else
	 {
		newItem = (My402ListElem*)malloc(sizeof(My402ListElem));
		 if(NULL == newItem) 
		 {
			 //print error
			 return FALSE;
		 }
		 newItem->obj = obj;

		 elem->prev->next = newItem;
		 newItem->prev = elem->prev;

		 newItem->next = elem;
		 elem->prev = newItem;

		 ++list->num_members;
		 return TRUE;
	 }
	 return FALSE;
 }

 My402ListElem *My402ListFirst(My402List* list)
 {
	 if(list->num_members == 0)
		 return NULL;
	 else
		 return list->anchor.next;
 }

 My402ListElem *My402ListLast(My402List* list)
 {
	 if(list->num_members == 0)
		 return NULL;
	 else
		 return list->anchor.prev;
 }

 My402ListElem *My402ListNext(My402List* list, My402ListElem* elem)
 {
	 if(elem->next == &list->anchor)
		 return NULL;
	 else
		 return elem->next;
 }

 My402ListElem *My402ListPrev(My402List* list, My402ListElem* elem)
 {
	 if(elem->prev == &list->anchor)
		 return NULL;
	 else
		 return elem->prev;

 }

 My402ListElem *My402ListFind(My402List* list, void* obj)
 {
	 My402ListElem *elem=NULL;

	 for (elem=My402ListFirst(list); elem != NULL; elem=My402ListNext(list, elem))
	 {
		 if(elem->obj == obj)
			 return elem;
	 }	
	 return NULL;
 }

 int My402ListInit(My402List* list)
 {
	 //how could there be an error here?
	 list->num_members = 0;
	 list->anchor.next = NULL;
	 list->anchor.prev = NULL;
	 list->anchor.obj = NULL;
	 return TRUE;
 }
 
 
