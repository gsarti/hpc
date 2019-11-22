/* =============================================================================
 * This file is part of the exercises for the Lectures on 
 *   "Foundations of High Performance Computing"
 * given at 
 *   Master in HPC and 
 *   Master in Data Science and Scientific Computing
 * @ SISSA, ICTP and University of Trieste
 *
 * contact: luca.tornatore@inaf.it
 *
 *     This is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 3 of the License, or
 *     (at your option) any later version.
 *     This code is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License 
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>


/* + -------------------------------------------------------------------------- +
 * |                                                                            |
 * |  PAPI-related section                                                      |
 * |                                                                            |
 * |  everything related to PAPI will be included between                       |
 * |   #ifdef USE_PAPI / #endif clause                                          |
 * |  note that some maco (GET_IPC and ACC_CNTRS) are defined empty if USE_PAPI |
 * |  is not defined                                                            |
 * + -------------------------------------------------------------------------- +
 */

#ifdef USE_PAPI	       /* ----------------------------------------------------- */

#include <papi.h>		                   /* include the needed header */


                                                   /* we use the high-level PAPI
						      interface here; specifically,
						      we are only interested in IPC
						      counts. Then there is no need
						      to rely on low-level, more
						      complicated interface */

						   /* GET_IPC give the interface to
						      PAPI's get_ipc
						      First call will initialize the
						      PAPI High Level interface,
						      set up the counters to monitor
						      PAPI_TOT_INS and PAPI_TOT_CYC
						      events and start the counters.
						      Subsequent calls will read the
						      counters and return total real time,
						      total process time, total inss
						      since the start of the measurement
						      and the instructions per cycle
						      rate since latest call to PAPI_ipc()*/
#define GET_IPC( RT, PT, I, IPC ) PAPI_ipc( (RT), (PT), (I), (IPC) )

						   /* this will stop the counters */
#define PAPI_STOP PAPI_stop_counters()

				                   /* This accumulates the partial
						      counters */
#define ACC_CNTRS( RT1, PT1, I1, IPC1, RT2, PT2, I2, IPC2,		\
		   RT, PT, I, IPC ) {					\
    RT  += RT2 - RT1;							\
    PT  += PT2 - PT1;							\
    I   += I2 - I1;							\
    IPC += IPC2; }

#else                  /* USE_PAPI -------------------------------------------- */

						   /* define empty macros       */
#define GET_IPC( RT, PT, I, IPC )
#define ACC_CNTRS( RT1, PT1, I1, IPC1, RT2, PT2, I2, IPC2,	\
		   RT, PT, I, IPC )
#define PAPI_STOP

#endif                 /* USE_PAPI -------------------------------------------- */




						   /* define the macro to measure
						      execution time              */
#define CPU_TIME (clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts ), (double)ts.tv_sec +	\
		  (double)ts.tv_nsec * 1e-9)

						   /* define the DRAM clock and data
						      width to calculate the max
						      theoretical transfer from
						      memory */

#ifndef _MEM_CLOCK
#define _MEM_CLOCK 1867	  // the clock of the DRAM
#endif
#ifndef _MEM_WIDTH
#define _MEM_WIDTH 64	  // the width in bits of a DRAM chunk
#endif





