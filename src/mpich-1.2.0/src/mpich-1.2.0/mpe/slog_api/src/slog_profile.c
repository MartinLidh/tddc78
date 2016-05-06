#include <stdlib.h>
#include <stdio.h>
#include "str_util.h"
#include "bswp_fileio.h"
#include "slog_fileio.h"
#include "slog_header.h"
#include "slog_profile.h"              /*I "slog_profile.h" I*/



int SLOG_PROF_IsDuplicated( const SLOG_prof_t       *profile,
                            const SLOG_intvlinfo_t  *in_info )
{
    SLOG_intvlinfo_t  *cur_info;
    int                ii;
    int                NotMatched = 1;

    for ( ii = 0; NotMatched && (ii < profile->Nentries); ii++ ) {
        cur_info = &( profile->entries[ ii ] );
        NotMatched = ! SLOG_IntvlInfo_IsKeyEqualTo( cur_info, in_info );
    }

    return ! NotMatched;
}


/*@C
    SLOG_PROF_Open - Creates the SLOG Display Profile, 
                     SLOG_prof_t, needed to write the to
                     the disk.

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where SLOG Display Profile is
         located.

  Modified Output Variables :
. returned value - integer return status.

  Include file needed : 
  slog_profile.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_PROF_Open( SLOG_STREAM  *slog )
{
    slog->prof = ( SLOG_prof_t * ) malloc( sizeof( SLOG_prof_t ) );
    if ( slog->prof == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PROF_Open() - "
                          "malloc() fails, Cannot allocate memory for "
                          "SLOG_prof_t\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    slog->prof->increment  = 10;
    slog->prof->capacity   = slog->prof->increment;
    slog->prof->Nentries   = 0;
    slog->prof->entries    = ( SLOG_intvlinfo_t * )
                               malloc(   slog->prof->capacity
                                       * sizeof( SLOG_intvlinfo_t ) );
    slog->prof->file_loc   = SLOG_fptr_NULL;

    return SLOG_SUCCESS;
}



/*@C
    SLOG_PROF_AddIntvlInfo - Add one display description of an interval to 
                             the SLOG Display Profile table.

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where SLOG Display Profile Table is
         located.

  Unmodified Input Variables :
. intvltype - index to the interval type.
. classtype - character string for the classtype of the interval
. label     - character string for the label of the interval
. color     - character string for the color of the interval

  Modified Output Variables :
. returned value - integer return status.

  Usage Notes on this subroutine :
     all the input characters strings, classtype, label and color are
     allowed to contain alphanumeric, underscore, blank and tab characters.
     But tab characters, if exist, in the string are all converted to
     blanks for ease of management.

.N SLOG_EXTRA_INTERVAL_INFOS

  Include file needed : 
  slog_profile.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_PROF_AddIntvlInfo(       SLOG_STREAM      *slog,
                            const SLOG_intvltype_t  intvltype,
                            const char *            classtype,
                            const char *            label,
                            const char *            color )
{
    SLOG_prof_t   *profile;
    SLOG_uint32    idx;

    profile = slog->prof;

#if defined( DEBUG )
    if ( profile == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PROF_AddIntvlInfo() - "
                          "the SLOG_prof_t pointer in slog is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    /*  Check if there is any space left for a new record definition */
    if ( profile->Nentries + 1 > profile->capacity ) {
        profile->capacity += profile->increment;
        profile->entries   = ( SLOG_intvlinfo_t * )
                             realloc( profile->entries,
                                      profile->capacity
                                    * sizeof( SLOG_intvlinfo_t )
                                    );
        if ( profile->entries == NULL ) {
            fprintf( errfile, __FILE__":SLOG_PROF_AddIntvlInfo() - "
                              "realloc() fails, cannot increase the size "
                              "of profile->entries[]\n" );
            fprintf( errfile, "\t""The input Interval Info = [ "
                              fmt_itype_t", %s, %s, %s ] \n",
                              intvltype, classtype, label, color );
            fflush( errfile );
            return SLOG_FAIL;
        }
    }

    
    idx = profile->Nentries;
    SLOG_IntvlInfo_Assign( &( profile->entries[ idx ] ),
                           intvltype, classtype, label, color );
    SLOG_IntvlInfo_SetUsed( &( profile->entries[ idx ] ), SLOG_FALSE );
    if ( SLOG_PROF_IsDuplicated( profile, &(profile->entries[ idx ]) ) ) {
        fprintf( errfile, __FILE__":SLOG_PROF_AddIntvlInfo() - \n"
                          "\t""there is already a copy of input interval\n" );
        fprintf( errfile, "\t""information with identical keys "
                          "in the SLOG Display Profile table\n" );
        fprintf( errfile, "\t""The input Interval Info = [ "
                          fmt_itype_t", %s, %s, %s ]\n",
                          intvltype, classtype, label, color );
        fflush( errfile );
        return SLOG_FAIL;
    }
    profile->Nentries += 1;

    return SLOG_SUCCESS;
}


