/*
 * file: preemptive.h (modified from cooperative.h)
 */

#ifndef __PREEMPTIVE_H__
#define __PREEMPTIVE_H__

#define MAXTHREADS 4 /* not including the scheduler */
/* the scheduler does not take up a thread of its own */

#define CNAME(s) _ ## s
#define LABEL(label) label ## $

#define SemaphoreWaitBody(s, label){ 				\
	__asm							 				\
		LABEL(label):	MOV ACC, CNAME(s)			\
						JZ  LABEL(label)			\
	   					DEC  CNAME(s) 				\
	__endasm;										\
}

// signal() semaphore s
#define SemaphoreSignal(s){		\
	__asm 						\
		INC CNAME(s)			\
	__endasm;					\
}

typedef char ThreadID;
typedef void (*FunctionPtr)(void);

void SemaphoreCreate(char *, char);

ThreadID ThreadCreate(FunctionPtr);
void ThreadYield(void);
void ThreadExit(void);
void myTimer0Handler();


#endif
