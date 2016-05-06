/*
   Old-style Picl trace file handling routines for Upshot

   Ed Karrels
   Argonne National Laboratory
*/


#define DEBUG 0


#include <stdio.h>
#include <ctype.h>

#ifdef NEEDS_STDLIB_PROTOTYPES
#include "protofix.h"
#endif


/* here's hoping that sys/stat.h is common */
#ifdef __FreeBSD__
#include <sys/types.h>
#endif
#include <sys/stat.h>


#include "tcl.h"
#include "events.h"
#include "states.h"
#include "msgs.h"
#include "procs.h"
#include "log.h"
#include "picl.h"



#define MAX_LINE_LEN 1024
#define MAX_WORD_LEN 64

   /* If the user defines a whole buch of different tasks, say 0 through
      100, we dont want to create a different state for every one, just
      do a 'mod' down to 5 unique task types. */
#define PICL_MAX_TASK_NO 5

typedef enum {
  min_recordType=0,
  trace_start,			/* 1 */
  open,				/* 2 */
  load,				/* 3 */
  send,				/* 4 */
  probe,			/* 5 */
  recv,				/* 6 */
  recv_blocking,		/* 7 */
  recv_waking,			/* 8 */
  message,			/* 9 */
  sync,				/* 10 */
  compstats,			/* 11 */
  commstats,			/* 12 */
  close,			/* 13 */
  trace_level,			/* 14 */
  trace_mark,			/* 15 */
  trace_message,		/* 16 */
  trace_stop,			/* 17 */
  trace_flush,			/* 18 */
  trace_exit,			/* 19 */
  block_begin,			/* 20 */
  block_end,			/* 21 */
  trace_marks,			/* 22 */
  max_recordType
} recordTypeEnum;

typedef enum {
   min_blockType=-5,
   global_op,
   bcast1,
   bcast0,
   barrier,
   max_blockType
} blockTypeEnum;


static char *recordTypeNames[] = {
   "trace_start",
   "open",
   "load",
   "send",
   "probe",			/* why left out? */
   "recv",
   "recv_blocking",
   "recv_waking",
   "message",
   "sync",
   "compstats",
   "commstats",
   "close",
   "trace_level",
   "trace_mark",
   "trace_message",
   "trace_stop",
   "trace_flush",
   "trace_exit",
   "block_begin",
   "block_end",
   "trace_marks"
};
#define N_RECORD_TYPES 22

static char *blockTypeNames[] = {
   "global_op",
   "bcast0",
   "bcast1",
   "barrier"
};
#define N_BLOCK_TYPES 4


#define PICL_TYPE_NAME_LEN     20
#define PICL_HOST_NODE     -32768


   /* for keeping track of what mode each process is in in this silly
      format */
typedef struct procMode_ {
   recordTypeEnum mode;
   int active;
      /* for messages: */
   int other_node;
   int msg_type;
   int msg_len;
} procMode;


typedef struct piclData_ {
   Tcl_Interp *interp;
   logFile *log;

   double startTime, endTime;	/* earliest and latest events in the file */

   int file_size;		/* size of the file */
   int bytes_read;		/* # of bytes read so far */
   int line_no;			/* current line # */
   int isFirstTime;		/* is this the first line to be read? */
   int hasHostNode;		/* does this tracefile have host node info
				   in it? */
   int maxProc;			/* largest process # */

      /* When a time-consuming event occurs, such as a send, two compstats
	 records will be generated; one at the start of the send, and
	 one at the end.  So, when one of these time-consuming events occurs,
	 log the beginning of a state, and save what kind it is, so when
	 a compstats event comes in, we know what kind of state to
	 associate it with.  When this first compstats comes in, mark
	 this process as 'active'.  When the second compstats comes in,
	 log the end of a state, and set the process back to normal. */

   procMode *mode;		/* Allocate this to have on element for
				   each process.  Store the mode each
				   process is in. */

      /* to mesh will the rest of Upshot, every action that will take
	 up time must be mapped to a 'state'.  A new state definition
	 is created with a call to State_AddDef().  This will return
	 a state definition # that we should hang onto.  These arrays
	 will hold state definition #'s or -1 if a state hasn't been
	 created for the event type yet. */

				/* state number for each record type */
				/* if no state created yet, set to -1 */
   int recordType_state_no[N_RECORD_TYPES];

				/* state number for each block type */
				/* 0 - 3 will refer to the collective ops, */
				/* 4+ will be for user-defined states */
   int blockType_state_no[N_BLOCK_TYPES + PICL_MAX_TASK_NO];
   
} piclData;


