/* clog.c */
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "mpeconf.h"
#include "clogimpl.h"

char   CLOG_outdir[CLOG_DIR_LEN];
int    CLOG_status = 2;	/* initialized to CLOG not init, but logging on */
int    CLOG_Comm;		/* Default communicator */
void   *CLOG_ptr;		/* pointer into buffer, where next rec goes */
void   *CLOG_block_end;		/* pointer to end of buffer */
CLOG_BLOCK *CLOG_first, *CLOG_currbuff; /* blocks of buffer */
int    CLOG_intsperdouble, CLOG_charsperdouble;
int    CLOG_srcid = 900;	/* next id for source code location */
int    CLOG_nextevent = CLOG_MAXEVENT;
int    CLOG_nextstate = CLOG_MAXSTATE;
char   CLOG_execname[256];	/* name used for naming logfile (executable) */
char   CLOG_tmpfilename[30];    /* temp log file name (abhi) */ 
int    CLOG_tempFD = -5;        /* temp log file descriptor (abhi) */
double *out_buffer, *left_buffer,
       *right_buffer;           /* buffers for clog-merge (abhi) */ 
int    CLOG_num_blocks = 0;
static int me;                  /* process id of this process required for
				   error messages (abhi)*/
long   event_count = 0;         /* (abhi) to count number of events */
void   *slog_buffer;            /* (abhi) to allocate memory for SLOG */

/*@
    CLOG_Init - Initialize for CLOG logging
@*/
void CLOG_Init( void )
{
    int one = 1;
    PMPI_Comm_rank(MPI_COMM_WORLD, &me); /* (abhi) */
    CLOG_Comm      = 0;		/* until we pass communicator in to CLOG_Init*/
    CLOG_timeinit();		/* initialize timer */
    CLOG_init_buffers();        /*(abhi)*/
    CLOG_status	  &= 0x01;	/* set initialized  */
    CLOG_intsperdouble  = sizeof(double) / sizeof(int);
    CLOG_charsperdouble = sizeof(double) / sizeof(char);
    CLOG_init_tmpfilename();    /*(abhi)*/
}

/*@
    CLOG_init_buffers - initialize necessary buffers for clog logging.
@*/
void CLOG_init_buffers( void )
{
    int process_id = me ;
    
    if(me == 0) {
        slog_buffer   = (double *) MALLOC ( SLOG_MEMORY_REQUIREMENT*1024 ); 
	/*
	  (abhi) logging type isnt known yet. It is assumed to be SLOG 
	  and hence the extra memory is allocated to make sure CLOG2SLOG
	  works fine. Since only the process with rank 0 will be converting
	  CLOG records to SLOG records we only need to allocate memory 
	  there. This memory will be deallocated at the beginning of 
	  the merge process.
	*/
    }
    left_buffer   = (double *) MALLOC ( CLOG_BLOCK_SIZE );
    right_buffer  = (double *) MALLOC ( CLOG_BLOCK_SIZE );
    out_buffer    = (double *) MALLOC ( CLOG_BLOCK_SIZE );
    CLOG_newbuff(&CLOG_first);	/* get first buffer */
    if((!left_buffer) || (!right_buffer) || (!out_buffer) || (!CLOG_first)) {
        fprintf(stderr, "Unable to allocate memory for logging at process %d\n",
		process_id);
	fflush(stderr);
	PMPI_Abort(MPI_COMM_WORLD, 1);
    }
}


/*@
    CLOG_Finalize - Finalize  CLOG logging
@*/
void CLOG_Finalize( void )
{
    CLOG_LOGENDLOG();
}

/*@
    CLOG_newbuff - get and initialize new block of buffer

Input Parameter:

. bufptr - pointer to be filled in with address of new block

@*/
void CLOG_newbuff( CLOG_BLOCK **bufptr )
{
    if(CLOG_num_blocks == MAX_CLOG_BLOCKS)
      CLOG_temp_log();
    else if(CLOG_tempFD == -5) {
      *bufptr = (CLOG_BLOCK *) MALLOC ( sizeof (CLOG_BLOCK) );
      if(*bufptr == NULL)
	CLOG_temp_log();
      else {
	(*bufptr)->next = NULL;
	CLOG_currbuff = *bufptr;
      }
    }
    else {
      /*bufptr = (*bufptr)->next;*/
      if(*bufptr == NULL) 
	CLOG_temp_log();
      else
	CLOG_currbuff = *bufptr;
    }	
    CLOG_num_blocks++;
    /*CLOG_currbuff = *bufptr;*/
    CLOG_ptr = CLOG_currbuff->data;
    CLOG_block_end = (void *) ((char *) CLOG_ptr + CLOG_BLOCK_SIZE);
}

