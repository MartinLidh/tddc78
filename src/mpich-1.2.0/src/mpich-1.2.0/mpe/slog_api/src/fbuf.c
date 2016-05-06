#include <stdlib.h>
#include "fbuf.h"

/*  Global constant for file_buffering code  */
const fbuf_uint32_t base_fbufunit = sizeof( fbuf_byte_t );

/*  On 1/4/99, byte2char is used in memcpy  */
/*
size_t byte2char( const size_t bytesize )
{
    return ( bytesize % sizeof( char ) == 0 ?
             bytesize / sizeof( char ) : bytesize / sizeof( char ) + 1 );
}
*/
#if ! defined( BYTE2CHAR )
#define BYTE2CHAR
#define byte2char( bytesize )                    \
        ( (bytesize) % sizeof( char ) == 0 ?     \
          (bytesize) / sizeof( char ) : (bytesize) / sizeof( char ) + 1 )
#endif



fbuf_uint32_t byte2fbufunit( const fbuf_uint32_t bytesize )
{
    return ( bytesize % base_fbufunit == 0 ?
             bytesize / base_fbufunit : bytesize / base_fbufunit + 1 );
}

/*  Create a file buffer with "buffer_size" bytes  */
filebuf_t *fbuf_create( const fbuf_uint32_t buffer_size )
{
    filebuf_t *fbuf;

    fbuf = ( filebuf_t * ) malloc( sizeof( filebuf_t ) );
    if ( fbuf == NULL ) {
        return NULL;
    }

    /*
        fbuf->bsize : current buffer _byte_ size in File/Disk
        fbuf->head  : point at the 1st available byte of the buffer
        fbuf->end   : point at the 1st un-available byte of the buffer
        fbuf->ptr   : point at the currently available byte of the buffer
    */
    fbuf->bsize = byte2fbufunit( buffer_size ) * base_fbufunit ;
    fbuf->head  = ( fbuf_byte_t * ) malloc( fbuf->bsize );
    fbuf->end   = fbuf->head + byte2fbufunit( buffer_size );
    fbuf->ptr   = fbuf->head;
    fbuf->fd    = NULL;

    return fbuf;
}

/*  Define the fd field in filebuf_t  */
void fbuf_filedesc( filebuf_t *fbuf, FILE *infd )
{
    if ( infd != NULL )
        fbuf->fd = infd;
}

/*  Free all filebuf_t's allocated memory which is allocated by fbuf_create() */
void fbuf_free( filebuf_t *fbuf )
{
    if ( fbuf != NULL ) {
        if ( fbuf->head != NULL ) {
            free( fbuf->head );
            fbuf->head = NULL;
        }
        free( fbuf );
        fbuf = NULL;
    }
}

fbuf_uint32_t fbuf_bufsz( const filebuf_t *fbuf )
{
    return( fbuf->bsize );
}

/*  Advance Forward the pointer of deposition in filebuf_t  */
int fbuf_increment( const fbuf_uint32_t bytesize, filebuf_t *fbuf )
{
    fbuf_uint32_t fbufunit_sz;

    fbufunit_sz = byte2fbufunit( bytesize );
    if ( fbuf->ptr + fbufunit_sz <= fbuf->end ) {
        fbuf->ptr += fbufunit_sz;
        return 1;
    }
    else
        return 0;
}

/*  Advance Backward the pointer of deposition in filebuf_t  */
int fbuf_decrement( const fbuf_uint32_t bytesize, filebuf_t *fbuf )
{
    fbuf_uint32_t fbufunit_sz;

    fbufunit_sz = byte2fbufunit( bytesize );
    if ( fbuf->ptr - fbufunit_sz >= fbuf->head ) {
        fbuf->ptr -= fbufunit_sz;
        return 1;
    }
    else
        return 0;
}

void fbuf_ptr2head( filebuf_t  *fbuf )
{
    fbuf->ptr = fbuf->head;
}

/*  Empty the content of the file buffer to the file pointed by "fd"  */
int fbuf_empty( filebuf_t  *fbuf )
{
    if ( fwrite( fbuf->head, fbuf->bsize, 1, fbuf->fd ) < 1 )
        return EOF;
    fbuf->ptr = fbuf->head;
    return 0;
}

/*  Check if there is enough space left in the buffer to deposit the object  */
int fbuf_ok2deposit( const fbuf_uint32_t obj_bsize, filebuf_t *fbuf )
{
    return ( fbuf->ptr + byte2fbufunit( obj_bsize ) <= fbuf->end );
}

