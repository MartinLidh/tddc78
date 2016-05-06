#include <stdlib.h>
#include <stdio.h>

#include "str_util.h"
#include "bswp_fileio.h"
#include "slog_fileio.h"
#include "slog_header.h"
#include "slog_pstat.h"



/*@C
    SLOG_SetPreviewStatisticsSize - Set/Override the default number of bins 
                                    of the statistics array.

  Unmodified Input parameter:
. stat_bin_size - the number of bin of the statistics array.

  Modified Input parameter :
. slog - pointer to the SLOG_STREAM where interval records are stored.

  Modified Output parameter :
. returned value - integer return status code.

  Include file need:
  slog_pstat.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_SetPreviewStatisticsSize(       SLOG_STREAM     *slog,
                                   const SLOG_uint32      stat_bin_size )
{
    if ( slog == NULL ) {
        fprintf( errfile, __FILE__":SLOG_SetPreviewStatisticsSize() - \n"
                          "\t""the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->pstat == NULL ) {
        fprintf( errfile, __FILE__":SLOG_SetPreviewStatisticsSize() - \n"
                          "\t""the SLOG_STREAM's Preview Statistics pointer "
                          "is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( stat_bin_size <= SLOG_PSTAT_STATSIZE_MAX )
        slog->pstat->Nbin = stat_bin_size;
    else {
        slog->pstat->Nbin = SLOG_PSTAT_STATSIZE_MAX;
        fprintf( errfile, __FILE__":SLOG_SetPreviewStatisticsSize() - \n"
                          "\t""The input number of bins, "fmt_ui32", is "
                          "too big.  Instead the maximum, "fmt_ui32", is "
                          "used\n", stat_bin_size, SLOG_PSTAT_STATSIZE_MAX );
        fflush( errfile );
    }

    return SLOG_SUCCESS;
}



SLOG_pstat_t *SLOG_PSTAT_Create( const SLOG_uint32   stat_set_size )
{
    SLOG_pstat_t   *slog_pstat;

    if ( stat_set_size == 0 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Create() - \n"
                          "Input set size, " fmt_ui32", is zero\n",
                          stat_set_size );
        fflush( errfile );
        return NULL;
    }

    slog_pstat = ( SLOG_pstat_t * ) malloc( sizeof( SLOG_pstat_t ) );
    if ( slog_pstat == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Create() - malloc() fails\n" );
        fflush( errfile );
        return NULL;
    }

    /*  Initailize */
    slog_pstat->starttime         = SLOG_time_NULL;
    slog_pstat->endtime           = SLOG_time_NULL;
    slog_pstat->bin_width_in_time = SLOG_time_NULL;
    slog_pstat->Nset              = 0;
    slog_pstat->Nbin              = stat_set_size;

    slog_pstat->sets              = NULL;
    slog_pstat->seq_idx_vals      = NULL;

    return slog_pstat;
}



SLOG_pstat_t *SLOG_PSTAT_CreateContent(       SLOG_pstat_t *slog_pstat,
                                        const SLOG_uint32   N_stat_set )
{
    SLOG_uint32     idx;

    if ( N_stat_set == 0 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_CreateContent() - \n"
                          "Input No. of sets, "fmt_ui32", is zero\n",
                          N_stat_set );
        fflush( errfile );
        return NULL;
    }

    if ( slog_pstat == NULL )
        slog_pstat = ( SLOG_pstat_t * )  malloc( sizeof( SLOG_pstat_t ) );
    if ( slog_pstat == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_CreateContent() - "
                          "malloc()/realloc() fails\n" );
        fflush( errfile );
        return NULL;
    }

    /*  Initailize */
    slog_pstat->Nset = N_stat_set;
    slog_pstat->sets = ( SLOG_statset_t ** )
                       malloc( slog_pstat->Nset * sizeof( SLOG_statset_t * ) );
    if ( slog_pstat->Nset > 0 && slog_pstat->sets == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_CreateContent() - \n"
                          "\t""malloc() fails for slog_pstat->sets[]\n" );
        fflush( errfile );
        return NULL;
    }

    if ( slog_pstat->Nbin == 0 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_CreateContent() - \n"
                          "\t""Number of bin per set, slog_pstat->Nbin, "
                          fmt_ui32", is zero\n", slog_pstat->Nbin );
        fflush( errfile );
        return NULL;
    }

    for ( idx = 0; idx < slog_pstat->Nset; idx++ ) {
        slog_pstat->sets[ idx ] = SLOG_StatSet_Create( slog_pstat->Nbin );
        if ( slog_pstat->sets[ idx ] == NULL ) {
            fprintf( errfile, __FILE__":SLOG_PSTAT_CreateContent() - \n"
                              "\t""SLOG_StatSet_Create() fails for "
                              "slog_pstat->sets["fmt_ui32"]\n", idx );
            fflush( errfile );
            return NULL;
        }
    }

    slog_pstat->seq_idx_vals = ( SLOG_int32 * )
                               malloc( slog_pstat->Nset
                                     * sizeof( SLOG_int32 ) );
    if ( slog_pstat->Nset > 0 && slog_pstat->seq_idx_vals == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_CreateContent() - \n"
                          "\t""malloc() fails for "
                          "slog_pstat->seq_idx_vals[]\n" );
        fflush( errfile );
        return NULL;
    }

    /*  SLOG_FAIL( -1 ) indicates that it has NOT been used  */
    for ( idx = 0; idx < slog_pstat->Nset; idx++ ) {
        slog_pstat->seq_idx_vals[ idx ] = SLOG_FAIL;
    }

    return slog_pstat;
}



