#include<process_ring.h>
volatile int inbox[64];
volatile sid32 sem[64];
volatile sid32 complete_sem;
volatile int value;

shellcmd xsh_process_ring(int argc, char *argv[])
{
	int p_count = 2; // Default value for process count
	int r_count = 3; // Default value for round count
	uint32 starttime, endtime, totaltime, i, j;
	int method = 0;
	for(i = 1; i< argc; i++)
	{
		if(strncmp("-r", argv[i],3) == 0) // Checking if -r matches the command line argument passed
		{
			if((i+1)> argc)
			{
				printf("-r expected an argument");
				return SHELL_ERROR;
			}

			r_count = atoi(argv[i+1]);

			if (r_count < 0)
			{
				printf("-r expects a positive argument");
				return SHELL_ERROR;
			}

			if (r_count < 0 && r_count > 100)
			{
				printf("-r expected an argument between 0 and 100");
				return SHELL_ERROR;
			}

			i += 1;
		}

		if (strncmp("-p", argv[i],3) == 0) // Checking if -p matches the command line argument passed
		{
			if ((i+1)>argc)
			{
				printf("-p expected an argument");
				return SHELL_ERROR;
			}

			p_count = atoi(argv[i+1]);
			
			if (p_count < 0)
			{
				printf("-p expects a positive argument");
				return SHELL_ERROR;
			}

			if (p_count < 0 && p_count > 64)
			{
				printf("-p expected an argument between 0 and 64");
				return SHELL_ERROR;
			}

			i += 1;
		}
		if(strncmp("--help", argv[i], 7) == 0)
		{
			printf("\nThe arguments have to be passed according to the below format");
			printf("\nThe number of processes -p <0-64> default 2");
			printf("\nThe number of rounds -r <uint32> default 3");
			printf("\nThe implementation -i <poll or sync> default poll");
			printf("\nExample usage: process_ring -p 64 -r 1 -i poll");
			return SHELL_OK;
		}
		 
		/* Setting a random variable method to 0 for polling and 1 for semaphores */
		if (strncmp("-i", argv[i], 3)==0)
		{
			if (strncmp("poll", argv[i], 5) == 0)
			{
				method = 0; 
			}

			else if (strncmp("sync", argv[i], 5) == 0)
			{
				method = 1;
			}
		}
	}

	inbox[0] = (p_count * r_count) - 1;
	value = inbox[0];

	printf("\nNumber of processes: %d", p_count);
	printf("\nNumber of rounds: %d", r_count);
	
	gettime(&starttime); // Start the time
	
	if (method == 0)
	{
		for (i = 0; i < p_count; i++)
		{
			resume(create(polling, 1024, 20, "polling", 3, i, p_count, r_count)); // Create process using polling
		}
		while(value != -1);
	}
	else
	{
		for (j = 0; j < p_count; j++)
		{
			sem[j] = semcreate(0);
			if (j == 0) // Checking if this is the first semaphore, if it is then signal is initiated
			{	
				signal(sem[j]); // Setting the value of the first process to 1
			}
			resume(create(sync, 1024, 20, "sync-semaphore", 3, j, p_count, r_count)); // Create the process using semaphores
		}

		for (j = 0; j < p_count; j++)
		{
			wait(complete_sem);
			semdelete(sem[j]);
		}
	}

	gettime(&endtime); // End the time
	totaltime = endtime - starttime;
	printf("\nTotal time taken: %d\n",totaltime);
	return SHELL_OK;
}