/*  Deposit the array type of object into the file buffer  */
fbuf_uint32_t fbuf_deposit( const void           *src,
                            const fbuf_uint32_t   elem_sz,
                            const fbuf_uint32_t   Nelem,
                                  filebuf_t      *fbuf )
{
#if ! defined( WORDS_BIGENDIAN )
    void          *buf;
#endif
    fbuf_uint32_t  total_bytesize;
    fbuf_uint32_t  total_fbufunit;

    total_bytesize = elem_sz * Nelem;
    total_fbufunit = byte2fbufunit( total_bytesize );
    if ( fbuf->ptr + total_fbufunit <= fbuf->end ) {
#if ! defined( WORDS_BIGENDIAN )
#  if ! defined( HAVE_ALLOCA )
        buf = ( char * ) malloc( total_bytesize );
#  else
        buf = ( char * ) alloca( total_bytesize );
#  endif
        if ( buf == NULL ) return (fbuf_uint32_t) 0;
        memcpy( buf, src, byte2char( total_bytesize ) );
        byteswap( Nelem, elem_sz, buf );
        memcpy( fbuf->ptr, buf, byte2char( total_bytesize ) );
#  if ! defined( HAVE_ALLOCA )
        free( buf );
#  endif
#else
        memcpy( fbuf->ptr, src, byte2char( total_bytesize ) );
#endif
        fbuf->ptr += total_fbufunit;
        return Nelem;
    }
    else 
        return (fbuf_uint32_t) 0;
}

/*  Deposit the array and Empty the buffer to the file if the buffer is full  */
fbuf_uint32_t fbuf_write( const void           *src,
                          const fbuf_uint32_t   elem_sz,
                          const fbuf_uint32_t   Nelem,
                                filebuf_t      *fbuf )
{
    fbuf_uint32_t ierr;

    ierr = fbuf_deposit( src, elem_sz, Nelem, fbuf );
    if ( ierr == 0 ) fbuf_empty( fbuf );
    return ierr;
}

/*  Fill up the content of the buffer from the file  */
int fbuf_refill( filebuf_t  *fbuf )
{
    if ( fread( fbuf->head, fbuf->bsize, 1, fbuf->fd ) < 1 )
        return EOF;
    fbuf->ptr = fbuf->head;
    return 0;
}

/*  Check if there is "obj_bsize" bytes in the buffer to be withdrawn  */
int fbuf_ok2withdraw( const fbuf_uint32_t obj_bsize, filebuf_t *fbuf )
{
    return ( fbuf->ptr + byte2fbufunit( obj_bsize ) <= fbuf->end );
}

/*  Withdraw the array type of object from the file buffer  */
fbuf_uint32_t fbuf_withdraw(       void           *dest,
                             const fbuf_uint32_t   elem_sz,
                             const fbuf_uint32_t   Nelem,
                                   filebuf_t      *fbuf )
{
#if ! defined( WORDS_BIGENDIAN )
    void          *buf;
#endif
    fbuf_uint32_t  total_bytesize;
    fbuf_uint32_t  total_fbufunit;

    total_bytesize = elem_sz * Nelem;
    total_fbufunit = byte2fbufunit( total_bytesize );
    if ( fbuf->ptr + total_fbufunit <= fbuf->end ) {
#if ! defined( WORDS_BIGENDIAN )
#  if ! defined( HAVE_ALLOCA )
        buf = ( char * ) malloc( total_bytesize );
#  else
        buf = ( char * ) alloca( total_bytesize );
#  endif
        if ( buf == NULL ) return (fbuf_uint32_t) 0;
        memcpy( buf, fbuf->ptr, byte2char( total_bytesize ) );
        byteswap( Nelem, elem_sz, buf );
        memcpy( dest, buf, byte2char( total_bytesize ) );
#  if ! defined( HAVE_ALLOCA )
        free( buf );
#  endif
#else
        memcpy( dest, fbuf->ptr, byte2char( total_bytesize ) );
#endif
        fbuf->ptr += total_fbufunit;
        return Nelem;
    }
    else 
        return (fbuf_uint32_t) 0;
}

/*  Withdraw the array and refill the buffer if necessary  */
fbuf_uint32_t fbuf_read(       void           *dest,
                         const fbuf_uint32_t   elem_sz,
                         const fbuf_uint32_t   Nelem,
                               filebuf_t      *fbuf )
{
    fbuf_uint32_t ierr;

    ierr = fbuf_withdraw( dest, elem_sz, Nelem, fbuf );
    if ( ierr == 0 ) fbuf_refill( fbuf );
    return ierr;
}
