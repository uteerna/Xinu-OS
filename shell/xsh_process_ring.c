/* Run the program using: process_ring -p process_count -r round_count -i implementation */
#include<process_ring.h>
volatile int inbox[64];
volatile sid32 sem[64];
volatile sid32 complete_sem;
volatile int value;

shellcmd xsh_process_ring(int argc, char *argv[])
{
	int p_count = 2; // Default value for process count
	uint32 r_count = 3; // Default value for round count
	uint32 starttime, endtime, totaltime, i, j;
	int method = 0;
        		
	for(i = 1; i<= argc; i++)
	{
		/* Check if --r, -r or --round is passed as a flag */
		if((strncmp("-r", argv[i], 3) == 0) || (strncmp("--r", argv[i], 4)==0) || (strncmp("--round", argv[i], 8)==0)) 

		{	
			if((i+1) >= argc)
			{
				printf("\nThe arguments passed are less than the required\n");
				return SHELL_ERROR;

 			}
	
			for (j = 0; j<strlen(argv[i+1]);j++)
			{
				if(!(isdigit(argv[i+1][j])))
				{
					printf("\n-r expects an integer\n");
					return SHELL_ERROR;
				}
			}
			
			r_count = atoi(argv[i+1]); // Convert the string to the integer
			
			if (r_count == 0) // In case atoi fails then it returns an error
			{
				printf("-r expects an argument\n");
				return SHELL_ERROR;
			}

			if (!(r_count > 0 && r_count <= UINT_MAX))
			{	
				printf("-\nr expected an argument between 1 and UINT_MAX\n");
				return SHELL_ERROR;
			
			}
			i += 1;
		}
		
		/* Checking if -p, --p or --process is passed as a flag */
		if ((strncmp("-p", argv[i], 3) == 0) || (strncmp("--p", argv[i], 4 == 0) || (strncmp("--process", argv[i], 10) == 0)))
		{
			if ((i+1) >= argc)
			{
				printf("\nThe arguments passed are less than the required\n");
				return SHELL_ERROR;
			}
		       
                        for (j = 0; j<strlen(argv[i+1]);j++)
                        {
				
				if(!(isdigit(argv[i+1][j])))
                                {
                                        printf("\n-p expects an integer\n");
                                        return SHELL_ERROR;
                                }
                        }

			p_count = atoi(argv[i+1]); // Convert string to an integer
					
			if (p_count == 0) // In case atoi fails then it returns an error
			{
				printf("\n-p expects an argument\n");
				return SHELL_ERROR;
			}	

			if (!(p_count > 0 && p_count <= 64))
			{
				printf("\n-p expected an argument between 1 and 64\n");
				return SHELL_ERROR;
			}
			i += 1;
		}
		
		/* Checking if -h or --help is passes as a flag */
		if ((strncmp("--help", argv[i], 7) == 0) || (strncmp("-h", argv[i], 3) == 0))
                {
                      	printf("\nThe arguments have to be passed according to the below format");
			printf("\nThe number of processes -p <0-64> default 2");
			printf("\nThe number of rounds -r <uint32> default 3");
			printf("\nThe implementation -i <poll or sync> default poll");
			printf("\nThe flags can also be passed as --process, --p, --round, --r");
			printf("\nExample usage: process_ring -p 64 -r 1 -i poll\n");
			return SHELL_OK;
		}
		 
		/* Setting a variable method to 0 for polling and 1 for semaphores */
		if ((strncmp("-i", argv[i], 3)==0) || (strncmp("--i", argv[i], 4)==0))
		{
			if (strncmp("poll", argv[i+1], 5) == 0)
			{
				printf("\nThe implementation is: %s",argv[i+1]);
				method	= 0; 
			}

			else if (strncmp("sync", argv[i+1], 5) == 0)
			{
				printf("\nThe implementation is: %s",argv[i+1]);
	        		method = 1;
			}
			else
			{
				printf("\n-i expects either poll or sync as an argument\n");
				return SHELL_ERROR;
			}
			i += 1;
		}
	}
	
	inbox[0] = (p_count * r_count) - 1; // Calculating the first inbox value
	value = inbox[0];
	
	printf("\nNumber of processes: %d", p_count);
	printf("\nNumber of rounds: %d", r_count);

	for (j = 0; j < p_count; j++)
	{
		sem[j] = semcreate(0); // Create a semaphore
	}

	gettime(&starttime); // Start the time
	
	if(method==0)
	{
		for (j =0;j<p_count;j++)
		{
			resume(create(polling, 1024, 20, "polling", 3, j, p_count, r_count)); // Create process using polling
		}
		while(value != -1);
	}

	for (j=0; j < p_count; j++)
	{
		if(method == 1) 
		{
			if (j == 0)
			{
				signal(sem[j]); // Setting the value of the first process to 1
			}
			resume(create(sync, 1024, 20, "sync-semaphore", 3, j, p_count, r_count)); // Create the process using semaphores
		}
	}

	if(method == 1)
	{
		for (j = 0; j < p_count; j++)
		{
			wait(complete_sem);
			semdelete(sem[j]);
		}
	}
	else
	{
		while(value != -1); // Waiting for the value to be -1 then exit
	}
	
	gettime(&endtime); // End the time
	totaltime = endtime - starttime;
	printf("\nTotal time taken: %d\n",totaltime);
	return SHELL_OK;
}
