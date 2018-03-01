#include<xinu.h>
#include<future.h>

syscall future_get(future *f, int *value)
{
	if(f->flag == FUTURE_EXCLUSIVE)
	{
		if(f->state == FUTURE_EMPTY)
		{	
			f->pid = getpid();
			f->state = FUTURE_WAITING;
			intmask mask = disable();
			proctab[f->pid].prstate = PR_WAIT;
			resched();
			restore(mask);
	
			if(f->state == FUTURE_VALID)
			{
				f->state = FUTURE_EMPTY;
				*value = *(f->value);
				f->pid = NULL;
				return OK;
			}
		}
		
		if (f->state == FUTURE_VALID)
		{
			intmask mask = disable();
			f->state = FUTURE_EMPTY;
			*value = *(f->value);
			f->pid = NULL;
			restore(mask);
			return OK;
		}
		return SYSERR;
	}
	return SYSERR;
}	 	
