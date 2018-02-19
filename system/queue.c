/* queue.c - enqueue, dequeue */

#include <xinu.h>

struct qentry	queuetab[NQENT];	/* Table of process queues	*/

/*------------------------------------------------------------------------
 *  enqueue  -  Insert a process at the tail of a queue
 *------------------------------------------------------------------------
 */
pid32	enqueue(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q		/* ID of queue to use		*/
	)
{
	struct qentry *tail; 
	struct qentry *prev;		
	struct qentry *new_node;	/* Create a new node */
		
	new_node = (struct qentry*)getmem(sizeof(struct qentry));	/* Allocating the space to new node */
	new_node->pid = pid;	/* Set the pid of new node */

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}
	
	/* Add a node at the end of the queue before the tail */
	tail = &queuetab[queuetail(q)];
	prev = tail->qprev;
	new_node->qnext = tail;
	tail->qprev=new_node;
	new_node->qprev =  prev;
	prev->qnext = new_node;
	return pid;
}

/*------------------------------------------------------------------------
 *  dequeue  -  Remove and return the first process on a list
 *------------------------------------------------------------------------
 */
pid32	dequeue(
	  qid16		q		/* ID queue to use		*/
	)
{
	pid32	pid;			/* ID of process removed	*/

	if (isbadqid(q)) {
		return SYSERR;
	} else if (isempty(q)) {
		return NULL;
	}
	
	/* Remove the node from the front of the queue after the head */
	pid = getfirst(q);	
	return pid;
}