typedef struct piclLineData_ {
   int line_no;			/* line # */
   recordTypeEnum type;		/* recordtype of event */
   double clock;		/* time of the event */
   int node;			/* node on which the event occurred */
   int other_node;		/* for sends, the receiver; for receives, */
				/* the sender */
   int msg_type;		/* message type */
   int msg_len;			/* length of message in bytes */
   int type_no;			/* user-defined event # */
   int block_type;		/* for block start/end events, the type of */
				/* block: barrier, bcast0, user-defined, ... */
   
} piclLineData;  

/* Open up a picl instance */
static piclData *Create ANSI_ARGS(( logFile *log ));

/* go through the tracefile, getting the first&last timestamps,
   number of processes, and list of all the states that will be used */
static int Preprocess ANSI_ARGS(( piclData *picl ));

/* load the tracefile into memory */
static int Load ANSI_ARGS(( piclData *picl ));

/* Close picl-specific tracefile info, but leave the main logfile
   information */
static int Close ANSI_ARGS(( piclData *picl ));

/* Get one word from the given stream, returning -1 if EOF reached, 0
   if end-of-line, or the length of the word read.  If EOL read, increment
   picl->line_no. */
static int GetWord ANSI_ARGS(( piclData *picl, FILE *fp, char *wordbuf,
			  int maxlen ));

/* Given a record type and, if it is a block_begin or block_end,
   a block type, create a string that will define this state.
   'description' should point to at least PICL_TYPE_NAME_LEN bytes. */
/*
static int GetRecordTypeDesc ANSI_ARGS(( piclData *picl, int recordType,
				    int blockType, char *description ));
*/

/* Given a record type description, return the record type number or
   -1 for an invalid type */
static recordTypeEnum GetRecordTypeFromVerbose ANSI_ARGS(( char *str ));

/* Get one line from the tracefile, parsing the data into 'line'.
   Return -1 on EOF, 0 on blank line, and the length of the line
   on success. */
static int GetLine ANSI_ARGS(( piclData *picl, FILE *fp, piclLineData *line ));

/* Create a state definition, if one hasn't been created already, for
   the given record type of block type. */
static int AddState ANSI_ARGS(( piclData *picl,  int recordType, int blockType ));


/*
   Given a record type and, if needed, a block type, return the state
   definition number for the state represented.
*/
static int GetStateNo ANSI_ARGS(( piclData *picl, int recordType, int blockType ));


/*
   Mark the start of a state.
*/
static int StartState ANSI_ARGS(( piclData *picl, int node, double time,
			     int recordType, int blockType ));

/*
   Mark the end of a state.
*/
static int EndState ANSI_ARGS(( piclData *picl, int node, double time,
			   int recordType, int blockType ));




int Picl_Open( log )
logFile *log;
{
  piclData *picl;

  picl = Create( log );
  if (!picl) {
    Tcl_SetResult( log->interp, "Out of memory opening picl tracefile",
		   TCL_STATIC );
    return TCL_ERROR;
  }

  if (Preprocess( picl ) != TCL_OK) goto err;
  if (Load( picl ) != TCL_OK) goto err;
  Close( picl );
  return TCL_OK;

 err:
  Close( picl );

  return TCL_ERROR;
}



