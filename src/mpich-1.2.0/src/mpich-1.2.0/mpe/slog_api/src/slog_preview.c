#include <stdlib.h>
#include <stdio.h>
#include "bswp_fileio.h"
#include "slog_fileio.h"
#include "slog_header.h"
#include "slog_preview.h"

/*  Copy the content of the one stream to another  */
static int filecopy( FILE *infd, FILE *outfd )
{
    int           character;
    unsigned int  count;

    count = 0;
    while ( ( character = fgetc( infd ) ) != EOF ) {
        fputc( character, outfd );
        count++;
    }

    return count;
}



/*@C
    SLOG_SetPreviewName - Set the filename of the to be copied Preview file
                          and Copy the file's content to the SLOG file

  Unmodified Input parameter:
. pview_filename - pointer to the characters of the Preview's filename.

  Modified Input parameter :
. slog - pointer to the SLOG_STREAM where interval records are stored.

  Modified Output parameter :
. returned value - integer return status code.

  Include file need:
  slog_preview.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_SetPreviewName( SLOG_STREAM *slog, const char *pview_filename )
{
    int        ierr;

    if ( slog == NULL ) { 
        fprintf( errfile, __FILE__":SLOG_SetPreviewName() - "
                          "the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->pview != NULL ) {
        fprintf( errfile, __FILE__":SLOG_SetPreviewName() - "
                          "the SLOG_STREAM's Preview pointer is NOT NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( pview_filename == NULL ) {
        fprintf( errfile, __FILE__":SLOG_SetPreviewName() - "
                          "the input filename is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    slog->pview = SLOG_PVIEW_Create( pview_filename, NULL );

    ierr = SLOG_PVIEW_Write( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_SetPreviewName() - "
                                  "SLOG_PVIEW_Write() fails!\n" );
        fflush( errfile );
        return ierr;
    }

    return SLOG_SUCCESS;
}



SLOG_pview_t *SLOG_PVIEW_Create( const char *pview_filename,
                                 const char *pview_data )
{
    SLOG_pview_t *slog_pview;

    slog_pview = ( SLOG_pview_t * ) malloc( sizeof( SLOG_pview_t ) );
    if ( slog_pview == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PVIEW_Create() - malloc() fails\n" );
        fflush( errfile );
        return NULL;
    }

    if ( pview_filename != NULL ) {
        slog_pview->Nchar    = strlen( pview_filename );
        slog_pview->filename = ( char * ) malloc( ( slog_pview->Nchar + 1 )
                                                  * sizeof( char ) );
        strcpy( slog_pview->filename, pview_filename );
    }
    else {
        slog_pview->Nchar    = 0;
        slog_pview->filename = NULL;
    }

    if ( pview_data != NULL ) {
        slog_pview->Nbytes   = strlen( pview_data );
        slog_pview->data     = ( char * ) malloc( ( slog_pview->Nbytes + 1 )
                                                  * sizeof( char ) );
        strcpy( slog_pview->data, pview_data );
    }
    else {
        slog_pview->Nbytes   = 0;
        slog_pview->data     = NULL;
    }

    return slog_pview;
}



void SLOG_PVIEW_Free( SLOG_pview_t  *slog_pview )
{
    if ( slog_pview != NULL ) {
        if ( slog_pview->Nchar > 0 && slog_pview->filename != NULL ) {
            free( slog_pview->filename );
            slog_pview->filename = NULL;
        }
        if ( slog_pview->Nbytes > 0 && slog_pview->data != NULL ) {
           free ( slog_pview->data );
           slog_pview->data   = NULL;
           slog_pview->Nbytes = 0;
        }
        free( slog_pview );
        slog_pview = NULL;
    }
}



int SLOG_PVIEW_Write( SLOG_STREAM *slog )
{
    FILE         *slog_preview;     /* File descriptor for the SLOG preview */
    SLOG_fptr     file_loc_saved;
    SLOG_uint32   Nchar;
    int           ierr;

    if ( slog == NULL ) { 
        fprintf( errfile, __FILE__":SLOG_PVIEW_Write() - "
                          "the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->hdr == NULL ) { 
        fprintf( errfile, __FILE__":SLOG_PVIEW_Write() - "
                          "the input SLOG_hdr_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->pview == NULL ) {
        fprintf( errfile, __FILE__":SLOG_PVIEW_Write() - "
                          "the input SLOG_pview_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Update the content of SLOG_hdr_t to both memory and disk  */
    slog->hdr->fptr2preview = slog_ftell( slog->fd );
    ierr = SLOG_WriteUpdatedHeader( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile,  __FILE__":SLOG_PVIEW_Write() - "
                           "SLOG_WriteUpdatedHeader() fails\n" );
        fflush( errfile );
        return ierr;
    }

    /*  Write the content of SLOG Profile to the disk  */
    if ( ( slog_preview = fopen( slog->pview->filename, "r" ) ) == NULL ){
        fprintf( errfile, __FILE__":SLOG_PVIEW_Write() - file %s "
                          "cannot be opened for reading\n",
                          slog->pview->filename );
        fflush( errfile );
        return SLOG_FAIL;
    }

    file_loc_saved = slog_ftell( slog->fd );
    Nchar = 0;
    bswp_fwrite( &Nchar, SLOG_typesz[ ui32 ], 1, slog->fd );

    Nchar = filecopy( slog_preview, slog->fd );
    fclose( slog_preview );

    slog->file_loc = slog_ftell( slog->fd );
    slog_fseek( slog->fd, file_loc_saved, SEEK_SET );
    bswp_fwrite( &Nchar, SLOG_typesz[ ui32 ], 1, slog->fd );
    slog_fseek( slog->fd, slog->file_loc, SEEK_SET );

    return SLOG_SUCCESS;
}



int SLOG_PVIEW_Read( SLOG_STREAM *slog )
{
    int           ierr;

    if ( slog == NULL ) { 
        fprintf( errfile, __FILE__":SLOG_PVIEW_Read() - "
                          "the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->hdr == NULL ) { 
        fprintf( errfile, __FILE__":SLOG_PVIEW_Read() - "
                          "the input SLOG_hdr_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Seek to the beginning of the Profile in the SLOG file  */
    if ( slog->hdr->fptr2preview == SLOG_fptr_NULL ) {
        fprintf( errfile, __FILE__":SLOG_PVIEW_Read() - "
                          "slog->hdr->fptr2preview == SLOG_fptr_NULL\n" );
        fprintf( errfile, "\t""SLOG_PVIEW_Write() may NOT be called\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    ierr = slog_fseek( slog->fd, slog->hdr->fptr2preview, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_PVIEW_Read() - "
                          "slog_fseek( fptr2preview ) fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->pview == NULL )
        slog->pview = SLOG_PVIEW_Create( NULL, NULL );

    /*  Read in the Profile  */
    ierr = bswp_fread( &( slog->pview->Nbytes ),
                       SLOG_typesz[ ui32 ], 1, slog->fd );
    if ( ierr != 1 ) {
        fprintf( errfile,  __FILE__":SLOG_PVIEW_Read() - "
                           "Reading number of characters fails\n" );
        fflush( errfile );
        return SLOG_EOF;
    }

    slog->pview->data = ( char * ) malloc( ( slog->pview->Nbytes+1 )
                                         * sizeof( char ) );
    if ( slog->pview->data == NULL ) {
        fprintf( errfile,  __FILE__":SLOG_PVIEW_Read() - "
                          "malloc() fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    ierr = bswp_fread( slog->pview->data, 1, slog->pview->Nbytes, slog->fd );
    if ( ierr != slog->pview->Nbytes ) {
        fprintf( errfile,  __FILE__":SLOG_PVIEW_Read() - "
                           "Reading data section of Preview fails\n" );
        fflush( errfile );
        return SLOG_EOF;
    }

    slog->pview->data[ slog->pview->Nbytes ] = '\0';

#if defined( DEBUG )
    fprintf( outfile, __FILE__":SLOG_PVIEW_Read() - Read :\n" );
    SLOG_PVIEW_Print( outfile, slog->pview );
    fprintf( outfile, "\n" );
    fflush( outfile );
#endif

    return SLOG_SUCCESS;
}



void SLOG_PVIEW_Print( FILE* fd, const SLOG_pview_t *slog_pview )
{
    fprintf( fd, "%s\n", slog_pview->data );
}
