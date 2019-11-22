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

#define myFloat     double
#define myFloat_sum double

#define NUM  10000000
#define ITER 12

#define WIPE_CACHE  for ( int i = 0; i < N_CACHE; i++ ) wipe_cache[i] = (double)i*2.0;

int main( int argc, char **argv)
{
  int     N = NUM;                                                                   // < ----------------+
  int     N_CACHE;								     //                   |
  int     init_array = 1;							     //                   |
  struct  timespec ts;								     //  set-up variables |
										     //  local to main()  |
  myFloat *array;								     //                   |
  myFloat *wipe_cache;								     // < ----------------+

  // check arguments and init memory  

										     // < ----------------+ 
  if ( argc >= 2 )						                     //                   | < -----------+             
    N = atoi( *(argv+1) );         				                     //                   |              |
										     //                   |  read in     |
  if ( argc >= 3 )	                                                             //                   |  arguments   |
    init_array= atoi( *(argv+2) );                                                   //                   |              |
  N_CACHE = N + 128;	                                                             //                   | < -----------+ 
                                                                                     //                   | < -----------+             
  if ( (array = (myFloat*)malloc(N * sizeof(myFloat)) ) == NULL )                    //                   |              |
    {			                                                             //                   |              |
      printf("not enough memory to allow %d doubles (%llu)\n",                       //                   |              |           
	     N,		                                                             //                   |              |           
	     (unsigned long long)N * sizeof(myFloat) );	                             //                   |              |           
      exit(1);		                                                             //                   |              |           
    }                                                                                //                   |  memory      |                 
                                                                                     //                   |  allocation  |                     
  if ( (wipe_cache = (myFloat*)malloc(N_CACHE * sizeof(myFloat)) ) == NULL )         //                   |              |           
    {                                                                                //                   |              |           
      printf("not enough memory to allow %d doubles (%llu) for cache wiping\n",	     // initialization:   |              |       
	     N_CACHE,                                                                // this preamble is  |              |           
	     (unsigned long long)N_CACHE * sizeof(myFloat) );                        // common to all the |              |           
      exit(1);							                     // prefetch2.v?.c    |              |           
    }                                                                                //                   | < -----------+           
                                                                                     //                   |                                                 
                        					                     //                   |                         
										     //                   |                         
										     //                   |                         
  // init the array with pseudo-random myFloats                                      //                   |                         
  if ( init_array )						                     //                   | < -----------+                        
    {								                     //                   |              |          
      printf("initialize array with random floats..\n");	                     //                   |              |          
      srand48(19751221);					                     //                   |  init array  |          
      for ( int i = N-1; i >= 0; i-- )				                     //                   |  with random |         
	array [ i ] = drand48();				                     //                   |  numbers     |          
      WIPE_CACHE;						                     //                   |              |             
    }                                                                                //                   | < -----------+          
										     // < ----------------+                      

  // - V0, basic naive version
  //   straigthforward sum
  //                                                                                 // < ----------------+
  double avg = 0, stdv = 0;                                                          //                   | < -------------+
  myFloat_sum  sum = 0;								     //                   |  set variables |
  										     //                   | < -------------+
  for ( int r = 0; r < ITER; r++ )						     //                   | < -------------+
    {										     //                   |                |
      WIPE_CACHE;								     //  actual           |                | < -- wipe cache -+
      sum = 0;									     //  computation      |                |
      PAPI_START_CNTR;								     //                   |                |
      double t0 = CPU_TIME;							     //  this section     |  repetition    |
      										     //  changes in       |  loop          | < -------------+
      for ( int i = 0; i < N; i++ )						     //  each file        |                |                |
	sum += array[ i ];							     //  prefetching2.v?.c|                |   main loop    |
      										     //                   |                | < -------------+
      double t = CPU_TIME - t0;							     //                   |                |
      PAPI_STOP_CNTR;                                                                //                   |                |
      avg  += t;								     //                   |                |
      stdv += t * t; 								     // < ----------------+                |
    }										     //                   |< --------------+                

  avg /= ITER;									     // 
  printf("%33s sum is %g, took %g secs (stdv: %g)\n",				     // 
	 "no prefetching, no unrolling - ",					     // 
	 sum, avg, sqrt(stdv/ITER - avg*avg));					     // 
  										     // 
  // get the maximum transfer rate in GB/sec                                         // < ----------------+
  // _MEM_CLOCK is in Mhz                                                            //                   |
  double max_GB_per_sec          = (double)_MEM_CLOCK / 1000 * (_MEM_WIDTH / 8);     //  check the actual |
  // get the transfer rate obtained                                                  //  transfer rate    |
  double transfer_rate_in_GB_sec = (double)N*2*sizeof(double) /                      //  with the theoret.|
    (1024*1024*1024) / avg;                                                          //  one              |
  printf("transfer rate was %6.3g GB/sec "                                           //                   |
	 "(%7.4g%% of theoretical max that is %5.2g GB/sec)\n",                      //                   |
	 transfer_rate_in_GB_sec,                                                    //                   |
	 transfer_rate_in_GB_sec / max_GB_per_sec * 100, max_GB_per_sec);            // < ----------------+
  										     //
                                                                                     //
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
                                                                                     //
  free(wipe_cache);								     //  < -- free the allocated memory -+
  free(array);									     //  < -- note the reverse order    -+
  return 0;									     //                   
}
