#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main ( int argc, char **argv )
{

  if ( argc > 1)
    // arg 0 is the name of the program itself
    {
      printf( "\nexploring my %d argument%c:\n", argc-1, (argc>2)?'s':' ' );
	for ( int i = 1; i < argc; i++ )       
	  {
	    printf( "\targument %d is : %s\n", i, *(argv+i) );
	  }
	printf( "\n" );
    }
  
  else
    
    printf ( "no arguments were given\n\n" );

  return 0;
}
