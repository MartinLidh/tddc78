#include <stdio.h>
#include <string.h>
#include "elem.h"



main( int argc, char **argv )
{
	fbuf_uint32_t buf_sz = 512;
        int           N_frames = 2;
	const char    filename[] = "fbuf.data";

	filebuf_t    *fbuf_ptr;
        MixedElem    *elem_ptr;

	int           icontinue;
        fbuf_uint32_t frame_count;
        fbuf_uint32_t frame_idx_max;
        fbuf_uint32_t frame_idx;
        fbuf_uint32_t N_frame_elem;
        fbuf_uint32_t elem_idx;

	if ( argc > 1 ) 
		if ( isdigit( argv[1][0] ) ) buf_sz = atoi( argv[1] );
	if ( argc > 2 )
		if ( isdigit( argv[2][0] ) ) N_frames = atoi( argv[2] );

	fbuf_ptr       = fbuf_create( buf_sz );
        fbuf_filedesc( fbuf_ptr, fopen( filename, "wb" ) );
	bswp_fwrite( &(fbuf_ptr->bsize),
                     sizeof( fbuf_uint32_t ), 1, fbuf_ptr->fd );

        elem_idx = 0;
        for ( frame_count = 0; frame_count < N_frames; frame_count++ ) {
	        fbuf_increment( sizeof( fbuf_uint32_t ), fbuf_ptr );
	        icontinue = true; frame_idx = 0;
	        while( icontinue ) {
			elem_ptr = elem_create( ( double ) ( elem_idx * 10 ),
	                                        ( int ) elem_idx );
			if ( icontinue = elem2fbuf( fbuf_ptr, elem_ptr ) ) {
				printf( "elem_idx = %d,  used buffer = %d\n",
				        elem_idx, 
                                        fbuf_ptr->ptr - fbuf_ptr->head );
				frame_idx++; elem_idx++;
 			}
			else {
	 			frame_idx_max = frame_idx - 1;
				N_frame_elem  = frame_idx;
			}
			elem_free( elem_ptr );
		}
		/*  set fbuf to its head  */
		fbuf_decrement( fbuf_ptr->ptr - fbuf_ptr->head, fbuf_ptr );
		fbuf_deposit( &N_frame_elem, sizeof( fbuf_uint32_t ), 1,
                              fbuf_ptr );
                /*
		memcpy( fbuf_ptr->head, &N_frame_elem,
	                sizeof( fbuf_uint32_t )/sizeof( char ) );
                */

	        fbuf_empty( fbuf_ptr );
        }

	printf( "sizeof( MixedElem ) = %d\n",
                sizeof( MixedElem )/sizeof( char ) );
	printf( "The elem_idx_max is %d\n", elem_idx-1 );

	fclose( fbuf_ptr->fd );
	fbuf_free( fbuf_ptr );
}
