

CC = sdcc
CFLAGS = -c 
LDFLAGS = 


C_OBJECTS = testpreempt.rel preemptive.rel

all: testpreempt.hex

testpreempt.hex:   $(C_OBJECTS) $(ASM_OBJECTS)
				$(CC) $(LDFLAGS) -o testpreempt.hex $(C_OBJECTS)

clean:
	rm *.hex *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym *.asm

%.rel:  %.c    preemptive.h Makefile
	$(CC) $(CFLAGS) $<
