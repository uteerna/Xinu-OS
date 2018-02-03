#include<process_ring.h>
process polling(int p_id, int p_count, int r_count)
{
	int i = 0;
	int last_printed_value = -1;		
	while(i < r_count)
	{
		if (inbox[p_id] != last_printed_value)
		{
			value--;
			printf("\nRing element %d : Round %d : Value : %d", p_id, i, inbox[p_id]);
			last_printed_value = inbox[p_id];
			inbox[(p_id + 1) % p_count] = inbox[p_id] - 1;
			i++;
		}
	}
	return SHELL_OK;
}

process sync(int p_id, int p_count, int r_count)
{
	int i = 0;
	int last_printed_value = -1;
	while(i < r_count)
	{
		if (inbox[p_id] != last_printed_value)
		{
			wait(sem[p_id]); // Waiting for the last process to update the inbox value
			printf("\nRing element %d : Round %d : Value : %d", p_id, i, inbox[p_id]);
			last_printed_value = inbox[p_id];
			inbox[(p_id + 1) % p_count] = inbox[p_id] - 1;			
			signal(sem[(p_id + 1) % p_count]);
			i++;
		}
	}
	signal(complete_sem);
	return SHELL_OK;
}

			
		
