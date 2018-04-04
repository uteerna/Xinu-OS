#include <xinu.h>
#include <future.h>
#include<stdlib.h>

uint32 future_test(int nargs, char *args[]);
int ffib(int n);
future **fibfut;

/**
 *  * Test Futures
 *   */

/* Creating a command "run" */
shellcmd xsh_run(int nargs, char *args[])
{
  future_test(nargs,args);
	return SHELL_OK;
}

uint future_prod(future* fut,int n) 
{
  printf("Produced %d\n",n);
  future_set(fut, &n);
  return OK;
}

uint future_cons(future* fut) 
{
  int i, status;
  status = (int)future_get(fut, &i);
  if (status < 1) {
    printf("future_get failed\n");
    return -1;
  }
  printf("Consumed %d\n", i);
  return OK;
}

uint32 future_test(int nargs, char *args[])
{
  /*Number of arguments less than 3*/
  if(nargs<3)
  {
    fprintf(stderr, "%s: too few arguments\n", args[0]);
    return SHELL_ERROR;
  }

  /*Number of arguments more than 4*/
  if (nargs > 4)
  {
    fprintf(stderr, "%s: too many arguments\n", args[0]);
    return SHELL_ERROR;
  }

  /* Checking if the arguments passed to run are future_test -pc */
  if (nargs == 3 && strncmp(args[1], "future_test", 13)==0 && strncmp(args[2], "-pc", 4) == 0)
  {
    future* f_exclusive,
            * f_shared,
            * f_queue;
    f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
    f_shared    = future_alloc(FUTURE_SHARED);
    f_queue     = future_alloc(FUTURE_QUEUE);
   
    //Test FUTURE_EXCLUSIVE
    resume(create(future_cons, 1024, 20, "fcons1", 1, f_exclusive));
    resume(create(future_prod, 1024, 20, "fprod1", 2, f_exclusive, 1));

    //Test FUTURE_SHARED
    resume(create(future_cons, 1024, 20, "fcons2", 1, f_shared));
    resume(create(future_cons, 1024, 20, "fcons3", 1, f_shared));
    resume(create(future_cons, 1024, 20, "fcons4", 1, f_shared)); 
    resume(create(future_cons, 1024, 20, "fcons5", 1, f_shared));
    resume(create(future_prod, 1024, 20, "fprod2", 2, f_shared, 2));

    //Test FUTURE_QUEUE
    resume(create(future_cons, 1024, 20, "fcons6", 1, f_queue));
    resume(create(future_cons, 1024, 20, "fcons7", 1, f_queue));
    resume(create(future_cons, 1024, 20, "fcons8", 1, f_queue));
    resume(create(future_cons, 1024, 20, "fcons9", 1, f_queue));
    resume(create(future_prod, 1024, 20, "fprod3", 2, f_queue, 3));
    resume(create(future_prod, 1024, 20, "fprod4", 2, f_queue, 4));
    resume(create(future_prod, 1024, 20, "fprod5", 2, f_queue, 5));
    resume(create(future_prod, 1024, 20, "fprod6", 2, f_queue, 6));
    sleep(1);
    return SHELL_OK;
  }

  /* Checking if the number of args is 4 and the arguments passed to run are future_test -f N */
  if (nargs == 4 && strncmp(args[1], "future_test", 13)==0 && strncmp(args[2], "-f", 2) == 0)
  {
    int array_size,i;
    array_size = atoi(args[3]);
    int result = 0;
    fibfut = (future**)getmem((array_size+1)*sizeof(future*));

    for(i=0; i<=array_size;i++)
    {
      fibfut[i] = future_alloc(FUTURE_SHARED);
    }
    for(i=0; i<=array_size; i++)
    {
      char name = "Fibonacci"+(char)i;
      resume(create(ffib, 1024, 20, name, 1, i));
    }
    future_get(fibfut[array_size], &result);

    printf("Futures Fibonacci for N = %d.\n",array_size);
    printf("Nth Fibonacci value for N = %d is %d\n", array_size,result);
    return OK;
  }
  else
  {
    printf("Invalid command!!\n");
    return SHELL_OK;
  }
}  

/* Fibonacci */
int ffib(int n)
{
  int minus1 = 0;
  int minus2 = 0;
  int this = 0;
  int zero = 0;
  int one = 1;

  if (n == 0) {
    future_set(fibfut[0], &zero);
    return OK;
  }

  if (n == 1) {
    future_set(fibfut[1], &one);
    return OK;
  }

  future_get(fibfut[n-2], &minus2);
  future_get(fibfut[n-1], &minus1);

  this = minus1 + minus2;

  future_set(fibfut[n], &this);

  return(0);

}