#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>



#ifdef USE_PAPI

#include <papi.h>

#define GET_IPC( RT, PT, I, IPC ) PAPI_ipc( (RT), (PT), (I), (IPC) )
#define ACC_CNTRS( RT1, PT1, I1, IPC1, RT2, PT2, I2, IPC2, RT, PT, I, IPC ) { \
  RT  += RT2 - RT1;							\
  PT  += PT2 - PT1;							\
  I   += I2 - I1;							\
  IPC += IPC2; }

#else

#define GET_IPC( RT, PT, I, IPC )
#define ACC_CNTRS( RT1, PT1, I1, IPC1, RT2, PT2, I2, IPC2, RT, PT, I, IPC )

#endif


#define CPU_TIME (clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts ), (double)ts.tv_sec +	\
		  (double)ts.tv_nsec * 1e-9)


#ifndef _MEM_CLOCK
#define _MEM_CLOCK 1867            // the clock of the DRAM
#endif
#ifndef _MEM_WIDTH
#define _MEM_WIDTH 64              // the width in bits of a DRAM chunk
#endif

int main(int argc, char **argv)
{
  double *array1, *array2;
  int    N, i;
  struct timespec ts;
  
#ifdef USE_PAPI
  float realtime1, proctime1, ipc1;
  float realtime2, proctime2, ipc2;
  float realtime, proctime, ipc;
  long_long ins1, ins2, ins;
#endif  


  if(argc > 1)
    N = atoi(*(argv+1));
  else
    N = 1000000;

  array1 = (double*)calloc(N*2, sizeof(double) );
  array2 = array1 + N;
  srand48(12983476);

  printf("generating %d numbers..", 2*N);  fflush(stdout);
  {
    int i = 2*N;
    while ( i )
      array1[--i] = drand48();
  }
  printf ( "done\n" );
  
#define ITER 10
  double std_dev =0,  avg_time = 0, min_time = N;
  double sum = 0;
  GET_IPC( &realtime1, &proctime1, &ins1, &ipc1 );
  double tstart_all = CPU_TIME;
  for(int k = 0; k < ITER; k++)
    {
      sum = 0;
            
      double tstart = CPU_TIME;
      //#pragma ivdep    <-- you may try to use this directive to reassure the compiler
      //                     that each loop iteration is independent..
      //                     look at the compiler's report -fopt-info-missed

#pragma ivdep
      double sum4[2] = {0, 0};//, 0, 0};
      for ( int i = 0; i < N; i++ )
	sum4[i&1] += array1[i] * array2[i];

      sum = sum4[0] + sum4[1];// + sum4[2] + sum4[3];      
      
      double time      = CPU_TIME - tstart;

      avg_time += time;
      std_dev  += time * time;
      if ( time < min_time )
	min_time = time;
      
    }
  double tend_all = CPU_TIME - tstart_all;
  
  GET_IPC( &realtime2, &proctime2, &ins2, &ipc2 );
  ACC_CNTRS( realtime1, proctime1, ins1, ipc1,	   \
		 realtime2, proctime2, ins2, ipc2, \
		 realtime, proctime, ins, ipc );

  avg_time /= ITER;
  std_dev = sqrt(std_dev / ITER - avg_time*avg_time);
  
  printf("sum is %g\ntime is :%g (min %g, std_dev %g, all %g)\n",
	 sum, avg_time, min_time, std_dev, tend_all);


  // get the maximum transfer rate in GB/sec
  // _MEM_CLOCK is in Mhz
  double max_GB_per_sec          = (double)_MEM_CLOCK / 1000 * (_MEM_WIDTH / 8);
  // get the transfer rate obtained
  double transfer_rate_in_GB_sec = (double)N*2*sizeof(double) / (1024*1024*1024) / avg_time;  
  printf("transfer rate was %6.3g GB/sec (%7.4g%% of theoretical max that is %5.2g GB/sec)\n",
	 transfer_rate_in_GB_sec, transfer_rate_in_GB_sec / max_GB_per_sec * 100, max_GB_per_sec);

#ifdef USE_PAPI
  printf( "\tIPC: %4.2g\n"
	  "\t[ time: %6.3gsec  -  ins: %5.2g ]\n",
	  ipc / ITER,
	  proctime / ITER,
	  (double)ins / ITER );
#endif
  
  memset ( array1, 0, sizeof(double)*N*2 );
  
  free( array1 );
  
  return 0;
}
