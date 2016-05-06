#include <stdlib.h>
#include <stdio.h>
#include "bswp_fileio.h"
#include "slog_fileio.h"
#include "slog_header.h"
#include "slog_bebits.h"
#include "slog_impl.h"
#include "slog_recdefs.h"              /*I "slog_recdefs.h" I*/



int SLOG_RDEF_IsDuplicated( const SLOG_recdefs_table_t  *recdefs,
                            const SLOG_recdef_t         *in_def )
{
    SLOG_recdef_t  *cur_def;
    int             ii;
    int             NotMatched = 1;

    for ( ii = 0; NotMatched && (ii < recdefs->Nentries); ii++ ) {
        cur_def = &( recdefs->entries[ ii ] );
        NotMatched = ! SLOG_RecDef_IsKeyEqualTo( cur_def, in_def );
    }

    return ! NotMatched;
}


/*@C
    SLOG_RDEF_Open - Creates the SLOG Record Definition Table, 
                     SLOG_recdefs_table_t, needed to write the to
                     the disk.

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where SLOG Record Definition Table is
         located.

  Modified Output Variables :
. returned value - integer return status.

  Include file needed : 
  slog_recdefs.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_RDEF_Open( SLOG_STREAM  *slog )
{
    slog->rec_defs = ( SLOG_recdefs_table_t * )
                     malloc( sizeof( SLOG_recdefs_table_t ) );
    if ( slog->rec_defs == NULL ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_Open() - "
                          "malloc() fails, Cannot allocate memory for "
                          "SLOG_recdefs_table_t\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    slog->rec_defs->increment  = 10;
    slog->rec_defs->capacity   = slog->rec_defs->increment;
    slog->rec_defs->Nentries   = 0;
    slog->rec_defs->entries    = ( SLOG_recdef_t * )
                                 malloc(   slog->rec_defs->capacity
                                         * sizeof( SLOG_recdef_t ) );
    slog->rec_defs->file_loc   = SLOG_fptr_NULL;

    return SLOG_SUCCESS;
}



/*@C
    SLOG_RDEF_AddRecDef - Add one definition to the SLOG Record Definition
                          table.

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where SLOG Record Definition Table is
         located.

  Unmodified Input Variables :
. intvltype - label to the interval type.
. bebit_0 - the 1st bebit.
. bebit_1 - the 2nd bebit.
. Nassocs - Number of assocications for this interval record type.
. Nargs - Number of MPI call argument for this interval record type.

  Modified Output Variables :
. returned value - integer return status.

  Usage Notes on this subroutine :

.N SLOG_EXTRA_RECDEFS

  Include file needed : 
  slog_recdefs.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_RDEF_AddRecDef(       SLOG_STREAM          *slog,
                         const SLOG_intvltype_t      intvltype,
                         const SLOG_bebit_t          bebit_0,
                         const SLOG_bebit_t          bebit_1,
                         const SLOG_N_assocs_t       Nassocs,
                         const SLOG_N_args_t         Nargs )
{
    SLOG_uint32             idx;
    SLOG_recdefs_table_t   *recdefs;

    recdefs = slog->rec_defs;

#if defined( DEBUG )
    if ( recdefs == NULL ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_AddRecDef() - "
                          "the SLOG_recdefs_table_t pointer in slog "
                          "is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    /*  Check if there is any space left for a new record definition */
    if ( recdefs->Nentries + 1 > recdefs->capacity ) {
        recdefs->capacity += recdefs->increment;
        recdefs->entries = ( SLOG_recdef_t * )
                           realloc( recdefs->entries,
                                    recdefs->capacity * sizeof( SLOG_recdef_t )
                                  );
        if ( recdefs->entries == NULL ) {
            fprintf( errfile, __FILE__":SLOG_RDEF_AddRecDef() - "
                              "realloc() fails, cannot increase the size "
                              "of recdefs->entries[]\n" );
            fprintf( errfile, "\t""The input record definition = [ "fmt_itype_t
                              ", ("fmt_bebit_t", "fmt_bebit_t
                              "), "fmt_Nassocs_t", "fmt_Nargs_t" ] \n",
                              intvltype, bebit_0, bebit_1, Nassocs, Nargs );
            fflush( errfile );
            return SLOG_FAIL;
        }
    }

    
    idx = recdefs->Nentries;
    SLOG_RecDef_Assign( &( recdefs->entries[ idx ] ),
                        intvltype, bebit_0, bebit_1, Nassocs, Nargs );
    SLOG_RecDef_SetUsed( &( recdefs->entries[ idx ] ), SLOG_FALSE );
    if ( SLOG_RDEF_IsDuplicated( recdefs, &(recdefs->entries[ idx ]) ) ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_AddRecDef() - "
                          "there is a copy of input record\n " );
        fprintf( errfile, "\t""definition with identical keys "
                          "in the SLOG record definition table\n" );
        fprintf( errfile, "\t""The input record definition = [ "fmt_itype_t
                          ", ("fmt_bebit_t", "fmt_bebit_t
                          "), "fmt_Nassocs_t", "fmt_Nargs_t" ] \n",
                          intvltype, bebit_0, bebit_1, Nassocs, Nargs );
        fflush( errfile );
        return SLOG_FAIL;
    }
    recdefs->Nentries += 1;

    return SLOG_SUCCESS;
}


