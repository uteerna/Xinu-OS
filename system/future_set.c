#include<xinu.h>
#include<future.h>

syscall future_set(future *f, int *value)
{
	if(f->flag == FUTURE_EXCLUSIVE)
	{
		if(f->state == FUTURE_EMPTY || f->state == FUTURE_WAITING)
		{
			*(f->value) = *value;
			f->state = FUTURE_VALID;
			intmask mask = disable();
			ready(f->pid);
			restore(mask);
			return OK;
		}	
		if (f->state == FUTURE_VALID)
		{
			return SYSERR;
		}
	}
	return SYSERR;	
}
