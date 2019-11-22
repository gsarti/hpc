
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

double mysecond()
{
        struct timeval tp;
        int i;
 
        i = gettimeofday(&tp,NULL);
        return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}
 

int main(int argc, char **argv) 
{
    int i, j;
    const int  n=1000, m=1000;
    double  a[n][m];
    double  t1,t2;

    t1=mysecond();
    for ( i=0; i<1000; ++i) {
        for ( j=0; j<1000; ++j) {
           a[i][j] = i+j;
        }
    }
    t2=mysecond();
  
    printf("time used %g\n", t2-t1);
    return 0;
}