static piclData *Create( log )
logFile *log;
{
   piclData *picl;
   struct stat statBuf;
   int i;

      /* get the size of the file, complain if we can't. */
   if (stat( log->filename, &statBuf ) == -1) {
      Tcl_AppendResult( log->interp, "Couldn't get the size of ",
		       log->filename, (char*)0 );
      return 0;
   }

   picl = (piclData*)malloc( sizeof( piclData ) );
   if (!picl) return 0;

   picl->interp = log->interp;
   picl->log = log;
   picl->isFirstTime = 1;	/* set only for the first line */
   picl->line_no = 1;		/* start counting at line 1 */
   picl->bytes_read = 0;	/* nothing read so far */
   picl->file_size = statBuf.st_size; /* save the size of the file */
   picl->hasHostNode = 0;	/* assume no host node unless -32768 events
				   are found */

      /* set all state definition #'s to 0 - nobody has created a state
	 for themselves yet.  */
   for (i=0; i<N_RECORD_TYPES; i++) {
      picl->recordType_state_no[i] = -1;
   }
   for (i=0; i<N_BLOCK_TYPES + PICL_MAX_TASK_NO; i++) {
      picl->blockType_state_no[i] = -1;
   }

   return picl;
}



/*
   Get one word from the input stream, returning 0 if the end of the line
   was reached and read, -1 if EOF reached, or a the number of characters
   in the word.
*/
static int GetWord( picl, fp, wordbuf, maxlen )
piclData *picl;
FILE *fp;
char *wordbuf;
int maxlen;
{
   int len, c;

      /* set to output word buffer to a zero-length string */
   wordbuf[0] = 0;

   c = getc( fp );
   picl->bytes_read++;

      /* strip out space between words */
   while (c != '\n' && isspace( c )) {
      c = getc( fp );
      picl->bytes_read++;
   }

      /* if EOL reached, return 0 */
   if (c == '\n') {
      picl->line_no++;
      return 0;
   }

      /* get the word, stopping at newline, space, or EOF */
   len = 0;
   while (c != '\n' && c != EOF && !isspace( c )) {
      wordbuf[len++] = c;
      c = getc( fp );
      picl->bytes_read++;
   }

      /* If we hit the end of the line or file and nothing has been read yet,
         return 0.  If something has been read, return that and push
	 the newline back into the stream */
   if (c == '\n' || c == EOF) {
      if (len) {
	 picl->bytes_read--;
	 ungetc( c, fp );
      } else {
	 wordbuf[0] = 0;
	    /* return 0 if newline, -1 if EOF */
	 if (c == '\n') {
	    picl->line_no++;
	    return 0;
	 } else {
	    return -1;
	 }
      }
   }

      /* terminate the string */
   wordbuf[len] = 0;
      /* return the # of characters in the word */
   return len;
}


   /* flush input through the end of the line */
static int GetEOL( picl, fp )
piclData *picl;
FILE *fp;
{
   while (getc( fp ) != '\n') picl->bytes_read++;
   picl->line_no++;
   return TCL_OK;
}



/*
   Given a record type (and block type, if applicable),
   return a string describing that block type.
   The 'desc' parameter should point to at least
   PICL_TYPE_NAME_LEN characters of usable memory.
*/
static int GetRecordTypeDesc( picl, recordType, blockType, description )
piclData *picl;			/* all logfile info */
int recordType;			/* record type */
int blockType;			/* if record type==block_{start,end}, specify
				   the block type, be it a collective
				   operation or a 'task #' */
char *description;		/* where to store the description */
{
   if (recordType < block_begin) {
         /* one of the standard record types, which start number at 1 */
      strcpy( description, recordTypeNames[recordType-1] );
   } else {

         /* a block_begin or block_end */
      if (blockType < 0) {
            /* one of the collective operations */
            /* these are numbered from -4 to -1.  Remap this into 0 - 3 */
	 strcpy( description, blockTypeNames[blockType+4] );
      } else {

	    /* a user-defined task #, from 0 to whatever.  Scale it back. */
	 blockType %= PICL_MAX_TASK_NO;
	 sprintf( description, "state_%d", blockType );
      }
   }

   return 0;
}



/*
   Given a string form of a record type, such as "trace_start", return
   the recordTypeEnum index to this record type.  Return -1
   if not recognized.
*/
static recordTypeEnum GetRecordTypeFromVerbose( str )
char *str;
{
   int i;

   for (i=0; i<N_RECORD_TYPES; i++) {
      if (!strcmp( recordTypeNames[i], str )) {
	 return (recordTypeEnum)i;
      }
   }
   return (recordTypeEnum)-1;
}



