#include <stdio.h>
#include <string.h>
#include "elem.h"



main( int argc, char **argv )
{
	fbuf_uint32_t buf_sz;
	const char    filename[] = "fbuf.data";

	filebuf_t    *fbuf_ptr;
        MixedElem    *elem_ptr;

        FILE         *infd;

	int           icontinue;
        fbuf_uint32_t frame_count;
        fbuf_uint32_t frame_idx_max;
        fbuf_uint32_t frame_idx;
        fbuf_uint32_t N_frame_elem;
        fbuf_uint32_t elem_idx;
        fbuf_uint32_t ii;

        infd           = fopen( filename, "rb" );
        bswp_fread( &buf_sz, sizeof( fbuf_uint32_t ), 1, infd );
	fbuf_ptr       = fbuf_create( buf_sz );
        fbuf_filedesc( fbuf_ptr, infd );

        elem_idx = 0; frame_count = 0;
	while( fbuf_refill( fbuf_ptr ) != EOF ) {
	        fbuf_withdraw( &N_frame_elem, sizeof( fbuf_uint32_t ), 1,
                               fbuf_ptr );
		printf( "frame_count = %d : N_frame_elem = %d\n",
                        frame_count, N_frame_elem );
                fflush( stdout );
	        for ( frame_idx = 0; frame_idx < N_frame_elem; frame_idx++ ) {
	                elem_ptr = fbuf2elem( fbuf_ptr );

	                printf( "elem_idx = %d : ", elem_idx );
                        printf( "felem = %f, ", elem_ptr->felem );
	                printf( "icount = %d, ", elem_ptr->icount );
	                fflush(stdout);

	                printf( "[ " );
	                for ( ii = 0; ii < elem_ptr->icount; ii++ )
	                        printf( "%d ", elem_ptr->iary[ ii ] );
	                printf( "]\n" );
	                fflush(stdout);
        
                        elem_idx++;
	                elem_free( elem_ptr );
	        }
                frame_count++;
	}

	printf( "sizeof( MixedElem ) = %d\n",
                sizeof( MixedElem )/sizeof( char ) );
	printf( "The elem_idx_max is %d\n", elem_idx-1 );

	fclose( fbuf_ptr->fd );
	fbuf_free( fbuf_ptr );
}