void SLOG_PSTAT_Free( SLOG_pstat_t  *slog_pstat )
{
    SLOG_uint32  idx;

    if ( slog_pstat != NULL ) {
        if ( slog_pstat->Nset > 0 && slog_pstat->sets != NULL ) {
            for ( idx = 0; idx < slog_pstat->Nset; idx++ )
                SLOG_StatSet_Free( slog_pstat->sets[ idx ] );
            free( slog_pstat->sets );
            slog_pstat->sets = NULL;
        }
        if ( slog_pstat->Nset > 0 && slog_pstat->seq_idx_vals != NULL ) {
            free( slog_pstat->seq_idx_vals );
            slog_pstat->seq_idx_vals = NULL;
        }
        free( slog_pstat );
        slog_pstat = NULL;
    }
}



int SLOG_PSTAT_Open( SLOG_STREAM  *slog )
{
    if ( slog == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Open() - "
                          "the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    slog->pstat = SLOG_PSTAT_Create( SLOG_PSTAT_STATSIZE_MAX );
    if ( slog->pstat == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Open() - "
                          "SLOG_PSTAT_Create() fails !\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    return SLOG_SUCCESS;
}



int SLOG_PSTAT_Init( SLOG_STREAM  *slog )
{
    SLOG_pstat_t   *pstat;
    SLOG_prof_t    *profile;
    SLOG_uint32     pstat_Nset;
    SLOG_uint32     pstat_Nbin;
    int             idx;
    int             ierr;

    if ( slog == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Init() - "
                          "the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->hdr == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Init() - "
                          "the pointer to SLOG_hdr_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->prof == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Init() - "
                          "the pointer to SLOG_prof_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->rec_defs == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Init() - "
                          "the pointer to SLOG_recdefs_table_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->pstat == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Init() - "
                          "the pointer to SLOG_pstat_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->prof->Nentries != slog->rec_defs->Nentries ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Init() - \n"
                          "\t""Display Profile's Nentries("fmt_ui32") != "
                          "Record Definition Table's Nentries("fmt_ui32") !\n",
                          slog->prof->Nentries, slog->rec_defs->Nentries );
        fflush( errfile );
        return SLOG_FAIL;
    }

    slog->pstat = SLOG_PSTAT_CreateContent( slog->pstat,
                                            slog->prof->Nentries );
    if ( slog->pstat == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Init() - "
                          "SLOG_PSTAT_CreateContent( Nset="fmt_ui32" ) "
                          "fails !\n", slog->prof->Nentries );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Update the content of SLOG_hdr_t to both memory and disk  */
    slog->hdr->fptr2statistics = slog_ftell( slog->fd );
    ierr = SLOG_WriteUpdatedHeader( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Init() - "
                          "SLOG_WriteUpdatedHeader() fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*
        Write the Nset=0 and Nbin=0 of SLOG_pstat_t to the disk  
        to indicate that Nothing has been written to the disk
    */
    pstat_Nset = 0;
    ierr = bswp_fwrite( &( pstat_Nset ),
                        sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Init() - \n"
                          "\t""Fails at writing the total number of sets "
                          "of the Preview Statistics Arrays\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    pstat_Nbin = 0;
    ierr = bswp_fwrite( &( pstat_Nbin ),
                        sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Open() - \n"
                          "\t""Fails at writing the bin size "
                          "of each Preview Statistics Array\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Set the properties of SLOG_pstat_t  */
    profile  = slog->prof;
    pstat    = slog->pstat;

    if ( profile->Nentries != pstat->Nset ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Init() - "
                          "Display Profile's Nentries("fmt_ui32") != "
                          "Preview Statistics's Nset("fmt_ui32") !!!!!\n",
                          profile->Nentries, pstat->Nset );
        fflush( errfile );
        return SLOG_FAIL;
    }

    for ( idx = 0; idx < pstat->Nset; idx++ ) {
        SLOG_StatSet_SetIntvltype( pstat->sets[ idx ],
                                   ( profile->entries[ idx ] ).intvltype );
        SLOG_StatSet_SetLabel( pstat->sets[ idx ],
                               ( profile->entries[ idx ] ).label );
        pstat->seq_idx_vals[ idx ] = pstat->sets[ idx ]->intvltype;
    }

    return SLOG_SUCCESS;
}



/*  The following Write() routine can be called multiple times  */
/*  But be sure the slog->hdr->fptr2statistics is set  */
int SLOG_PSTAT_Write( SLOG_STREAM  *slog )
{
    filebuf_t    *tmp_fbuf;
    SLOG_uint32   expected_bsize;
    SLOG_uint32   Nbytes_written;
    int           idx;
    int           ierr;

    if ( slog == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Write() - "
                          "the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->hdr == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Write() - "
                          "the input SLOG_hdr_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->pstat == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Write() - "
                          "the input SLOG_pstat_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->pstat->Nset > 0 && slog->pstat->sets == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Write() - \n"
                          "\t""SLOG_pstat_t inconsistency detected\n"
                          "\t""slog->pstat->Nset = "fmt_ui32" and "
                          "slog->pstat->sets[] = NULL\n",
                          slog->pstat->Nset );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->hdr->fptr2statistics == SLOG_fptr_NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Write() - \n"
                          "\t""the SLOG_STREAM's fptr2statistics points at "
                          "SLOG_fptr_NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Compute the expected byte size of slog->pstat in the file buffer  */
    expected_bsize = SLOG_typesz[ ui32 ] * 2
                   + SLOG_typesz[  ts  ] * 2
                   + SLOG_PSTAT_SizeOfStatSetInFile( slog->pstat->Nbin )
                   * slog->pstat->Nset;

    /*  Create and initial the file buffer  */
    tmp_fbuf = ( filebuf_t * ) fbuf_create( expected_bsize );
    if ( tmp_fbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Write() - "
                          "fbuf_create() fails!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    fbuf_filedesc( tmp_fbuf, slog->fd );

    /*  Deposit the content of SLOG_pstat_t to the file buffer  */
    Nbytes_written = 0;

    ierr = fbuf_deposit( &( slog->pstat->Nset ), SLOG_typesz[ ui32 ],
                         1, tmp_fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Write() - Cannot "
                          "deposit the Nset to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_deposit( &( slog->pstat->Nbin ), SLOG_typesz[ ui32 ],
                         1, tmp_fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Write() - Cannot "
                          "deposit the Nbin to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_deposit( &( slog->pstat->starttime ), SLOG_typesz[  ts  ],
                         1, tmp_fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Write() - Cannot "
                          "deposit the StartTime to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[  ts  ];

    ierr = fbuf_deposit( &( slog->pstat->endtime ), SLOG_typesz[  ts  ],
                         1, tmp_fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Write() - Cannot "
                          "deposit the EndTime to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[  ts  ];

    for ( idx = 0; idx < slog->pstat->Nset; idx++ ) {
        ierr = SLOG_StatSet_DepositToFbuf( slog->pstat->sets[ idx ],
                                           tmp_fbuf );
        if ( ierr == SLOG_FAIL ) {
            fprintf( errfile, __FILE__":SLOG_PSTAT_Write() - Cannot "
                              "deposit %d-th StatSet to the SLOG filebuffer, "
                              fmt_ui32" bytes written so far\n", idx,
                              Nbytes_written );
            fflush( errfile );
            return SLOG_FAIL;
        }
        Nbytes_written += ierr;
    }

    /*  File Buffer usage test  */
    if ( Nbytes_written != expected_bsize ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Write() - "
                          "Nbytes written("fmt_ui32") != expected_bsize "
                          "written("fmt_ui32")\n", Nbytes_written,
                          expected_bsize );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Empty the content of tmp_fbuf to the disk at the specified location  */
    ierr = slog_fseek( slog->fd, slog->hdr->fptr2statistics, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Write() - "
                          "slog_fseek( slog->hdr->fptr2statistics ) fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    fbuf_empty( tmp_fbuf );
    fflush( slog->fd );

    fbuf_free( tmp_fbuf );

    return SLOG_SUCCESS;
}



int SLOG_PSTAT_Read( SLOG_STREAM  *slog )
{
    SLOG_fptr      file_loc_saved;
    filebuf_t     *tmp_fbuf;
    SLOG_uint32    expected_bsize;
    SLOG_uint32    Nbytes_read;
    SLOG_uint32    pstat_Nset;
    SLOG_uint32    pstat_Nbin;
    int            ierr;
    int            idx;

    if ( slog == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - "
                          "the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->hdr == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - "
                          "the input SLOG_hdr_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->hdr->fptr2statistics == SLOG_fptr_NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - \n"
                          "\t""the SLOG_STREAM's fptr2statistics points at "
                          "SLOG_fptr_NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Save the current file pointer position  */
    file_loc_saved = slog_ftell( slog->fd );

    /*  Locate where to read the Preview Statistics Array  */
    ierr = slog_fseek( slog->fd, slog->hdr->fptr2statistics, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - "
                          "slog_fseek( slog->hdr->fptr2statistics ) fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Read in the sizes of the Preview Statistics Arrays  */
    ierr = bswp_fread( &( pstat_Nset ),
                       sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - \n"
                          "\t""Fails at reading the total number of sets "
                          "of the Preview Statistics Arrays\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    ierr = bswp_fread( &( pstat_Nbin ),
                       sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - \n"
                          "\t""Fails at reading the bin size "
                          "of each Preview Statistics Array\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( pstat_Nset == 0 || pstat_Nbin == 0 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - Warning !!! \n"
                          "\t""Invalid sizes of the Preview Statistics Arrays, "
                          "Nset = "fmt_ui32",  Nbin = "fmt_ui32"\n",
                          pstat_Nset, pstat_Nbin );
        fprintf( errfile, "\t""This may be casued by incompleted statistics "
                          "collection process during the 2nd pass." );
        fflush( errfile );
        return SLOG_SUCCESS;
    }

    /*  Compute the expected size of the file buffer  */
    expected_bsize = SLOG_typesz[  ts  ] * 2
                   + SLOG_PSTAT_SizeOfStatSetInFile( pstat_Nbin )
                   * pstat_Nset;

    /*  Create and Initialize the file buffer  */
    tmp_fbuf = ( filebuf_t * ) fbuf_create( expected_bsize );
    if ( tmp_fbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - "
                          "fbuf_create() fails!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    fbuf_filedesc( tmp_fbuf, slog->fd );

    /*  Fill up the file buffer from the disk  */
    if ( fbuf_refill( tmp_fbuf ) == EOF ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - \n"
                          "\t""fbuf_refill( tmp_fbuf ) fails\n" );
        fflush( errfile );
        return SLOG_EOF;
    }

    /*  
        Create the SLOG_pstat_t data structure in SLOG_STREAM
        if either pstat_Nset or pstat_Nbin is zero, or both are
        zero, slog->pstat will remain unchanged, i.e. NULL
    */
    slog->pstat = SLOG_PSTAT_Create( pstat_Nbin );
    if ( slog->pstat == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - \n"
                          "\t""SLOG_PSTAT_Create( Nbin="fmt_ui32" ) fails\n",
                          pstat_Nbin );
        fflush( errfile );
        return SLOG_FAIL;
    }

    slog->pstat = SLOG_PSTAT_CreateContent( slog->pstat, pstat_Nset );
    if ( slog->pstat == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - \n"
                          "\t""SLOG_PSTAT_CreateContent( Nset="fmt_ui32" ) "
                          "fails\n", pstat_Nset  );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Withdraw SLOG_pstat_t from the tmp_fbuf  */
    Nbytes_read = 0;

    ierr = fbuf_withdraw( &( slog->pstat->starttime ), SLOG_typesz[  ts  ],
                          1, tmp_fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - Cannot "
                          "withdraw the StartTime from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[  ts  ];

    ierr = fbuf_withdraw( &( slog->pstat->endtime ), SLOG_typesz[  ts  ],
                          1, tmp_fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - Cannot "
                          "withdraw the EndTime from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[  ts  ];

    for ( idx = 0; idx < slog->pstat->Nset; idx++ ) {
        ierr = SLOG_StatSet_WithdrawFromFbuf( slog->pstat->sets[ idx ],
                                              tmp_fbuf );
        if ( ierr == SLOG_FAIL ) {
            fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - Cannot "
                              "withdraw %d-th StatSet to the SLOG filebuffer, "
                              fmt_ui32" bytes read so far\n", idx,
                              Nbytes_read );
            fflush( errfile );
            return SLOG_FAIL;
        }
        Nbytes_read += ierr;
    }

    /*  File Buffer usage test  */
    if ( Nbytes_read != expected_bsize ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - \n"
                          "\t""Nbytes read("fmt_ui32") != expected_bsize "
                          "read("fmt_ui32")\n", Nbytes_read, expected_bsize );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Restore the saved file pointer position  */
    ierr = slog_fseek( slog->fd, file_loc_saved, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Read() - slog_fseek() "
                          "fails, cannot restore the orginal file position "
                          "before the end of the subroutine call\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    fbuf_free( tmp_fbuf );

    return SLOG_SUCCESS;
}



int SLOG_PSTAT_SetBinWidthInTime( SLOG_STREAM  *slog )
{
    SLOG_pstat_t  *pstat;
    SLOG_dir_t    *dir;

    if ( slog == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_SetBinWidthInTime() - "
                          "the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->pstat == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_SetBinWidthInTime() - "
                          "the input SLOG_pstat_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if (    ( slog->frame_dir ).Nframe > 0 
         && ( slog->frame_dir ).entries == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_SetBinWidthInTime() - "
                          "the input SLOG_dir_t is inconsistent!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( ( slog->frame_dir ).Nframe < 1 )
        return SLOG_SUCCESS;

    dir   = &( slog->frame_dir );
    pstat = slog->pstat;

    pstat->starttime = ( dir->entries[ 0 ] ).starttime;
    pstat->endtime   = ( dir->entries[ dir->Nframe-1 ] ).endtime;

    if ( pstat->Nbin > 0 )
        pstat->bin_width_in_time = ( pstat->endtime - pstat->starttime )
                                 / pstat->Nbin;
    else
        pstat->bin_width_in_time = ( pstat->endtime - pstat->starttime );

    return SLOG_SUCCESS;
}



static double div_quod_int( double numerator, double denominator,
                            int *quodient )
{
    double fval;
    int    iquodient;
    double remainder;

    fval      = numerator / denominator;
    iquodient = ( int ) fval;
    remainder = fval - ( double ) iquodient;

    *quodient = iquodient;
    return remainder;
}



static double SLOG_PSTAT_div( double  numerator,  double  denominator,
                              int     upperbound, int    *quodient )
{
    int    iquodient;
    double remainder;

    remainder = div_quod_int( numerator, denominator, &iquodient );
    if ( iquodient < 0 || iquodient > upperbound) {
#if defined( DEBUG )
        fprintf( errfile, __FILE__
                          ":SLOG_PSTAT_div() - Warning!! \n"
                          "\t""the computed integral quodient is "
                          "out of range\n"
                          "\t""Original  : iquodient = %d, remainder = %f,  "
                          "upperbound = %d\n",
                          iquodient, remainder, upperbound );
        fflush( errfile );
#endif

        if ( iquodient < 0 ) {
            remainder += iquodient;
            iquodient  = 0;
        }
        else {
            remainder += iquodient - upperbound;
            iquodient  = upperbound;
        }

#if defined( DEBUG )
        fprintf( errfile, "\t""Corrected : iquodient = %d, remainder = %f,  "
                          "upperbound = %d\n",
                          iquodient, remainder, upperbound );
        fflush( errfile );
#endif
    }   /*  Endof if ( iquodient < 0 || iquodient > upperbound )  */
    
    *quodient = iquodient;
    return remainder;
}



int SLOG_PSTAT_Update(       SLOG_pstat_t           *pstat,
                       const SLOG_intvlrec_blist_t  *src_bbuf )
{
    const SLOG_intvlrec_lptr_t  *lptr;
    const SLOG_intvlrec_t       *irec;
          SLOG_statset_t        *statset;
          SLOG_time              bin_width_in_time;
          SLOG_time              file_starttime;
          SLOG_uint32            last_bin_idx;
          SLOG_int32             set_idx;
          SLOG_int32             bin_idx;
          SLOG_time              adj_irec_starttime, adj_irec_endtime;
          SLOG_int32             beg_idx, end_idx;
          SLOG_statbintype       beg_rem, end_rem;

    if ( pstat == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Update() - "
                          "the input SLOG_pstat_t pointer is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    bin_width_in_time = pstat->bin_width_in_time;
    file_starttime    = pstat->starttime;
    last_bin_idx      = pstat->Nbin - 1;

    if ( bin_width_in_time <= 0.0 ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Update() - \n"
                          "\t""the input SLOG_pstat_t's bin_width_in_time = "
                          fmt_time"\n", bin_width_in_time );
        fflush( errfile );
        return SLOG_FAIL;
    }

    for ( lptr = src_bbuf->lhead; lptr != NULL; lptr = lptr->next ) {

        irec = lptr->irec;

#if defined( NO_ARROW_STAT )
        /*
           Exclude the Off-diagonal record, i.e. arrow, when counting
           the event for statistical preview
        */
        if ( ! SLOG_global_IsOffDiagRec( irec->rectype ) ) {
#endif

            set_idx = SLOG_PSTAT_GetSeqIdx( pstat, irec->intvltype );
            if ( set_idx != SLOG_FAIL ) {
                statset = pstat->sets[ set_idx ];

                adj_irec_starttime = irec->starttime - file_starttime;
                adj_irec_endtime   = adj_irec_starttime + irec->duration;
                beg_rem = SLOG_PSTAT_div( adj_irec_starttime,
                                          bin_width_in_time,
                                          last_bin_idx, &beg_idx );
                end_rem = SLOG_PSTAT_div( adj_irec_endtime,
                                          bin_width_in_time,
                                          last_bin_idx, &end_idx );
                    
                if ( end_idx > beg_idx ) {
                    statset->bins[ beg_idx ] += 1.0 - beg_rem;
                    for ( bin_idx = beg_idx + 1;
                          bin_idx < end_idx;
                          bin_idx++ )
                       statset->bins[ bin_idx ] += 1.0;
                    statset->bins[ end_idx ] += end_rem;
                }
                else if ( end_idx == beg_idx )
                    statset->bins[ beg_idx ] += end_rem - beg_rem;
                else {  /*  if ( end_idx < beg_idx )  */
                    fprintf( errfile, __FILE__
                                      ":SLOG_PSTAT_Update() - Warning!! \n"
                                      "\t""the computed BIN indexes are "
                                      "invalid, beg_bin_idx(%d) < "
                                      "end_bin_idx(%d)", beg_idx, end_idx );
                    fprintf( errfile, "\t""startime = "fmt_time",  "
                                      "endtime = "fmt_time"\n",
                                      pstat->starttime, pstat->endtime );
                    fprintf( errfile, "\t""irec = " );
                    SLOG_Irec_Print( irec, errfile );
                    fprintf( errfile, "\n" );
                    fflush( errfile );
                }
            } 
            else {
                fprintf( errfile, __FILE__":SLOG_PSTAT_Update() - Warning!! \n"
                                  "\t""the computed SET index to "
                                  "SLOG_pstat_t[] is out of range\n " );
                fprintf( errfile, "\t""irec = " );
                SLOG_Irec_Print( irec, errfile );
                fprintf( errfile, "\n" );
                fflush( errfile );
            }   /*  Endof if ( set_idx != SLOG_FAIL ) */

#if defined( NO_ARROW_STAT )
        }   /* Endof if ( ! SLOG_global_IsOffDiagRec( lptr->irec->rectype ) ) */
#endif

    }   /*  for ( lptr = src_bbuf->lhead; lptr != NULL; lptr = lptr->next )  */

    return SLOG_SUCCESS;
}



int SLOG_PSTAT_Close( SLOG_STREAM  *slog )
{
    int    ierr;

    if ( slog == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Close() - "
                          "the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->pstat == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PSTAT_Close() - "
                          "the input SLOG_pstat_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Write the updated statistics info onto the disk again  */
    ierr = SLOG_PSTAT_Write( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_CloseOutputStream() - "
                          "SLOG_PSTAT_Write( slog ) fails!!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    return SLOG_SUCCESS;
}



SLOG_uint32 SLOG_PSTAT_SizeOfStatSetInFile( const SLOG_uint32  pstat_Nbin  )
{
    return ( SLOG_typesz[ rtype_t ] + SLOG_typesz[ itype_t ]
           + sizeof( SLOG_uint16 )
           + SLOG_STRING_LEN * sizeof( char )
           + SLOG_typesz[ ui32 ]
           + pstat_Nbin      * SLOG_typesz[ sbin ] );
}



int SLOG_PSTAT_GetSeqIdx( const SLOG_pstat_t     *pstat,
                          const SLOG_intvltype_t  in_intvltype )
{
    int   idx;
    
    for ( idx = 0; idx < pstat->Nset; idx++ ) {
        if ( pstat->seq_idx_vals[ idx ] == ( SLOG_int32 ) in_intvltype )
            return idx;
    }

    return SLOG_FAIL;
}



void SLOG_PSTAT_Print( FILE *outfd, const SLOG_pstat_t  *pstat )
{
    int   idx;

    if ( pstat != NULL ) {
        fprintf( outfd, "Collect At : Starttime = "fmt_time", "
                        "Endtime = "fmt_time"\n\n",
                        pstat->starttime, pstat->endtime );
        for ( idx = 0; idx < pstat->Nset; idx++ ) {
            fprintf( outfd, "\t""Preview Statistics Set %d : \n", idx );
            SLOG_StatSet_Print( outfd, pstat->sets[ idx ] );
            fprintf( outfd, "\n" );
        }
        fprintf( outfd, "\n" );
        fflush( outfd );
    }
    else {
        fprintf( outfd, __FILE__":SLOG_PSTAT_Print() - "
                        "input pointer SLOG_pstat_t is NULL \n" );
        fflush( outfd );
    }
}



/*  -- Component level supporting subroutines  -- */

        /* SLOG_StatSet_xxxx methods */

SLOG_statset_t *SLOG_StatSet_Create( const SLOG_uint32 stat_set_size )
{
    SLOG_statset_t   *statset;
    SLOG_uint32       ii;

    if ( stat_set_size == 0 ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_Create() - Input set size, "
                          fmt_ui32", is zero\n", stat_set_size );
        fflush( errfile );
        return NULL;
    }

    statset = ( SLOG_statset_t * ) malloc ( sizeof( SLOG_statset_t ) );
    if ( statset == NULL ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_Create() - malloc() fails\n" );
        fflush( errfile );
        return NULL;
    }

    statset->Nbin = stat_set_size;
    statset->bins = ( SLOG_statbintype * )
                    malloc( statset->Nbin * sizeof( SLOG_statbintype ) );
    if ( statset->Nbin > 0 && statset->bins == NULL ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_Create() - \n"
                          "\t""malloc() fails for slog_pstat->bins[]\n" );
        fflush( errfile );
        return NULL;
    }

    /*  Initialize  */
    statset->rectype    = 0;
    statset->intvltype  = 0;
    for ( ii = 0; ii < statset->Nbin; ii++ )
        statset->bins[ ii ] = ( SLOG_statbintype ) 0.0;

    return statset;
}



void SLOG_StatSet_Free( SLOG_statset_t *statset )
{
    if ( statset != NULL ) {
        if ( statset->Nbin > 0 && statset->bins != NULL ) {
            free( statset->bins );
            statset->bins = NULL;
        }
        free( statset );
        statset = NULL;
    }
}



int SLOG_StatSet_SetRectype(       SLOG_statset_t   *statset,
                             const SLOG_rectype_t    in_rectype )
{
#if defined( DEBUG )
    if ( statset == NULL ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_SetRectype() - \n"
                          "\t""the input SLOG_statset_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    statset->rectype = in_rectype;

    return SLOG_SUCCESS;
}



int SLOG_StatSet_SetIntvltype(       SLOG_statset_t   *statset,
                               const SLOG_intvltype_t  in_intvltype )
{
#if defined( DEBUG )
    if ( statset == NULL ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_SetIntvltype() - \n"
                          "\t""the input SLOG_statset_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    statset->intvltype = in_intvltype;

    return SLOG_SUCCESS;
}



int SLOG_StatSet_SetLabel(       SLOG_statset_t   *statset,
                           const char             *in_label )
{
#if defined( DEBUG )
    if ( statset == NULL ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_SetLabel() - \n"
                          "\t""the input SLOG_statset_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    SLOG_str_ncopy_set( statset->label, in_label, SLOG_STRING_LEN );

    return SLOG_SUCCESS;
}



SLOG_uint32 SLOG_StatSet_ByteSizeInFile( const SLOG_statset_t   *statset )
{
    return ( SLOG_typesz[ rtype_t ] + SLOG_typesz[ itype_t ]
           + sizeof( SLOG_uint16 )
           + SLOG_STRING_LEN * sizeof( char )
           + SLOG_typesz[ ui32 ]
           + statset->Nbin   * SLOG_typesz[ sbin ] );
}



int SLOG_StatSet_DepositToFbuf( const SLOG_statset_t   *statset,
                                      filebuf_t        *fbuf )
{
    const char          newline = '\n';
          SLOG_uint32   Nbytes_written;
          SLOG_uint16   label_size;
          int           ierr;

    Nbytes_written = 0;

    ierr = fbuf_deposit( &( statset->rectype ), SLOG_typesz[ rtype_t ],
                         1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_DepositToFbuf() - Cannot "
                          "deposit the RECORD_TYPE to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ rtype_t ];

    ierr = fbuf_deposit( &( statset->intvltype ), SLOG_typesz[ itype_t ],
                         1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_DepositToFbuf() - Cannot "
                          "deposit the INTVL_TYPE to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ itype_t ];

    label_size = SLOG_STRING_LEN;
    ierr = fbuf_deposit( &label_size, sizeof( SLOG_uint16 ), 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_DepositToFbuf() - Cannot "
                          "deposit the LABEL SIZE to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * sizeof( SLOG_uint16 );

    ierr = fbuf_deposit( &( statset->label ), sizeof( char ),
                         SLOG_STRING_LEN-1, fbuf );
    if ( ierr < SLOG_STRING_LEN-1 ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_DepositToFbuf() - Cannot "
                          "deposit the LABEL to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * sizeof( char );

    ierr = fbuf_deposit( &newline, sizeof( char ), 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_DepositToFbuf() - Cannot "
                          "deposit the newline char to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * sizeof( char );

    ierr = fbuf_deposit( &( statset->Nbin ), SLOG_typesz[ ui32 ],
                         1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_DepositToFbuf() - Cannot "
                          "deposit the Nbin to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_deposit( statset->bins, SLOG_typesz[ sbin ],
                         statset->Nbin, fbuf );
    if ( ierr < statset->Nbin ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_DepositToFbuf() - Cannot "
                          "deposit the bins[0.."fmt_ui32"-1] to the SLOG "
                          "filebuffer, "fmt_ui32" bytes written so far\n",
                          statset->Nbin, Nbytes_written );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ sbin ];

    return Nbytes_written;
}



int SLOG_StatSet_WithdrawFromFbuf( SLOG_statset_t   *statset,
                                   filebuf_t        *fbuf )
{
          SLOG_uint32   Nbytes_read;
          SLOG_uint16   label_size;
          int           ierr;

    Nbytes_read = 0;

    ierr = fbuf_withdraw( &( statset->rectype ), SLOG_typesz[ rtype_t ],
                          1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_WithdrawFromFbuf() - Cannot "
                          "withdraw the RECORD_TYPE from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ rtype_t ];

    ierr = fbuf_withdraw( &( statset->intvltype ), SLOG_typesz[ itype_t ],
                          1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_WithdrawFromFbuf() - Cannot "
                          "withdraw the INTVL_TYPE from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ itype_t ];

    ierr = fbuf_withdraw( &label_size, sizeof( SLOG_uint16 ), 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_WithdrawToFbuf() - Cannot "
                          "withdraw the LABEL SIZE from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * sizeof( SLOG_uint16 );

    ierr = fbuf_withdraw( &( statset->label ), sizeof( char ),
                          label_size, fbuf );
    if ( ierr < label_size ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_WithdrawFromFbuf() - Cannot "
                          "withdraw the LABEL from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * sizeof( char );

    ierr = fbuf_withdraw( &( statset->Nbin ), SLOG_typesz[ ui32 ],
                          1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_StatSet_WithdrawFromFbuf() - Cannot "
                          "withdraw the Nbin from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_withdraw( statset->bins, SLOG_typesz[ sbin ],
                          statset->Nbin, fbuf );
    if ( ierr < statset->Nbin ) {
        fprintf( errfile, __FILE__
                          ":SLOG_StatSet_WithdrawFromFbuf() - Cannot "
                          "withdraw the bins[0.."fmt_ui32"-1] from the SLOG "
                          "filebuffer, "fmt_ui32" bytes read so far\n",
                          statset->Nbin, Nbytes_read );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ sbin ];

    return Nbytes_read;
}



void SLOG_StatSet_Print( FILE *outfd, const SLOG_statset_t  *statset )
{
    const   int   Nitems_per_line = 5;
            int   ii;

    fprintf( outfd, "\t""rectype = "fmt_rtype_t",  "
                    "intvltype = "fmt_itype_t"\n",
                    statset->rectype, statset->intvltype );
    fprintf( outfd, "\t""label = %s \n", statset->label );
    for ( ii = 0; ii < statset->Nbin; ii++ ) {
        fprintf( outfd, fmt_sbin", ", statset->bins[ ii ] );
        if ( ii % Nitems_per_line == Nitems_per_line - 1 )
            fprintf( outfd, "\n" );
    }
    fprintf( outfd, "\n" );
    fflush( outfd );
}
