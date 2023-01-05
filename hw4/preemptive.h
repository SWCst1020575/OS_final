/*
 * file: preemptive.h (modified from cooperative.h)
 */

#ifndef __PREEMPTIVE_H__
#define __PREEMPTIVE_H__

#define MAXTHREADS 4 /* not including the scheduler */
/* the scheduler does not take up a thread of its own */
#define RR 5  

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

#define SAVERIGISTER {	\
	__asm				\
        MOV A, R0		\
        PUSH ACC		\
        MOV A, R1		\
        PUSH ACC		\
        MOV A, R2		\
        PUSH ACC		\
        MOV A, R3		\
        PUSH ACC		\
        MOV A, R4		\
        PUSH ACC		\
        MOV A, R5		\
        PUSH ACC		\
        MOV A, R6		\
        PUSH ACC		\
        MOV A, R7		\
        PUSH ACC		\
    __endasm;			\
}
#define RESTORERIGISTER {	\
	__asm					\
        POP ACC				\
        MOV R7, A			\
        POP ACC				\
        MOV R6, A			\
        POP ACC				\
        MOV R5, A			\
        POP ACC				\
        MOV R4, A			\
        POP ACC				\
        MOV R3, A			\
        POP ACC				\
        MOV R2, A			\
        POP ACC				\
        MOV R1, A			\
        POP ACC				\
        MOV R0, A			\
    __endasm;				\
}

typedef char ThreadID;
typedef void (*FunctionPtr)(void);

void SemaphoreCreate(char *, char);

ThreadID ThreadCreate(FunctionPtr);
void ThreadYield(void);
void ThreadExit(void);
void myTimer0Handler();


#endif
