# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# if you are cross-compiling, you need to point to the right executables
# and change the flags to ld and the build procedure for as
#GCCDIR = ~/gnu/local/decstation-ultrix/bin/
GCCDIR = /usr/local/decstation-ultrix/bin/
LDFLAGS = -T script -N
ASFLAGS = -mips1
CPPFLAGS = $(INCDIR)


# if you aren't cross-compiling:
#GCCDIR =
#LDFLAGS = -N -T 0
#ASFLAGS =
#CPPFLAGS = -P $(INCDIR)


CC = $(GCCDIR)gcc
AS = $(GCCDIR)as
LD = $(GCCDIR)ld

CPP = /lib/cpp
INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR) -mips1

all: pruebaExec2ndTry programa

#all: halt pruebasSyscall pruebaExec shell matmult sort

#start.o: start.s ../userprog/syscall.h
#	$(CPP) $(CPPFLAGS) start.s > strt.s
#	$(AS) $(ASFLAGS) -o start.o strt.s
#	rm strt.s
#
#halt.o: halt.c
#	$(CC) $(CFLAGS) -c halt.c
#halt: halt.o start.o
#	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
#	../bin/coff2noff halt.coff halt
#
#shell.o: shell.c
#	$(CC) $(CFLAGS) -c shell.c
#shell: shell.o start.o
#	$(LD) $(LDFLAGS) start.o shell.o -o shell.coff
#	../bin/coff2noff shell.coff shell
#
#sort.o: sort.c
#	$(CC) $(CFLAGS) -c sort.c
#sort: sort.o start.o
#	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
#	../bin/coff2noff sort.coff sort
#
#matmult.o: matmult.c
#	$(CC) $(CFLAGS) -c matmult.c
#matmult: matmult.o start.o
#	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
#	../bin/coff2noff matmult.coff matmult
#
#pruebasSyscall.o: pruebasSyscall.c
#	$(CC) $(CFLAGS) -c pruebasSyscall.c
#pruebasSyscall: pruebasSyscall.o start.o
#	$(LD) $(LDFLAGS) start.o pruebasSyscall.o -o pruebasSyscall.coff
#	../bin/coff2noff pruebasSyscall.coff pruebasSyscall
#	
#pruebaExec.o: pruebaExec.c
#	$(CC) $(CFLAGS) -c pruebaExec.c
#pruebaExec: pruebaExec.o start.o
#	$(LD) $(LDFLAGS) start.o pruebaExec.o -o pruebaExec.coff
#	../bin/coff2noff pruebaExec.coff pruebaExec

pruebaExec2ndTry.o: pruebaExec2ndTry.c
	$(CC) $(CFLAGS) -c pruebaExec2ndTry.c
pruebaExec2ndTry: pruebaExec2ndTry.o start.o
	$(LD) $(LDFLAGS) start.o pruebaExec2ndTry.o -o pruebaExec2ndTry.coff
	../bin/coff2noff pruebaExec2ndTry.coff pruebaExec2ndTry
	
programa.o: programa.c
	$(CC) $(CFLAGS) -c programa.c
programa: programa.o start.o
	$(LD) $(LDFLAGS) start.o programa.o -o programa.coff
	../bin/coff2noff programa.coff programa

# Estas reglas sirven para compilar programas simples,
# que consistan en un �nico fuente.
# Las reglas anteriores para construir los ejecutables
# halt, shell, sort y matmult se podr�an suprimir

%: %.c 
	make start.o 
	$(CC) $(CFLAGS) -c $<
	$(LD) $(LDFLAGS) start.o $*.o -o $*.coff
	../bin/coff2noff $*.coff $@
	

