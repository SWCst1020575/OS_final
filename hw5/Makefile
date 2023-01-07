

CC = sdcc
CFLAGS = -c 
LDFLAGS = 


C_OBJECTS = testparking.rel preemptive.rel

all: testparking.hex

testparking.hex:   $(C_OBJECTS) $(ASM_OBJECTS)
				$(CC) $(LDFLAGS) -o testparking.hex $(C_OBJECTS)

clean:
	rm *.hex *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym *.asm *.lk

%.rel:  %.c    preemptive.h Makefile
	$(CC) $(CFLAGS) $<
