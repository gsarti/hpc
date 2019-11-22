#include <stdio.h>
#include <stdlib.h>

int main( int argc,char **argv) {
  int i;
  // define a costantsize vector 
  int local_vector_size=100;
  
  // initialize the vector
  double *a,*b;
 
  a= (double *) malloc ( local_vector_size*sizeof(double)); 
  b= (double *) malloc ( local_vector_size*sizeof(double)); 
 
  // loop over them:
  //
  for (i=0; i<local_vector_size;i++) {
     a[i]= (double)i ;
     b[i]= (double)i ;
   }

  // compute dot product:
  double sum=0.0;

    for (i=0; i<local_vector_size+2;i++) {
     sum += a[i]*b[i];
   }

    printf( "the dot product is %g\n",sum);
   
   // deallocate
   free(a);
   free(b);
  return 0;
}