/*
   Get one line of a picl tracefile, parsing data out of it.
   Return 0 on success, -1 on EOF, -2 on failure.
*/
static int GetLine( picl, fp, line )
piclData *picl;
FILE *fp;
piclLineData *line;
{
   static char word[MAX_WORD_LEN];
   int readStatus;
   int verbose;
   
#define GET_WORD \
   if (GetWord( picl, fp, word, MAX_WORD_LEN ) < 1) \
      goto format_error

      /* clear the members of the line info structure that might not
	 get set */
   line->msg_type = line->msg_len = line->type_no = line->block_type =
      line->other_node = 0;

      /* skip over blank lines */
   while ((readStatus = GetWord( picl, fp, word, MAX_WORD_LEN )) == 0);

      /* end of file */
   if (readStatus == -1) {
      return -1;
   }

   line->line_no = picl->line_no;

      /* if the first character is a letter, this is a verbose line */
   verbose = isalpha( word[0] );

   line->type = min_recordType;
   if (verbose) {
         /* interpret the record type */
      line->type = GetRecordTypeFromVerbose( word );
   } else {
      line->type = (recordTypeEnum)atoi( word );
   }

   if (line->type >= max_recordType ||
       line->type <= min_recordType ) {
      sprintf( picl->interp->result, "unrecognized record type in line %d",
	       picl->line_no );
      Tcl_AppendResult( picl->interp, " of ", picl->log->filename, (char*)0 );
      return -2;
   }

   if (verbose) {
         /* skip the 'clock' label */
      GET_WORD;
   } 

      /* get # of seconds */
   GET_WORD;
   line->clock = atoi( word );
      /* get microseconds */
   GET_WORD;
   line->clock += .000001 * atoi( word );

   if (picl->isFirstTime) {
      picl->startTime = line->clock;
      picl->endTime = line->clock;
      picl->isFirstTime = 0;
   } else {
      if (line->clock < picl->startTime) {
	 picl->startTime = line->clock;
      } else if (line->clock > picl->endTime) {
	 picl->endTime = line->clock;
      }
   }

   if (verbose) {
         /* skip the 'node' label */
      GET_WORD;
   }

      /* get the node # */
   GET_WORD;
   line->node = atoi( word );

   if (line->type == send ||
       line->type == recv ||
       line->type == recv_waking) {
      if (verbose) GET_WORD;			/* skip "to"/"from" */
      GET_WORD;
      line->other_node = atoi(word);
      if (verbose) GET_WORD;        		/* skip "type" */
      GET_WORD;
      line->msg_type = atoi(word);
      if (verbose) GET_WORD;			/* skip "lth" */
      GET_WORD;
      line->msg_len = atoi(word);
   } else if (line->type == recv_blocking) {
      if (verbose) GET_WORD;			/* skip "type" */
      GET_WORD;
      line->msg_type = atoi(word);
   } else if (line->type == trace_mark) {
      if (verbose) GET_WORD;			/* skip "type" */
      GET_WORD;
      line->type_no = atoi(word);
   } else if (line->type == block_begin || line->type == block_end) {
      if (verbose) GET_WORD;			/* skip block_type */
      GET_WORD;
      line->block_type = atoi(word);
   }

   GetEOL( picl, fp );

   return 0;

 format_error:
   sprintf( picl->interp->result, "Format error in line %d", picl->line_no );
   Tcl_AppendResult( picl->interp, " of ", picl->log->filename, (char*)0 );
   return -2;

}

   
static int Preprocess( picl )
piclData *picl;
{
   FILE *fp;
   piclLineData line;

   fp = fopen( picl->log->filename, "r" );

   picl->maxProc = 0;

   while (GetLine( picl, fp, &line ) == 0) {
      if (line.node > picl->maxProc) {
         picl->maxProc = line.node;
      } else if (line.node == PICL_HOST_NODE) {
            /* set flag marking that a host node is being represented */
         picl->hasHostNode = 1;
      }

         /* if the state type occurs once, add it as an official new state
	         type */
      if (line.type == send ||
          line.type == recv ||
          line.type == recv_blocking ||
          line.type == sync) {
         AddState( picl, line.type, 0 );

      } else if (line.type == block_begin ||
		 line.type == block_end ) {
            /* a block marks the start and end of a collective operation */
         AddState( picl, line.type, line.block_type );

      }
   }

      /* set stuff needed by all the logfile routines */
   picl->log->starttime = picl->startTime;
   picl->log->endtime = picl->endTime;
      /* if node=-32768 events were encountered, shift everyone
	 up one to make way for a host node at 0 */
   picl->log->np = picl->maxProc + 1 + picl->hasHostNode;

#if DEBUG
   printf( "States defined:\n" );
   for (i=0; i<N_RECORD_TYPES; i++) {
      char *name, *color, *bitmap;
      if (picl->recordType_state_no[i] != -1) {
	 State_GetDef( picl->log->states, picl->recordType_state_no[i],
		       &name, &color, &bitmap );
	 printf( "  %d: %s %s %s\n", picl->recordType_state_no[i],
		 name, color, bitmap );
      }
   }
   for (i=0; i<N_BLOCK_TYPES + PICL_MAX_TASK_NO; i++) {
      char *name, *color, *bitmap;
      if (picl->blockType_state_no[i] != -1) {
	 State_GetDef( picl->log->states, picl->blockType_state_no[i],
		       &name, &color, &bitmap );
	 printf( "  %d: %s %s %s\n", picl->blockType_state_no[i],
		 name, color, bitmap );
      }
   }
#endif

   return TCL_OK;
}

       
    
    
  

