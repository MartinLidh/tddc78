#if !defined(HAVE_STDLIB_H)
#include <stdlib.h>
#else
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#endif
#include <stdio.h>
#include "msgs.h"
#include "expandingList.h"
#include "str_dup.h"

#ifndef MPI_PROC_NULL
#define MPI_PROC_NULL (-1)
#endif

/* We use this instead of the macro incase the value can be extracted from
   the log file (and is different from the default system) */

int proc_null = MPI_PROC_NULL;

#define TESTING 1
#define MALLOC_DEBUG 0

#define USE_QSORT 0
  /* kind of broken right now... */

#define DEF_COLOR "black"

#ifdef __STDC__

static int PostMsg( msgData *msg_data, xpandList *q, int proc, int tag,
		    double time );
  /* Mark a message as being sent or received.  All that is stored
     is the time it was sent or received.  When a matching recv
     or send is logged, the time retrieved and overwritten with -1.0. */
     
static int MatchingMsgPosted( msgData *msg_data, xpandList *q, int proc,
			      int tag, double *time );
  /* Check if the other end of the message has been logged.  For example,
     if a recv is logged, check if the matching send has been logged
     yet.  Returns nonzero if match is found, and returns the time of the
     match in *time. */

static int AddMsg( msgData *msg_data, int sender, double sendTime, int recver,
		   double recvTime, int tag, int size );
  /* When both the send and recv of a message are found, store the
     message with AddMsg(). */

static int GetTagIdx( msgData *msg_data, int tag );
  /* Get the index of a given tag.
     If the tag has not been encountered yet, allocate it a slot
     and return the index of that slot. */

static int CreateIdx( msgData *msg_data );
  /* Create indices */

static int SortSend( msgData *msg_data, int *array, int n );
  /* sort an array of message indices based on send time */

static int SortRecv( msgData *msg_data, int *array, int n );
  /* sort an array of message indices based on recv time */

#else
static int Addmsg();
static int PostMsg();
static int MatchingMsgPosted();
static int GetTempIdx();
static int CreateIdx();
static int SortSend();
static int SortRecv();

#endif



static int GetTagIdx( msg_data, tag )
msgData *msg_data;
int tag;
{
  int i;
  msgDefInfo msg_def;

  for (i=0; i<msg_data->defs->nused; i++) {
    if (tag == ListItem( msg_data->defs, msgDefInfo, i ).tag) {
      return i;
    }
  }

  msg_def.tag = tag;
  msg_def.name = STRDUP( "" );
  msg_def.color = STRDUP( DEF_COLOR );
  ListAddItem( msg_data->defs, msgDefInfo, msg_def );
    /* add tag definition */
  return i;
}


int Msg_Def( msg_data, tag, name, color )
msgData *msg_data;
int tag;
char *name, *color;
{
  msgDefInfo msg_def;

  msg_def.tag = tag;
  msg_def.name = STRDUP(name);
  msg_def.color = STRDUP(color);

    /* add tag definition */
  ListAddItem( msg_data->defs, msgDefInfo, msg_def );

  return ListSize( msg_data->defs, msgDefInfo )-1;
}


int Msg_GetDefNum( msg_data, tag )
msgData *msg_data;
int tag;
{
  return GetTagIdx( msg_data, tag );
}

int Msg_Ndefs( msg_data )
msgData *msg_data;
{
  return ListSize( msg_data->defs, msgDefInfo );
}

int Msg_GetDef( msg_data, def_num, name, tag, color )
msgData *msg_data;
int def_num, *tag;
char **name, **color;
{
  msgDefInfo *ptr;

  ptr = ListHeadPtr( msg_data->defs, msgDefInfo ) + def_num;

  *tag = ptr->tag;
  *name = ptr->name;
  *color = ptr->color;

  return 0;
}


