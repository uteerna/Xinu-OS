#include<xinu.h>
#include<future.h>

syscall future_free(future* f)
{
	intmask mask = disable();	/* Disable the interrupts */
	int status = freemem(f, sizeof(struct futent)); 	/* Free the space occupied by the future */

	/* Check if the freemem returns SYSERR or OK, if it returns SYSERR then return SYSERR else return OK */
	 if(status == SYSERR)	
		status = SYSERR;
	else
		status = OK;

	restore(mask); /* Enable the interrupts */
	return status;
}
