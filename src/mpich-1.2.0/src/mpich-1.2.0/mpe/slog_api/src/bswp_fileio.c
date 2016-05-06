#include <stdio.h>
#include "bswp_fileio.h"

/*  On 1/4/99, byte2char is used in memcpy  */
/*
size_t byte2char( const size_t bytesize )
{
    return ( bytesize % sizeof( char ) == 0 ?
             bytesize / sizeof( char ) : bytesize / sizeof( char ) + 1 );
}
#endif
*/
#if ! defined( BYTE2CHAR )
#define BYTE2CHAR
#define byte2char( bytesize )                    \
        ( (bytesize) % sizeof( char ) == 0 ?     \
          (bytesize) / sizeof( char ) : (bytesize) / sizeof( char ) + 1 )
#endif



#if ! defined( WORDS_BIGENDIAN )
void byteswap( const bswp_uint32_t   Nelem, 
               const bswp_uint32_t   elem_sz, 
                     char           *bytes )
{
    char *bptr;
    char  btmp;
    int end_ii;
    int ii, jj;

    bptr = bytes;
    for ( jj = 0; jj < Nelem; jj++ ) {
         for ( ii = 0; ii < elem_sz/2; ii++ ) {
             end_ii          = elem_sz - 1 - ii;
             btmp            = bptr[ ii ];
             bptr[ ii ]      = bptr[ end_ii ];
             bptr[ end_ii ] = btmp;
         }
         bptr += elem_sz;
    }
}



bswp_uint32_t bswp_fwrite( const void           *src,
                           const bswp_uint32_t   elem_sz,
                           const bswp_uint32_t   Nelem,
                                 FILE           *outfd )
{
    char *buf;
    bswp_uint32_t total_bytesize;
    bswp_uint32_t count;

    total_bytesize = Nelem * elem_sz;
#if ! defined( HAVE_ALLOCA )
    buf = ( char * ) malloc( total_bytesize );
#else
    buf = ( char * ) alloca( total_bytesize );
#endif
    if ( buf == NULL ) return 0;
        
    memcpy( buf, src, byte2char( total_bytesize ) );
    byteswap( Nelem, elem_sz, buf );
    count = fwrite( buf, elem_sz, Nelem, outfd );
#if ! defined( HAVE_ALLOCA )
    free( buf ); 
#endif
    return count;
}



bswp_uint32_t bswp_fread(       void           *dest,
                          const bswp_uint32_t   elem_sz,
                          const bswp_uint32_t   Nelem,
                                FILE           *infd )
{
    char *buf;
    bswp_uint32_t total_bytesize;
    bswp_uint32_t count;

    total_bytesize = Nelem * elem_sz;
#if ! defined( HAVE_ALLOCA )
    buf = ( char * ) malloc( total_bytesize );
#else
    buf = ( char * ) alloca( total_bytesize );
#endif
    if ( buf == NULL ) return 0;
        
    count = fread( buf, elem_sz, Nelem, infd );
    byteswap( Nelem, elem_sz, buf );
    memcpy( dest, buf, byte2char( total_bytesize ) );
#if ! defined( HAVE_ALLOCA )
    free( buf ); 
#endif
    return count;
}

#else        /* of if ! defined( WORDS_BIGENDIAN )  */

bswp_uint32_t bswp_fwrite( const void           *src,
                           const bswp_uint32_t   elem_sz,
                           const bswp_uint32_t   Nelem,
                                 FILE           *outfd )
{ return( fwrite( src, elem_sz, Nelem, outfd ) ); }

bswp_uint32_t bswp_fread(       void           *dest,
                          const bswp_uint32_t   elem_sz,
                          const bswp_uint32_t   Nelem,
                                FILE           *infd )
{ return( fread( dest, elem_sz, Nelem, infd ) ); }

#endif       /* of if ! defined( WORDS_BIGENDIAN )  */
