#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
int main(int nargs, char *args[])
{
	pid_t child_pid;
	int fd[2];
	char string[50];
	char *cmd_name;
	pipe(fd);
	child_pid = fork();
	if (child_pid > 0)
	{
		printf("Parent PID = %d\n", getpid());
		close(fd[0]);
		write(fd[1], args[1], strlen(args[1]));
		wait(NULL);
	}
	else if (child_pid == 0)
	{
		close(fd[1]);
		read(fd[0], string, sizeof(string));
		printf("Child PID = %d\n", getpid);
		cmd_name = &string[5];
		if(strcmp("echo", cmd_name)==0)
		{
			execl(args[1], string, "Hello World!", NULL);
		}
		else
		{
			execl(args[1], string, 0, NULL);
		}
		exit(0);
	}		
	return (0);
}
