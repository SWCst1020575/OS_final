#include "cooperative.h"

#include <8051.h>

/*
 * @@@ [2 pts] declare the static globals here using
 *        __data __at (address) type name; syntax
 * manually allocate the addresses of these variables, for
 * - saved stack pointers (MAXTHREADS)
 * - current thread ID
 * - a bitmap for which thread ID is a valid thread;
 *   maybe also a count, but strictly speaking not necessary
 * - plus any temporaries that you need.
 */

__data __at (0x30) char threadSP[4];
__data __at (0x34) char bitmap;
__data __at (0x35) ThreadID currentThread;
__data __at (0x36) char oldThreadSP;
__data __at (0x37) ThreadID newThread;
__data __at (0x38) ThreadID threadNum;
__data __at (0x20) char i;



/*
 * @@@ [8 pts]
 * define a macro for saving the context of the current thread by
 * 1) push ACC, B register, Data pointer registers (DPL, DPH), PSW
 * 2) save SP into the saved Stack Pointers array
 *   as indexed by the current thread ID.
 * Note that 1) should be written in assembly,
 *     while 2) can be written in either assembly or C
 */
#define SAVESTATE               		\
    {                           		\
        __asm                   		\
            push ACC 					\
			push B						\
			push DPL					\
			push DPH					\
			push PSW					\
		__endasm;       				\
		threadSP[currentThread] = SP;	\
    }

/*
 * @@@ [8 pts]
 * define a macro for restoring the context of the current thread by
 * essentially doing the reverse of SAVESTATE:
 * 1) assign SP to the saved SP from the saved stack pointer array
 * 2) pop the registers PSW, data pointer registers, B reg, and ACC
 * Again, popping must be done in assembly but restoring SP can be
 * done in either C or assembly.
 */
#define RESTORESTATE            		\
    {   								\
  		SP = threadSP[currentThread];	\
        __asm                   		\
            pop PSW						\
			pop DPH						\
			pop DPL						\
			pop B						\
			pop ACC						\
   		__endasm;       				\
    }

/*
 * we declare main() as an extern so we can reference its symbol
 * when creating a thread for it.
 */

extern void main(void);

/*
 * Bootstrap is jumped to by the startup code to make the thread for
 * main, and restore its context so the thread can run.
 */

void Bootstrap(void)
{
    /*
     * @@@ [2 pts]
     * initialize data structures for threads (e.g., mask)
     *
     * optional: move the stack pointer to some known location
     * only during bootstrapping. by default, SP is 0x07.
     *
     * @@@ [2 pts]
     *     create a thread for main; be sure current thread is
     *     set to this thread ID, and restore its context,
     *     so that it starts running main().
     */
	bitmap = 0;
	/*for(i=0;i<4;i++)
		threadSP[i] = 0x3F + 0x10 * i;*/
	currentThread = ThreadCreate(main);
	RESTORESTATE;
}

/*
 * ThreadCreate() creates a thread data structure so it is ready
 * to be restored (context switched in).
 * The function pointer itself should take no argument and should
 * return no argument.
 */
ThreadID ThreadCreate(FunctionPtr fp)
{
    /*
     * @@@ [2 pts]
     * check to see we have not reached the max #threads.
     * if so, return -1, which is not a valid thread ID.
     */
	if(bitmap == 15)
		return -1;


    /*
     * @@@ [5 pts]
     *     otherwise, find a thread ID that is not in use,
     *     and grab it. (can check the bit mask for threads),
     *
     * @@@ [18 pts] below
     * a. update the bit mask
         (and increment thread count, if you use a thread count,
          but it is optional)
       b. calculate the starting stack location for new thread
       c. save the current SP in a temporary
          set SP to the starting location for the new thread
       d. push the return address fp (2-byte parameter to
          ThreadCreate) onto stack so it can be the return
          address to resume the thread. Note that in SDCC
          convention, 2-byte ptr is passed in DPTR.  but
          push instruction can only push it as two separate
          registers, DPL and DPH.
       e. we want to initialize the registers to 0, so we
          assign a register to 0 and push it four times
          for ACC, B, DPL, DPH.  Note: push #0 will not work
          because push takes only direct address as its operand,
          but it does not take an immediate (literal) operand.
       f. finally, we need to push PSW (processor status word)
          register, which consist of bits
           CY AC F0 RS1 RS0 OV UD P
          all bits can be initialized to zero, except <RS1:RS0>
          which selects the register bank.
          Thread 0 uses bank 0, Thread 1 uses bank 1, etc.
          Setting the bits to 00B, 01B, 10B, 11B will select
          the register bank so no need to push/pop registers
          R0-R7.  So, set PSW to
          00000000B for thread 0, 00001000B for thread 1,
          00010000B for thread 2, 00011000B for thread 3.
       g. write the current stack pointer to the saved stack
          pointer array for this newly created thread ID
       h. set SP to the saved SP in step c.
       i. finally, return the newly created thread ID.
     */
		
	// a, b
	for(threadNum=0;threadNum<4;threadNum++)
		if(!(bitmap & (1 << threadNum))){
			newThread = threadNum;
			bitmap |= (1 << threadNum);
			break;
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
		PUSH ACC  // ACC
		PUSH ACC  // B
		PUSH ACC  // DPL
		PUSH ACC  // DPH
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
	// i
	return newThread;

}

/*
 * this is called by a running thread to yield control to another
 * thread.  ThreadYield() saves the context of the current
 * running thread, picks another thread (and set the current thread
 * ID to it), if any, and then restores its state.
 */

void ThreadYield(void)
{
    SAVESTATE;
    do {
        /*
         * @@@ [8 pts] do round-robin policy for now.
         * find the next thread that can run and
         * set the current thread ID to it,
         * so that it can be restored (by the last line of
         * this function).
         * there should be at least one thread, so this loop
         * will always terminate.
         */
		currentThread++;
		if(currentThread > 3)
			currentThread = 0;
		if(bitmap & (1 << currentThread))
			break;
    } while (1);
    RESTORESTATE;
}

/*
 * ThreadExit() is called by the thread's own code to terminate
 * itself.  It will never return; instead, it switches context
 * to another thread.
 */
void ThreadExit(void)
{
    /*
     * clear the bit for the current thread from the
     * bit mask, decrement thread count (if any),
     * and set current thread to another valid ID.
     * Q: What happens if there are no more valid threads?
     */
	bitmap ^= (1 << currentThread);
	do{		
		currentThread++;
		if(currentThread > 3)
			currentThread = 0;
		if(currentThread == 0 && (bitmap & 0x01) == 0x01)
			break;
		else if(currentThread == 1 && (bitmap & 0x02) == 0x02)
			break;
		else if(currentThread == 2 && (bitmap & 0x04) == 0x04)
            break;
		else if(currentThread == 3 && (bitmap & 0x08) == 0x08)
			break;
	}while(1);
    
	RESTORESTATE;
}
