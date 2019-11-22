**Debugging simple codes with the help of a debugger**

Compile examples with: make

- ex01.f / ex01.f90 / ex01.c./ dotprod_serial.c § 
      -	Task: run the program under the control of a debugger, set/delete break points, watches, inspect data.

- ex02.f / ex02.f90 / ex02.c introduce an "out-of-bounds" bug in one of the loops, by increasing the loop length without 
  changing the size of the allocated memory block. 
      - Task: find cause for segmentation fault using the debugger, either through running in debugger or by generating 
        a core dump and inspecting it with the debugger.

- ex03.f / ex03.f90 have a similar, but more subtle version of the same bug. Here we don't get a segmentation fault, 
  but rather a faulty result, due to the array "b" being located in memory just behind "a" and thus out-of-bounds accesses 
  of "a" will write to "b". Note how the result of "b" is correct in the second part, and that it will get corrupted after 
  we checked it. 
      - Task: run and notice the inconsistency. Then compile with bounds checking enabled and locate the bug.

- ex04.c / ex05.c / ex06.c: examples for memory leaks. We first allocate the array and then in ex04.c nothing is free()'d. 
  ex05.c corrects for the individual array storage, but misses the list of arrays allocation. ex06 is corrected. 
      - Task: run with valgrind and see the memory leak(s).


