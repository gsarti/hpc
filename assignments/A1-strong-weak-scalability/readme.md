# part 1: learn to use the Ulysses queue system 

- how to login into Ulysses 
- how to use modules 
- how to compile in serial
- how to compile an MPI application 
- how to submit jobs 
 - interactive usage 
 - batch usage
 - selecting resources ( cores and nodes/ walltime)
- how to run an MPI application on Ulysses


## Ulisse Environment Setup
Login:
```
$ ssh sissausername@frontend2.hpc.sissa.it
```
## use module 
Software is encapsulated in modules, check avaliability and and load modules with:
```
$ module avail
$ module load
```
## Run Serial Code
```
$ gcc pi.c -o serial_pi
$ ./serial_pi 1000000
```
## Run Parallel code

### Interactive mode
Load mpi modules
```
$ module load openmpi
loading dependency gnu/4.9.2
loading dependency numactl/2.0.10
loading dependency hwloc/1.10.0
```
Compile in parallel:
```
$ mpicc mpi_pi.c -o parallel_pi
```
Login on a node in interactive mode:
```
qsub -l nodes=1:ppn=20 -I
```
with this line we require one node in interactive mode and ask for 20 procecessors per node.
```
[user@login1 solution]$ qsub -l nodes=1:ppn=20 -I
qsub: waiting for job 199513.master1 to start
qsub: job 199513.master1 ready

[user@cn01-20 ~]$
```
Load mpi module again.
```
$ module load openmpi
loading dependency gnu/4.9.2
loading dependency numactl/2.0.10
loading dependency hwloc/1.10.0
```
Run the program:
```
$ mpirun -np 4 parallel_pi 1000000
```


### Non interactive

just submit jobs..

Reserved queue `reserved3`.

 
  
# part 2: compute strong and weak scalability of an HPC application

You will learn how to establish the scalability on a HPC application. The application we are using here is a toy  application:
a Monte-Carlo integration of a quarter of a unit circle  to compute the number PI (given as area_computed*4) 
We provide a basic implementation of the algorithm ( program pi.c ) and we also give a parallel MPI implementation ( MPIpi.c ) that computes PI by the same algorithm  using MPI to compute the algorithm in parallel. 
Your exercise is to see how well this application scales up to the total number of cores of one node. 
You can modify the codes we have given, or you can write your own.

Steps to do: 

 1. Determine the CPU time required to calculate PI with the serial calculation using 10 millions iterations (stone throws). Make sure that this is the actual run time and does not include any
system time.

 Hint: use /usr/bin/time command to time all the application  
 
 2. Get the MPI code running for the same number (10 millions) of iterations.

 3. First let us do some running that constitutes a strong scaling test. 
 
This means keeping the problem size constant, or in other words, keeping Niter = 10 millions.
Start by running the MPI code with only one processor doing the numerical computation. A comparison of this to the serial calculation gives you some idea of the overhead associated with MPI.

 4. Again keeping Niter = 10000000, run the MPI code for 2, 4, 8 and 16 and 20 MPI processs. 

In principle, all that needs to be done is to run the program multiple times, changing the -np argument to mpirun each time. Such a study could be as simple as the following bash for loop:
```
 for procs in 1 2 4 8 16 32 64 ; do
 time mpirun -np ${procs} my-program the-input
 done
```
A real study is of course a litte bit more complicated because we neeed to get the performance of each of those runs. 
Furthermore we run on Ulysses and then we need to submit jobs.

In any case it is important to script your job executions, so that you can quickly measure scalability. So in this part of the exercise you need to implement a strong scaling script.
Script will be implemented by means of bash scripting, or if you prefer you can use python as well.
Please also implement a way to easily change the number of iteration and check for different number of them how scalability varies.
hints: try at least 100 million and if time is left  1 billion / 10 billions..

 5. Make a plot of run time versus number of cores from the data you have collected.

 6. Strong scalability here would yield a straight line graph. Comment on your results.

 7. Now let us do some running that constitutes a weak scaling test. 
 
This means increasing the problem size simultaneously with the number of nodes being used. In the present case,
increasing the number of iterations, Niter.

 8. Record the run time for each number of nodes and make a plot of the run time versus number of computing nodes.

 9. Weak scaling would imply that the runtime remains constant as the problem size and the
number of compute nodes increase in proportion. Comment on your results.

 10. Is this problem more appropriate for weak or strong scaling?
