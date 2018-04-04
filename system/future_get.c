	#include<xinu.h>
#include<future.h>

syscall future_get(future *f, int *value)
{
	if(f->flag == FUTURE_EXCLUSIVE) /* Check if the mode of operation of the future is FUTURE_EXCLUSIVE, if it is not then return SYSERR */
	{
		/* Check if the state of the future is FUTURE_EMPTY, if it is then change the state to FUTURE_WAITING and get the value of the future and store it into *value */
		if(f->state == FUTURE_EMPTY)	
		{	
			f->pid = getpid();
			f->state = FUTURE_WAITING;
			proctab[f->pid].prstate = PR_WAIT;	/* Change the state of the process to PR_WAIT */
			resched();
			if(f->state == FUTURE_VALID)
			{
				f->state = FUTURE_EMPTY;
				*value = *(f->value);
				f->pid = NULL;
				return OK;
			}
		}
		
		/* Check if the state of the future is FUTURE_VALID */ 
		if (f->state == FUTURE_VALID)
		{
			f->state = FUTURE_EMPTY; /* Set the state of the passed future to FUTURE_EMPTY */
			*value = *(f->value);	/* Set the value of the future in the variable *value */
			f->pid = NULL;
			return OK;
		}

		/* If the state of the future is FUTURE_WAITING, then return SYSERR */
		if (f->state == FUTURE_WAITING)
		{
			return SYSERR; 
		}
	}


	if(f->flag == FUTURE_SHARED)
	{
		if(f->state == FUTURE_EMPTY)
		{
			f->pid = getpid();
			f->state = FUTURE_WAITING;
			struct queue_entry *new_node;
			new_node = (struct queue_entry*)getmem(sizeof(struct queue_entry));
			f->get_queue = *new_node;
			f->get_queue.pid = f->pid;
			f->get_queue.qnext = NULL;
			suspend(f->pid);
		}
		if(f->state == FUTURE_WAITING)
		{
			struct queue_entry *temp;
			temp = &f->get_queue;
			struct queue_entry *curr;
            while(temp != NULL)
            {
            	curr = temp;
            	temp = temp->qnext;        	
            }
            temp = (struct queue_entry*)getmem(sizeof(struct  queue_entry));
            curr->qnext = temp;
            temp->pid = getpid();
            temp->qnext = NULL;
            suspend(temp->pid);
		}
		if(f->state == FUTURE_VALID)
		{
			*value = *(f->value);
		}
	}

	if(f->flag == FUTURE_QUEUE)
	{
		if(f->set_queue.qnext == NULL)
		{
			struct queue_entry *new_node;
			struct queue_entry *curr;
			new_node = (struct queue_entry*)getmem(sizeof(struct queue_entry));
			//printf("1: %d", &new_node);
			pid32 q_pid = getpid();
			//f->state = FUTURE_WAITING;
			new_node->qnext = NULL;
			new_node->pid = q_pid;
			curr = &f->get_queue;
			while(curr->qnext != NULL)
			{
				curr = curr->qnext;
			}
			curr->qnext = new_node;
			suspend(new_node->pid);
			*value = new_node->value;
		}
		else
		{
			struct queue_entry *curr;
			curr = f->set_queue.qnext;
			*value = curr->value;
			f->set_queue.qnext = curr->qnext;
			resume(curr->pid);
			freemem(curr, sizeof(struct queue_entry));
		}
	}
	return OK;
}