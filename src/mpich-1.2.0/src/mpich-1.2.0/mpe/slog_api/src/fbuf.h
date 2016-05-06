#if !defined ( _FBUF )
#define _FBUF

#include <stdio.h>
#include <string.h>
#ifdef HAVE_SLOGCONF_H
#include "slog_config.h"
#endif
#if defined( HAVE_ALLOCA_H )
#    include <alloca.h>
#endif

/*
    1 - Assume that the buffer size is no larger than what can be hold
        in a 4 bytes unsigned integer.
    2 - This code does NOT assume the sizeof( char ) = 1 byte.
    3 - Theoretically, one can modify the basic type of the buffer
        by changing fbuf_byte_t.  The code will calculate the actual space
        needed in the buffer for object through use of byte2fbufunit().
*/
typedef  unsigned int  fbuf_uint32_t;
typedef  char          fbuf_byte_t;

typedef struct {
    fbuf_uint32_t  bsize;   /* Current buffer _byte_ size in File/Disk        */
    fbuf_byte_t   *head;    /* point at the 1st available byte of the buffer  */
    fbuf_byte_t   *end;     /* point at the 1st un-available byte of the buf  */
    fbuf_byte_t   *ptr;     /* point at the currently available byte ...      */
    FILE          *fd;
} filebuf_t;

fbuf_uint32_t byte2fbufunit( const fbuf_uint32_t bytesize );

filebuf_t *fbuf_create( const fbuf_uint32_t buffer_size );

void fbuf_filedesc( filebuf_t *fbuf, FILE *infd );

void fbuf_free( filebuf_t *fbuf );

fbuf_uint32_t fbuf_bufsz( const filebuf_t *fbuf );

int fbuf_increment( const fbuf_uint32_t bytesize, filebuf_t *fbuf );

int fbuf_decrement( const fbuf_uint32_t bytesize, filebuf_t *fbuf );

void fbuf_ptr2head( filebuf_t  *fbuf );

int fbuf_empty( filebuf_t  *fbuf );

int fbuf_ok2deposit( const fbuf_uint32_t obj_bsize, filebuf_t *fbuf );

fbuf_uint32_t fbuf_deposit( const void           *src,
                            const fbuf_uint32_t   elem_sz,
                            const fbuf_uint32_t   Nelem,
                                  filebuf_t      *fbuf );

fbuf_uint32_t fbuf_write( const void           *src,
                          const fbuf_uint32_t   elem_sz,
                          const fbuf_uint32_t   Nelem,
                                filebuf_t      *fbuf );

int fbuf_refill( filebuf_t  *fbuf );

int fbuf_ok2withdraw( const fbuf_uint32_t obj_bsize, filebuf_t *fbuf );

fbuf_uint32_t fbuf_withdraw(       void           *dest,
                             const fbuf_uint32_t   elem_sz,
                             const fbuf_uint32_t   Nelem,
                                   filebuf_t      *fbuf );

fbuf_uint32_t fbuf_read(       void           *dest,
                         const fbuf_uint32_t   elem_sz,
                         const fbuf_uint32_t   Nelem,
                               filebuf_t      *fbuf );

#endif        /* if  !defined( _FBUF )  */
