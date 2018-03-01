#include<xinu.h>
#include<future.h>

future* future_alloc(int future_flag)
{
	struct futent* nfuture;
	nfuture = (struct futent*)getmem(sizeof(struct futent));	/* Allocate a new future */
	nfuture->state = FUTURE_EMPTY;	/* Allocate the future with a FUTURE_EMPTY state */
	nfuture->flag = future_flag;	/* Set the flag of the new future to the one sent as a parameter to the future_alloc function */
	nfuture->pid = NULL;	/* Set the pid to NULL */
	nfuture->value = NULL; 	/* Set the value of the future to NULL */	
	return nfuture;		/* Return the future */
}	
