#include<xinu.h>
#include<future.h>

syscall future_free(future* f)
{
	intmask mask = disable();
	int status = freemem(f, sizeof(struct futent)); 
	 if(status == SYSERR)
		status = SYSERR;
	else
		status = OK; 
	restore(mask);
	return status;
}
