/* sys_infp.c - sys_info */

#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
int main(int nargs, char *args[])
{
	pid_t child_pid; 		/* Child process */
	int fd[2];			/* File descriptors */
	char string[50];		/* command to be executed */
	char *cmd_name;
	pipe(fd);			/* Create a pipe */
	child_pid = fork();		/* Fork the new child process */

	/* Checking if the child process is created or not */

	if (child_pid > 0)		
	{
		printf("Parent PID = %d\n", getpid());
		close(fd[0]);
		write(fd[1], args[1], strlen(args[1]));		/* Writing the string to the input file descriptor */
		wait(&child_pid);	
	}

	else if (child_pid == 0)
	{
		close(fd[1]);
		read(fd[0], string, sizeof(string));		/* Reading the string from the output file descriptor */
		printf("Child PID = %d\n", getpid);
		cmd_name = &string[5];
							
		/* Executing the echo command using execl */

		if(strcmp("echo", cmd_name)==0)
		{
			execl(args[1], string, "Hello World!", NULL);
		}
		
		/* Executing the other commands using execl */

		else
		{
			execl(args[1], string, 0, NULL);
		}
		exit(0);
	}		
	return (0);
}
