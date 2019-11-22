SHELL=/bin/sh
CC=gcc
FC=gfortran
F90=gfortran
#FC=ifort
#F90=ifort
CFLAGS=-g
FFLAGS=-g
BCFLAGS=-fbounds-check
#BCFLAGS=-CB
.SUFFIXES:
.SUFFIXES: .f90 .c .f

ALL=ex01-f77 ex01-f90 ex01-c ex02-f77 ex02-f90 ex02-c 	\
 ex03-f77 ex03-f90 ex03-f77-bc ex03-f90-bc ex04 ex05 ex06 dotprod_serial

all: $(ALL)

ex01-f77: ex01.f
	$(FC) $(FFLAGS) -o $@ $<
ex01-f90: ex01.f90
	$(F90) $(FFLAGS) -o $@ $<
ex01-c  : ex01.c
	$(CC) $(CFLAGS) -o $@ $<

ex02-f77: ex02.f
	$(FC) $(FFLAGS) -o $@ $<
ex02-f90: ex02.f90
	$(F90) $(FFLAGS) -o $@ $<
ex02-c  : ex02.c
	$(CC) $(CFLAGS) -o $@ $<

ex03-f77: ex03.f
	$(FC) $(FFLAGS) -o $@ $<
ex03-f90: ex03.f90 helper.o
	$(F90) $(FFLAGS) -o $@ $< helper.o
ex03-c  : ex03.c
	$(CC) $(CFLAGS) -o $@ $<

ex03-f77-bc: ex03.f
	$(FC) $(BCFLAGS) $(FFLAGS) -o $@ $<
ex03-f90-bc: ex03.f90 helper.o
	$(F90) $(BCFLAGS) $(FFLAGS) -o $@ $< helper.o

ex04: ex04.c
	$(CC) $(CFLAGS) -o $@ $<
ex05: ex05.c
	$(CC) $(CFLAGS) -o $@ $<
ex06: ex06.c
	$(CC) $(CFLAGS) -o $@ $<
dotprod_serial: dotprod_serial.c
	$(CC) $(CFLAGS) -o $@ $<

helper.o: helper.f
	$(FC) $(FFLAGS) -c -o $@ $<

clean:
	rm -f $(ALL) *~ a.out core helper.o
