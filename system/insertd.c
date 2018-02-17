/* insertd.c - insertd */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  insertd  -  Insert a process in delta list using delay as the key
 *------------------------------------------------------------------------
 */
status	insertd(			/* Assumes interrupts disabled	*/
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q,		/* ID of queue to use		*/
	  int32		key		/* Delay from "now" (in ms.)	*/
	)
{
	//int32	next;			/* Runs through the delta list	*/
	//int32	prev;			/* Follows next through the list*/
	struct qentry* new_node;
	struct qentry* prev;
	struct qentry* next;

	new_node = (struct qentry*)getmem(sizeof(struct qentry));

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	prev = &queuetab[queuehead(q)];
	next = queuetab[queuehead(q)].qnext;
	while ((next->pid != queuetail(q)) && (next->qkey <= key)) {
		//key -= queuetab[next].qkey;
		//prev = next;
		//next = queuetab[next].qnext;
		key -= next->qkey;
		prev = next;
		next = next->qnext;
	}

	/* Insert new node between prev and next nodes */

	/*queuetab[pid].qnext = next;
	queuetab[pid].qprev = prev;
	queuetab[pid].qkey = key;
	queuetab[prev].qnext = pid;
	queuetab[next].qprev = pid; */
		
	new_node->qprev = prev;
	prev->qnext = new_node;
	new_node->qnext = next;
	next->qprev = new_node;
 
	if (next->pid != queuetail(q)) {
		next->qkey -= key;
	}

	return OK;
}
