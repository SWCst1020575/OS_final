/*
 * file: testcoop.c
 */
#include <8051.h>

#include "preemptive.h"

__data __at(0x35) ThreadID currentThread;
__data __at(0x3C) char currentChar1;
__data __at(0x3D) char currentChar2;
__data __at(0x3E) char shared;
__data __at(0x20) char full;
__data __at(0x21) char mutex;
__data __at(0x22) char empty;
__data __at(0x23) char cars[4];
__data __at(0x27) char id;

void print(char a, char b) {

    TMOD = 0x20;
    TH1 = -6;
    SCON = 0x50;
    TR1 = 1;
    for(id = 0; id < 4;id++){
        switch (id){
        case 0:
            SBUF = a;
            break;
        case 1:
            SBUF = b;
            break;
        case 2:
            SBUF = (time & 7) + '0';
            break;
        case 3:
            SBUF = '\n'
            break;
        }
        // polling
        while(!TI){}
        TI = 0;
    }
}

void Producer(void) {
    while (1) {
        //SemaphoreWaitBody(lock1, __COUNTER__);
        SemaphoreWaitBody(empty, __COUNTER__);
        SemaphoreWaitBody(mutex, __COUNTER__);
        __critical{
            shared = currentChar1;
            if (currentChar1 == 'Z')
                currentChar1 = 'A';
            else
                currentChar1++;
            RRcount--;
            if( currentChar1 == '0' ){ 
                currentChar1 = cars[currentThread];
                print(cars[currentThread],'i');
            }else if( currentChar2 == '0' ){
                currentChar2 = cars[currentThread];
                print(cars[currentThread],'i');
            }
        }
        SemaphoreSignal(mutex);
        delay(2);
        __critical{
            if( currentChar1 == cars[currentThread] ){
                currentChar1 = '0';
                print(cars[currentThread], 'o');
            }else if( currentChar2 == cars[currentThread] ){ 
                currentChar2 = '0';
                print(cars[currentThread], 'o');
            }
        }
        SemaphoreSignal(empty);
        
    }
}




void main(void) {
    SemaphoreCreate(&full, 0);
    SemaphoreCreate(&mutex, 1);
    SemaphoreCreate(&empty, 1);
    
    ThreadCreate(Producer);
    //Consumer();
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
