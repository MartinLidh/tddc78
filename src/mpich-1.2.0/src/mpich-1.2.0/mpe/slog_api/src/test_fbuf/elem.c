#include <stdio.h>
#include <string.h>
#include "elem.h"

MixedElem *elem_create( const double dval, const int ival )
{
        MixedElem *elem;
        int ii;

        elem = ( MixedElem * ) malloc( sizeof ( MixedElem ) );

        elem->felem  = dval;
        elem->icount = ival;
        elem->iary   = ( int * ) malloc( elem->icount * sizeof( int ) );

        if ( elem->iary != NULL || elem->icount == 0 ) {
                for ( ii = 0; ii < elem->icount; ii++ )
                        elem->iary[ ii ] = ii;
                return elem;
        }
        else
                return NULL;
}

void elem_free( MixedElem *elem )
{
        if ( elem != NULL ) {
                if ( elem->iary != NULL ) free( elem->iary );
                free( elem );
        }
}



int elem2fbuf( filebuf_t *fbuf, const MixedElem *elem )
{
        int elem_bsize;

        elem_bsize = sizeof( double ) + sizeof( int )
                   + elem->icount * sizeof( int );
        if ( fbuf_ok2deposit( elem_bsize, fbuf ) ) {
                fbuf_deposit( &(elem->felem), sizeof( double ), 1, fbuf );
                fbuf_deposit( &(elem->icount), sizeof( int ), 1, fbuf );
                fbuf_deposit( elem->iary, sizeof( int ), elem->icount, fbuf );
                return true;
        }
        else
                return false;
}



MixedElem *fbuf2elem( filebuf_t *fbuf )
{
        MixedElem *elem;
        int        elem_icount;
        size_t     elem_bsize;

        elem_bsize = sizeof( double ) + sizeof( int );
        if ( fbuf_ok2withdraw( elem_bsize, fbuf ) ) {
                fbuf_increment( sizeof( double ), fbuf );
                fbuf_withdraw( &elem_icount, sizeof( int ), 1, fbuf );
                fbuf_decrement( elem_bsize, fbuf );
        } 

        elem       = elem_create( 0.0, elem_icount );

        elem_bsize = sizeof( double ) + sizeof( int )
                   + elem->icount * sizeof( int );

        if ( fbuf_ok2withdraw( elem_bsize, fbuf ) ) {
                fbuf_withdraw( &(elem->felem), sizeof( double ), 1, fbuf );
                fbuf_withdraw( &(elem->icount), sizeof( int ), 1, fbuf );
                fbuf_withdraw( elem->iary, sizeof( int ), elem->icount, fbuf );
                return elem;
        }
        else {
                return NULL;
        }
}