/*@C
    SLOG_PROF_Close - Write the SLOG_prof_t onto the disk

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where SLOG Display Profile Table is
         located.

  Modified Output Variables :
. returned value - integer status code.

  Usage Notes on this subroutine :

.N SLOG_EXTRA_INTERVAL_INFOS

  Include file needed : 
  slog_profile.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_PROF_Close( SLOG_STREAM  *slog )
{
    SLOG_prof_t            *profile;
    int                     ierr;
    int                     ii;

    /*  Update the content of SLOG_hdr_t to both memory and disk  */
    slog->hdr->fptr2profile = slog_ftell( slog->fd );
    ierr = SLOG_WriteUpdatedHeader( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile,  __FILE__":SLOG_PROF_Close() - "
                                   "SLOG_WriteUpdatedHeader() fails\n" );
        fflush( errfile );
        return ierr;
    }

    /*  Write the content of SLOG Record Definition Table to the disk  */
    profile = slog->prof;
    
    ierr = bswp_fwrite( &( profile->Nentries ),
                        sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile,  __FILE__":SLOG_PROF_Close() - \n"
                           "\t""fails at "
                           "writing the number of interval infos to "
                           "table section of logfile\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    for ( ii = 0; ii < profile->Nentries; ii++ ) {
        ierr = SLOG_IntvlInfo_WriteToFile( &( profile->entries[ ii ] ),
                                        slog->fd );
        if ( ierr != SLOG_TRUE ) {
            fprintf( errfile,  __FILE__":SLOG_PROF_Close() - \n"
                               "\t""SLOG_IntvlInfo_WriteToFile() fails at "
                               "%d-th addition of the interval infos to "
                               "table section of logfile\n", ii );
            fflush( errfile );
            return SLOG_FAIL;
        }
    }
  
    return SLOG_SUCCESS;
}



