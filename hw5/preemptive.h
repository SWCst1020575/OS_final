/*
 * file: preemptive.h (modified from cooperative.h)
 */

#ifndef __PREEMPTIVE_H__
#define __PREEMPTIVE_H__

#define MAXTHREADS (char)4 /* not including the scheduler */
/* the scheduler does not take up a thread of its own */
#define RR 5  

#define CNAME(s) _ ## s
#define LABEL(label) label ## $

typedef char ThreadID;
typedef void (*FunctionPtr)(void);

__data __at(0x35) ThreadID currentThread;
__data __at(0x38) unsigned char time;
__data __at(0x39) unsigned char delays[4];
__data __at(0x2B) char timeInterval;

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

#define delay(n)\
        delays[currentThread] = time + n;\
        while( delays[currentThread] != time ){}

#define PrintInterupt {while(!TI){} TI = 0;}


void SemaphoreCreate(char *, char);

ThreadID ThreadCreate(FunctionPtr);
void ThreadYield(void);
void ThreadExit(void);
void myTimer0Handler();
//void delay(unsigned char);
unsigned char now(void);

#endif