/*@C
    SLOG_RDEF_Close - Write the SLOG_recdefs_table_t onto the disk

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where SLOG Record Definition Table is
         located.

  Modified Output Variables :
. returned value - integer status code.

  Usage Notes on this subroutine :

.N SLOG_EXTRA_RECDEFS

  Include file needed : 
  slog_recdefs.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_RDEF_Close( SLOG_STREAM  *slog )
{
    SLOG_recdefs_table_t   *recdefs;
    int                     ierr;
    int                     ii;

    /*  Update the content of SLOG_hdr_t to both memory and disk  */
    slog->hdr->fptr2recdefs = slog_ftell( slog->fd );
    ierr = SLOG_WriteUpdatedHeader( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile,  __FILE__":SLOG_RDEF_Close() - "
                                   "SLOG_WriteUpdatedHeader() fails\n" );
        fflush( errfile );
        return ierr;
    }

    /*  Write the content of SLOG Record Definition Table to the disk  */
    recdefs = slog->rec_defs;
    
    ierr = bswp_fwrite( &( recdefs->Nentries ),
                        sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile,  __FILE__":SLOG_RDEF_Close() - \n"
                           "\t""fails at "
                           "writing the number of record definitions to "
                           "table section of logfile\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    for ( ii = 0; ii < recdefs->Nentries; ii++ ) {
        ierr = SLOG_RecDef_WriteToFile( &( recdefs->entries[ ii ] ),
                                        slog->fd );
        if ( ierr != SLOG_TRUE ) {
            fprintf( errfile,  __FILE__":SLOG_RDEF_Close() - \n"
                               "\t""SLOG_RecDef_WriteToFile() fails at "
                               "%d-th addition of the record definition to "
                               "table section of logfile\n", ii );
            fflush( errfile );
            return SLOG_FAIL;
        }
    }
  
    return SLOG_SUCCESS;
}



