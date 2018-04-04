#include<xinu.h>
#include<future.h>

syscall future_set(future *f, int *value)
{
	/* Check if the mode of operation of the future is FUTURE_EXCLUSIVE, if it is not then return SYSERR */
	if(f->flag == FUTURE_EXCLUSIVE)
	{
		if(f->state == FUTURE_EMPTY || f->state == FUTURE_WAITING)	/* Check if the state of the future is FUTURE_EMPTY or FUTURE_WAITING */
		{
			*(f->value) = *value;	/* Set the value passed to *(f->value) */
			f->state = FUTURE_VALID;	/* Set the value state of the futue to FUTURE_VALID */
			ready(f->pid);
			return OK;
		}	
		if (f->state == FUTURE_VALID)	/* Check if the state of the future is FUTURE_VALID, if it is then return SYSERR */
		{
			return SYSERR;
		}
	}

	if(f->flag == FUTURE_SHARED)
	{
		if(f->state == FUTURE_EMPTY || f->state == FUTURE_WAITING)
		{
			struct queue_entry *curr;
			curr = &f->get_queue;
			*(f->value) = *value;
			f->state = FUTURE_VALID;
			while(curr != NULL) 
			{
				resume(curr->pid);
				curr = curr->qnext;
			}
		}
		if(f->state == FUTURE_VALID)
		{
			return SYSERR;
		}
		return OK;
	}

	if(f->flag == FUTURE_QUEUE)
	{
		if(f->get_queue.qnext == NULL)
		{
			*(f->value) = *value;
			struct queue_entry *new_node;
			struct queue_entry *curr;
			new_node = (struct queue_entry*)getmem(sizeof(struct queue_entry));
			pid32 q_pid = getpid();
			f->state = FUTURE_WAITING;
			new_node->qnext = NULL;
			new_node->pid = q_pid;
			curr = &f->set_queue;
			while(curr->qnext != NULL)
			{
				curr = curr->qnext;
			}
			curr->qnext = new_node;
			*value = new_node->value;
			suspend(new_node->pid);
		}
		else
		{
			struct queue_entry *curr;
			curr = f->get_queue.qnext;
			curr->value = *value;
			f->get_queue.qnext = curr->qnext;
			resume(curr->pid);

		}
	}
	return OK;
}