/*
      GetRecordTypeDesc( picl, line.type, line.block_type, desc );
      printf( "Line %d: type %d (%s), clock %f, node %d, %d %d %d %d %d\n",
	      line.line_no, line.type, desc, 
	      line.clock, line.node, line.other_node,
	      line.msg_type, line.msg_len, line.type_no, line.block_type );
*/



static int Close( picl )
piclData *picl;
{
   free( (char*)picl );
   return 0;
}



static int AddState( picl, recordType, blockType )
piclData *picl;
int recordType, blockType;
{
   char desc[PICL_TYPE_NAME_LEN];

   if ((recordTypeEnum)recordType < block_begin) {
         /* normal record type */

         /* recordType comes in starting at 1 */
      recordType--;

         /* check if it's already been defined */
      if (picl->recordType_state_no[recordType] == -1) {
	    /* get string description of the type */
	 GetRecordTypeDesc( picl, recordType+1, blockType, desc );
	    /* add state definition */
	 picl->recordType_state_no[recordType] =
	    State_AddDef( picl->log->states, (char*)0, (char*)0, desc );

      }
   } else {

         /* block type */

         /* wrap around user-defined types to a sane limit */
      if (blockType >= 0) {
	 blockType %= PICL_MAX_TASK_NO;
      }

         /* blockType comes in starting at -4 */
      blockType += 4;

         /* check if it's already been defined */
      if (picl->blockType_state_no[blockType] == -1) {
            /* get string description of the type */
	 GetRecordTypeDesc( picl, recordType, blockType-4, desc );
            /* add state definition */
	 picl->blockType_state_no[blockType] =
	    State_AddDef( picl->log->states, (char*)0, (char*)0, desc );

      }
   }

   return TCL_OK;
}


