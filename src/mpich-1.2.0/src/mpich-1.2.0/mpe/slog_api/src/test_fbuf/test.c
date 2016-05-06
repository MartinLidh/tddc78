#include <stdio.h>

main( int argc, char **argv )
{
	unsigned int buf_sz;

	if ( argc > 1 )
		if ( isdigit( argv[1][0] ) ) buf_sz = atoi( argv[1] );

	printf( "mod( buf_sz, sizeof( char ) ) = %d\n",
                buf_sz % sizeof( char ) );
}