/*@
    CLOG_temp_log - dump buffers into temporary log file.
@*/
void CLOG_temp_log( void )
{
    int return_code;
    CLOG_BLOCK *buffer_parser;

    if(CLOG_tempFD == -5) {
      CLOG_tempFD = open(CLOG_tmpfilename, O_RDWR|O_CREAT|O_TRUNC, 0600);
      if(CLOG_tempFD == -1) {
	fprintf(stderr, "Unable to open temporary log file %s\n",
		CLOG_tmpfilename);
	fflush(stderr);
	PMPI_Abort(MPI_COMM_WORLD, 1);
      }
    }
    buffer_parser = CLOG_first;
    while((buffer_parser) && (CLOG_num_blocks--)) {
      return_code = write(CLOG_tempFD, buffer_parser, sizeof (CLOG_BLOCK) );
      if(return_code != sizeof (CLOG_BLOCK)) {
	fprintf(stderr, "Unable to write to temporary log file %s\n",
		CLOG_tmpfilename);
	fflush(stderr);
	PMPI_Abort(MPI_COMM_WORLD, 1);
      }
      buffer_parser = buffer_parser->next;
    }
    CLOG_currbuff = CLOG_first;
    CLOG_num_blocks = 0;
}
/*@ 
    CLOG_init_tmpfilename - initializes the temporary logfile name
    to "/tmp/temp_clogXXXXXX" (XXXXXX = process id) so as to keep
    it unique of anyother file.
@*/
void CLOG_init_tmpfilename( void )
{

    char number_array[] = "0123456789",
         temp_array[20];
    int process_id,
        remainder,
        ii,jj;

    CLOG_tmpfilename[0] = '\0';
    strcat(CLOG_tmpfilename,"/tmp/temp_clog");
/*
    strcat(CLOG_tmpfilename,"/sandbox/chan/temp_clog");
*/
    process_id = getpid();
    ii=0;
    while(process_id > 0) {
      temp_array[ii++] = number_array[process_id % 10];
      process_id = process_id/10;
    }
    temp_array[ii] = '\0';
    ii = strlen(temp_array) - 1;
    jj = strlen(CLOG_tmpfilename);
    for(;(ii >= 0) && (jj < 29); ii--,jj++)
      CLOG_tmpfilename[jj] = temp_array[ii];
    
    CLOG_tmpfilename[jj] = '\0';
}
    
    
    

/************* to become macros once debugged ***********************/

void CLOG_put_hdr( type )
int type;
{
    if (((char *) CLOG_ptr + CLOG_MAX_REC_LEN) >= (char *) CLOG_block_end) {
	CLOG_LOGENDBLOCK();
	CLOG_newbuff(&CLOG_currbuff->next);
    }
    ((CLOG_HEADER *) CLOG_ptr)->timestamp = CLOG_timestamp(); 
    ((CLOG_HEADER *) CLOG_ptr)->rectype   = type;
				/* int length will be filled in later */
				/* int procid will be filled in later */
    CLOG_ptr = ((CLOG_HEADER *) CLOG_ptr)->rest; /* point past header */
}

void CLOG_LOGMSG( etype, tag, partner, comm, size )
int etype, tag, partner, comm, size;
{
    if (CLOG_OK) {                                                    
	{   
	    static int first = 1;
	    static int srcloc;
	    if (first) {
		first = 0;
		srcloc = CLOG_srcid++;
		CLOG_LOGSRCLOC(srcloc, __LINE__, __FILE__);
	    }
	    CLOG_put_hdr(CLOG_MSGEVENT);
	    ((CLOG_MSG *) CLOG_ptr)-> etype   = etype;
	    ((CLOG_MSG *) CLOG_ptr)-> tag     = tag;
	    ((CLOG_MSG *) CLOG_ptr)-> partner = partner;
	    ((CLOG_MSG *) CLOG_ptr)-> comm    = comm;
	    ((CLOG_MSG *) CLOG_ptr)-> size    = size;
	    ((CLOG_MSG *) CLOG_ptr)-> srcloc  = srcloc;
	    CLOG_ptr			      = ((CLOG_MSG *) CLOG_ptr)->end;
	}
    }                                                                 
    else if (CLOG_ERROR)                                              
	CLOG_not_init;
}

