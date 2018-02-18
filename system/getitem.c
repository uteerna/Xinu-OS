/* getitem.c - getfirst, getlast, getitem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getfirst  -  Remove a process from the front of a queue
 *------------------------------------------------------------------------
 */
pid32	getfirst(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
	//pid32	head;
	struct qentry* head;	/* Creating a head pointer */
	
	if (isempty(q)) {
		return EMPTY;
	}

	head = &queuetab[queuehead(q)];
	return getitem((head->qnext)->pid);
}

/*------------------------------------------------------------------------
 *  getlast  -  Remove a process from end of queue
 *------------------------------------------------------------------------
 */
pid32	getlast(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
	//pid32 tail;
	struct qentry* tail;	/* Creating a tail pointer */

	if (isempty(q)) {
		return EMPTY;
	}

	tail = &queuetab[queuetail(q)];
	return getitem((tail->qprev)->pid);
}

/*------------------------------------------------------------------------
 *  getitem  -  Remove a process from an arbitrary point in a queue
 *------------------------------------------------------------------------
 */
pid32	getitem(
	  pid32		pid		/* ID of process to remove	*/
	)
{
	struct qentry* prev; 
	struct qentry* next;
	struct qentry* c_node; 	

	if(proctab[pid].prstate == PR_READY)	/* Checking in the ready queue */
	{
		c_node  = &queuetab[readylist];
		while(c_node->pid != pid && c_node !=NULL)
		{
			c_node = c_node->qnext;
		}
	}
	if(proctab[pid].prstate == PR_SLEEP)	/* Checking in the sleep queue */
	{
		c_node = &queuetab[sleepq];
		while(c_node->pid != pid && c_node != NULL)
		{
			c_node = c_node->qnext;
		}
	}
			
	next = c_node->qnext;	/* Following node in list	*/
	prev = c_node->qprev;	/* Previous node in list 	*/
	
	next->qprev = prev;	/* Points the previous of next node to prev */	
	prev->qnext = next;	/* Points the next of previous to next */
	freemem(c_node,sizeof(struct qentry));	/* Freeing the space occupied by the node which is to be deleted by the queue */
	return pid;
}
