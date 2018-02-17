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
	struct qentry* curr;		/* Runs through items in a queue*/
	struct qentry* prev;		/* Holds previous node index	*/
	struct qentry* new_node;
	
	new_node = (struct qentry*)getmem(sizeof(struct qentry));

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	curr = &queuetab[queuehead(q)];
	while (curr != NULL && curr -> qkey >= key) {
		curr = curr->qnext;
	}

	/* Insert process between curr node and previous node */
	
	new_node->pid = pid;
	new_node->qkey = key;

	prev = curr->qprev;	/* Get index of previous node	*/
	new_node->qprev = prev;
	prev->qnext = new_node;
	new_node->qnext = curr;
	curr->qprev = new_node;
	//queuetab[pid].qnext = curr;
	//queuetab[pid].qprev = prev;
	//queuetab[pid].qkey = key;
	//queuetab[pid].pid = pid;
	//queuetab[prev->pid].qnext = &queuetab[pid];
	//queuetab[curr->pid].qprev = &queuetab[pid];
	return OK;
}
