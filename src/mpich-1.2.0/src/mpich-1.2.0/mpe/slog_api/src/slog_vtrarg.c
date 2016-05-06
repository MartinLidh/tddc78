#include <stdlib.h>
#include <stdio.h>
#include "slog_vtrarg.h"            /*I  "slog_vtrarg.h"  I*/



/*  If the 2nd argument args[] = NULL, the vtr_arg will NOT be initialized  */
SLOG_vtrarg_t *SLOG_Varg_Create( const SLOG_N_args_t  N_args,
                                 const SLOG_arg_t    *args )
{
    SLOG_vtrarg_t *vtr_args;
    int            ii;

    if ( N_args <= 0 )
        return NULL;

    vtr_args = ( SLOG_vtrarg_t * ) malloc( sizeof( SLOG_vtrarg_t ) );
    if ( vtr_args == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Varg_Create() - "
                          "malloc() for vtr_args fails\n" );
        fflush( errfile );
        return NULL;
    }

    vtr_args->size = N_args;

    vtr_args->values = ( SLOG_arg_t * )
                       malloc( vtr_args->size * sizeof( SLOG_arg_t ) );
    if ( vtr_args->size > 0 && vtr_args->values == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Varg_Create() - "
                          "malloc() for vtr_arg->values[] fails\n" );
        fflush( errfile );
        return NULL;
    }

    if ( args != NULL )
        for ( ii = 0; ii < vtr_args->size; ii++ )
            vtr_args->values[ ii ] = args[ ii ];

    return vtr_args;
}



void SLOG_Varg_Free( SLOG_vtrarg_t *vtr_arg )
{
    if ( vtr_arg != NULL ) {
        if ( vtr_arg->size > 0 && vtr_arg->values != NULL ) {
            free( vtr_arg->values );
            vtr_arg->values = NULL;
        }
        free( vtr_arg );
        vtr_arg = NULL;
    }
}



SLOG_vtrarg_t *SLOG_Varg_Copy( const SLOG_vtrarg_t *src )
{
    if ( src != NULL )
        return( SLOG_Varg_Create( src->size, src->values ) );
    else
        return NULL;
}



int SLOG_Varg_Print( const SLOG_vtrarg_t *vtr_arg,  FILE *outfd )
{
    int  ii;

    if ( vtr_arg == NULL ) {
        fprintf( outfd, __FILE__":SLOG_Varg_Print() - the input interval "
                        "record pointer is NULL\n" );
        fflush( outfd );
        return SLOG_FAIL;
    }

    fprintf( outfd, "[ " );
    for ( ii = 0; ii < vtr_arg->size; ii++ )
        fprintf( outfd, fmt_arg_t" ", vtr_arg->values[ii] );
    fprintf( outfd, "] " );

    fflush( outfd );

    return SLOG_SUCCESS;
}


int SLOG_Varg_IsEqualTo( const SLOG_vtrarg_t *vtr1, const SLOG_vtrarg_t *vtr2 )
{
    int ii;

    if ( vtr1 != vtr2 ) {
        if ( vtr1->size != vtr2->size )
            return SLOG_FALSE;

        if ( vtr1->values != vtr2->values ) {
            for ( ii = 0; ii < vtr1->size; ii++ )
                if ( vtr1->values[ ii ] != vtr2->values[ ii ] )
                    return SLOG_FALSE;
        }
    }

    return SLOG_TRUE;
}



SLOG_N_vtrargs_t SLOG_Vargs_NumOfVtrs( const SLOG_vtrarg_lptr_t  *vtrs )
{
    const SLOG_vtrarg_lptr_t  *vptr;
          SLOG_N_vtrargs_t     count;

    count = 0;
    for ( vptr = vtrs; vptr != NULL; vptr = vptr->next )
        count++;

    return count;
}



int SLOG_Vargs_AreEqualTo( const SLOG_vtrarg_lptr_t  *vtrs1,
                           const SLOG_vtrarg_lptr_t  *vtrs2 )
{
    const SLOG_vtrarg_lptr_t  *vptr1;
    const SLOG_vtrarg_lptr_t  *vptr2;

    if ( vtrs1 != vtrs2 ) {
        if ( SLOG_Vargs_NumOfVtrs( vtrs1 ) != SLOG_Vargs_NumOfVtrs( vtrs2 ) )
            return SLOG_FALSE;

        /*
            A very restricted form of equivalent is implemented
            Here 2 vtrargs are considered equal if the underneath
            set of vectors are arranged in the SAME order and the
            vectors in the same place of vtrargs are equal as well
        */
        for ( vptr1 = vtrs1, vptr2 = vtrs2;
              vptr1 != NULL && vptr2 != NULL;
              vptr1 = vptr1->next, vptr2 = vptr2->next )
            if ( ! SLOG_Varg_IsEqualTo( vptr1->vtr, vptr2->vtr ) )
                return SLOG_FALSE;
    }

    return SLOG_TRUE;
}
