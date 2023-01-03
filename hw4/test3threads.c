/*
 * file: testcoop.c
 */
#include <8051.h>

#include "preemptive.h"

__data __at(0x30) char firstThreadSP;
__data __at(0x35) ThreadID currentThread;
__data __at(0x3A) char currentChar1;
__data __at(0x3B) char currentChar2;
__data __at(0x3C) char shared;
__data __at(0x25) char full;
__data __at(0x26) char mutex;
__data __at(0x27) char empty;

void Producer1(void) {
    currentChar1 = 'A';
    while (1) {
        SemaphoreWaitBody(empty, __COUNTER__);
        SemaphoreWaitBody(mutex, __COUNTER__);
        __critical{
            shared = currentChar1;
            if (currentChar1 == 'Z')
                currentChar1 = 'A';
            else
                currentChar1++;
            
        }
        SemaphoreSignal(mutex);
        SemaphoreSignal(full);
    }
}
void Producer2(void) {
    currentChar2 = '0';
    while (1) {
        SemaphoreWaitBody(empty, __COUNTER__);
        SemaphoreWaitBody(mutex, __COUNTER__);
        __critical{
            shared = currentChar2;
            if (currentChar2 == '9')
                currentChar2 = '0';
            else
                currentChar2++;
        }
        SemaphoreSignal(mutex);
        SemaphoreSignal(full);
    }
}

void Consumer(void) {

    
    TMOD = 0x20;
    TH1 = -6;
    SCON = 0x50;
    TR1 = 1;
    TI = 1;
    while (1) {
        SemaphoreWaitBody(full, __COUNTER__);
        SemaphoreWaitBody(mutex, __COUNTER__);
        __critical{
            // polling
            while(!TI){}
            SBUF = shared;
            TI = 0;
        }
        SemaphoreSignal(mutex);
        SemaphoreSignal(empty);
        
    }
}

void main(void) {
    SemaphoreCreate(&full, 0);
    SemaphoreCreate(&mutex, 1);
    SemaphoreCreate(&empty, 1);

    currentThread = ThreadCreate(Producer1);
    currentThread = ThreadCreate(Producer2);
    currentThread = 0;
    SP = firstThreadSP;
    Consumer();
} 

void _sdcc_gsinit_startup(void) {
    __asm
		ljmp _Bootstrap
	__endasm;
}

void _mcs51_genRAMCLEAR(void) {}
void _mcs51_genXINIT(void) {}
void _mcs51_genXRAMCLEAR(void) {}
void timer0_ISR(void) __interrupt(1) {
    __asm
		ljmp _myTimer0Handler
	__endasm;
}
