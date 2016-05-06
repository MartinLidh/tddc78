#include <stdio.h>

main( int argc, char **argv )
{
          char             filename[80] = "SLOG_RecDefs.dat";
    const int              iNaN = -99999;
          int              iread;
          FILE            *infd;
    const char             FileSeparator = '/';
          int              name_char;

    int count;

    if ( argc > 1 ) {
        printf( "argv[1] = _%s_\n", argv[1] );
        name_char = (int) argv[1][0];
        if (  isalnum( name_char )
           || name_char == FileSeparator || name_char == '.' )
            strcpy( filename, argv[1] );
    }

    infd = fopen( filename, "rb" );

    count = 0;
    iread    =  iNaN;
    while ( bswp_fread( &iread, sizeof( int ), 1, infd ) == 1 ) {
        count ++;
        printf( "%d, iread = %d \n", count, iread );
        iread = iNaN;
    };

    printf( "\n" );

    fclose( infd );
}