void CLOG_LOGRAW( etype, data, string )
int etype, data;
char *string;
{
    if (CLOG_OK) {                                                    
        event_count++;
	{   
	    static int first = 1;
	    static int srcloc;
	    if (first) {
		first = 0;
		srcloc = CLOG_srcid++;
		CLOG_LOGSRCLOC(srcloc, __LINE__, __FILE__);
	    }
	    CLOG_put_hdr(CLOG_RAWEVENT);
	    ((CLOG_RAW *) CLOG_ptr)-> etype   = etype;
	    ((CLOG_RAW *) CLOG_ptr)-> data    = data;
	    ((CLOG_RAW *) CLOG_ptr)-> srcloc  = srcloc;
	    if (string)
		strncpy(((CLOG_RAW *)CLOG_ptr)->string, string,
			sizeof(CLOG_DESC));
	    else
		* ((char *) ((CLOG_RAW *)CLOG_ptr)->string) = '\0';

	    CLOG_ptr			      = ((CLOG_RAW *) CLOG_ptr)->end;
	    * ((char *) CLOG_ptr - 1)	= '\0'; /* ensure null terminated */
	}
    }                                                                 
    else if (CLOG_ERROR)                                              
	CLOG_not_init;                                                  
}

void CLOG_LOGCOLL( etype, root, size, comm )
int etype, root, size, comm;
{
    if (CLOG_OK) {                                                    
	{   
	    static int first = 1;
	    static int srcloc;
	    if (first) {
		first = 0;
		srcloc = CLOG_srcid++;
		CLOG_LOGSRCLOC(srcloc, __LINE__, __FILE__);
	    }
	    CLOG_put_hdr(CLOG_COLLEVENT);
	    ((CLOG_COLL *) CLOG_ptr)-> etype   = etype;
	    ((CLOG_COLL *) CLOG_ptr)-> root    = root;
	    ((CLOG_COLL *) CLOG_ptr)-> comm    = comm;
	    ((CLOG_COLL *) CLOG_ptr)-> size    = size;
	    ((CLOG_COLL *) CLOG_ptr)-> srcloc  = srcloc;
	    CLOG_ptr			      = ((CLOG_COLL *) CLOG_ptr)->end;
	}
    }                                                                 
    else if (CLOG_ERROR)                                              
	CLOG_not_init;                                                  
}

void CLOG_LOGCOMM( etype, parent, newcomm )
int etype, parent, newcomm;
{
    if (CLOG_OK) {
	{   
	    static int first = 1;
	    static int srcloc;
	    if (first) {
		first = 0;
		srcloc = CLOG_srcid++;
		CLOG_LOGSRCLOC(srcloc, __LINE__, __FILE__);
	    }
	    CLOG_put_hdr(CLOG_COMMEVENT);
	    ((CLOG_COMM *) CLOG_ptr)-> etype   = etype;
	    ((CLOG_COMM *) CLOG_ptr)-> parent  = parent;
	    ((CLOG_COMM *) CLOG_ptr)-> newcomm = newcomm;
	    ((CLOG_COMM *) CLOG_ptr)-> srcloc  = srcloc;
	    CLOG_ptr			       = ((CLOG_COMM *) CLOG_ptr)->end;
	}	
    }                                                                 
    else if (CLOG_ERROR)                                              
	CLOG_not_init;                                                  
}

void CLOG_LOGSTATE( stateid, startetype, endetype, color, description )
int stateid, startetype, endetype;
char *color, *description;
{
    if (CLOG_OK) {
	{   
	    static int first = 1;
	    static int srcloc;
	    if (first) {
		first = 0;
		srcloc = CLOG_srcid++;
		CLOG_LOGSRCLOC(srcloc, __LINE__, __FILE__);
	    }
	    CLOG_put_hdr(CLOG_STATEDEF);
	    ((CLOG_STATE *) CLOG_ptr)-> stateid	   = stateid;
	    ((CLOG_STATE *) CLOG_ptr)-> startetype = startetype;
	    ((CLOG_STATE *) CLOG_ptr)-> endetype   = endetype;
	    if (color) {
		strncpy(((CLOG_STATE *)CLOG_ptr)->color, color,
			sizeof(CLOG_CNAME) - 1); /*(abhi)*/
		((CLOG_STATE *)CLOG_ptr)->color[sizeof(CLOG_CNAME) - 1] = '\0';
	    }
	    else
		* ((char *) ((CLOG_STATE *)CLOG_ptr)->color) = '\0';

	    if (description) {
		strncpy(((CLOG_STATE *)CLOG_ptr)->description, description,
			sizeof(CLOG_DESC) - 1);
		((CLOG_STATE *)CLOG_ptr)->description[sizeof(CLOG_DESC) - 1] 
		  = '\0';
	    }
	    else
		* ((char *) ((CLOG_STATE *)CLOG_ptr)->description) = '\0';

	    CLOG_ptr	              = ((CLOG_STATE *) CLOG_ptr)->end;
	    * ((char *) CLOG_ptr - 1) = '\0'; /* ensure null terminated */
	}	
    }                                                                 
    else if (CLOG_ERROR)                                              
	CLOG_not_init;                                                  
}