static int Load( picl )
piclData *picl;
{
   FILE *fp;
   piclLineData line;
   int np;
   int i;
   procMode *mode;

   fp = fopen( picl->log->filename, "r" );
   
   np = Log_Np( picl->log );

      /* let all the data gatherers that I'm comin' */
   Event_DataInit( picl->log->events, np );
   State_DataInit( picl->log->states, np );
   Msg_DataInit( picl->log->msgs, np );
   Process_DataInit( picl->log->processes, np );

      /* allocate storage area for process mode info */
   picl->mode = (procMode*)malloc( sizeof(procMode) * np );
   if (!picl->mode) {
      Tcl_AppendResult( picl->interp, "Out of memory loading PICL file",
		        (char *)0 );
      return TCL_ERROR;
   }

      /* initialize process modes */
   for (i=0; i<np; i++) {
      picl->mode[i].mode = min_recordType;
   }
      

      /* go through the logfile again */
   while (GetLine( picl, fp, &line ) == 0) {
         /* if node=-32768 events were encountered, shift everyone
	    up one to make way for a host node at 0 */
      if (picl->hasHostNode) {
	 if (line.node == PICL_HOST_NODE) {
	    line.node = 0;
	 } else {
	    line.node++;
	 }
      }

         /* get easy-access pointer the mode info */
      mode = picl->mode + line.node;

      switch (line.type) {
	    /* for events that will take some time */
       case send:
       case recv:
	    /* save message info */
	 mode->other_node = line.other_node;
	 mode->msg_type = line.msg_type;
	 mode->msg_len = line.msg_len;
       case probe:
       case sync:
	    /* save this info for the followup compstats call */
	 mode->mode = line.type;
	 mode->active = 0;

	    /* these two don't rely on the compstats kludge */
       case block_begin:
       case recv_blocking:

	    /* mark the beginning of a state */
	 StartState( picl, line.node, line.clock, line.type, line.block_type );
	 break;

       case recv_waking:
	    /* Ooo, a message finally came in! */
	 Msg_Recv( picl->log->msgs, line.node, line.other_node,
		   line.clock, line.msg_type, line.msg_len );
	    /* just so the state from recv_blocking is used */
	 line.type = recv_blocking;

       case block_end:
	 EndState( picl, line.node, line.clock, line.type, line.block_type );
	 break;

       case compstats:
	    /* get easy-access pointer the mode info */
	 mode = picl->mode + line.node;
	    /* make sure we really are waiting for something */
	 if (mode->mode != min_recordType) {
	       /* if the process is not 'active' in this mode, this is the
		  first compstats entry.  Now it should be set to active,
		  to wait for the second compstats entry. */
	    if (!mode->active) {
	       mode->active = 1;
	    } else {
	          /* finally, the end of the state */
	       EndState( picl, line.node, line.clock, mode->mode, 0 );

	          /* if this was a message... */
	       if (mode->mode == send) {
		  Msg_Send( picl->log->msgs, line.node, mode->other_node,
			    line.clock, mode->msg_type, mode->msg_len );
	       } else if (mode->mode == recv) {
		  Msg_Recv( picl->log->msgs, line.node, mode->other_node,
			    line.clock, mode->msg_type, mode->msg_len );
	       }

	       mode->active = 0;
	       mode->mode = min_recordType;
	    }
	 }
	 break;

       case min_recordType:
       case trace_start:
       case open:
       case load:
       case message:
       case commstats:
       case close:
       case trace_level:
       case trace_mark:
       case trace_message:
       case trace_stop:
       case trace_flush:
       case trace_exit:
       case trace_marks:
       case max_recordType:
	    /* do nothing for these guys, but keep the compiler happy */
	 break;

      }   /* switch */
   }   /* while */

   free( (char*)picl->mode );
   fclose( fp );

   return 0;
}


/*
   Given a record and block type, return the state definition # created
   for this type of state.
*/
static int GetStateNo( picl, recordType, blockType )
piclData *picl;
int recordType, blockType;
{
   if ((recordTypeEnum)recordType < block_begin) {
         /* this is a normal record type */
         /* recordType comes in starting at 1 */
      return picl->recordType_state_no[recordType-1];
   } else {
         /* wrap around big user-defined event types */
      if (blockType >= 0) blockType %= PICL_MAX_TASK_NO;
         /* this is a block record type */
         /* blockType comes in starting at -4 */
      return picl->blockType_state_no[blockType+4];
   }
}



static int StartState( picl, node, time, recordType, blockType )
piclData *picl;
int node, recordType, blockType;
double time;
{
   State_Start( picl->log->states, GetStateNo( picl, recordType, blockType ),
	        node, time );

   return TCL_OK;
}



static int EndState( picl, node, time, recordType, blockType )
piclData *picl;
int node, recordType, blockType;
double time;
{
   State_End( picl->log->states, GetStateNo( picl, recordType, blockType ),
	      node, time );

   return TCL_OK;
}
