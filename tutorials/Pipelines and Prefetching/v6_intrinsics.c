
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


/* -------------------------------------------------------------------------------
 *   LOOK AT v0.C FOR MORE THOUROUGH AND DETAILED COMMENTS ABOUT THE CODE
 ------------------------------------------------------------------------------- */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <immintrin.h>

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

#define CPU_TIME (clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts ), (double)ts.tv_sec + \
		   (double)ts.tv_nsec * 1e-9)

#ifdef __AVX512__

#define V_DSIZE 8
typedef __m512d vd;
vd _dzero_ = {0, 0, 0, 0, 0, 0, 0, 0};
#define MUL_ADD( A1, A2, R ) _m512_fmadd_pd( (A1), (A2), (R) )

#elif defined ( __AVX__ ) || defined ( __AVX2__ )

#define V_DSIZE 4
typedef __m256d vd;
vd _dzero_ = {0, 0, 0, 0};
#define MUL_ADD( A1, A2, R ) _mm256_fmadd_pd( (A1), (A2), (R) )

#elif defined ( __SSE4__ ) || defined ( __SSE2__ )

#define V_DSIZE 2
typedef __m128 vd;
vd _dzero_ = {0, 0};
#define MUL_ADD( A1, A2, R ) _mm128_fmadd_pd( (A1), (A2), (R) )

#else

#define V_DSIZE 1
typedef double vd;
vd _zero_ = 0;
#define MUL_ADD( A1, A2, R ) (R) += (A1) * (A2)

#endif


typedef union {
  vd   V;
  double v[V_DSIZE];
}vd_u;


#define _DO_NOT_OPTIMIZE_BEGIN asm volatile("" ::: "memory");
#define _DO_NOT_OPTIMIZE_END asm volatile("" ::: "memory");


#ifndef _MEM_CLOCK
#define _MEM_CLOCK 1867            // the clock of the DRAM
#endif
#ifndef _MEM_WIDTH
#define _MEM_WIDTH 64              // the width in bits of a DRAM chunk
#endif

int main(int argc, char **argv)
{
  double * restrict array1 __attribute__((aligned(64)));
  double * restrict array2 __attribute__((aligned(64)));
  double tstart, tend;
  int    N;
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

  array1 = (double*)aligned_alloc(64, N * sizeof(double));
  array2 = (double*)aligned_alloc(64, N * sizeof(double));
  srand48(12983476);

  printf("generating %d numbers..", 2*N); fflush(stdout);
  tstart = CPU_TIME;
  {
    int i;
    i = N;
    while ( i )
      array1[--i] = drand48();

    i = N;
    while ( i )
      array2[--i] = drand48();
  }
  tend = CPU_TIME - tstart;
  printf("done (in %6.3gsec\n", tend);
  
#define ITER 10
  double std_dev =0,  avg_time = 0, min_time = N;
  double sum;
  
  double tstart_all = CPU_TIME;
  GET_IPC( &realtime1, &proctime1, &ins1, &ipc1 );

  for(int k = 0; k < ITER; k++)
    {
      sum = 0;
      vd sum_ __attribute__ ((aligned(64)));
      sum_ = _dzero_;
      
      vd * restrict A1 __attribute__ ((aligned(64)));
      vd * restrict A2 __attribute__ ((aligned(64)));
      A1  = (vd*)array1 ;
      A2  = (vd*)array2 ;
      
      int N__ = N / V_DSIZE;
      int N_  = N__ * V_DSIZE;

      tstart = CPU_TIME;
      
#pragma ivdep
      for( int i = 0; i < N__; i++)
	sum_ = MUL_ADD( A1[i], A2[i], sum_ );

      for ( int i = N_; i < N; i++ )
	sum += array1[ i ] * array2[ i ];     

      vd_u sum_u __attribute__ ((aligned(64)));
      
      sum_u.V = sum_;
      
      sum_u.v[0] = sum_u.v[0] + sum_u.v[1];
      sum_u.v[2] = sum_u.v[2] + sum_u.v[3];
      
      sum += sum_u.v[0] + sum_u.v[2];

      tend = CPU_TIME - tstart;
      
      avg_time += tend;
      std_dev  += tend * tend;
      if ( tend < min_time )
	min_time = tend;
    }
  double tend_all = CPU_TIME - tstart_all;
  
  GET_IPC( &realtime2, &proctime2, &ins2, &ipc2 );
  ACC_CNTRS( realtime1, proctime1, ins1, ipc1,	   \
	     realtime2, proctime2, ins2, ipc2,	   \
	     realtime, proctime, ins, ipc );

  avg_time /= ITER;
  std_dev = sqrt(std_dev / ITER - avg_time*avg_time);    
  
  printf("sum is %g\ntime is: %g (min %g, std_dev %g, all %g)\n",
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
	  "\t[ time: %6.4gsec  -  ins: %9.6g ]\n",
	  ipc,
	  proctime / ITER,
	  (double)ins / ITER );
#endif

  
  memset ( array1, 0, sizeof(double) * N );
  memset ( array2, 0, sizeof(double) * N );

  free( array2 );
  free( array1 );
  
  return 0;
}
