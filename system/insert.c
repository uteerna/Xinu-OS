/* insert.c - insert */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  insert  -  Insert a process into a queue in descending key order
 *------------------------------------------------------------------------
 */
status	insert(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q,		/* ID of queue to use		*/
	  int32		key		/* Key for the inserted process	*/
	)
{
	struct qentry* curr;		/* Current node pointer*/
	struct qentry* prev;		/* Previous node */
	struct qentry* new_node;	/* New node to be inserted */
	
	new_node = (struct qentry*)getmem(sizeof(struct qentry));	/* Allocating the space to the new node */

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	curr = &queuetab[queuehead(q)];
	while (curr != NULL && curr -> qkey >= key) {	/* Checking till key of current is greater then the given key input */
		curr = curr->qnext;
	}	

	/* Setting the pid and key of the new node */
	new_node->pid = pid;
	new_node->qkey = key;
	
	/* Insert process between curr node and previous node */
	prev = curr->qprev;	/* Get previous node	*/
	new_node->qprev = prev;
	prev->qnext = new_node;
	new_node->qnext = curr;
	curr->qprev = new_node;
	return OK;
}
