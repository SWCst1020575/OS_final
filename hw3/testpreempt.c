/*
 * file: testcoop.c
 */
#include <8051.h>

#include "preemptive.h"

__data __at(0x3A) char currentChar;
__data __at(0x3B) char shared;
__data __at(0x3C) char full;
__data __at(0x3D) char mutex;
__data __at(0x3E) char empty;

void Producer(void) {
    currentChar = 'A';
    while (1) {
        // don't need yield
        /*if(buffer)
                ThreadYield();*/
        /*
        if (!buffer) {
            shared = currentChar;
            if (currentChar == 'Z')
                currentChar = 'A';
            else
                currentChar++;
            buffer = 1;
            // ThreadYield();
        }
        */
        SemaphoreWaitBody(empty, __COUNTER__);
        SemaphoreWaitBody(mutex, __COUNTER__);
        __critical{
            shared = currentChar;
            if (currentChar == 'Z')
                currentChar = 'A';
            else
                currentChar++;
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
        // don't need yield
        /*if (!buffer)
                ThreadYield();*/
        /*
        if (buffer) {
            // polling
            while (!TI) {
            }
            SBUF = shared;
            TI = 0;
            buffer = 0;
            // ThreadYield();
        }
        */
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
    SemaphoreCreate(&empty,1);
    ThreadCreate(Producer);
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