void CLOG_LOGEVENT( etype, description )
int etype;
char *description;
{
    if (CLOG_OK) {
	{   
	    static int first = 1;
	    static int srcloc;
	    if (first) {
		first = 0;
		srcloc = CLOG_srcid++;
		CLOG_LOGSRCLOC(srcloc, __LINE__, __FILE__);
	    }
	    CLOG_put_hdr(CLOG_EVENTDEF);
	    ((CLOG_EVENT *) CLOG_ptr)-> etype	  = etype;

	    if (description)
		strncpy(((CLOG_EVENT *)CLOG_ptr)->description, description,
			sizeof(CLOG_CNAME));
	    else
		* ((char *) ((CLOG_EVENT *)CLOG_ptr)->description) = '\0';

	    CLOG_ptr	       = ((CLOG_EVENT *) CLOG_ptr)->end;
	    * ((char *) CLOG_ptr - 1)	= '\0'; /* ensure null terminated */
	}	
    }                                                                 
    else if (CLOG_ERROR)                                              
	CLOG_not_init;                                                  
}

void CLOG_LOGSRCLOC( srcloc, lineno, filename )
int srcloc, lineno;
char *filename;
{
    if (CLOG_OK) {
	CLOG_put_hdr(CLOG_SRCLOC);
	((CLOG_SRC *) CLOG_ptr)->srcloc	= srcloc; 
	((CLOG_SRC *) CLOG_ptr)->lineno	= lineno; 
        strncpy(((CLOG_SRC *)CLOG_ptr)->filename, filename, sizeof(CLOG_FILE));
	CLOG_ptr			= ((CLOG_SRC *) CLOG_ptr)->end;
	* ((char *) CLOG_ptr - 1)	= '\0'; /* ensure null terminated */
    }
    else if (CLOG_ERROR)
	CLOG_not_init;
}

void CLOG_LOGTIMESHIFT( shift )
double shift;
{
    if (CLOG_OK) {
	CLOG_put_hdr(CLOG_SHIFT);
	((CLOG_TSHIFT *) CLOG_ptr)->timeshift = shift; 
	CLOG_ptr		= ((CLOG_TSHIFT *) CLOG_ptr)->end;
    }
    else if (CLOG_ERROR)
	CLOG_not_init;
}

void CLOG_LOGENDBLOCK()
{
    /* assumes there is room for this record */
    if (CLOG_OK) {
	((CLOG_HEADER *) CLOG_ptr)->timestamp = CLOG_timestamp(); 
	((CLOG_HEADER *) CLOG_ptr)->rectype   = CLOG_ENDBLOCK;
	CLOG_ptr = ((CLOG_HEADER *) CLOG_ptr)->rest; 
    }
    else
	if (CLOG_ERROR)                                              
	    CLOG_not_init;                                                  
}
    
void CLOG_LOGENDLOG()
{
    /* assumes there is room for this record */
    if (CLOG_OK) {
	((CLOG_HEADER *) CLOG_ptr)->timestamp = CLOG_timestamp(); 
	((CLOG_HEADER *) CLOG_ptr)->rectype   = CLOG_ENDLOG;
	CLOG_ptr = ((CLOG_HEADER *) CLOG_ptr)->rest; 
    }
    else
	if (CLOG_ERROR)                                              
	    CLOG_not_init;                                                  
}
       
/*@
    CLOG_get_new_event - obtain unused event id
@*/
int CLOG_get_new_event( void )
{
    return CLOG_nextevent++;
}

/*@
    CLOG_get_new_state - obtain unused state id
@*/
int CLOG_get_new_state( void )
{
    return CLOG_nextstate++;
}