int Msg_SetDef( msg_data, def_num, name, tag, color )
msgData *msg_data;
int def_num, tag;
char *name, *color;
{
  msgDefInfo *ptr;

  ptr = ListHeadPtr( msg_data->defs, msgDefInfo ) + def_num;

    /* free space */
  if (ptr->name)  free( ptr->name );
  if (ptr->color) free( ptr->color );

    /* make copies */
  ptr->tag   = tag;
  ptr->name  = STRDUP( name );
  ptr->color = STRDUP( color );

  return 0;
}


msgData *Msg_Create()
{
  msgData *msg_data;

  msg_data = (msgData*)malloc( sizeof(msgData) );
  if (!msg_data) {
    fprintf( stderr, "Out of memory in Msg_Create.\n" );
    return 0;
  }

    /* list of tag information */
  ListCreate( msg_data->defs, msgDefInfo, 10 );

    /* list of lists of all messages */
  ListCreate( msg_data->list, msgInfo, 10 );

  msg_data->sendq = 0;
  msg_data->recvq = 0;
  msg_data->idx_send = 0;
  msg_data->idx_recv = 0;
  msg_data->idx_proc_send = 0;
  msg_data->idx_proc_recv = 0;

  return msg_data;
}


int Msg_DataInit( msg_data, np )
msgData *msg_data;
int np;
{
  int proc;

  msg_data->np = np;

  /* create lists of send/recv queues for each process */
  msg_data->sendq = (xpandList *)malloc( sizeof(xpandList) * np );
  msg_data->recvq = (xpandList *)malloc( sizeof(xpandList) * np );
  for (proc=0; proc<np; proc++) {
    ListCreate( msg_data->sendq[proc], msgPost, 3 );
    ListCreate( msg_data->recvq[proc], msgPost, 3 );
    /* incoming and outgoing queues */
  }
    
  return 0;
}


static int AddMsg( msg_data, sender, sendTime, recver,
		   recvTime, tag, size )
msgData *msg_data;
int sender, recver, tag, size;
double sendTime, recvTime;
{
  int tagIdx;
  msgInfo msg;

  tagIdx = GetTagIdx( msg_data, tag );
    /* Get the index of this tag type from the temporary list.  If this
       tag type has not been encountered, add it to the list. */

  msg.type = tagIdx;
  msg.size = size;
  msg.sender = sender;
  msg.recver = recver;
  msg.sendTime = sendTime;
  msg.recvTime = recvTime;
  ListAddItem( msg_data->list, msgInfo, msg );
    /* add a message to the master list */
  return 0;
}



static int PostMsg( msg_data, q, proc, tag, time )
msgData *msg_data;
xpandList *q;
int proc, tag;
double time;
{
  int i;
  msgPost newPosting;

#if MALLOC_DEBUG
  fprintf( stderr, "enter PostMsg\n" );
  malloc_verify();
  fprintf( stderr, "entered PostMsg\n" );
#endif

  newPosting.isUsed = 1;
  newPosting.tag = tag;
  newPosting.time = time;

  if (proc == proc_null) {
      return 0;
  }
  if (proc < 0 || proc > msg_data->np) {
      fprintf( stderr, "Message to invalid partner (%d)\n", proc );
      return 0;
      }
  /* if there is an empty slot in the queue */
  for (i=0; i<q[proc]->nused; i++) {
    if (ListItem( q[proc], msgPost, i ).isUsed == 0) {
        /* insert new post in the slot */
      ListItem( q[proc], msgPost, i) = newPosting;
#if MALLOC_DEBUG
      malloc_verify();
      fprintf( stderr, "exit PostMsg\n" );
#endif
      return i;
    }
  }

    /* if the queue is full, add the post as a new element in the list */
  ListAddItem( q[proc], msgPost, newPosting );

#if MALLOC_DEBUG
  malloc_verify();
  fprintf( stderr, "exit PostMsg\n" );
#endif

  return i;
}

