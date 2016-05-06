#include <stdio.h>

#define SLOG_PROFILENAME  "SLOG_Profile.txt"

/*  Calculate the length of the one stream in "char" ( byte? )  */
size_t filelen( FILE *infd )
{
    int    c;
    size_t count;

    count = 0;
    while ( ( c = fgetc( infd ) ) != EOF ) {
        printf( "%d ", c );
        if ( c != '\n' ) count++;
    }

    return count;
}

main()
{
	const char filename[] = SLOG_PROFILENAME;
	FILE *fd;

        fd = fopen( filename, "r" );
	printf( "\n the length of %s = %d\n", filename, filelen( fd ) );

        printf( "newline character is %d\n", '\n' );
        printf( "EOF is %d\n", EOF );
        fclose( fd );
}
