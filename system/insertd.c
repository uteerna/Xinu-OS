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
	struct qentry* new_node;
	struct qentry* prev;
	struct qentry* next;

	new_node = (struct qentry*)getmem(sizeof(struct qentry));

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	prev = &queuetab[queuehead(q)]; /* Gives the reference to the head node */
	next = queuetab[queuehead(q)].qnext; /* Gives the next node after the head */
	
	/*Searching for the position where the new node is to be inserted */
	while ((next->pid != queuetail(q)) && (next->qkey <= key)) {
		key -= next->qkey;
		prev = next;
		next = next->qnext;
	}

	/* Insert new node between prev and next nodes */	
	new_node->qprev = prev;
	prev->qnext = new_node;
	new_node->qnext = next;
	next->qprev = new_node;
 
	if (next->pid != queuetail(q)) {
		next->qkey -= key;
	}

	return OK;
}
