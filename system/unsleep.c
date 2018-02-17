/* unsleep.c - unsleep */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  unsleep  -  Internal function to remove a process from the sleep
 *		    queue prematurely.  The caller must adjust the delay
 *		    of successive processes.
 *------------------------------------------------------------------------
 */
status	unsleep(
	  pid32		pid		/* ID of process to remove	*/
        )
{
	intmask	mask;			/* Saved interrupt mask		*/
        struct	procent	*prptr;		/* Ptr to process' table entry	*/
	struct qentry* curr;		/* Current is initially the head of sleepq */
	struct qentry* pidnext;

        //pid32	pidnext;		/* ID of process on sleep queue	*/
					/*   that follows the process	*/
					/*   which is being removed	*/
	
	mask = disable();

	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	/* Verify that candidate process is on the sleep queue */

	prptr = &proctab[pid];
	if ((prptr->prstate!=PR_SLEEP) && (prptr->prstate!=PR_RECTIM)) {
		restore(mask);
		return SYSERR;
	}

	/* Increment delay of next process if such a process exists */
	
	curr = &queuetab[sleepq];

	while(pid != curr-> pid && curr != NULL)
	{
		curr = curr->qnext;
	}

	//pidnext = queuetab[pid].qnext;
	pidnext = curr->qnext;

	if (pidnext->pid < NPROC) {
		//queuetab[pidnext].qkey += queuetab[pid].qkey;
		pidnext->qkey += curr->qkey;
	}

	getitem(pid);			/* Unlink process from queue */
	restore(mask);
	return OK;
}
