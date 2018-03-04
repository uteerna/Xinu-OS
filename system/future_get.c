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
		return SYSERR; /* If the state of the future is FUTURE_WAITING */
	}
	return SYSERR;
}	 	
