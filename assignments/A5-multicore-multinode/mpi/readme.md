# MPI intranode and internode

We will use Intel MPI benchmarks available here [https://github.com/intel/mpi-benchmarks]
Some more info are here [https://software.intel.com/en-us/articles/intel-mpi-benchmarks/]

Compilation is trivial (see readme file) and you can compile it with any kind of MPI implementation.

However you do not even need to compile it: it is available on Ulysses cluster.

Apparentely `impi` is avaliable:
```
$ module load impi-trial/5.0.1.035 
```
Run the benchmark:
```
 $ mpirun -np 2 /u/shared/programs/x86_64/intel/impi_5.0.1/bin64/IMB-MPI1 PingPong
```
and understand what is measuring and how to use it 



## Latency measure

To measure latency we will identify the time it takes to ping/pong the smallest messages: this time it is roughtly constant for pamessages size from 0 to 16/32 bytes.

## Bandwidth measure

to measure bandwidth we will look at the largest messages when bandwith tend to flat..

To measure/estimate latency and time please repeat the measure for at least three time to evalute the error bar associated.

```
 $ mpirun -np 2 /u/shared/programs/x86_64/intel/impi_5.0.1/bin64/IMB-MPI1 PingPong
mpirun -np 2 /u/shared/programs/x86_64/intel/impi_5.0.1/bin64/IMB-MPI1 -iter 10000 PingPong
```
For istance here the below the  runs statistics:

|bytes   | Run 1 [us]| Run 2 [us]| Run 3 [us]| Min [us]  |  
|-------:|------:|------:|------:|------:|
| 0      | 0.50  | 0.58  | 0.48  | 0.48  |
| 1      | 0.51  | 0.64  | 0.52  | 0.51  |
| 2      | 0.52  | 0.64  | 0.52  | 0.52  |

## Final task: Gauging Default Intranode/Internode Communication Performance


We would like to evaluate the latency among cores belonging to the same socket: 

```
mpirun -np 2 hwloc-bind core:0 core:7 /u/shared/programs/x86_64/intel/impi_5.0.1/bin64/IMB-MPI1 PingPong
```

and then estimate the latency among cores which are on different sockets: 

```
mpirun -np 2 hwloc-bind core:0 core:13 /u/shared/programs/x86_64/intel/impi_5.0.1/bin64/IMB-MPI1 PingPong
```

and finally estimate latency between to cores which are no different nodes:

```
 mpirun -np 2 -npernode 1  /u/shared/programs/x86_64/intel/impi_5.0.1/bin64/IMB-MPI1 PingPong
```

Collect all this numbers in one brief report and comment them. 

