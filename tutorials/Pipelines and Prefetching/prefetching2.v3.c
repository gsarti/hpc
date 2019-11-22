/*
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


/* -------------------------------------------------------------------------------------
 *   LOOK AT prefetching2.v0.C FOR MORE THOUROUGH AND DETAILED COMMENTS ABOUT THE CODE
 -------------------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>


#ifdef USE_PAPI
#include <papi.h>

#define PAPI_EVENTS_NUM 5
int       papi_events[PAPI_EVENTS_NUM] = {PAPI_TOT_INS, PAPI_TOT_CYC, PAPI_L1_DCM, PAPI_L2_DCM, PAPI_L3_LDM};
long long papi_values[PAPI_EVENTS_NUM] = {0};

#define PAPI_CHECK_RES( R ) { if ( (R) != PAPI_OK ) {printf("a problem with PAPI (code %d) arise at line %d\n", (R), __LINE__); return (R); } }

#define PAPI_START_CNTR { int res = PAPI_start_counters(papi_events, PAPI_EVENTS_NUM); PAPI_CHECK_RES(res); }
#define PAPI_STOP_CNTR  { int res = PAPI_stop_counters(papi_values, PAPI_EVENTS_NUM); PAPI_CHECK_RES(res); }

#else

#define PAPI_CHECK_RES( R )
#define PAPI_START_CNTR
#define PAPI_STOP_CNTR

#endif

#define CPU_TIME (clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts ), \
		  (double)ts.tv_sec +				  \
		  (double)ts.tv_nsec * 1e-9)

#ifndef _MEM_CLOCK
#define _MEM_CLOCK 1867	  // the clock of the DRAM
#endif
#ifndef _MEM_WIDTH
#define _MEM_WIDTH 64	  // the width in bits of a DRAM chunk
#endif


#define myFloat double
#define myFloat_sum double

#define NUM 10000000
#define ITER 12

#define WIPE_CACHE  for ( int i = 0; i < N_CACHE; i++ ) wipe_cache[i] = (double)i*2.0;


int main( int argc, char **argv)
{
  int     N = NUM;
  int     N_CACHE;
  int     init_array = 1;
  struct  timespec ts;

  myFloat *array;
  myFloat *wipe_cache;

  // check arguments and init memory
  
  if ( argc >= 2 )
    N = atoi( *(argv+1) );
  
  if ( argc >= 3 )
    init_array = atoi( *(argv+2) );
  
  N_CACHE = N + 128;  
  
  // allocate memory for the array that must be summed up
  if ( (array = (myFloat*)malloc(N * sizeof(myFloat)) ) == NULL )
    {
      printf("not enough memory to allow %d doubles (%llu)\n",
	     N, 
	     (unsigned long long)N * sizeof(myFloat) );
      exit(1);
    }

  // allocate memory for another array to be used to wipe the cache content
  if ( (wipe_cache = (myFloat*)malloc(N_CACHE * sizeof(myFloat)) ) == NULL )
    {
      printf("not enough memory to allow %d doubles (%llu) for cache wiping\n",
	     N_CACHE, 
	     (unsigned long long)N_CACHE * sizeof(myFloat) );
      exit(1);
    }

  
  // init the array with pseudo-random myFloats
  if ( init_array )
    {
      printf("initialize array with random floats..\n");
      srand48(19751221);
      for ( int i = N-1; i >= 0; i-- )
	array [ i ] = drand48();
      WIPE_CACHE;
    }
  


  // sum the array with induced prefetching, I
  double avg = 0, stdv = 0;
  myFloat_sum  sum = 0;
  
  for ( int r = 0; r < ITER; r++ )
    {
#define UNROLL 4
      int N_U = (N / UNROLL) * UNROLL;
      myFloat A[UNROLL+1];
      
      WIPE_CACHE;
      sum = 0;            

      myFloat *line = array + UNROLL;
      myFloat *stop = array + N_U;
      myFloat_sum register sumA = 0;
      myFloat_sum register sumB = 0;

      PAPI_START_CNTR;      
      double t0 = CPU_TIME;
      
      A [ 0 ] = array[0];
      
      for ( ; line < stop; line += UNROLL )
	/*
	 * This for cycle tries to:
	 *   - hide memory latency by inducing pre-fetching; in fact,
	 *     the instruction
	 *         A [ UNROLL ] = line[UNROLL]
	 *     "touches" the memory 4 bytes ahead, trying to populate
	 *     the cache for the next iteration.
	 *     while data travels from main memory to the cache, A
	 *     is populated with data that are already in the cache.
	 *   + same things than version v2
	 */
	{
	  A[ UNROLL ] = line[ UNROLL ];    // We access the UNROLL-th (5th in this case)
					   // entry, counting from the memory position
	                                   // pointed by line*
					   // If it wasn't in the cachem we force it -
					   // and the following data - to be loaded in
	  
	  A[      1 ] = line[      1 ];    // Here we access data that are already in the
	  A[      2 ] = line[      2 ];    // cache because they were in the same line
	  A[      3 ] = line[      3 ];    // than the element accessed in the previous
					   // iteration

	  sum  += sumA + sumB;             // here we try to hide the latency of fp sum
	  sumA  = A[ 0 ] + A[ 1 ];	   // operation: sumA and sumB have been calculated
	  sumB  = A[ 2 ] + A[ 3 ];	   // in the previous iteration
	  
	  A[0] = A[UNROLL];   		   // now the UNROLL-th element is available
					   // (or much more "closer" than before)
	}
      
      sum += sumA + sumB;
      
      for ( int i = N_U; i < N; i++ )
	// account for final elements in case N % 4 > 0
	sum += array [ i ];

      double t = CPU_TIME - t0;
      PAPI_STOP_CNTR;
      
      avg  += t;
      stdv += t * t; 
    }
  
  avg /= ITER;
  printf("%33s sum is %g, took %g secs (stdv = %g)\n",
	 "ind prefetching I, unrolling - ",
	 sum, avg, sqrt(stdv/ITER - avg*avg));

    // get the maximum transfer rate in GB/sec
  // _MEM_CLOCK is in Mhz                                                           
  double max_GB_per_sec          = (double)_MEM_CLOCK / 1000 * (_MEM_WIDTH / 8);    
  // get the transfer rate obtained                                                 
  double transfer_rate_in_GB_sec = (double)N*2*sizeof(double) /                     
    (1024*1024*1024) / avg;                                                    
  printf("transfer rate was %6.3g GB/sec "                                          
	 "(%7.4g%% of theoretical max that is %5.2g GB/sec)\n",                     
	 transfer_rate_in_GB_sec,                                                   
	 transfer_rate_in_GB_sec / max_GB_per_sec * 100, max_GB_per_sec);           

 #ifdef USE_PAPI
double tot_accesses = N;
  printf("\tPAPI events:\n"
	 "\tINS  : %12lld\n"
	 "\tCYC  : %12lld\n"
	 "\tL1 DM: %12lld (%5.3g)\n"
	 "\tL2 DM: %12lld (%5.3g)\n"
	 "\tL3 LM: %12lld (%5.3g)\n",
	 papi_values[0], papi_values[1],
	 papi_values[2], papi_values[2] / tot_accesses,
	 papi_values[3], papi_values[3] / tot_accesses,
	 papi_values[4], papi_values[4] / tot_accesses);
#endif
  
  free(wipe_cache);
  free(array);
  return 0;
}