static int MatchingMsgPosted( msg_data, q, proc, tag, time )
msgData *msg_data;
xpandList *q;
int proc, tag;
double *time;
{
  int i;
  double earliestTime = 0;
  int earliestIdx;

#if MALLOC_DEBUG
  fprintf( stderr, "enter MatchingMsgPosted\n" );
  malloc_verify();
  fprintf( stderr, "entered MatchingMsgPosted\n" );
#endif

  /* This isn't correct, since different MPI's will have different values of
     MPI_PROC_NULL (e.g., MPICH uses -1, IBM use -3) */
  if (proc == proc_null) {
      return 1;
  }

  if (proc < 0 || proc > msg_data->np) {
      fprintf( stderr, "Matching message to invalid partner (%d)\n", proc );
      return 0;
      }

  /* tagIdx = GetTagIdx( msg_data, tag ); */

  earliestIdx = -1;
  for (i=0; i<q[proc]->nused; i++) {
    if (ListItem( q[proc], msgPost, i).isUsed &&
	ListItem( q[proc], msgPost, i).tag == tag) {
      /* if this post matches our search, */

      if (earliestIdx == -1 ||
	  ListItem( q[proc], msgPost, i).time < earliestTime) {
	earliestIdx = i;
	earliestTime = ListItem( q[proc], msgPost, i ).time;
      }
    }
  }

  if (earliestIdx == -1) {
    /* didn't find any matches */
#if MALLOC_DEBUG
  malloc_verify();
  fprintf( stderr, "exit MatchingMsgPosted\n" );
#endif
    return 0;
  }

  *time = earliestTime;
  ListItem( q[proc], msgPost, earliestIdx ).isUsed = 0;
    /* found match, return it and clear the slot */

#if MALLOC_DEBUG
  malloc_verify();
  fprintf( stderr, "exit MatchingMsgPosted\n" );
#endif

  return 1;
}




int Msg_Send( msg_data, sender, recver, time, tag, size )
msgData *msg_data;
int sender, recver, tag, size;
double time;
{
  double recvTime;

    /* check if someone says they already received this */
  if (MatchingMsgPosted( msg_data, msg_data->recvq, recver, tag, &recvTime )) {
      /* we have a tachyon! */
    AddMsg( msg_data, sender, time, recver, recvTime, tag, size );
    return 1;
  }

  PostMsg( msg_data, msg_data->sendq, sender, tag, time );
  return 0;
}




int Msg_Recv( msg_data, recver, sender, time, tag, size )
msgData *msg_data;
int sender, recver, tag, size;
double time;
{
  double sendTime;

    /* check if someone says they already sent this */
  if (MatchingMsgPosted( msg_data, msg_data->sendq, sender, tag, &sendTime )) {
    AddMsg( msg_data, sender, sendTime, recver, time, tag, size );
    return 0;
  }

  PostMsg( msg_data, msg_data->recvq, recver, tag, time );
    /* received before being sent -- a tachyon */
  return 1;
}



int Msg_DoneAdding( msg_data )
msgData *msg_data;
{

    /* shrinkwrap the xpandList of message definitions */
  ListShrinkToFit( msg_data->defs, msgDefInfo );

    /* shrinkwrap the master list of messages */
  ListShrinkToFit( msg_data->list, msgInfo );

    /* create various indices */
  CreateIdx( msg_data );

  return 0;
}



int Msg_N( msg_data )
msgData *msg_data;
{
  return ListSize( msg_data->list, msgInfo );
}

int Msg_Get( msg_data, n, type, sender, recver, sendTime, recvTime, size )
msgData *msg_data;
int n, *type, *size, *sender, *recver;
double *sendTime, *recvTime;
{
  msgInfo *m;

  m = ListHeadPtr( msg_data->list, msgInfo ) + n;

  *type     = m->type;
  *size     = m->size;
  *sender   = m->sender;
  *recver   = m->recver;
  *sendTime = m->sendTime;
  *recvTime = m->recvTime;

  return 0;
}


#if USE_QSORT
static msgData *sort_data;

static int SetSortData( data )
msgData *data;
{
  sort_data = data;
  return 0;
}


