
CC = sdcc
CFLAGS = -c 
LDFLAGS = 


C_OBJECTS = testcoop.rel cooperative.rel

all: testcoop.hex

testcoop.hex:   $(C_OBJECTS) $(ASM_OBJECTS)
				$(CC) $(LDFLAGS) -o testcoop.hex $(C_OBJECTS)

clean:
	rm *.hex *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym *.asm

%.rel:  %.c    cooperative.h Makefile
	$(CC) $(CFLAGS) $<