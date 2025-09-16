CC = gcc
CFLAGS = -Wall -Wextra -Wno-c11-extensions -Wno-unused-parameter -Wno-unused-label -std=c99 -pedantic -O3
OBJECTS = ray1.o ray2.o ray3.o ray4.o reala.o logexptbl.o bump1.o

%.o: %.c reala.h trace.h
	$(CC) $(CFLAGS) -c $< -o $@

%.s: %.c reala.h trace.h
	$(CC) $(CFLAGS) -S $< -o $@

raytrace: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

testreal: testreal.o reala.o logexptbl.o
	$(CC) $(CFLAGS) -o testreal testreal.o reala.o logexptbl.o

ray2.diff ray4.diff: %.diff: %.c %.generated.c
	-diff -u $*.generated.c $*.c > $@

ray2.generated.c ray4.generated.c: ray%.generated.c: ../asm/RAY%.ASM asm2c.rb
	ruby asm2c.rb $< $@

