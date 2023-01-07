/*
 * file: testcoop.c
 */
#include <8051.h>

#include "preemptive.h"


__data __at(0x3D) char currentChar1;
__data __at(0x3E) char currentChar2;
__data __at(0x20) char mutex;
__data __at(0x21) char empty;
__data __at(0x22) char car;
__data __at(0x23) char nextCar;
__data __at(0x24) char cars[MAXTHREADS];
__data __at(0x28) char carID;
__data __at(0x29) char i;
__data __at(0x2A) char wait;


void print(char a, char b) {

    TMOD = 0x20;
    TH1 = -6;
    SCON = 0x50;
    TR1 = 1;
    for(i = 0; i < 6;i++){
        switch (i){
            case 0:
                SBUF = a;
                break;
            case 1:
                SBUF = b;
                break;
            case 2:
                if(now() < 10)
                    SBUF = now() + '0';
                else{
                    SBUF = (now() / (char)10) + '0';
                    PrintInterupt;
                    SBUF = (now() % (char)10) + '0';
                }
                break;
            case 3:
                SBUF = ' ';
                break;
            case 4:
                SBUF = '/';
                break;
            case 5:
                SBUF = ' ';
                break;
        }
        PrintInterupt;
    }
}

void Producer(void) {
    SemaphoreWaitBody(wait, __COUNTER__);
    SemaphoreWaitBody(empty, __COUNTER__);
    SemaphoreWaitBody(mutex, __COUNTER__);
    // car arrive
    SemaphoreSignal(wait);
    EA = 0;
        if( currentChar1 == ('A' - 1) ){ 
            currentChar1 = cars[currentThread];
            print(cars[currentThread],'i');
        }else if( currentChar2 == ('A' - 1) ){
            currentChar2 = cars[currentThread];
            print(cars[currentThread],'i');
        }
    EA = 1;
    SemaphoreSignal(mutex);
    delay(3);
    // car leave
    EA = 0;
        if( currentChar1 == cars[currentThread] ){
            currentChar1 = 'A' - 1;
            print(cars[currentThread], 'o');
        }else if( currentChar2 == cars[currentThread] ){ 
            currentChar2 = 'A' - 1;
            print(cars[currentThread], 'o');
        }
    EA = 1;
    SemaphoreSignal(empty);
    SemaphoreSignal(nextCar);
    ThreadExit();
}




void main(void) {
    SemaphoreCreate(&mutex, 1);
    SemaphoreCreate(&empty, 2); // 2 parking space
    SemaphoreCreate(&nextCar, 0);
    SemaphoreCreate(&wait, 1);
    
    EA = 1;
    //ThreadCreate(Producer);
    currentChar1 = 'A' - 1;
    currentChar2 = 'A' - 1;
    car = 'A';

    // permission for 3 cars wait for parking space
    carID = ThreadCreate(Producer);
    cars[carID] = car;
    car++;

    carID = ThreadCreate(Producer);
    cars[carID] = car;
    car++;

    carID = ThreadCreate(Producer);
    cars[carID] = car;
    car++;
    while(time < 127){
        SemaphoreWaitBody(nextCar, __COUNTER__);
        carID = ThreadCreate(Producer);
        cars[carID] = car;
        car++;
        if(car > 'Z')
            car = 'A';
    }
    ThreadExit();
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
