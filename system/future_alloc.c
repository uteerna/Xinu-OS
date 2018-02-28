#include<xinu.h>
#include<future.h>

future* future_alloc(int future_flag)
{
	struct futent* nfuture;
	nfuture = (struct futent*)getmem(sizeof(struct futent));
	nfuture->state = FUTURE_EMPTY;
	nfuture->flag = future_flag;
	nfuture->pid = NULL;
	nfuture->value = NULL; 
	return nfuture;
}	
