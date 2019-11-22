## Simple Makefile to generate stream benchmark in serial and open-mp version
CC=gcc
ARRAY_SIZE=80000000
TIMES=50
CFLAGS= -DSTREAM_ARRAY_SIZE=$(ARRAY_SIZE) -DNTIMES=$(TIMES) -O3  
# openmp compilation
CFLAGS_OMP= -DSTREAM_ARRAY_SIZE=$(ARRAY_SIZE) -DNTIMES=$(TIMES) -O3 -fopenmp 

# these below are for intel compiler 
# CFLAGS=-DSTREAM_ARRAY_SIZE=400000000 -O3 -xHost -mcmodel=medium -openmp
EXE=stream.x stream_omp.x  

all:$(EXE)

stream.x : stream.c
	$(CC) $(CFLAGS) $< -o $@

stream_omp.x : stream.c
	$(CC) $(CFLAGS_OMP) $< -o $@
clean:
	rm $(EXE)
