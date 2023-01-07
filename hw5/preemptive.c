#include "preemptive.h"

#include <8051.h>

// create a counting semaphore s that is initialized to n
void SemaphoreCreate(char *s, char n) {
   __critical {
      *s = n;
   }
   return;
}

__data __at(0x30) char threadSP[MAXTHREADS];
__data __at(0x34) char bitmap; //check if thread exist
__data __at(0x36) char oldThreadSP;
__data __at(0x37) ThreadID newThread;
__data __at(0x2A) char j;


#define SAVESTATE {					\
	__asm push ACC					\
		push B						\
		push DPL					\
		push DPH					\
		push PSW					\
	__endasm;						\
	threadSP[currentThread] = SP;	\
}

#define RESTORESTATE {				\
	SP = threadSP[currentThread];	\
	__asm							\
		pop PSW						\
		pop DPH						\
		pop DPL						\
		pop B						\
		pop ACC						\
	__endasm;						\
}

extern void main(void);

void Bootstrap(void) {
    bitmap = 0;
    TMOD = 0;   // timer 0 mode 0
    IE = 0x82;  // enable timer 0 interrupt; keep consumer polling
    TR0 = 1;    // set bit TR0 to start running timer 0
    time = 0;
    timeInterval = 0;
    for (j = 0; j < MAXTHREADS; j++)
        threadSP[j] = 0x3F + 0x10 * j;
    currentThread = ThreadCreate(main);
    RESTORESTATE;
}

ThreadID ThreadCreate(FunctionPtr fp) {
    if (bitmap == 0x15)
        return -1;
    // a, b
    EA = 0;//__critical{
        if( !( bitmap & 1 ) ){
            bitmap |= 1;
            newThread = 0;
        }else if( !( bitmap & 2 ) ){
            bitmap |= 2;
            newThread = 1;
        }else if( !( bitmap & 4 ) ){
            bitmap |= 4;
            newThread = 2;
        }else if( !( bitmap & 8 ) ){
            bitmap |= 8;
            newThread = 3;
        }
    // c
    oldThreadSP = SP;
    SP = (0x3F) + (0x10) * newThread;

    // d
    __asm
        PUSH DPL
        PUSH DPH
    __endasm;
    // e
    __asm
        MOV A, 0x00
        PUSH ACC                    // ACC
        PUSH ACC          			// B
        PUSH ACC      				// DPL
        PUSH ACC  					// DPH
    __endasm;

    // f
    PSW = newThread << 3;
    __asm
        PUSH PSW
    __endasm;
    // g
    threadSP[newThread] = SP;
    // h
    SP = oldThreadSP;
    EA = 1;//}
    // i
    return newThread;
}

void ThreadYield(void) {
    SAVESTATE;
    do {
        currentThread++;
        if (currentThread == MAXTHREADS)
            currentThread = 0;
        if (bitmap & (1 << currentThread))
            break;
    } while (1);
    RESTORESTATE;
}

void ThreadExit(void) {
    EA = 0;
    bitmap -= (1 << currentThread);
    
    if(bitmap & 1)
        currentThread = 0;
    else if(bitmap & 2)
        currentThread = 1;
    else if(bitmap & 4)
        currentThread = 2;
    else if(bitmap & 8)
        currentThread = 3;
    else
        while(1){}
    RESTORESTATE;
    EA = 1;
}

void myTimer0Handler() {
    EA = 0;
    SAVESTATE;
    SAVERIGISTER;
    timeInterval++;
    if(timeInterval == 8){
        time++;
        timeInterval = 0;
    }
    if(time > 99)
        time = 0;
    
    do {
        currentThread++;
        if (currentThread == MAXTHREADS)
            currentThread = 0;
        if (bitmap & (1 << currentThread))
            break;
    } while (1);
    RESTORERIGISTER;
    RESTORESTATE;
    EA = 1;
    __asm
		RETI
	__endasm;
}


unsigned char now(void){
    return time;
}