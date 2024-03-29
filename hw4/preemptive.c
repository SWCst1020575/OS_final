#include "preemptive.h"

#include <8051.h>

// create a counting semaphore s that is initialized to n
void SemaphoreCreate(char *s, char n) {
   __critical {
      *s = n;
   }
   return;
}

__data __at(0x30) char threadSP[4];
__data __at(0x34) char bitmap; //check if thread exist
__data __at(0x35) ThreadID currentThread;
__data __at(0x36) char oldThreadSP;
__data __at(0x37) ThreadID newThread;
__data __at(0x38) ThreadID threadNum;
__data __at(0x39) ThreadID nextThread;

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

    currentThread = ThreadCreate(main);
    RESTORESTATE;
}

ThreadID ThreadCreate(FunctionPtr fp) {
    if (bitmap == 0x15)
        return -1;
    // a, b
    EA=0;
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
    EA=1;
    // i
    return newThread;
}

void ThreadYield(void) {
    SAVESTATE;
    do {
        currentThread++;
        if (currentThread > 3)
            currentThread = 0;
        if (bitmap & (1 << currentThread))
            break;
    } while (1);
    RESTORESTATE;
}

void ThreadExit(void) {
    bitmap ^= (1 << currentThread);
    do {
        currentThread++;
        if (currentThread > 3)
            currentThread = 0;
        if (currentThread == 0 && (bitmap & 0x01) == 0x01)
            break;
        else if (currentThread == 1 && (bitmap & 0x02) == 0x02)
            break;
        else if (currentThread == 2 && (bitmap & 0x04) == 0x04)
            break;
        else if (currentThread == 3 && (bitmap & 0x08) == 0x08)
            break;
    } while (1);

    RESTORESTATE;
}

void myTimer0Handler() {
    EA = 0;
    SAVESTATE;
    SAVERIGISTER;
    do {
        currentThread++;
        if (currentThread >= 3)
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
