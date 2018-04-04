#include<xinu.h>
#include<future.h>

/*struct queue_entry* future_new_queue(void)
{
	struct queue_entry *head;
	struct queue_entry *tail;
	head = (struct queue_entry*)getmem(sizeof(struct queue_entry));
	tail = (struct queue_entry*)getmem(sizeof(struct queue_entry));
	head->pid = MAXKEY;
	//head->qprev = NULL;
	head->qnext = tail;
	tail->pid = MINKEY;
	//tail->qprev = head;
	tail->qnext = NULL;
	return head;
}*/

void future_enqueue(struct queue_entry *head, pid32 pid)
{
	struct queue_entry *new_node;
	struct queue_entry *curr;

	new_node = (struct queue_entry*)getmem(sizeof(struct queue_entry));
	new_node->qnext = NULL;
	new_node->pid = pid;	
	curr = head;
	while(curr->qnext != NULL)
	{
		curr = curr->qnext;
	}
	//new_node->qnext = curr->qnext;
	curr->qnext = new_node;
}

// int check_empty(queue_entry *head)
// {
// 	if(head->qnext->pid == MINKEY)
// 	{
// 		return 1;
// 	}
// 	return 0;
// }

pid32 future_dequeue(struct queue_entry *head)
{
	if(head->qnext->pid == NULL)
	{
		return SYSERR;
	}
	struct queue_entry *curr; 
	curr = head->qnext; 
	pid32 pid = curr->pid;
	head->qnext = curr->qnext;
	freemem(curr, sizeof(struct queue_entry));
	return pid;
}