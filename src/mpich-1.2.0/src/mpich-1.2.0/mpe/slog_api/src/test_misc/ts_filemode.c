#include <stdio.h>

#define errfile stdout

void filemode( const char *path, const char *mode )
{
    if ( strlen( mode ) < 2 ) {
        fprintf( errfile, __FILE__":SLOG_OpenStream( %s, %s ) - "
                          "The input file mode string %s is too short\n",
                          path, mode, mode ); 
        fflush( errfile );
        return;
    }

    if ( strchr( mode, 'b' ) == NULL ) {
        fprintf( errfile, __FILE__":SLOG_OpenStream( %s, %s ) - "
                          "The input file mode string %s does NOT specify "
                          "binary file mode(b)\n",
                          path, mode, mode );
        fflush( errfile );
        return;
    }

    switch( mode[0] ) {
    case 'w' :
        /*  _WRITE_ SLOG_BUFFER_UNIT in the very beginning of the file  */
        fprintf( errfile, __FILE__":SLOG_OpenStream( %s, %s ) - it is OK\n",
                          path, mode );
        fflush( errfile );
        break;
    case 'r' :
        /*  _READ_ SLOG_BUFFER_UNIT in the very beginning of the file  */
        fprintf( errfile, __FILE__":SLOG_OpenStream( %s, %s ) - it is OK\n",
                          path, mode );
        fflush( errfile );
        break;
    default  :
        fprintf( errfile, __FILE__":SLOG_OpenStream( %s, %s ) - "
                          "The input file mode string %s is NEITHER for "
                          "writing(w) NOR for reading(r)\n",
                          path, mode, mode );
        fflush( errfile );
        return;
        break;
    }
}

main()
{
    int icase = 0;

    printf( "case %d\n", icase++);
    filemode( "tmp", "r" );
    printf( "case %d\n", icase++);
    filemode( "tmp", "w" );
    printf( "case %d\n", icase++);
    filemode( "tmp", "r " );
    printf( "case %d\n", icase++);
    filemode( "tmp", "wb" );
    printf( "case %d\n", icase++);
    filemode( "tmp", "ab" );
    printf( "case %d\n", icase++);
    filemode( "tmp", "r+b" );
    printf( "case %d\n", icase++);
    filemode( "tmp", "rb" );
    printf( "case %d\n", icase++);
    filemode( "tmp", "wb" );
}
