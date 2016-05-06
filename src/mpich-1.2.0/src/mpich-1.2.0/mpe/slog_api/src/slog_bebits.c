#include <stdio.h>
#include <stdlib.h>
#include "slog_bebits.h"                  /*I  "slog_bebits.h"  I*/


        /* SLOG_bebit(s)_xxxx methods */

int SLOG_bebit_valid( const SLOG_bebit_t bebit )
{
    return ( bebit == 0 || bebit == 1 );
}



int SLOG_bebits_valid( const SLOG_bebits_t bebits )
{
    /*  return( bebits >= 0 && bebits <= 3 );   */
    return( bebits <= 3 );
}



SLOG_bebit_t SLOG_bebit_conv( const SLOG_bebit_t bebit )
{
    return ( bebit > 0 ? 1 : 0 );
}



SLOG_bebits_t SLOG_bebits_encode( const SLOG_bebit_t bebit_0,
                                  const SLOG_bebit_t bebit_1 )
{
    /*
    return (   SLOG_bebit_conv( bebit_1 ) * 2
             + SLOG_bebit_conv( bebit_0 ) );
    */
    return (   ( SLOG_bebit_conv( bebit_1 ) << 1 )
             +   SLOG_bebit_conv( bebit_0 ) );
}



int SLOG_bebits_decode( const SLOG_bebits_t bebits, SLOG_bebit_t *bebit_ary )
{
    /*
    bebit_ary[ 0 ] = bebits % 2;
    bebit_ary[ 1 ] = bebits / 2;
    */

    bebit_ary[ 0 ] = bebits & 0x1;
    bebit_ary[ 1 ] = bebits >> 1;

    if ( SLOG_bebits_valid( bebits ) )
        return SLOG_SUCCESS;
    else {
        fprintf( errfile, __FILE__":SLOG_bebit_decode() - "
                          "Input bebitS is INVALID\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
}
