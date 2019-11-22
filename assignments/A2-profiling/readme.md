# Profile a code
You have learnd how to perform a basic code profiling, either using gprof or using valgrind --callgrind

----
**>>>>> if you have problems with perf <<<<**

***-ISSUE I-***

perf needs some privileges to access the call stack
you can lower the system threshold by putting a zero in the file /proc/sys/kernel/kptr_restrict

1) create a file, for instance enable_perf with
```echo 0 > /proc/sys/kernel/kptr_restrict```
2) give it execution flag
```chmod +x ./enable_perf```
3) execute as root
```sudo ./enabl_perf```


***-ISSUE II-***

also to profile routines not in user space perf needs some privileges.
If you put -1 in the file ```/proc/sys/kernel/perf_event_paranoid``` 

on your laptops you can cure permanently this system suspiciousness by lowering the paranoid parameter here:
```sudo sh -c 'echo kernel.perf_event_paranoid=1 > /etc/sysctl.d/local.conf'```

----

## Exercise assignment
1. Choose a code at your convenience.
The only requirement is that it should be suited for profiling: i.e. something more interesting than "hello World". Let's say it should have at least a couple of routines and at least 100-200 lines
2. profile the code, possibly with callgrind
3. report about the profiling:
   - the call tree, in whichever form you prefer
   - the time spent in each routine (also the main() is a routine)
   - optional: add as many details you may want (read the callgrind manual to find interesting things to explore)

Hints:
- if **really** you can not find something interesting, profile the code different_clock() that I've just uploaded in the Timing/ folder.
Of course, in this case you should modify it so that it contains 3 obvious routines, isn't it ?