/*@C
    SLOG_RDEF_SetExtraNumOfRecDefs - Write the starting file pointer 
                                     SLOG_recdefs_table_t to the slog's
                                     header and then set the maximum 
                                     extra number of Record Definitions.

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where SLOG Record Definition Table is
         located.

  Unmodifued Input Variables :
. Nentries_extra - Number of extra record definitions to be reserved
                   on the disk

  Modified Output Variables :
. returned value - integer status code.

  Usage Notes on this subroutine :

.N SLOG_EXTRA_RECDEFS

  Include file needed : 
  slog_recdefs.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_RDEF_SetExtraNumOfRecDefs(       SLOG_STREAM  *slog,
                                    const SLOG_uint32   Nentries_extra )
{
    SLOG_recdefs_table_t   *recdefs;
    SLOG_recdef_t           blank_def = { 0, {0,0}, 0, 0, 0 };
    SLOG_uint32             old_recdefs_capacity;
    int                     ierr;
    int                     ii;

    /*  Update the content of SLOG_hdr_t to both memory and disk  */
    slog->hdr->fptr2recdefs = slog_ftell( slog->fd );
    ierr = SLOG_WriteUpdatedHeader( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile,  __FILE__":SLOG_RDEF_SetExtraNumOfRecDefs() - "
                                   "SLOG_WriteUpdatedHeader() fails\n" );
        fflush( errfile );
        return ierr;
    }

    recdefs = slog->rec_defs;
    
    /*  Determine the new storage  */
    old_recdefs_capacity = recdefs->capacity;
    recdefs->capacity    = recdefs->Nentries + Nentries_extra;
    recdefs->entries = ( SLOG_recdef_t * )
                       realloc( recdefs->entries,
                                recdefs->capacity * sizeof( SLOG_recdef_t )
                              );
    if ( recdefs->entries == NULL ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_SetExtraNumOfRecDefs() - "
                          "realloc() fails, cannot increase the size "
                          "of recdefs->entries["fmt_ui32"]\n",
                          recdefs->capacity );
        fflush( errfile );
        recdefs->capacity = old_recdefs_capacity;
        return SLOG_FAIL;
    }

    /*  Write the content of SLOG Record Definition Table to the disk  */

    ierr = bswp_fwrite( &( recdefs->Nentries ),
                        sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile,  __FILE__":SLOG_RDEF_SetExtraNumOfRecDefs() - \n"
                           "\t""fails at "
                           "writing the number of record definitions to "
                           "table section of logfile\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    for ( ii = 0; ii < recdefs->Nentries; ii++ ) {
        ierr = SLOG_RecDef_WriteToFile( &( recdefs->entries[ ii ] ), 
                                        slog->fd );
        if ( ierr != SLOG_TRUE ) {
            fprintf( errfile,  __FILE__":SLOG_RDEF_SetExtraNumOfRecDefs() - \n"
                               "\t""SLOG_RecDef_WriteToFile() fails at the "
                               "%d-th addition of the record definition to "
                               "table section of logfile\n", ii );
            fflush( errfile ); 
            return SLOG_FAIL;
        }
    }

    /*  Save the location where the extra record definition can be inserted  */

    recdefs->file_loc = slog_ftell( slog->fd ); 

    /*  Write some blank record definitions to fill up the reserved space.  */

    for ( ii = recdefs->Nentries; ii < recdefs->capacity; ii++ ) {
        ierr = SLOG_RecDef_WriteToFile( &blank_def, slog->fd );
        if ( ierr != SLOG_TRUE ) {
            fprintf( errfile,  __FILE__":SLOG_RDEF_SetExtraNumOfRecDefs() - \n"
                               "\t""SLOG_RecDef_WriteToFile() fails at "
                               "%d-th addition of the blank definition to "
                               "table section of logfile\n", ii );
            fflush( errfile ); 
            return SLOG_FAIL;
        }
    }
  
    return SLOG_SUCCESS;
}



