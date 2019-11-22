## Multicore/Multinode hands-on and exercises 


###  openmp program and number of threads 

Try to compile run and execute the simple `omp_101.c` code and understand correctly how to change the number of threads


### learn how to use hwloc/numactl/likwid commands

Test all the commands presented in the slides and be familiar with them.
You can try to modify the omp_101.c program to make it run longer and try to assign it on different threads by means of numactl/hwloc commands.


### Exercise 1:  run IMPI benchmark (ping-pong) to measure latency among MPI processes assigned on different cores  and on different nodes 

You will run intel MPI ping-pong benchmark among to processors within the same node and you will try to estimate latency and bandwidth.
see readme.md file in the mpi  directory 


### Exercise 2 : run stream benchmark to test different performance when wrongly assigned the processor

Take the stream benchmark (https://www.cs.virginia.edu/stream/)
compile it and familiarize with it.

Tasks:

1. Estimate the overall bandwidth of one node of ulysess 
2. Estimate the bandwidth of one single core of ulysess 
3. Compare the bandwith of one single core reading from memory associated to the socket against one core reading from memory associated with the other socket 
4. Do it for all threads available in one socket (how many do we have ?)  and make a plot of the results 

5.Summarize all the results in a short report and commit it to your github account 


### Exercise 3: compile and run nodeperf.c program 

see readme.md file within the  nodeperf directory 
 