static int CompareMsgSendTimes( a, b )
#if !defined(sparc) || defined(__STDC__)
const
#endif
void *a, *b;
{
  return (ListItem( sort_data->list, msgInfo, *(int*)a ).sendTime >
	  ListItem( sort_data->list, msgInfo, *(int*)b ).sendTime) ? 1 : -1;
  /* Chances are slim to none that doubles will be equal.  Doesn't
     matter either way if they are.  */
}


static int CompareMsgRecvTimes( a, b )
#if !defined(sparc) || defined(__STDC__)
const
#endif
void *a, *b;
{
  return (ListItem( sort_data->list, msgInfo, *(int*)a ).recvTime >
	  ListItem( sort_data->list, msgInfo, *(int*)b ).recvTime) ? 1 : -1;
  /* Chances are slim to none that doubles will be equal.  Doesn't
     matter either way if they are.  */
}
#endif



static int CreateIdx( msg_data )
msgData *msg_data;
{
  /* Create the following indices:

     idx_send
     idx_recv
     idx_proc_send
     idx_proc_recv

     */

  int i, proc;
  msgInfo *msg;

  ListCreate( msg_data->idx_send, int, msg_data->list->nused );
  ListCreate( msg_data->idx_recv, int, msg_data->list->nused );

  msg_data->idx_proc_send = (xpandList *)
    malloc( sizeof(xpandList) * msg_data->np );
  msg_data->idx_proc_recv = (xpandList *)
    malloc( sizeof(xpandList) * msg_data->np );
    /* one list of index references per process */

  for (proc=0; proc<msg_data->np; proc++) {
    ListCreate( msg_data->idx_proc_send[proc], int,
	        msg_data->list->nused / msg_data->np );
    ListCreate( msg_data->idx_proc_recv[proc], int,
	        msg_data->list->nused / msg_data->np );
  }

  msg = &ListItem( msg_data->list, msgInfo, 0 );
  /* Discard if the msg is to or from proc null */
  if (! (msg->sender < 0 || msg->recver < 0) ) {
      for (i=0; i<msg_data->list->nused; i++,msg++) {
	  ListAddItem( msg_data->idx_send, int, i );
	  ListAddItem( msg_data->idx_recv, int, i );
	  ListAddItem( msg_data->idx_proc_send[msg->sender], int, i );
	  ListAddItem( msg_data->idx_proc_recv[msg->recver], int, i );
      }
  }
  for (proc=0; proc<msg_data->np; proc++) {
    ListShrinkToFit( msg_data->idx_proc_send[proc], int );
    ListShrinkToFit( msg_data->idx_proc_recv[proc], int );
  }

    /* sort the big lists */
#if USE_QSORT
  SetSortData( msg_data );
  qsort( msg_data->idx_send->list, msg_data->idx_send->nused, sizeof(int),
	 CompareMsgSendTimes );
  qsort( msg_data->idx_recv->list, msg_data->idx_recv->nused, sizeof(int),
	 CompareMsgRecvTimes );
#else
  SortSend( msg_data, (int *)msg_data->idx_send->list,
            msg_data->idx_send->nused );
  SortRecv( msg_data, (int *)msg_data->idx_recv->list,
            msg_data->idx_recv->nused );
#endif

    /* sort the per-processor lists */
  for (proc=0; proc<msg_data->np; proc++) {
#if USE_QSORT
    qsort( msg_data->idx_proc_send[proc]->list, msg_data->idx_send->nused,
	   sizeof(int), CompareMsgSendTimes );
    qsort( msg_data->idx_proc_recv[proc]->list, msg_data->idx_recv->nused,
	   sizeof(int), CompareMsgRecvTimes );
#else
    SortSend( msg_data, (int *)msg_data->idx_proc_send[proc]->list,
              msg_data->idx_proc_send[proc]->nused );
    SortRecv( msg_data, (int *)msg_data->idx_proc_recv[proc]->list,
              msg_data->idx_proc_recv[proc]->nused );
#endif
  }

  return 0;
}