/*@C
    SLOG_PROF_SetExtraNumOfIntvlInfos - Write the starting file pointer 
                                        SLOG_prof_t to the slog's
                                        header and then set the maximum
                                        extra number of Interval 
                                        Description entries
                                        in Display Profile Table.

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where SLOG Display Profile Table is
         located.

  Unmodifued Input Variables :
. Nentries_extra - Number of extra interval description to be reserved
                   on the disk

  Modified Output Variables :
. returned value - integer status code.

  Usage Notes on this subroutine :

.N SLOG_EXTRA_RECDEFS

  Include file needed : 
  slog_profile.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_PROF_SetExtraNumOfIntvlInfos(       SLOG_STREAM  *slog,
                                       const SLOG_uint32   Nentries_extra )
{
    /*
         fill in an unrealistic number for SLOG_uint16, 2^(16)-1, in
         intvltype of blank_info which is type SLOG_intvlinfo_t
    */
    SLOG_prof_t            *profile;
    SLOG_intvlinfo_t        blank_info = { 65535, " ", " ", " " };
    SLOG_uint32             old_profile_capacity;
    int                     ierr;
    int                     ii;

    /*  Update the content of SLOG_hdr_t to both memory and disk  */
    slog->hdr->fptr2profile = slog_ftell( slog->fd );
    ierr = SLOG_WriteUpdatedHeader( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile,  __FILE__":SLOG_PROF_SetExtraNumOfIntvlInfos() - "
                                   "SLOG_WriteUpdatedHeader() fails\n" );
        fflush( errfile );
        return ierr;
    }

    profile = slog->prof;
    
    /*  Determine the new storage  */
    old_profile_capacity = profile->capacity;
    profile->capacity    = profile->Nentries + Nentries_extra;
    profile->entries = ( SLOG_intvlinfo_t * )
                       realloc( profile->entries,
                                profile->capacity * sizeof( SLOG_intvlinfo_t )
                              );
    if ( profile->entries == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PROF_SetExtraNumOfIntvlInfos() - \n"
                          "\t""realloc() fails, cannot increase the size "
                          "of profile->entries["fmt_ui32"]\n",
                          profile->capacity );
        fflush( errfile );
        profile->capacity = old_profile_capacity;
        return SLOG_FAIL;
    }

    /*  Write the content of SLOG Record Definition Table to the disk  */

    ierr = bswp_fwrite( &( profile->Nentries ),
                        sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile,  __FILE__":SLOG_PROF_SetExtraNumOfIntvlInfos() - \n"
                           "\t""fails at "
                           "writing the number of interval descriptors to "
                           "table section of logfile\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    for ( ii = 0; ii < profile->Nentries; ii++ ) {
        ierr = SLOG_IntvlInfo_WriteToFile( &( profile->entries[ ii ] ), 
                                        slog->fd );
        if ( ierr != SLOG_TRUE ) {
            fprintf( errfile,  __FILE__
                               ":SLOG_PROF_SetExtraNumOfIntvlInfos() - \n"
                               "\t""SLOG_IntvlInfo_WriteToFile() fails at the "
                               "%d-th addition of the interval descriptors to "
                               "table section of logfile\n", ii );
            fflush( errfile ); 
            return SLOG_FAIL;
        }
    }

    /*
        Save the location where the extra interval descriptors
        can be inserted
    */

    profile->file_loc = slog_ftell( slog->fd ); 

    /*  Write some blank record definitions to fill up the reserved space.  */

    for ( ii = profile->Nentries; ii < profile->capacity; ii++ ) {
        ierr = SLOG_IntvlInfo_WriteToFile( &blank_info, slog->fd );
        if ( ierr != SLOG_TRUE ) {
            fprintf( errfile,  __FILE__
                               ":SLOG_PROF_SetExtraNumOfIntvlInfos() - \n"
                               "\t""SLOG_IntvlInfo_WriteToFile() fails at "
                               "%d-th addition of the blank descriptor to "
                               "table section of logfile\n", ii );
            fflush( errfile ); 
            return SLOG_FAIL;
        }
    }
  
    return SLOG_SUCCESS;
}



