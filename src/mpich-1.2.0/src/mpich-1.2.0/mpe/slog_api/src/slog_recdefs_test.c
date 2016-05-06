#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "slog_recdefs.h"

#if 0
#define  MaxAssocs  4
#define  MaxArgs    6
#else
#define  MaxAssocs  0
#define  MaxArgs    0
#endif

SLOG_intvltype_t GetIntvlType( const SLOG_uint32 IdxType,
                               const SLOG_uint32 IdxAssocs,
                               const SLOG_uint32 IdxArgs );

main( int argc, char **argv )
{
          char                  filename[80] = "SLOG_RecDefs.dat";

    const SLOG_intvltype_t      VarIrec_MinIdx = 0;
    const SLOG_intvltype_t      FixIrec_MinIdx = 1;
    const SLOG_intvltype_t      FixIrec_MaxIdx = 30;

          SLOG_STREAM          *slog;
          SLOG_intvltype_t      intvltype;
          SLOG_bebit_t          bebit_0 = 1;
          SLOG_bebit_t          bebit_1 = 1;
          SLOG_N_assocs_t       Nassocs;
          SLOG_N_args_t         Nargs;
    const char                  FileSeparator = '/';
          int                   name_char;
          int                   idx_type;
          int                   ierr;

    if ( argc > 1 ) {
        printf( "argv[1] = _%s_\n", argv[1] );
        name_char = (int) argv[1][0];
        if (  isalnum( name_char )
           || name_char == FileSeparator || name_char == '.' )
            strcpy( filename, argv[1] );
    }

    printf( "sizeof( SLOG_recdef_t ) = %d\n", sizeof( SLOG_recdef_t ) );
    printf( "component sum of SLOG_recdef_t = %d\n",
               sizeof( SLOG_intvltype_t ) + sizeof( SLOG_bebits_t )
             + sizeof( SLOG_N_assocs_t ) + sizeof( SLOG_N_args_t ) );

/*  First write the RecDefs table to the disk */

    /*  This code only uses fd and rec_defs of SLOG_STREAM  */
    slog = ( SLOG_STREAM * ) malloc ( sizeof ( SLOG_STREAM ) );

    if ( ( slog->fd = fopen( filename, "wb" ) ) == NULL ){
        fprintf( errfile, __FILE__":Main() - file %s "
                          "cannot be opened for writing\n", filename );
        fflush( errfile );
        exit( 1 );
    }


    if ( SLOG_RDEF_Open( slog ) != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":Main() - SLOG_RDEF_Open() fails " );
        fflush( errfile );
        exit( 1 );
    }
    /*  Create the Indexes for Variable Interval Record  */

    Nargs = 0;    /*  Initialization, does NOT mean anything  */
    for ( Nassocs = 0; Nassocs <= MaxAssocs; Nassocs++ ) {
        intvltype = GetIntvlType( VarIrec_MinIdx, Nassocs, Nargs );
        ierr = SLOG_RDEF_AddRecDef( slog, 
                                    intvltype, bebit_0, bebit_1,
                                    Nassocs, Nargs );
        if ( ierr == SLOG_FAIL ) {
            printf( "SLOG_RDEF_AddRecDef() fails at intvltype = "
                    fmt_itype_t"\n", intvltype );
            exit( 1 );
        }
    }


    /*  Create the Indexes for Fixed Interval Record  */

    for ( idx_type = FixIrec_MinIdx; idx_type <= FixIrec_MaxIdx; idx_type++ ) {
        for ( Nassocs = 0; Nassocs <= MaxAssocs; Nassocs++ ) {
            for ( Nargs = 0; Nargs <= MaxArgs; Nargs++ ) {
                intvltype = GetIntvlType( idx_type, Nassocs, Nargs );
                ierr = SLOG_RDEF_AddRecDef( slog, 
                                            intvltype, bebit_0, bebit_1,
                                            Nassocs, Nargs );
                if ( ierr == SLOG_FAIL ) {
                    printf( "SLOG_RDEF_AddRecDef() fails at intvltype = "
                            fmt_itype_t"\n", intvltype );
                    exit( 1 );
                }
            }
        }
    }

    /*  To check if the cross checking code is working  */
    intvltype = 900;
    ierr = SLOG_RDEF_AddRecDef( slog, 
                                intvltype, bebit_0, bebit_1,
                                Nassocs, Nargs );

    ierr = SLOG_RDEF_Close( slog );
    if ( ierr != SLOG_SUCCESS ) {
        printf( "SLOG_RDEF_Close() fails!\n" );
        exit( 1 );
    }

    ierr = fclose( slog->fd );
    if ( ierr ) {
        fprintf( errfile, __FILE__":Main() fails in closing "
                          " the file %s with ierr = %d\n",
                          filename, ierr );
        fflush( errfile );
        exit( 1 );
    }
    free( slog->rec_defs->entries );
    free( slog->rec_defs );
    free( slog );

/*  Then read the RecDefs table from the disk */

    /*  This code only uses fd and rec_defs of SLOG_STREAM  */
    slog = ( SLOG_STREAM * ) malloc ( sizeof ( SLOG_STREAM ) );

    if ( ( slog->fd = fopen( filename, "rb" ) ) == NULL ){
        fprintf( errfile, __FILE__":Main() - file %s "
                          "cannot be opened for reading\n", filename );
        fflush( errfile );
        exit( 1 );
    }

    ierr = SLOG_RDEF_ReadRecDefs( slog );

    ierr = fclose( slog->fd );
    if ( ierr ) {
        fprintf( errfile, __FILE__":Main() fails in closing "
                          " the file %s with ierr = %d\n",
                          filename, ierr );
        fflush( errfile );
        exit( 1 );
    }

    SLOG_RDEF_Print( outfile, slog->rec_defs );
    free( slog->rec_defs->entries );
    free( slog->rec_defs );
    free( slog );
}



SLOG_intvltype_t GetIntvlType( const SLOG_uint32 IdxType,
                               const SLOG_uint32 IdxAssocs,
                               const SLOG_uint32 IdxArgs )
{
    const unsigned int    UnitType       = 100;
    const unsigned int    UnitAssocs     = 10;
    const unsigned int    UnitArgs       = 1;
    /*  May want to check if UnitXXXX is in the correct range  */
    return ( IdxType * UnitType + IdxAssocs * UnitAssocs + IdxArgs * UnitArgs );
}
