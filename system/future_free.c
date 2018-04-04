#include<xinu.h>
#include<future.h>

syscall future_free(future* f)
{
	int status = 0;
	if(&f->get_queue != NULL)
	{
      freemem(&f->get_queue, sizeof(struct queue_entry));	/* Free space occupied by get queue */
    }
	if(&f->set_queue != NULL)
	{
	  freemem(&f->set_queue, sizeof(struct queue_entry));	/* Free space occupied by set queue */ 
	}	
	status = freemem(f, sizeof(struct futent)); 	/* Free the space occupied by the future */
	
	/* Check if the freemem returns SYSERR or OK, if it returns SYSERR then return SYSERR else return OK */
	if(status == SYSERR)	
		status = SYSERR;
	else
		status = OK;
	return status;
}