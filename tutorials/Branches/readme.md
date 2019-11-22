# Branches in loops

You find here 2 files, brancpred.c and branchpred2.c.
### [ branchpred.c ]

This is the example file for the case discusses during the lectures:
```c
// generate random numbers
for (cc = 0; cc < SIZE; cc++)
  data[cc] = rand() % TOP;

// take action depending on their value
for (ii = 0; ii < SIZE; ii++)
  {
    if (data[ii] > PIVOT)
       sum += data[ii];
  }
```

There are 3 compilation options: `-DBESMART` uses bitwise trickery, `DBESMART2` uses the ternary operator, no option uses the normal conditional instruction.
Try with different optimization levels and different compilers.
You may want to reprodice the PAPI infrastracture that you find in the next example.

### [ branchpred2.c ]

This is the example file for the following case:
```C
for (i = 0; i < SIZE; i++)
  {
	if ( A[i] < B[i] ) 
	   {
		t = B[i];
		B[i] = A[i];
		A[i] = t; 
    }
  }
```

There are 4 complation options (have a look in the slides to understand which does what): `-DBESMART` `-DBESMART2` `-DBESMART3` and no option at all.
The code is fairly well commented, you also find some commented lines to experiment with.

To activate the PAPI infrastructure, use `-DUSE_PAPI`:
```bash
$CC -DUSE_PAPI -DBESMART -o EXEC_NAME branchpred2.c -lpapi -lm
```

if something goes wrong with papi, try to link with the static library:
```bash
$CC -DUSE_PAPI -DBESMART -o EXEC_NAME branchpred2.c /usr/lib/libpapi.a -lm
```
