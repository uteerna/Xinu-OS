#include<xinu.h>

#ifndef _FUTURE_H_
#define _FUTURE_H_
 
/* define states */
#define FUTURE_EMPTY	  0
#define FUTURE_WAITING 	  1         
#define FUTURE_VALID 	  2         

/* modes of operation for future*/
#define FUTURE_EXCLUSIVE  1	
#define FUTURE_SHARED     2	    
#define FUTURE_QUEUE      3     

struct queue_entry 
{
	int32 pid;
	struct queue_entry *qnext;
	int32 value;
};

typedef struct futent
{
   int *value;		
   int flag;		
   int state;         	
   pid32 pid;
   struct queue_entry set_queue;     
   struct queue_entry get_queue;
}future;

/* Interface for system call */
future* future_alloc(int future_flags);
syscall future_free(future*);
syscall future_get(future*, int*);
syscall future_set(future*, int*);
 
#endif /* _FUTURE_H_ */