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
	return SYSERR;	
}