int main(int argc, char **argv)
{
  
  double *array1, *array2;                         /* the actual memory that will be loaded      */
  int     N;                                       /* N is the array size, in number of elements */
  struct timespec ts;                              /* this is used inside CPU_TIME               */

						   /* define accumulators for PAPI counters */
#ifdef USE_PAPI
  float realtime1, proctime1, ipc1;
  float realtime2, proctime2, ipc2;
  float realtime, proctime, ipc;
  long_long ins1, ins2, ins;
#endif  

						   /* check the arguments */
  if(argc > 1)
    N = atoi(*(argv+1));
  else
    N = 1000000;				   /* use default if no arg is given */


						   /* allocate memory, in one bunch */
  array1 = (double*)calloc( N * 2, sizeof(double) );
  array2 = array1 + N;
  srand48(12983476);				   /* initialize the pseudo-random generator
						      that will be used to initialize memory*/

  printf("generating %d numbers..", 2*N); fflush(stdout);
  {
    int N_ = 2*N;
    for ( int i = 0; i < N_; i++ )
      array1[i] = drand48();
  }
  printf ( "done\n" );
  
						   /* --------------------------------- 
						      : outer loop                    : 
						      : the experiment is iterated    :
						      : ITER times to have averaged   :
						      : quantities                    :
						      --------------------------------- */
#define ITER 10
  double std_dev =0,  avg_time = 0, min_time = N;
  double sum = 0;

  double tstart_all = CPU_TIME;
  GET_IPC( &realtime1, &proctime1, &ins1, &ipc1 ); // initialize the framework
  for(int k = 0; k < ITER; k++)
    {
      sum = 0;
      
      double tstart = CPU_TIME;
      //#pragma ivdep    <-- you may try to use this directive to reassure the compiler
      //                     that each loop iteration is independent..
      //                     look at the compiler's report -fopt-info-missed

      //GET_IPC( &realtime1, &proctime1, &ins1, &ipc1 );  // we may call PAPI encompassing
						          // the inner loop only

						   /* ------------------------------------
						      : inner loop                       :
						      : the simplest implementation.     :
						      : Note that this carries data      :
						      : dependencybetween two subsequent :
						      : iterations, because sum must be  :
						      : updated before accepting a new   :
						      : summation                        :
						      ------------------------------------ */
      for ( int i = 0; i < N; i++ )
	sum += array1[i] * array2[i];

      //GET_IPC( &realtime2, &proctime2, &ins2, &ipc2 );
      
      double time      = CPU_TIME - tstart;
      avg_time += time;
      std_dev  += time * time;
      if ( time < min_time )
	min_time = time;
      
      /* ACC_CNTRS( realtime1, proctime1, ins1, ipc1,\      */ // here we should accumulate 
      /* 		 realtime2, proctime2, ins2, ipc2, \*/ // for each iteration
      /* 		 realtime, proctime, ins, ipc ); */
    }
  double tend_all = CPU_TIME - tstart_all;
  GET_IPC( &realtime2, &proctime2, &ins2, &ipc2 );         // call PAPI only once
  ACC_CNTRS( realtime1, proctime1, ins1, ipc1,	           // accumulation is not really
	     realtime2, proctime2, ins2, ipc2,	           // necessary, because we have called
	     realtime, proctime, ins, ipc );               // get_ipc only once

    
  avg_time /= ITER;
  std_dev = sqrt(std_dev / ITER - avg_time*avg_time);
  
  printf( "sum is %g\ntime is :%g (min %g, std_dev %g, all %g)\n",
	  sum, avg_time, min_time, std_dev, tend_all );


  // get the maximum transfer rate in GB/sec
  // _MEM_CLOCK is in Mhz
  double max_GB_per_sec          = (double)_MEM_CLOCK / 1000 * (_MEM_WIDTH / 8);
  // get the transfer rate obtained
  double transfer_rate_in_GB_sec = (double)N*2*sizeof(double) / (1024*1024*1024) / avg_time;  
  printf("transfer rate was %6.3g GB/sec (%7.4g%% of theoretical max that is %5.2g GB/sec)\n",
	 transfer_rate_in_GB_sec, transfer_rate_in_GB_sec / max_GB_per_sec * 100, max_GB_per_sec);

#ifdef USE_PAPI
  printf( "\tIPC: %4.2g\n"
	  "\t[ time: %6.4gsec  -  ins: %9.6g ]\n",
	  ipc,
	  proctime / ITER,
	  (double)ins / ITER );
#endif

  
  memset ( array1, 0, sizeof(double)*2*N );
  free(array1);
  
  return 0;
}
