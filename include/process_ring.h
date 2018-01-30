#ifndef PROCESS_RING
#define PROCESS_RING
#include<xinu.h>
#include<stdio.h>
#include<stdlib.h>
#include<prototypes.h>
extern volatile int inbox[64];
extern volatile int value;
extern volatile sid32 sem[64];
extern volatile sid32 complete_sem;
extern process polling(int, int, int);
extern process sync(int, int, int);
#endif