/*@C
    SLOG_PROF_AddExtraIntvlInfo - Add one display description of an
                                  interval to the reserved space of
                                  the SLOG Display Profile table.

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where SLOG Display Profile Table is
         located.

  Unmodified Input Variables :
. intvltype - index to the interval type.
. classtype - character string for the classtype of the interval
. label     - character string for the label of the interval
. color     - character string for the color of the interval

  Modified Output Variables :
. returned value - integer return status.

  Usage Notes on this subroutine :
     all the input characters strings, classtype, label and color are
     allowed to contain alphanumeric, underscore, blank and tab characters.
     But tab characters, if exist, in the string are all converted to
     blanks for ease of management.

.N SLOG_EXTRA_INTERVAL_INFOS

  Include file needed : 
  slog_profile.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_PROF_AddExtraIntvlInfo(       SLOG_STREAM      *slog,
                                 const SLOG_intvltype_t  intvltype,
                                 const char *            classtype,
                                 const char *            label,
                                 const char *            color )
{
    SLOG_prof_t            *profile;
    SLOG_fptr               file_loc_saved;
    SLOG_uint32             idx;
    int                     ierr;

    profile = slog->prof;

#if defined( DEBUG )
    if ( profile == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PROF_AddExtraIntvlInfo() - "
                          "the SLOG_prof_t pointer in slog "
                          "is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    if ( profile->Nentries + 1 > profile->capacity ) {
        fprintf( errfile, __FILE__":SLOG_PROF_AddExtraIntvlInfo() - \n"
                          "\t""All reserved space in the "
                          "Record Definition Table of the logfile "
                          "has been used up!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    idx = profile->Nentries;
    SLOG_IntvlInfo_Assign( &( profile->entries[ idx ] ),
                           intvltype, classtype, label, color );
    SLOG_IntvlInfo_SetUsed( &( profile->entries[ idx ] ), SLOG_FALSE );
    if ( SLOG_PROF_IsDuplicated( profile, &(profile->entries[ idx ]) ) ) {
        fprintf( errfile, __FILE__":SLOG_PROF_AddExtraIntvlInfo() - "
                          "there is already a copy of input interval\n " );
        fprintf( errfile, "\t""information with identical keys "
                          "in the SLOG record definition table\n" );
        fprintf( errfile, "\t""The input Interval Info = [ "
                          fmt_itype_t", %s, %s, %s ]\n",
                          intvltype, classtype, label, color );
        fflush( errfile );
        return SLOG_FAIL;
    }
    profile->Nentries += 1;

    /*  Save the current position in the logfile for later restoration  */
    file_loc_saved = slog_ftell( slog->fd );

    /*  Update the number of interval descriptors on the file/disk  */
    ierr = slog_fseek( slog->fd, slog->hdr->fptr2profile, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_PROF_AddExtraIntvlInfo() - "
                          "slog_fseek( fptr2profile ) fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    ierr = bswp_fwrite( &( profile->Nentries ),
                        sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile,  __FILE__":SLOG_PROF_AddExtraIntvlInfo() - \n"
                           "\t""Update the number of entries in the record "
                           "definition table on the file fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Insert the new record definition to the reserved space  */
    ierr = slog_fseek( slog->fd, profile->file_loc, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_PROF_AddExtraIntvlInfo() - "
                          "slog_fseek( profile->file_loc ) fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    ierr = SLOG_IntvlInfo_WriteToFile( &( profile->entries[idx] ), slog->fd );
    if ( ierr != SLOG_TRUE ) {
        fprintf( errfile,  __FILE__":SLOG_PROF_AddExtraIntvlInfo() - \n"
                           "\t""SLOG_IntvlInfo_WriteToFile() fails at "
                           "%d-th addition of the interval decriptor to "
                           "table section of logfile\n", idx );
        fflush( errfile );
        return SLOG_FAIL;
    }
    fflush( slog->fd );

    /*  Update the file pointer to the reserved space of IntvlInfos in file */
    profile->file_loc = slog_ftell( slog->fd );

    /*  Restore the original file position in the logfile  */
    ierr = slog_fseek( slog->fd, file_loc_saved, SEEK_SET );

    return SLOG_SUCCESS;
}



SLOG_intvlinfo_t *SLOG_PROF_GetIntvlInfo(       SLOG_prof_t       *profile,
                                          const SLOG_intvltype_t   intvltype )
{
    SLOG_intvlinfo_t  *cur_def;
    int                ii;

    for ( ii = 0; ii < profile->Nentries; ii++ ) {
        cur_def = &( profile->entries[ ii ] );
        if ( cur_def->intvltype == intvltype ) {
             return cur_def;
        }
    }

    fprintf( errfile, __FILE__":SLOG_PROF_GetIntvlInfo() - \n"
                      "\t""Cannot find [ intvltype = "fmt_itype_t" ] "
                      "in the Display Profile Table\n", intvltype );
    fflush( errfile );
    return NULL;
}



int SLOG_PROF_CompressTable( SLOG_prof_t *profile )
{
    SLOG_intvlinfo_t       *entries_used;
    SLOG_intvlinfo_t       *def;
    SLOG_uint32             Nentries_used;
    SLOG_uint32             idx, ii;

#if defined( DEBUG )
    if ( profile == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PROF_CompressTable() - "
                          "the SLOG_prof_t pointer in slog "
                          "is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    /*  Compute the Number of __used__ entries in the display profile table  */
    Nentries_used = 0;
    for ( ii = 0; ii < profile->Nentries; ii++ )
         if ( ( profile->entries[ ii ] ).used )
             Nentries_used++;

    if ( Nentries_used > profile->Nentries ) {
        fprintf( errfile, __FILE__":SLOG_PROF_CompressTable() - \n"
                          "\t""Nentries_used("fmt_ui32") > "
                          "profile->Nentries("fmt_ui32
                          ") Inconsistency detected ! \n",
                          Nentries_used, profile->Nentries );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( Nentries_used == profile->Nentries )
        return SLOG_SUCCESS;

    /*  Allocate the temporary storage for the used definition  */
    entries_used = ( SLOG_intvlinfo_t * )
                   malloc( Nentries_used * sizeof( SLOG_intvlinfo_t ) );
    if ( Nentries_used > 0 && entries_used == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PROF_CompressTable() - \n"
                          "\t""malloc() for temporary storage, "
                          "entries_used, fails for Nentries_used = "
                          fmt_ui32" ! \n", Nentries_used );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Copy the used display profile definitions to the temporary storage  */
    idx = 0;
    for ( ii = 0; ii < profile->Nentries; ii++ ) {
        if ( ( profile->entries[ ii ] ).used ) {
            def = &( profile->entries[ ii ] );
            SLOG_IntvlInfo_Assign( &( entries_used[ idx ] ), def->intvltype,
                                   def->classtype, def->label, def->color );
            idx++;
        }
    }

    /*  Throw away the original display profile table  */
    if ( profile->Nentries > 0 && profile->entries != NULL ) {
        free( profile->entries );
        profile->entries    = NULL;
        profile->capacity   = 0;
        profile->Nentries   = 0;
    }

    /*  
        Shrink the original table in memory to store the used definitions 
        by pointing the display profile table to the temporary storage.
    */
    profile->capacity   = Nentries_used;
    profile->Nentries   = Nentries_used;
    profile->entries    = entries_used;

    return SLOG_SUCCESS;
}



int SLOG_PROF_CompressedTableToFile( SLOG_STREAM  *slog )
{
    SLOG_prof_t            *profile;
    SLOG_fptr               file_loc_saved;
    SLOG_uint32             ii;
    int                     ierr;

    profile = slog->prof;

#if defined( DEBUG )
    if ( profile == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PROF_CompressedTableToFile() - "
                          "the SLOG_prof_t pointer in slog "
                          "is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    /*  Save the current position in the logfile for later restoration  */
    file_loc_saved = slog_ftell( slog->fd );

    /*  Update the number of record definitions on the file/disk  */
    ierr = slog_fseek( slog->fd, slog->hdr->fptr2profile, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_PROF_CompressedTableToFile() - \n"
                          "\t""slog_fseek( fptr2profile ) fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    ierr = bswp_fwrite( &( profile->Nentries ),
                        sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_PROF_CompressedTableToFile() - \n"
                          "\t""Update the number of entries in the profile "
                          "definition table on the file fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    for ( ii = 0; ii < profile->Nentries; ii++ ) {
        ierr = SLOG_IntvlInfo_WriteToFile( &( profile->entries[ ii ] ),
                                           slog->fd );
        if ( ierr != SLOG_TRUE ) {
            fprintf( errfile,  __FILE__":SLOG_PROF_CompressedTableToFile() - \n"
                               "\t""SLOG_IntvlInfo_WriteToFile() fails at "
                               "%d-th addition of the record definition to "
                               "table section of logfile\n", ii );
            fflush( errfile );
            return SLOG_FAIL;
        }
    }

    /*  Probably unnecessary  */
    /*  Update the file pointer to the reserved space of RecDefs in file */
    profile->file_loc = slog_ftell( slog->fd );

    /*  Restore the original file position in the logfile  */
    ierr = slog_fseek( slog->fd, file_loc_saved, SEEK_SET );

    return SLOG_SUCCESS;
}



void SLOG_PROF_Free( SLOG_prof_t *profile )
{
    if ( profile != NULL ) {
        if ( profile->Nentries > 0 && profile->entries != NULL ) {
            free( profile->entries );
            profile->entries = NULL;
        }
        free( profile );
        profile = NULL;
    }
}



int SLOG_PROF_ReadIntvlInfos( SLOG_STREAM  *slog )
{
    SLOG_prof_t            *profile;
    int                     ii;
    int                     ierr;

    slog->prof = ( SLOG_prof_t * ) malloc( sizeof( SLOG_prof_t ) );
    if ( slog->prof == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PROF_ReadIntvlInfos() - "
                          "malloc() fails, Cannot allocate memory for "
                          "SLOG_prof_t\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Initialize the Record Definition table data structure  */
    slog->prof->increment  = 10;
    slog->prof->capacity   = slog->prof->increment;
    slog->prof->Nentries   = 0;
    slog->prof->entries    = NULL;
    
    /*  Seek to the beginning of the Record Def'n Table in the SLOG file  */
    if ( slog->hdr->fptr2profile == SLOG_fptr_NULL ) {
        fprintf( errfile, __FILE__":SLOG_PROF_ReadIntvlInfos() - "
                          "slog->hdr->fptr2profile == SLOG_fptr_NULL\n" );
        fprintf( errfile, "\t""SLOG_PROF_Close() may NOT be called\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    ierr = slog_fseek( slog->fd, slog->hdr->fptr2profile, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_PROF_ReadIntvlInfos() - "
                          "slog_fseek( fptr2profile ) fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Read in the data structure  */
    profile = slog->prof;
    
    ierr = bswp_fread( &( profile->Nentries ),
                       sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_PROF_ReadIntvlInfos() - \n"
                          "\t""Fails at reading the number of entries "
                          "of the display profile table\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    profile->capacity = profile->Nentries;
    profile->entries  = ( SLOG_intvlinfo_t * )
                        malloc( profile->capacity
                              * sizeof( SLOG_intvlinfo_t ) );
    if ( profile->Nentries > 0 && profile->entries == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PROF_ReadIntvlInfos() - "
                          "malloc() fails for profile->entries[]\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    
    for ( ii = 0; ii < profile->Nentries; ii++ ) {
        ierr = SLOG_IntvlInfo_ReadFromFile( &( profile->entries[ ii ] ), 
                                            slog->fd );
        if ( ierr != SLOG_TRUE ) {
            fprintf( errfile, __FILE__":SLOG_PROF_ReadIntvlInfos() - \n"
                              "\t""SLOG_IntvlInfo_ReadFromFile() fails at "
                              "%d-th reading of interval decriptor from "
                              "the table section of logfile\n", ii );
            fflush( errfile ); 
            return SLOG_FAIL;
        }
    }

#if defined( DEBUG )
    fprintf( outfile, __FILE__":SLOG_PROF_ReadIntvlInfos() - Read\n" );
    SLOG_PROF_Print( outfile, profile );
    fflush( outfile );
#endif
  
    return SLOG_SUCCESS;
}



void SLOG_PROF_Print( FILE* fd, const SLOG_prof_t *profile )
{
    int ii;

    for ( ii = 0; ii < profile->Nentries; ii++ ) {
        fprintf( fd, " def[%i] = ", ii );
        SLOG_IntvlInfo_Print( &( profile->entries[ ii ] ), fd );
        fprintf( fd, "\n" );
    }
    fflush( fd );
}



/*  -- Component level supporting subroutines  -- */

        /* SLOG_IntvlInfo_xxxx methods */

void SLOG_IntvlInfo_Assign(       SLOG_intvlinfo_t        *intvlinfo,
                            const SLOG_intvltype_t         in_intvltype,
                            const char *                   in_classtype,
                            const char *                   in_label,
                            const char *                   in_color )
{
    intvlinfo->intvltype = in_intvltype;
    SLOG_str_ncopy_set( intvlinfo->classtype, in_classtype, SLOG_STRING_LEN );
    SLOG_str_ncopy_set( intvlinfo->label,     in_label,     SLOG_STRING_LEN );
    SLOG_str_ncopy_set( intvlinfo->color,     in_color,     SLOG_STRING_LEN );
}



void SLOG_IntvlInfo_Print( const SLOG_intvlinfo_t *info, FILE *fd )
{
    fprintf( fd, "[ " fmt_itype_t ", %s, %s, %s ]",
                 info->intvltype, info->classtype, info->label, info->color );
}



int SLOG_IntvlInfo_WriteToFile( SLOG_intvlinfo_t *info, FILE *fd )
{
    const char       delimiter = '\t';
    const char       newline   = '\n';
          char       intvltype_str[SLOG_STRING_LEN] = "                     ";
          int        lgth;
          int        ierr;

    /*  snprintf() counts '\0' in the copied char length parameter  */
    /*
    ierr = bswp_fwrite( &( info->intvltype ),
                        sizeof( SLOG_intvltype_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;
    ierr = snprintf( intvltype_str,
                     lgth, "%u", info->intvltype );
    */

    lgth = SLOG_STRING_LEN - 2;

    sprintf( intvltype_str, "%u", info->intvltype );
    if ( intvltype_str[ lgth+1 ] != ' ' ) {
        fprintf( errfile, __FILE__":SLOG_IntvlInfo_WriteToFile() - \n"
                          "\t""intvltype of input SLOG_intvlinfo_t is "
                          "too large to fit into %d characters, intvltype "
                          "= "fmt_itype_t"\n", lgth, info->intvltype );
        fflush( errfile ); 
        return SLOG_FALSE;
    }
    
    intvltype_str[ lgth+1 ] = delimiter;
    ierr = bswp_fwrite( intvltype_str,
                        sizeof( char ), SLOG_STRING_LEN, fd );
    if ( ierr < SLOG_STRING_LEN ) return SLOG_FALSE;

    info->classtype[ lgth+1 ] = delimiter;
    ierr = bswp_fwrite( &( info->classtype ),
                        sizeof( char ), SLOG_STRING_LEN, fd );
    if ( ierr < SLOG_STRING_LEN ) return SLOG_FALSE;

    info->label[ lgth+1 ] = delimiter;
    ierr = bswp_fwrite( &( info->label ),
                        sizeof( char ), SLOG_STRING_LEN, fd );
    if ( ierr < SLOG_STRING_LEN ) return SLOG_FALSE;

    info->color[ lgth+1 ] = delimiter;
    ierr = bswp_fwrite( &( info->color ),
                        sizeof( char ), SLOG_STRING_LEN, fd );
    if ( ierr < SLOG_STRING_LEN ) return SLOG_FALSE;

    ierr = bswp_fwrite( &newline, sizeof( char ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    return SLOG_TRUE;
}



int SLOG_IntvlInfo_ReadFromFile( SLOG_intvlinfo_t *info, FILE *fd )
{
          char          newline = '\n';
          char          classtype_str[ SLOG_STRING_LEN ];
          int           ierr;

    /*
    ierr = bswp_fread( &( info->intvltype ),
                       sizeof( SLOG_intvltype_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;
    */
    ierr = bswp_fread( classtype_str,
                       sizeof( char ), SLOG_STRING_LEN, fd );
    if ( ierr < SLOG_STRING_LEN ) return SLOG_FALSE;
    info->intvltype = atoi( classtype_str );

    ierr = bswp_fread( &( info->classtype ),
                       sizeof( char ), SLOG_STRING_LEN, fd );
    if ( ierr < SLOG_STRING_LEN ) return SLOG_FALSE;

    ierr = bswp_fread( &( info->label ),
                       sizeof( char ), SLOG_STRING_LEN, fd );
    if ( ierr < SLOG_STRING_LEN ) return SLOG_FALSE;

    ierr = bswp_fread( &( info->color ),
                       sizeof( char ), SLOG_STRING_LEN, fd );
    if ( ierr < SLOG_STRING_LEN ) return SLOG_FALSE;

    ierr = bswp_fread( &newline, sizeof( char ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    return SLOG_TRUE;
}



int SLOG_IntvlInfo_IsKeyEqualTo( const SLOG_intvlinfo_t *info1,
                                 const SLOG_intvlinfo_t *info2 )
{
    return ( info1->intvltype == info2->intvltype );
}

int SLOG_IntvlInfo_IsValueEqualTo( const SLOG_intvlinfo_t *info1,
                                   const SLOG_intvlinfo_t *info2 )
{
    return (    !strcmp( info1->classtype, info2->classtype )
             && !strcmp( info1->label,     info2->label )
             && !strcmp( info1->color,     info2->color ) );
}

int SLOG_IntvlInfo_IsEqualTo( const SLOG_intvlinfo_t *info1,
                              const SLOG_intvlinfo_t *info2 )
{
    return (    SLOG_IntvlInfo_IsKeyEqualTo( info1, info2 )
             && SLOG_IntvlInfo_IsValueEqualTo( info1, info2 ) );
}


void SLOG_IntvlInfo_SetUsed(       SLOG_intvlinfo_t *info,
                             const int               boolean_flag )
{
    if ( info != NULL )
        info->used = boolean_flag;
}
