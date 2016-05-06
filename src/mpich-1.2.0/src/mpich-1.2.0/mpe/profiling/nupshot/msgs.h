/* Message stuff */


#ifndef _MSGS_H_
#define _MSGS_H_

#include "expandingList.h"
/*
#include "idx.h"
*/

#define MSG_SEND 1
#define MSG_RECV 2

#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

/* 
   Value of MPI_PROC_NULL to use in log file 
   Another alternative is to trap these in the log file generator; this
   gives us more flexibility
 */
extern int proc_null;

typedef struct msgDefInfo_ {
  int tag;
  char *name;
  char *color;
} msgDefInfo;


  /* type is the index of this message's definition */
typedef struct msgInfo_ {
  double sendTime, recvTime;
  int type, size, sender, recver;
} msgInfo;


typedef struct msgPost_ {
  int isUsed;
  int tag;
  double time;
} msgPost;


typedef struct msgData_ {
  xpandList /*msgDefInfo*/ defs;
    /* definitions for each tag */
  xpandList /*msgInfo*/ list;
    /* list of all messages, in the order they were encountered */
  xpandList /*msgPost*/ *sendq;
    /* an in/out queue for each process */
  xpandList /*msgPost*/ *recvq;
    /* and tag (array of lists of msgPosts */
  /* intIdx *tagList; */
    /* quick reference list of tag definitions */
  xpandList /*int*/ idx_send;
    /* index sorted by send time */
  xpandList /*int*/ idx_recv;
    /* index sorted by recv time */
  xpandList /*int*/ *idx_proc_send;
    /* indexed by process number, sorted by send time */
  xpandList /*int*/ *idx_proc_recv;
    /* indexed by process number, sorted by recv time */
  int np;			/* number of processes */
} msgData;
  /* As the messages are being read in, build list of tags.
     When all done, sort them. */

typedef struct msgsVisible_ {
  xpandList /*intIdx*/ list;
} msgsVisible;


  /* allocate memory for message data structure */
msgData *Msg_Create ANSI_ARGS(( void ));

  /* describe a message */
int Msg_Def ANSI_ARGS(( msgData *, int tag, char *name, char *color ));

  /* return the # of message descriptions */
int Msg_Ndefs ANSI_ARGS(( msgData * ));

  /* get a message description */
int Msg_GetDef ANSI_ARGS(( msgData *, int def_num, char **name, int *tag,
		      char **color ));

  /* set a message description */
int Msg_SetDef ANSI_ARGS(( msgData *, int def_num, char *name, int tag,
		      char *color ));

  /* allocate memory for message data */
int Msg_DataInit ANSI_ARGS(( msgData *msg_data, int np ));

  /* store the 'send' side of a message */
  /* return 1 if the message was a tachyon */
int Msg_Send ANSI_ARGS(( msgData *msg_data, int sender, int recver,
	      double time, int tag, int size ));

int Msg_Recv ANSI_ARGS(( msgData *msg_data, int recver, int sender,
	      double time, int tag, int size ));
  /* store the 'recv' side of a message */
  /* return 1 if the message was a tachyon */

int Msg_DoneAdding ANSI_ARGS(( msgData *msg_data ));
  /* Shrinkwrap memory used by message data.  Sort messages.
     Might want to use an insertion
     sort since the data might already be well sorted. */

  /* Return the number of messages logged */
int Msg_N ANSI_ARGS(( msgData *msg_data ));

int Msg_Get ANSI_ARGS(( msgData *msg_data, int n, int *type,
		   int *sender, int *recver, double *sendTime,
		   double *recvTime, int *size ));

#if TESTING
  /* Prints a list of all messages read into memory. */
int Msg_PrintAll ANSI_ARGS(( msgData *msg_data ));
#endif

int Msg_Close ANSI_ARGS(( msgData * ));
  /* free all memory associated with messages */


#endif