/*@C
    SLOG_RDEF_AddExtraRecDef - Add one extra definition to the reserved
                               space of SLOG Record Definition table.

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where SLOG Record Definition Table is
         located.

  Unmodified Input Variables :
. intvltype - label to the interval type.
. bebit_0 - the 1st bebit.
. bebit_1 - the 2nd bebit.
. Nassocs - Number of assocications for this interval record type.
. Nargs - Number of MPI call argument for this interval record type.

  Modified Output Variables :
. returned value - integer return status.

  Usage Notes on this subroutine :

.N SLOG_EXTRA_RECDEFS

  Include file needed : 
  slog_recdefs.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_RDEF_AddExtraRecDef(       SLOG_STREAM          *slog,
                              const SLOG_intvltype_t      intvltype,
                              const SLOG_bebit_t          bebit_0,
                              const SLOG_bebit_t          bebit_1,
                              const SLOG_N_assocs_t       Nassocs,
                              const SLOG_N_args_t         Nargs )
{
    SLOG_recdefs_table_t   *recdefs;
    SLOG_fptr               file_loc_saved;
    SLOG_uint32             idx;
    int                     ierr;

    recdefs = slog->rec_defs;

#if defined( DEBUG )
    if ( recdefs == NULL ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_AddExtraRecDef() - "
                          "the SLOG_recdefs_table_t pointer in slog "
                          "is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    if ( recdefs->Nentries + 1 > recdefs->capacity ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_AddExtraRecDef() - \n"
                          "\t""All reserved space in the "
                          "Record Definition Table of the logfile "
                          "has been used up!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    idx = recdefs->Nentries;
    SLOG_RecDef_Assign( &( recdefs->entries[ idx ] ),
                        intvltype, bebit_0, bebit_1, Nassocs, Nargs );
    SLOG_RecDef_SetUsed( &( recdefs->entries[ idx ] ), SLOG_FALSE );
    if ( SLOG_RDEF_IsDuplicated( recdefs, &(recdefs->entries[ idx ]) ) ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_AddExtraRecDef() - "
                          "there is a copy of input record\n " );
        fprintf( errfile, "\t""definition with identical keys "
                          "in the SLOG record definition table\n" );
        fprintf( errfile, "\t""The input record definition = [ "fmt_itype_t
                          ", ("fmt_bebit_t", "fmt_bebit_t
                          "), "fmt_Nassocs_t", "fmt_Nargs_t" ] \n",
                          intvltype, bebit_0, bebit_1, Nassocs, Nargs );
        fflush( errfile );
        return SLOG_FAIL;
    }
    recdefs->Nentries += 1;

    /*  Save the current position in the logfile for later restoration  */
    file_loc_saved = slog_ftell( slog->fd );

    /*  Update the number of record definitions on the file/disk  */
    ierr = slog_fseek( slog->fd, slog->hdr->fptr2recdefs, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_AddExtraRecDef() - "
                          "slog_fseek( fptr2recdefs ) fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    ierr = bswp_fwrite( &( recdefs->Nentries ),
                        sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile,  __FILE__":SLOG_RDEF_AddExtraRecDef() - \n"
                           "\t""Update the number of entries in the record "
                           "definition table on the file fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Insert the new record definition to the reserved space  */
    ierr = slog_fseek( slog->fd, recdefs->file_loc, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_AddExtraRecDef() - "
                          "slog_fseek( recdefs->file_loc ) fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    ierr = SLOG_RecDef_WriteToFile( &( recdefs->entries[idx] ), slog->fd );
    if ( ierr != SLOG_TRUE ) {
        fprintf( errfile,  __FILE__":SLOG_RDEF_AddExtraRecDef() - \n"
                           "\t""SLOG_RecDef_WriteToFile() fails at "
                           "%d-th addition of the record definition to "
                           "table section of logfile\n", idx );
        fflush( errfile );
        return SLOG_FAIL;
    }
    fflush( slog->fd );

    /*  Update the file pointer to the reserved space of RecDefs in file */
    recdefs->file_loc = slog_ftell( slog->fd );

    /*  Restore the original file position in the logfile  */
    ierr = slog_fseek( slog->fd, file_loc_saved, SEEK_SET );

    return SLOG_SUCCESS;
}



SLOG_recdef_t *SLOG_RDEF_GetRecDef(       SLOG_recdefs_table_t   *recdefs,
                                    const SLOG_intvltype_t        intvltype,
                                    const SLOG_bebit_t            bebit_0,
                                    const SLOG_bebit_t            bebit_1 )
{
    SLOG_recdef_t  *cur_def;
    int             ii;

    for ( ii = 0; ii < recdefs->Nentries; ii++ ) {
        cur_def = &( recdefs->entries[ ii ] );
        if (    ( cur_def->intvltype == intvltype )
             && ( cur_def->bebits[0] == bebit_0 )
             && ( cur_def->bebits[1] == bebit_1 ) ) {
             return cur_def;
        }
    }

    fprintf( errfile, __FILE__":SLOG_RDEF_GetRecDef() - \n"
                      "\t""Cannot find [ intvltype = "fmt_itype_t", ("
                      fmt_bebit_t", "fmt_bebit_t") ] "
                      "in the Record Definition Table\n",
                      intvltype, bebit_0, bebit_1 );
    fflush( errfile );
    return NULL;
}



int SLOG_RDEF_CompressTable( SLOG_recdefs_table_t *recdefs )
{
    SLOG_recdef_t          *entries_used;
    SLOG_recdef_t          *recdef;
    SLOG_uint32             Nentries_used;
    SLOG_uint32             idx, ii;

#if defined( DEBUG )
    if ( recdefs == NULL ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_CompressTable() - "
                          "the SLOG_recdefs_table_t pointer in slog "
                          "is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    /*  Compute the Number of __used__ entries in the record def table  */
    Nentries_used = 0;
    for ( ii = 0; ii < recdefs->Nentries; ii++ )
         if ( ( recdefs->entries[ ii ] ).used )
             Nentries_used++;

    if ( Nentries_used > recdefs->Nentries ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_CompressTable() - \n"
                          "\t""Nentries_used("fmt_ui32") > "
                          "recdefs->Nentries("fmt_ui32
                          ") Inconsistency detected ! \n",
                          Nentries_used, recdefs->Nentries );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( Nentries_used == recdefs->Nentries )
        return SLOG_SUCCESS;

    /*  Allocate the temporary storage for the used definition  */
    entries_used = ( SLOG_recdef_t * ) malloc(   Nentries_used
                                               * sizeof( SLOG_recdef_t ) );
    if ( Nentries_used > 0 && entries_used == NULL ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_CompressTable() - \n"
                          "\t""malloc() for temporary storage, "
                          "entries_used, fails for Nentries_used = "
                          fmt_ui32" ! \n", Nentries_used );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Copy the used record definitions to the temporary storage  */
    idx = 0;
    for ( ii = 0; ii < recdefs->Nentries; ii++ ) {
        if ( ( recdefs->entries[ ii ] ).used ) {
            recdef = &( recdefs->entries[ ii ] );
            SLOG_RecDef_Assign( &( entries_used[ idx ] ),
                                recdef->intvltype,
                                recdef->bebits[0], recdef->bebits[1],
                                recdef->Nassocs, recdef->Nargs );
            idx++;
        }
    }

    /*  Throw away the original record definition table  */
    if ( recdefs->Nentries > 0 && recdefs->entries != NULL ) {
        free( recdefs->entries );
        recdefs->entries    = NULL;
        recdefs->capacity   = 0;
        recdefs->Nentries   = 0;
    }

    /*  
        Shrink the original table in memory to store the used definitions 
        by pointing the record definition table to the temporary storage.
    */
    recdefs->capacity   = Nentries_used;
    recdefs->Nentries   = Nentries_used;
    recdefs->entries    = entries_used;

    return SLOG_SUCCESS;
}



/*
    Update the disk content of record definition table to reflect
    the compression done on the table in memory
*/
int SLOG_RDEF_CompressedTableToFile( SLOG_STREAM  *slog )
{
    SLOG_recdefs_table_t   *recdefs;
    SLOG_fptr               file_loc_saved;
    SLOG_uint32             ii;
    int                     ierr;

    recdefs = slog->rec_defs;

#if defined( DEBUG )
    if ( recdefs == NULL ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_CompressedTableToFile() - "
                          "the SLOG_recdefs_table_t pointer in slog "
                          "is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    /*  Save the current position in the logfile for later restoration  */
    file_loc_saved = slog_ftell( slog->fd );

    /*  Update the number of record definitions on the file/disk  */
    ierr = slog_fseek( slog->fd, slog->hdr->fptr2recdefs, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_CompressedTableToFile() - \n"
                          "\t""slog_fseek( fptr2recdefs ) fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    ierr = bswp_fwrite( &( recdefs->Nentries ),
                        sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_CompressedTableToFile() - \n"
                          "\t""Update the number of entries in the record "
                          "definition table on the file fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    for ( ii = 0; ii < recdefs->Nentries; ii++ ) {
        ierr = SLOG_RecDef_WriteToFile( &( recdefs->entries[ ii ] ),
                                        slog->fd );
        if ( ierr != SLOG_TRUE ) {
            fprintf( errfile,  __FILE__":SLOG_RDEF_CompressedTableToFile() - \n"
                               "\t""SLOG_RecDef_WriteToFile() fails at "
                               "%d-th addition of the record definition to "
                               "table section of logfile\n", ii );
            fflush( errfile );
            return SLOG_FAIL;
        }
    }

    /*  Probably unnecessary  */
    /*  Update the file pointer to the reserved space of RecDefs in file */
    recdefs->file_loc = slog_ftell( slog->fd );

    /*  Restore the original file position in the logfile  */
    ierr = slog_fseek( slog->fd, file_loc_saved, SEEK_SET );

    return SLOG_SUCCESS;
}



void SLOG_RDEF_Free( SLOG_recdefs_table_t *recdefs )
{
    if ( recdefs != NULL ) {
        if ( recdefs->Nentries > 0 && recdefs->entries != NULL ) {
            free( recdefs->entries );
            recdefs->entries = NULL;
        }
        free( recdefs );
        recdefs = NULL;
    }
}



int SLOG_RDEF_ReadRecDefs( SLOG_STREAM  *slog )
{
    SLOG_recdefs_table_t   *recdefs;
    int                     ii;
    int                     ierr;

    slog->rec_defs = ( SLOG_recdefs_table_t * )
                     malloc( sizeof( SLOG_recdefs_table_t ) );
    if ( slog->rec_defs == NULL ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_ReadRecDefs() - "
                          "malloc() fails, Cannot allocate memory for "
                          "SLOG_recdefs_table_t\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Initialize the Record Definition table data structure  */
    slog->rec_defs->increment  = 10;
    slog->rec_defs->capacity   = slog->rec_defs->increment;
    slog->rec_defs->Nentries   = 0;
    slog->rec_defs->entries    = NULL;
    
    /*  Seek to the beginning of the Record Def'n Table in the SLOG file  */
    if ( slog->hdr->fptr2recdefs == SLOG_fptr_NULL ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_ReadRecDefs() - "
                          "slog->hdr->fptr2recdefs == SLOG_fptr_NULL\n" );
        fprintf( errfile, "\t""SLOG_RDEF_Close() may NOT be called\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    ierr = slog_fseek( slog->fd, slog->hdr->fptr2recdefs, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_ReadRecDefs() - "
                          "slog_fseek( fptr2recdefs ) fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Read in the data structure  */
    recdefs = slog->rec_defs;
    
    ierr = bswp_fread( &( recdefs->Nentries ),
                       sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_ReadRecDefs() - \n"
                          "\t""Fails at reading the number of entries "
                          "of the record definition table\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    recdefs->capacity = recdefs->Nentries;
    recdefs->entries  = ( SLOG_recdef_t * )
                        malloc( recdefs->capacity * sizeof( SLOG_recdef_t ) );
    if ( recdefs->Nentries > 0 && recdefs->entries == NULL ) {
        fprintf( errfile, __FILE__":SLOG_RDEF_ReadRecDefs() - "
                          "malloc() fails for recdefs->entries[]\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    
    for ( ii = 0; ii < recdefs->Nentries; ii++ ) {
        ierr = SLOG_RecDef_ReadFromFile( &( recdefs->entries[ ii ] ), 
                                         slog->fd );
        if ( ierr != SLOG_TRUE ) {
            fprintf( errfile, __FILE__":SLOG_RDEF_ReadRecDefs() - \n"
                              "\t""SLOG_RecDef_ReadFromFile() fails at "
                              "%d-th reading of record definition from "
                              "the table section of logfile\n", ii );
            fflush( errfile ); 
            return SLOG_FAIL;
        }
    }
  
    return SLOG_SUCCESS;
}



void SLOG_RDEF_Print( FILE* fd, const SLOG_recdefs_table_t *recdefs )
{
    int ii;

    for ( ii = 0; ii < recdefs->Nentries; ii++ ) {
        fprintf( fd, " def[%i] = ", ii );
        SLOG_RecDef_Print( &( recdefs->entries[ ii ] ), fd );
        fprintf( fd, "\n" );
    }
    fflush( fd );
}



/*  -- Component level supporting subroutines  -- */

        /* SLOG_RecDef_xxxx methods */


void SLOG_RecDef_Assign(       SLOG_recdef_t        *recdef,
                         const SLOG_intvltype_t      in_intvltype,
                         const SLOG_bebit_t          in_bebit_0,
                         const SLOG_bebit_t          in_bebit_1,
                         const SLOG_N_assocs_t       in_Nassocs,
                         const SLOG_N_args_t         in_Nargs )
{
    recdef->intvltype = in_intvltype;
    recdef->bebits[0] = in_bebit_0;
    recdef->bebits[1] = in_bebit_1;
    recdef->Nassocs   = in_Nassocs;
    recdef->Nargs     = in_Nargs;
}



void SLOG_RecDef_Print( const SLOG_recdef_t *def, FILE *fd )
{
    fprintf( fd, "[ "fmt_itype_t", ("fmt_bebit_t", "fmt_bebit_t"), "
                 fmt_Nassocs_t", "fmt_Nargs_t" ]", 
                 def->intvltype, def->bebits[0], def->bebits[1],
                 def->Nassocs, def->Nargs );
}



int SLOG_RecDef_WriteToFile( const SLOG_recdef_t *def, FILE *fd )
{
    SLOG_bebits_t           bebits;
    int                     ierr;

    ierr = bswp_fwrite( &( def->intvltype ),
                        sizeof( SLOG_intvltype_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    bebits = SLOG_bebits_encode( def->bebits[0], def->bebits[1] );
    ierr = bswp_fwrite( &( bebits ),
                        sizeof( SLOG_bebits_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    ierr = bswp_fwrite( &( def->Nassocs ),
                        sizeof( SLOG_N_assocs_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    ierr = bswp_fwrite( &( def->Nargs ),
                        sizeof( SLOG_N_args_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    return SLOG_TRUE;
}



int SLOG_RecDef_ReadFromFile( SLOG_recdef_t *def, FILE *fd )
{
    SLOG_bebits_t           bebits;
    int                     ierr;

    ierr = bswp_fread( &( def->intvltype ),
                       sizeof( SLOG_intvltype_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    ierr = bswp_fread( &( bebits ),
                       sizeof( SLOG_bebits_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;
    SLOG_bebits_decode( bebits, def->bebits );

    ierr = bswp_fread( &( def->Nassocs ),
                       sizeof( SLOG_N_assocs_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    ierr = bswp_fread( &( def->Nargs ),
                       sizeof( SLOG_N_args_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    return SLOG_TRUE;
}



int SLOG_RecDef_IsKeyEqualTo( const SLOG_recdef_t *def1,
                              const SLOG_recdef_t *def2 )
{
    return (    ( def1->intvltype == def2->intvltype )
             && ( def1->bebits[0] == def2->bebits[0] )
             && ( def1->bebits[1] == def2->bebits[1] ) );
}

int SLOG_RecDef_IsValueEqualTo( const SLOG_recdef_t *def1,
                                const SLOG_recdef_t *def2 )
{
    return (    ( def1->Nassocs == def2->Nassocs )
             && ( def1->Nargs   == def2->Nargs ) );
}

int SLOG_RecDef_IsEqualTo( const SLOG_recdef_t *def1,
                           const SLOG_recdef_t *def2 )
{
    return (    SLOG_RecDef_IsKeyEqualTo( def1, def2 )
             && SLOG_RecDef_IsValueEqualTo( def1, def2 ) );
}



/*  Looks up the total number of associations for given record type  */
/*  SLOG_N_assocs_t  */
SLOG_int32
SLOG_RecDef_NumOfAssocs( const SLOG_recdefs_table_t   *recdefs,
                         const SLOG_intvltype_t        intvltype,
                         const SLOG_bebit_t            bebit_0,
                         const SLOG_bebit_t            bebit_1 )
{
    SLOG_recdef_t  *cur_def;
    int             ii;

    for ( ii = 0; ii < recdefs->Nentries; ii++ ) {
        cur_def = &( recdefs->entries[ ii ] );
        if (    ( cur_def->intvltype == intvltype )
             && ( cur_def->bebits[0] == bebit_0 )
             && ( cur_def->bebits[1] == bebit_1 ) )
            return cur_def->Nassocs;
    }

    fprintf( errfile, __FILE__":SLOG_RecDef_NumOfAssocs() - "
                      "Cannot find [ intvltype = "fmt_itype_t", ("
                      fmt_bebit_t", "fmt_bebit_t") ] "
                      "in the Record Definition Table\n",
                      intvltype, bebit_0, bebit_1 );
    fflush( errfile );
    return SLOG_FAIL;
}



/*  Looks up the number of all the MPI arguments for given record type  */
/*  SLOG_N_args_t  */
SLOG_int32
SLOG_RecDef_NumOfArgs( const SLOG_recdefs_table_t   *recdefs,
                       const SLOG_intvltype_t        intvltype,
                       const SLOG_bebit_t            bebit_0,
                       const SLOG_bebit_t            bebit_1 )
{
    SLOG_recdef_t  *cur_def;
    int             ii;

    for ( ii = 0; ii < recdefs->Nentries; ii++ ) {
        cur_def = &( recdefs->entries[ ii ] );
        if (    ( cur_def->intvltype == intvltype )
             && ( cur_def->bebits[0] == bebit_0 )
             && ( cur_def->bebits[1] == bebit_1 ) )
            return cur_def->Nargs;
    }

    fprintf( errfile, __FILE__":SLOG_RecDef_NumOfArgs() - "
                      "Cannot find [ intvltype = "fmt_itype_t", ("
                      fmt_bebit_t", "fmt_bebit_t") ] "
                      "in the Record Definition Table\n",
                      intvltype, bebit_0, bebit_1 );
    fflush( errfile );
    return SLOG_FAIL;
}




/*
   This routine can be modified to give the maximum possible size for
   the corresponding interval record, even for __variable__ interval record.
   if the N_args column in Record Definition Table is modified to give
   the maximum estimated size of the set of vector arguments.
*/
SLOG_int32
SLOG_RecDef_SizeOfIrecInFile( const SLOG_recdefs_table_t   *recdefs,
                              const SLOG_rectype_t          rectype, 
                              const SLOG_intvltype_t        intvltype,
                              const SLOG_bebit_t            bebit_0,
                              const SLOG_bebit_t            bebit_1 )
{
    int              sizeof_irec;    
    SLOG_N_assocs_t  N_assocs;
    SLOG_N_args_t    N_args;

    if ( SLOG_global_IsVarRec( rectype ) ) {
        fprintf( errfile, __FILE__":SLOG_RecDef_SizeOfIrecInFile() - \n" );
        fprintf( errfile, "\t""the input rectype, "fmt_rtype_t", is variable "
                          "interval record\n", rectype );
        fflush( errfile );
        return SLOG_FAIL;
    }

    N_assocs = SLOG_RecDef_NumOfAssocs( recdefs, intvltype, bebit_0, bebit_1 );
    N_args   = SLOG_RecDef_NumOfArgs( recdefs, intvltype, bebit_0, bebit_1 );
    sizeof_irec = SLOG_typesz[ min_IntvlRec ]
                + N_assocs * sizeof( SLOG_assoc_t )
                + N_args * sizeof( SLOG_arg_t );

    if ( SLOG_global_IsOffDiagRec( rectype ) )
        sizeof_irec += SLOG_typesz[ taskID_t ];

    return sizeof_irec;
}



/*  boolean_flag can be either SLOG_TRUE or SLOG_FALSE  */
void SLOG_RecDef_SetUsed(       SLOG_recdef_t *def,
                          const int            boolean_flag )
{
    if ( def != NULL )
        def->used = boolean_flag;
}
