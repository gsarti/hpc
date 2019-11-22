#include <stdlib.h>


#define MY_MACRO( x ) (x*x)

int main( void )
{

#define myX 2.0
  
  double x = MY_MACRO( myX );

#undef myX
#define  myX -3.14

  x = MY_MACRO( myX );
  
  return MY_MACRO( x );
}
