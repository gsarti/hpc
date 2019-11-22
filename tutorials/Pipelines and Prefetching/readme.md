# Pipelines / prefetching hands-on
## Some code snippets to play with


Here you find some code snippet to exemplify the concepts about expoiting pipelines and prefetching that we have seen during the classes.

All the codes have been instrumented using the simple high-level interface of PAPI, you can activate it by compiling with `-DUSE_PAPI`

**NOTE: in order to have an estimate of your memory transfer bandwidth, you must provide 2 compile-time variables to the compiler:
_MEM_CLOCK : the clock of your DRAMM
_MEM_WIDTH : the size of 1-cycle memory transfer (usually 64 bits) 
on your laptop you can find this figures by using
`sudo dmidecode --type memory`
or
`sudo lshw -short -C memory`**

**-------------------------------------------------------------------**
**-------------------------------------------------------------------**
### 1. Pipelines

The base case is the sum
```C
double *array1;
double array2;

array1 = (double*) malloc( N * sizeof(double))
array2 = (double*) malloc( N * sizeof(double))

for ( int i = 0; i < N ; i++ )
	sum += array1[i] + array2[i];
```

Here there is an obvious data dependency on the variable `sum` that hampers an efficient compilation if this code.
So, you are provided with some coding examples that show some techniques to make the same code more efficient.


#### [ base - `v0.c` ]
This version is the base "naked" version that implements the above loop.

#### [ step 1 - `v1.c` | `v1b.c` ]
_v1.c_
This version implements a simple 2-times unrolling of the loop.

_v1b.c_
This version is the same than the previous but directly uses 2 pointers in the loop instead of the integer counter.

#### [ step 2 - v3.c | v3b.c ]
_v3.c_
In this version we unroll the loop 4 times but use only 1 accumulator. That is still efficient because it makes it clear that 4 memory fecthes and multiplication are independent and can be executed at the same time

_v3b.c_
The same than before but tries a prefetching by preloading

#### [ step 3 - `v6.c` | `v6_intrinsics.c` ]
_v6.c_
In this version we opt for an explicit vectorisation of the loop using the vector declaration
`__attribute__ ((vector_size (4*sizeof(double))));`

_v6_intrinsics.c_
Here we use directly the intrinsics function calls provided by the compiler to call the native vector inistructions (NOTE: this version only works with Intel-like SIMD)

**-------------------------------------------------------------------**
**-------------------------------------------------------------------**
### 2. Prefetching

The base case is the sum (the so-called _prefix-sum_)
```C
double *array;

array = (double*) malloc( N * sizeof(double))

for ( int i = 0; i < N ; i++ )
	sum += array[i];
```

Here we are interested in how to prefetch the array entries to be crunched so that they are available in the cache at the moment they are needed.


#### [ step 0 - `prefetching2.v0.c` ]

This is the basic reference implementation of the loop above

#### [ step 0 - `prefetching2.v1.c` ]
A simple unrolling is presented with a prefetching by preloading and the attempt to partially hide the sum latency by postponing the accumulation of the _i_ th iteration data to the _i+1_th iteration

#### [ step 1 - `prefetching2.v2.c` ]
The same than the previous version, but using pointers in the loop instead of the integer counter

#### [ step 2 - `prefetching2.v3.c` ]
prefetching by preloading exactly as it has been show in the lecture's slide, plus unrolling and latency hiding

#### [ step 3 - `prefetching2.v4.c` ]
A different flavour of previous version

#### [ step 4 - `prefetching2.v5.c` ]
As version 1-3 but using explicit vectorization

#### [ step 5 - `prefetching2.v5.c` ]
As version 1-3 but using explicit vectorization

#### [ step 6 - `prefetching2.v6.c` ]
As version 5 but looping over the pointer

NOTE: version 2-4 shoule produce not-that-different performances. The more curious among you can inspect the assembler generated.
