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
	struct qentry *next;	
	struct qentry *new_node;
		
	new_node = (struct qentry*)getmem(sizeof(struct qentry));
	new_node->pid = pid;
	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}
	
	tail = &queuetab[queuetail(q)];
	//prev = queuetab[tail->pid].qprev;
	prev = tail->qprev;
	new_node->qnext = tail;
	tail->qprev=new_node;
	new_node->qprev =  prev;
	prev->qnext = new_node;
	//queuetab[pid].qnext  = tail;	/* Insert just before tail node	*/
	//queuetab[pid].qprev = prev;
	//queuetab[pid].pid = pid;
	//prev->qnext = &queuetab[pid];
	//tail->qprev = &queuetab[pid];
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

	pid = getfirst(q);
	//queuetab[pid].qprev = NULL;
	//queuetab[pid].qnext = NULL;
	return pid;
}