static int SortSend( msg_data, a, n )
msgData *msg_data;
int *a, n;     /* array and the number of elements in it */
{
  register int temp;
  register double time;
  int i, m, c;  /* insertPt, movePt, current */

  for (c=1; c<n-1; c++) {
    temp = a[c];
    time = ListItem( msg_data->list, msgInfo, a[c]).sendTime;
    for (i=c; i && time < ListItem( msg_data->list, msgInfo, a[i-1]).sendTime;
	 i--);
    for (m=c-1; m>=i; m--)
      a[m+1] = a[m];
    a[i] = temp;
  }
  return 0;
}

      
static int SortRecv( msg_data, a, n )
msgData *msg_data;
int *a, n;     /* array and the number of elements in it */
{
  register int temp;
  register double time;
  int i, m, c;  /* current, insertPt, movePt */

  for (c=1; c<n-1; c++) {
    temp = a[c];
    time = ListItem( msg_data->list, msgInfo, a[c]).recvTime;
    for (i=c; i && time < ListItem( msg_data->list, msgInfo, a[i-1]).recvTime;
	 i--);
    for (m=c-1; m>=i; m--)
      a[m+1] = a[m];
    a[i] = temp;
  }
  return 0;
}


int Msg_Close( data )
msgData *data;
{
  int proc;

  ListDestroy( data->defs, msgDefInfo );
  ListDestroy( data->list, msgInfo );

    /* if data has begun to be added */
  if (data->sendq) {
    for (proc=0; proc < data->np; proc++) {
      ListDestroy( data->sendq[proc], msgPost );
      ListDestroy( data->recvq[proc], msgPost );
    }
    free( data->sendq );
    free( data->recvq );
  }

    /* if the indices ahve been created */
  if (data->idx_send) {
    for (proc=0; proc < data->np; proc++) {
      ListDestroy( data->idx_proc_send[proc], int );
      ListDestroy( data->idx_proc_recv[proc], int );
    }
    free( data->idx_proc_send );
    free( data->idx_proc_recv );
    ListDestroy( data->idx_send, int );
    ListDestroy( data->idx_recv, int );
  }

  free( data );

  return 0;
}


#if TESTING

static int PrintMsg( msg )
msgInfo *msg;
{
  return fprintf( stderr, "Sent by %d at %f, Recvd by %d at %f, size %d\n",
		  msg->sender, msg->sendTime,
		  msg->recver, msg->recvTime, msg->size );
}

static int PrintMsgIdx( msg_data, idx )
msgData *msg_data;
xpandList /*int*/ idx;
{
  int n, i, *idxPtr;
  msgInfo *head;

  head = ListHeadPtr( msg_data->list, msgInfo );
  idxPtr = ListHeadPtr( idx, int );
  n = ListSize( idx, int );
  for (i=0; i<n; i++,idxPtr++) {
    PrintMsg( head + *idxPtr );
  }
  return 0;
}

int Msg_PrintAll( msg_data )
msgData *msg_data;
{
  msgInfo *msg;
  int nmsgs, i, proc;

  fprintf( stderr, "Master list of messages:\n" );

  nmsgs = ListSize( msg_data->list, msgInfo );
  msg = ListHeadPtr( msg_data->list, msgInfo );
  for (i=0; i<nmsgs; i++,msg++)
    PrintMsg( msg );

  if (!msg_data->idx_send) return 1;
  fprintf( stderr, "Indexed by send time:\n" );
  PrintMsgIdx( msg_data, msg_data->idx_send );

  fprintf( stderr, "Indexed by recv time:\n" );
  PrintMsgIdx( msg_data, msg_data->idx_recv );

  for (proc=0; proc<msg_data->np; proc++) {
    fprintf( stderr, "Messages on process %d:\n", proc );
    fprintf( stderr, "Indexed by send time:\n" );
    PrintMsgIdx( msg_data, msg_data->idx_proc_send[proc] );
    fprintf( stderr, "Indexed by recv time:\n" );
    PrintMsgIdx( msg_data, msg_data->idx_proc_recv[proc] );
  }
  return 0;
}
#endif

 
