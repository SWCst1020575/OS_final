/*
 * file: testcoop.c
 */
#include <8051.h>

#include "preemptive.h"

__data __at(0x35) ThreadID currentThread;
__data __at(0x3A) char currentChar1;
__data __at(0x3B) char currentChar2;
__data __at(0x3C) char shared;
__data __at(0x25) char full;
__data __at(0x26) char mutex;
__data __at(0x27) char empty;
__data __at(0x28) char lock1;
__data __at(0x29) char lock2;
__data __at(0x2A) char RRcount;

void Producer1(void) {
    currentChar1 = 'A';
    while (1) {
        SemaphoreWaitBody(lock1, __COUNTER__);
        SemaphoreWaitBody(empty, __COUNTER__);
        SemaphoreWaitBody(mutex, __COUNTER__);
        __critical{
            shared = currentChar1;
            if (currentChar1 == 'Z')
                currentChar1 = 'A';
            else
                currentChar1++;
            RRcount--;
        }
        SemaphoreSignal(mutex);
        SemaphoreSignal(full);
        if(RRcount == 0){
            RRcount = 5;
            SemaphoreSignal(lock2);
        }
        else
            SemaphoreSignal(lock1);
    }
}
void Producer2(void) {
    currentChar2 = '0';
    while (1) {
        SemaphoreWaitBody(lock2, __COUNTER__);
        SemaphoreWaitBody(empty, __COUNTER__);
        SemaphoreWaitBody(mutex, __COUNTER__);
        __critical{
            shared = currentChar2;
            if (currentChar2 == '9')
                currentChar2 = '0';
            else
                currentChar2++;
            RRcount--;
        }
        SemaphoreSignal(mutex);
        SemaphoreSignal(full);
        if(RRcount == 0){
            RRcount = 5;
            SemaphoreSignal(lock1);
        }
        else
            SemaphoreSignal(lock2);
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
            SBUF = shared;
        }
        SemaphoreSignal(mutex);
        SemaphoreSignal(empty);
        // polling
        while(!TI){}
        TI = 0;
    }
}

void main(void) {
    SemaphoreCreate(&full, 0);
    SemaphoreCreate(&mutex, 1);
    SemaphoreCreate(&empty, 1);
    SemaphoreCreate(&lock1, 1);
    SemaphoreCreate(&lock2, 0);
    RRcount = RR;
    ThreadCreate(Producer1);
    ThreadCreate(Producer2);
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
