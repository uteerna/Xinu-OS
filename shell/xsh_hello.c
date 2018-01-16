/* xsh_hello.c - xsh_hello */
	
#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------------------------------------------------
 * xsh_hello - check if there are one or more arguments in the prompt, if there are 2 then print the welcome string
 *------------------------------------------------------------------------------------------------------------------
 */ 
shellcmd xsh_hello(int nargs, char *args[]) {

	char welstr[60] = "Hello";  /* For storing the welcome string */

	/* Output info for '--help' argument */

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s\n\n", args[0]);
		return 0;
	}

	/* Check argument count */
	/* If the argumnet count is greater than 2 then return the error message "too many arguments" */

	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
	}
					
	/* If the argument count is less than 2 then returnt the error message "too few arguments" */
	
	if (nargs < 2) {
		fprintf(stderr, "%s: too few arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
	}
	
	/* If the argument count is 2 then print the welcome string "Hello <string>, Welcome to the world of Xiny!!" */

	if (nargs == 2) {
		printf("%s %s", welstr, args[1]);
		printf(", Welcome to the world of Xinu!!\n");
		return(0);
	}
}
