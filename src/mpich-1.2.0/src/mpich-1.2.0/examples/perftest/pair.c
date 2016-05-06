#include <stdio.h>

#include "mpi.h"

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
/*****************************************************************************

  Each collection of test routines contains:

  Initialization function (returns pointer to context to pass back to test
  functions

  Routine to change "distance"

  Routine to return test function (and set parameters) based on command-line
  arguments

  Routine to output "help" text

 *****************************************************************************/


/*****************************************************************************

 Here begin the test functions.  These all have the same format:

 double NAME(reps, len, ctx)
 
 Input Parameters:
. reps - number of times to perform operation
. len  - length of message (in bytes)
. ctx  - Pointer to structure containing ranks of participating processes

 Return value:
 Elapsed time for operation (not Elapsed time/reps), in seconds.

 These are organized as:
 head-to-head (each process sends to the other).  The blocking version
 can deadlock on systems with small amounts of buffering.

 round-trip (a single message is sent back and fourth between two nodes)

 In order to test both single and multiple senders and receivers, the 
 destination (partner) node is also set, and whether the node is a master or
 a slave (it may also be a bystander)
 *****************************************************************************/

#include "mpptest.h"

#if VARIABLE_TAG
#define MSG_TAG(iter) iter
#else
#define MSG_TAG(iter) 1
#endif

static int CacheSize = 1048576;

void *PairInit( proc1, proc2 )
int proc1, proc2;
{
    PairData *new;
    void PairChange();

    new	       = (PairData *)malloc(sizeof(PairData));   if (!new)return 0;;
    PairChange( 1, new );
    return new;
}

void PairChange( distance, ctx )
int      distance;
PairData *ctx;
{
    int proc2;

    if (__MYPROCID == 0) {
	proc2 = GetNeighbor( 0, distance, 1 );
    }
    else {
	proc2 = GetNeighbor( __MYPROCID, distance, 0 );
	if (proc2 == 0) {
	    /* Then I'm the slave for the root */
	    proc2 = __MYPROCID;
	}
	else {
	    proc2 = NO_NBR;
	}
    }

    ctx->proc1     = 0;
    ctx->proc2     = proc2;
    ctx->is_master = __MYPROCID == ctx->proc1;
    ctx->is_slave  = __MYPROCID == proc2;
    if (ctx->is_master) {
	ctx->partner     = proc2;
	ctx->destination = proc2;
	ctx->source      = proc2;
    }
    else if (ctx->is_slave) {
	ctx->partner     = ctx->proc1;
	ctx->destination = ctx->proc1;
	ctx->source      = ctx->proc1;
    }
    else {
	ctx->partner     = NO_NBR;
	ctx->source	     = NO_NBR;
	ctx->destination = NO_NBR;
    }
}

/* Bisection test can be done by involving all processes in the 
   communication.

   In order to insure that we generate a valid pattern, I create an array
   with an entry for each processor.  Starting from position zero, I mark
   masters, slaves, and ununsed.  Each new entry is marked as a master, 
   with the destination partner marked as a slave.  
 */
void *BisectInit( distance )
int distance;
{
    PairData *new;
    int      i, np;
    int      *marks, curpos;

    new	       = (PairData *)malloc(sizeof(PairData));   if (!new)return 0;;

    BisectChange( distance, new );

    return new;
}

void BisectChange( distance, ctx )
int distance;
PairData *ctx;
{
    int      i, np;
    int      *marks, curpos;
    int      partner;

    np    = __NUMNODES;
    marks = (int *)malloc((unsigned)(np * sizeof(int) ));   
    if (!marks) MPI_Abort( MPI_COMM_WORLD, 1 );
    for (i=0; i<np; i++) {
	marks[i] = NO_NBR;
    }
    curpos = 0;
    while (curpos < np) {
	partner = GetNeighbor( curpos, distance, 1 );
	if (marks[curpos] == NO_NBR && marks[partner] == NO_NBR) {
	    marks[curpos]  = 1;
	    marks[partner] = 2;
	}
	curpos++;
    }

    ctx->proc1     = NO_NBR;
    ctx->proc2     = NO_NBR;
    ctx->is_master = marks[__MYPROCID] == 1;
    ctx->is_slave  = marks[__MYPROCID] == 2;
    if (ctx->is_master) {
	ctx->partner = GetNeighbor( __MYPROCID, distance, 1 );
	ctx->destination = ctx->partner;
	ctx->source      = ctx->partner;
    }
    else if (ctx->is_slave) {
	ctx->partner = GetNeighbor( __MYPROCID, distance, 0 );
	ctx->destination = ctx->partner;
	ctx->source      = ctx->partner;
    }
    else {
	ctx->partner     = NO_NBR;
	ctx->destination = NO_NBR;
	ctx->source      = NO_NBR;
    }
    free(marks);
}

/* Print information on the ctx */
void PrintPairInfo( ctx )
PairData *ctx;
{
    MPE_Seq_begin(MPI_COMM_WORLD,1 );
    fprintf( stdout, "[%d] sending to %d, %s\n", __MYPROCID, ctx->partner, 
	     (ctx->is_master || ctx->is_slave) ? 
	     ( ctx->is_master ? "Master" : "Slave" ) : "Bystander" );
    fflush( stdout );
    MPE_Seq_end(MPI_COMM_WORLD,1 );
}

typedef enum { HEADtoHEAD, ROUNDTRIP } CommType;
typedef enum { Blocking, NonBlocking, ReadyReceiver, Persistant, Vector, 
               VectorType } 
               Protocol;

double exchange_forcetype();
double exchange_async();
double exchange_sync();

double round_trip_sync();
double round_trip_force();
double round_trip_async();
double round_trip_persis();
double round_trip_vector();
double round_trip_vectortype();

double round_trip_nc_sync();
double round_trip_nc_force();
double round_trip_nc_async();

/* Determine the timing function */
double ((*GetPairFunction( argc, argv, protocol_name )) ())
int *argc;
char **argv, *protocol_name;
{
    CommType comm_type = ROUNDTRIP;
    Protocol protocol  = Blocking;
    double (*f)();
    int      use_cache;

    f             = round_trip_sync;

    if (SYArgHasName( argc, argv, 1, "-force" )) {
	protocol      = ReadyReceiver;
	strcpy( protocol_name, "ready receiver" );
    }
    if (SYArgHasName( argc, argv, 1, "-async" )) {
	protocol      = NonBlocking;
	strcpy( protocol_name, "nonblocking" ); 
    }
    if (SYArgHasName( argc, argv, 1, "-sync"  )) {
	protocol      = Blocking;
	strcpy( protocol_name, "blocking" );
    }
    if (SYArgHasName( argc, argv, 1, "-persistant"  )) {
	protocol      = Persistant;
	strcpy( protocol_name, "persistant" );
    }
    if (SYArgHasName( argc, argv, 1, "-vector"  )) {
	int stride;
	protocol      = Vector;
	strcpy( protocol_name, "vector" );
	if (SYArgGetInt( argc, argv, 1, "-vstride", &stride ))
	    set_vector_stride( stride );
    }
    if (SYArgHasName( argc, argv, 1, "-vectortype"  )) {
	int stride;
	protocol      = VectorType;
	strcpy( protocol_name, "type_vector" );
	if (SYArgGetInt( argc, argv, 1, "-vstride", &stride ))
	    set_vector_stride( stride );
    }
    use_cache = SYArgGetInt( argc, argv, 1, "-cachesize", &CacheSize );
    if (SYArgHasName( argc, argv, 1, "-head"  ))     comm_type = HEADtoHEAD;
    if (SYArgHasName( argc, argv, 1, "-roundtrip" )) comm_type = ROUNDTRIP;

    if (comm_type == ROUNDTRIP) {
	if (use_cache) {
	    switch( protocol ) {
	    case ReadyReceiver: f = round_trip_nc_force; break;
	    case NonBlocking:   f = round_trip_nc_async; break;
	    case Blocking:      f = round_trip_nc_sync;  break;
		/* Rolling through the cache means using different buffers
		   for each op; not doable with persistent requests */
	    case Persistant:    f = 0;                   break;
	    case Vector:        f = 0;                   break;
	    case VectorType:    f = 0;                   break;
	    }
	}
	else {
	    switch( protocol ) {
	    case ReadyReceiver: f = round_trip_force;      break;
	    case NonBlocking:   f = round_trip_async;      break;
	    case Blocking:      f = round_trip_sync;       break;
	    case Persistant:    f = round_trip_persis;     break;
	    case Vector:        f = round_trip_vector;     break;
	    case VectorType:    f = round_trip_vectortype; break;
	    }
	}
    }
    else {
	switch( protocol ) {
	case ReadyReceiver: f = exchange_forcetype; break;
	case NonBlocking:   f = exchange_async;     break;
	case Blocking:      f = exchange_sync;      break;
	case Persistant:    f = 0;                  break;
	case Vector:        f = 0;                  break;
	case VectorType:    f = 0;                  break;
	}
    }
    if (!f) {
	fprintf( stderr, "Option %s not supported\n", protocol_name );
	MPI_Abort( MPI_COMM_WORLD, 1 );
    }
    return f;
}

/*****************************************************************************
 Here are the actual routines
 *****************************************************************************/
/* 
   Blocking exchange (head-to-head) 
*/
double exchange_sync(reps,len,ctx)
int      reps,len;
PairData *ctx;
{
  double elapsed_time;
  int  i,msg_id,myproc, to = ctx->destination, from = ctx->source;
  char *sbuffer,*rbuffer;
  double t0, t1;
  MPI_Status status;

  sbuffer = (char *)malloc(len);
  rbuffer = (char *)malloc(len);

  myproc       = __MYPROCID;
  elapsed_time = 0;
  if(ctx->is_master){
    MPI_Recv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
    t0=MPI_Wtime();
    for(i=0;i<reps;i++){
      MPI_Send(sbuffer,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
      MPI_Recv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
	       MPI_COMM_WORLD,&status);
    }
    t1 = MPI_Wtime();
    elapsed_time = t1-t0;
  }

  if(ctx->is_slave){
    MPI_Send(sbuffer,len,MPI_BYTE,from,0,MPI_COMM_WORLD);
    for(i=0;i<reps;i++){
      MPI_Send(sbuffer,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
      MPI_Recv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
	       MPI_COMM_WORLD,&status);
    }
  }

  free(sbuffer);
  free(rbuffer);
  return(elapsed_time);
}

/* 
   Nonblocking exchange (head-to-head) 
 */
double exchange_async(reps,len,ctx)
int      reps,len;
PairData *ctx;
{
  double         elapsed_time;
  int            i,myproc, to = ctx->destination, from = ctx->source;
  MPI_Request  msg_id;
  char           *sbuffer,*rbuffer;
  double   t0, t1;
  MPI_Status status;

  sbuffer = (char *)malloc(len);
  rbuffer = (char *)malloc(len);

  myproc = __MYPROCID;
  elapsed_time = 0;
  if(ctx->is_master){
    MPI_Recv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);  	
    t0=MPI_Wtime();
    for(i=0;i<reps;i++){
      MPI_Irecv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
		MPI_COMM_WORLD,&msg_id);
      MPI_Send(sbuffer,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
      MPI_Wait(&(msg_id),&status);
    }
    t1=MPI_Wtime();
    elapsed_time = t1-t0;
  }

  if(ctx->is_slave){
    MPI_Send(sbuffer,len,MPI_BYTE,from,0,MPI_COMM_WORLD);
    for(i=0;i<reps;i++){
      MPI_Irecv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
		MPI_COMM_WORLD,&msg_id);
      MPI_Send(sbuffer,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
      MPI_Wait(&(msg_id),&status);
    }
  }

  free(sbuffer);
  free(rbuffer);
  return(elapsed_time);
}

/* 
   head-to-head exchange using forcetypes.  This uses null messages to
   let the sender know when the receive is ready 
 */
double exchange_forcetype(reps,len,ctx)
int      reps,len;
PairData *ctx;
{
  double         elapsed_time;
  int            i,myproc, d1, *dmy = &d1, 
                 to = ctx->destination, from = ctx->source;
  MPI_Request  msg_id;
  MPI_Status   status;
  char           *sbuffer,*rbuffer;
  double   t0, t1;

  sbuffer = (char *)malloc(len);
  rbuffer = (char *)malloc(len);

  myproc = __MYPROCID;
  elapsed_time = 0;
  if(ctx->is_master){
    MPI_Recv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,3,MPI_COMM_WORLD,&status);
    t0=MPI_Wtime();
    for(i=0;i<reps;i++){
      MPI_Irecv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&(msg_id));
      MPI_Send(NULL,0,MPI_BYTE,to,2,MPI_COMM_WORLD);
      MPI_Recv(dmy,0,MPI_BYTE,MPI_ANY_SOURCE,2,MPI_COMM_WORLD,&status);
      MPI_Rsend(sbuffer,len,MPI_BYTE,to,0,MPI_COMM_WORLD);
      MPI_Wait(&(msg_id),&status);
    }
    t1=MPI_Wtime();
    elapsed_time = t1-t0;
  }

  if(ctx->is_slave){
    MPI_Send(sbuffer,len,MPI_BYTE,from,3,MPI_COMM_WORLD);
    for(i=0;i<reps;i++){
      MPI_Irecv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&(msg_id));
      MPI_Send(NULL,0,MPI_BYTE,to,2,MPI_COMM_WORLD);
      MPI_Recv(dmy,0,MPI_BYTE,MPI_ANY_SOURCE,2,MPI_COMM_WORLD,&status);
      MPI_Rsend(sbuffer,len,MPI_BYTE,to,0,MPI_COMM_WORLD);
      MPI_Wait(&(msg_id),&status);
    }
  }

  free(sbuffer);
  free(rbuffer);
  return(elapsed_time);
}

/* 
   Blocking round trip (always unidirectional) 
 */
double round_trip_sync(reps,len,ctx)
int      reps,len;
PairData *ctx;
{
  double elapsed_time;
  double mean_time;
  int  i,pid,myproc, to = ctx->destination, from = ctx->source;
  char *rbuffer,*sbuffer;
  MPI_Status status;
  double t0, t1;

  myproc = __MYPROCID;
  sbuffer = (char *)malloc(len);
  rbuffer = (char *)malloc(len);

  elapsed_time = 0;
  if(ctx->is_master){
    MPI_Recv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
    t0=MPI_Wtime();
    for(i=0;i<reps;i++){
      MPI_Send(sbuffer,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
      MPI_Recv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
	       MPI_COMM_WORLD,&status);
    }
    t1=MPI_Wtime();
    elapsed_time = t1 -t0;
  }

  if(ctx->is_slave){
    MPI_Send(sbuffer,len,MPI_BYTE,from,0,MPI_COMM_WORLD);
    for(i=0;i<reps;i++){
      MPI_Recv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
	       MPI_COMM_WORLD,&status);
      MPI_Send(sbuffer,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
    }
  }

  free(sbuffer);
  free(rbuffer);
  return(elapsed_time);
}

/* 
   Ready-receiver round trip
 */
double round_trip_force(reps,len,ctx)
int      reps,len;
PairData *ctx;
{
  double elapsed_time;
  double mean_time;
  int  i,pid,myproc, to = ctx->destination, from = ctx->source;
  char *rbuffer,*sbuffer;
  double t0, t1;
  MPI_Request rid;
  MPI_Status  status;

  myproc = __MYPROCID;
  sbuffer = (char *)malloc(len);
  rbuffer = (char *)malloc(len);

  elapsed_time = 0;
  if(ctx->is_master){
    MPI_Recv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
    t0=MPI_Wtime();
    for(i=0;i<reps;i++){
      MPI_Irecv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
		MPI_COMM_WORLD,&(rid));
      MPI_Rsend(sbuffer,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
      MPI_Wait(&(rid),&status);
    }
    t1=MPI_Wtime();
    elapsed_time = t1 -t0;
  }

  if(ctx->is_slave){
    MPI_Irecv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
	      MPI_COMM_WORLD,&(rid));
    MPI_Send(sbuffer,len,MPI_BYTE,from,0,MPI_COMM_WORLD);
    for(i=0;i<reps-1;i++){
      MPI_Wait(&(rid),&status);
      MPI_Irecv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
		MPI_COMM_WORLD,&(rid));
      MPI_Rsend(sbuffer,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
    }
    MPI_Wait(&(rid),&status);
    MPI_Rsend(sbuffer,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
  }

  free(sbuffer);
  free(rbuffer);
  return(elapsed_time);
}

/* 
   Nonblocking round trip
 */
double round_trip_async(reps,len,ctx)
int      reps,len;
PairData *ctx;
{
  double elapsed_time;
  double mean_time;
  int  i,pid,myproc, to = ctx->destination, from = ctx->source;
  char *rbuffer,*sbuffer;
  MPI_Status status;
  double t0, t1;
  MPI_Request rid;

  myproc = __MYPROCID;
  sbuffer = (char *)malloc(len);
  rbuffer = (char *)malloc(len);

  elapsed_time = 0;
  if(ctx->is_master){
    MPI_Recv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
    t0=MPI_Wtime();
    for(i=0;i<reps;i++){
      MPI_Irecv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
		MPI_COMM_WORLD,&(rid));
      MPI_Send(sbuffer,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
      MPI_Wait(&(rid),&status);
    }
    t1=MPI_Wtime();
    elapsed_time = t1 -t0;
  }

  if(ctx->is_slave){
    MPI_Irecv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
	      MPI_COMM_WORLD,&(rid));
    MPI_Send(sbuffer,len,MPI_BYTE,from,0,MPI_COMM_WORLD);
    for(i=0;i<reps-1;i++){
      MPI_Wait(&(rid),&status);
      MPI_Irecv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
		MPI_COMM_WORLD,&(rid));
      MPI_Send(sbuffer,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
    }
    MPI_Wait(&(rid),&status);
    MPI_Send(sbuffer,len,MPI_BYTE,to,1,MPI_COMM_WORLD);
  }

  free(sbuffer);
  free(rbuffer);
  return(elapsed_time);
}

/* MPI_SUCCESS may be an enum instead of a macro */
#define HAS_MPI
#if defined(MPI_SUCCESS) || defined(HAS_MPI)
/* 
   Persistant communication (only in MPI) 
 */
double round_trip_persis(reps,len,ctx)
int      reps,len;
PairData *ctx;
{
  double elapsed_time;
  double mean_time;
  int  i,pid,myproc, to = ctx->destination, from = ctx->source;
  char *rbuffer,*sbuffer;
  double t0, t1;
  MPI_Request sid, rid, rq[2];
  MPI_Status status, statuses[2];

  myproc = __MYPROCID;
  sbuffer = (char *)malloc(len);
  rbuffer = (char *)malloc(len);
  elapsed_time = 0;
  if(ctx->is_master){
    MPI_Send_init( sbuffer, len, MPI_BYTE, to, 1, MPI_COMM_WORLD, &sid );
    MPI_Recv_init( rbuffer, len, MPI_BYTE, to, 1, MPI_COMM_WORLD, &rid );
    rq[0] = rid;
    rq[1] = sid;
    MPI_Recv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
    t0=MPI_Wtime();
    for(i=0;i<reps;i++){
      MPI_Startall( 2, rq );
      MPI_Waitall( 2, rq, statuses );
    }
    t1=MPI_Wtime();
    elapsed_time = t1 -t0;
    MPI_Request_free( &rid );
    MPI_Request_free( &sid );
  }

  if(ctx->is_slave){
    MPI_Send_init( sbuffer, len, MPI_BYTE, from, 1, MPI_COMM_WORLD, &sid );
    MPI_Recv_init( rbuffer, len, MPI_BYTE, from, 1, MPI_COMM_WORLD, &rid );
    rq[0] = rid;
    rq[1] = sid;
    MPI_Start( &rid );
    MPI_Send(sbuffer,len,MPI_BYTE,from,0,MPI_COMM_WORLD);
    for(i=0;i<reps-1;i++){
      MPI_Wait( &rid, &status );
      MPI_Startall( 2, rq );
      MPI_Wait( &sid, &status );
    }
    MPI_Wait( &rid, &status );
    MPI_Start( &sid );
    MPI_Wait( &sid, &status );
    MPI_Request_free( &rid );
    MPI_Request_free( &sid );
  }

  free(sbuffer);
  free(rbuffer);
  return(elapsed_time);
}

static int VectorStride = 10;
int set_vector_stride( int n )
{
    VectorStride = n;
    return 0;
}

double round_trip_vector(int reps, int len, PairData *ctx)
{
  double elapsed_time;
  double mean_time;
  int  i,pid,myproc, to = ctx->destination, from = ctx->source;
  double *rbuffer,*sbuffer;
  double t0, t1;
  MPI_Datatype vec, types[2];
  int          blens[2];
  MPI_Aint     displs[2];
  MPI_Status   status;
  MPI_Comm     comm;

  /* Adjust len to be in bytes */
  len = len / sizeof(double);

  comm = MPI_COMM_WORLD;
  blens[0] = 1; displs[0] = 0; types[0] = MPI_DOUBLE;
  blens[1] = 1; displs[1] = VectorStride * sizeof(double); types[1] = MPI_UB;
  MPI_Type_struct( 2, blens, displs, types, &vec );
  MPI_Type_commit( &vec );

  myproc = __MYPROCID;
  sbuffer = (double *)malloc((unsigned)(VectorStride * len * sizeof(double) ));
  rbuffer = (double *)malloc((unsigned)(VectorStride * len * sizeof(double) ));
  if (!sbuffer)return 0;;
  if (!rbuffer)return 0;;

  elapsed_time = 0;
  if(ctx->is_master){
    MPI_Recv( rbuffer, len, vec, to, 0, comm, &status );
    t0=MPI_Wtime();
    for(i=0;i<reps;i++){
      MPI_Send( sbuffer, len, vec, to, MSG_TAG(i), comm );
      MPI_Recv( rbuffer, len, vec, from, MSG_TAG(i), comm, &status );
      }
    t1=MPI_Wtime();
    elapsed_time = t1 - t0;
    }

  if(ctx->is_slave){
    MPI_Send( sbuffer, len, vec, from, 0, comm );
    for(i=0;i<reps;i++){
	MPI_Recv( rbuffer, len, vec, from, MSG_TAG(i), comm, &status );
	MPI_Send( sbuffer, len, vec, to, MSG_TAG(i), comm );
	}
    }

  free(sbuffer);
  free(rbuffer);
  MPI_Type_free( &vec );
  return(elapsed_time);
}
double round_trip_vectortype(reps,len,ctx)
int      reps,len;
PairData *ctx;
{
  double elapsed_time;
  double mean_time;
  int  i,pid,myproc, to = ctx->destination, from = ctx->source;
  double *rbuffer,*sbuffer;
  double t0, t1;
  MPI_Datatype vec;
  MPI_Status   status;
  MPI_Comm     comm;

  /* Adjust len to be in doubles */
  len = len / sizeof(double);

  comm = MPI_COMM_WORLD;
  MPI_Type_vector( len, 1, VectorStride, MPI_DOUBLE, &vec );
  MPI_Type_commit( &vec );

  myproc = __MYPROCID;
  sbuffer = (double *)malloc((unsigned)(VectorStride * len * sizeof(double) ));
  rbuffer = (double *)malloc((unsigned)(VectorStride * len * sizeof(double) ));
  if (!sbuffer)return 0;;
  if (!rbuffer)return 0;;

  elapsed_time = 0;
  if(ctx->is_master){
    MPI_Recv( rbuffer, 1, vec, to, 0, comm, &status );
    t0=MPI_Wtime();
    for(i=0;i<reps;i++){
      MPI_Send( sbuffer, 1, vec, to, MSG_TAG(i), comm );
      MPI_Recv( rbuffer, 1, vec, from, MSG_TAG(i), comm, &status );
      }
    t1=MPI_Wtime();
    elapsed_time = t1 -t0;
    }

  if(ctx->is_slave){
    MPI_Send( sbuffer, 1, vec, from, 0, comm );
    for(i=0;i<reps;i++){
	MPI_Recv( rbuffer, 1, vec, from, MSG_TAG(i), comm, &status );
	MPI_Send( sbuffer, 1, vec, to, MSG_TAG(i), comm );
	}
    }

  free(sbuffer );
  free(rbuffer );
  MPI_Type_free( &vec );
  return(elapsed_time);
}
#else
double round_trip_persis(reps,len,ctx)
int      reps,len;
PairData *ctx;
{
    printf( "Persistent operations not defined\n" );
    MPI_Abort( MPI_COMM_WORLD, 1 );
    return 0.0;
}

int set_vector_stride( n )
int n;
{
return 0;
}
double round_trip_vector(reps,len,ctx)
int      reps,len;
PairData *ctx;
{
    printf( "Vector operations not defined\n" );
    exit(1 );
    return 0.0;
}
#endif
/*
    These versions try NOT to operate out of cache; rather, then send/receive
    into a moving window.
 */
/* 
   Blocking round trip (always unidirectional) 
 */
double round_trip_nc_sync(reps,len,ctx)
int      reps,len;
PairData *ctx;
{
  double elapsed_time;
  double mean_time;
  int  i,pid,myproc, to = ctx->destination, from = ctx->source;
  char *rbuffer,*sbuffer, *rp, *sp, *rlast, *slast;
  MPI_Status status;
  double t0, t1;

  myproc = __MYPROCID;
  sbuffer = (char *)malloc((unsigned)(2 * CacheSize ));
  slast   = sbuffer + 2 * CacheSize - len;
  rbuffer = (char *)malloc((unsigned)(2 * CacheSize ));
  rlast   = rbuffer + 2 * CacheSize - len;
  if (!sbuffer || !rbuffer) {
      fprintf( stderr, "Could not allocate %d bytes\n", 4 * CacheSize );
      exit(1 );
      }
  sp = sbuffer;
  rp = rbuffer;

  elapsed_time = 0;
  if(ctx->is_master){
    MPI_Recv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
    t0=MPI_Wtime();
    for(i=0;i<reps;i++){
      MPI_Send(sp,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
      MPI_Recv(rp,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
	       MPI_COMM_WORLD,&status);
      sp += len;
      rp += len;
      if (sp > slast) sp = sbuffer;
      if (rp > rlast) rp = rbuffer;
    }
    t1=MPI_Wtime();
    elapsed_time = t1 -t0;
  }

  if(ctx->is_slave){
    MPI_Send(sbuffer,len,MPI_BYTE,from,0,MPI_COMM_WORLD);
    for(i=0;i<reps;i++){
      MPI_Recv(rp,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
	       MPI_COMM_WORLD,&status);
      MPI_Send(sp,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
      sp += len;
      rp += len;
      if (sp > slast) sp = sbuffer;
      if (rp > rlast) rp = rbuffer;
    }
  }

  free(sbuffer );
  free(rbuffer );
  return(elapsed_time);
}

/* 
   Ready-receiver round trip
 */
double round_trip_nc_force(reps,len,ctx)
int      reps,len;
PairData *ctx;
{
  double elapsed_time;
  double mean_time;
  int  i,pid,myproc, to = ctx->destination, from = ctx->source;
  char *rbuffer,*sbuffer, *rp, *sp, *rlast, *slast;
  double t0, t1;
  MPI_Request rid;
  MPI_Status  status;

  myproc = __MYPROCID;
  sbuffer = (char *)malloc((unsigned)(2 * CacheSize ));
  slast   = sbuffer + 2 * CacheSize - len;
  rbuffer = (char *)malloc((unsigned)(2 * CacheSize ));
  rlast   = rbuffer + 2 * CacheSize - len;
  if (!sbuffer || !rbuffer) {
      fprintf( stderr, "Could not allocate %d bytes\n", 4 * CacheSize );
      exit(1 );
      }
  sp = sbuffer;
  rp = rbuffer;

  elapsed_time = 0;
  if(ctx->is_master){
    MPI_Recv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
    t0=MPI_Wtime();
    for(i=0;i<reps;i++){
      MPI_Irecv(rp,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
		MPI_COMM_WORLD,&(rid));
      MPI_Rsend(sp,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
      MPI_Wait(&(rid),&status);
      sp += len;
      rp += len;
      if (sp > slast) sp = sbuffer;
      if (rp > rlast) rp = rbuffer;
    }
    t1=MPI_Wtime();
    elapsed_time = t1 -t0;
  }

  if(ctx->is_slave){
    MPI_Irecv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
	      MPI_COMM_WORLD,&(rid));
    MPI_Send(sbuffer,len,MPI_BYTE,from,0,MPI_COMM_WORLD);
    for(i=0;i<reps-1;i++){
      MPI_Wait(&(rid),&status);
      rp += len;
      if (rp > rlast) rp = rbuffer;
      MPI_Irecv(rp,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
		MPI_COMM_WORLD,&(rid));
      MPI_Rsend(sp,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
      sp += len;
      if (sp > slast) sp = sbuffer;
    }
    MPI_Wait(&(rid),&status);
    MPI_Rsend(sp,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
  }

  free(sbuffer );
  free(rbuffer );
  return(elapsed_time);
}

/* 
   Nonblocking round trip
 */
double round_trip_nc_async(reps,len,ctx)
int      reps,len;
PairData *ctx;
{
  double elapsed_time;
  double mean_time;
  int  i,pid,myproc, to = ctx->destination, from = ctx->source;
  char *rbuffer,*sbuffer, *rp, *sp, *rlast, *slast;
  double t0, t1;
  MPI_Request rid;
  MPI_Status  status;

  myproc = __MYPROCID;
  sbuffer = (char *)malloc((unsigned)(2 * CacheSize ));
  slast   = sbuffer + 2 * CacheSize - len;
  rbuffer = (char *)malloc((unsigned)(2 * CacheSize ));
  rlast   = rbuffer + 2 * CacheSize - len;
  if (!sbuffer || !rbuffer) {
      fprintf( stderr, "Could not allocate %d bytes\n", 4 * CacheSize );
      exit(1 );
      }
  sp = sbuffer;
  rp = rbuffer;

  elapsed_time = 0;
  if(ctx->is_master){
    MPI_Recv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
    t0=MPI_Wtime();
    for(i=0;i<reps;i++){
      MPI_Irecv(rp,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
		MPI_COMM_WORLD,&(rid));
      MPI_Send(sp,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
      MPI_Wait(&(rid),&status);
      sp += len;
      rp += len;
      if (sp > slast) sp = sbuffer;
      if (rp > rlast) rp = rbuffer;
    }
    t1=MPI_Wtime();
    elapsed_time = t1 -t0;
  }

  if(ctx->is_slave){
    MPI_Irecv(rbuffer,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
	      MPI_COMM_WORLD,&(rid));
    MPI_Send(sbuffer,len,MPI_BYTE,from,0,MPI_COMM_WORLD);
    for(i=0;i<reps-1;i++){
      MPI_Wait(&(rid),&status);
      rp += len;
      if (rp > rlast) rp = rbuffer;
      MPI_Irecv(rp,len,MPI_BYTE,MPI_ANY_SOURCE,MSG_TAG(i),
		MPI_COMM_WORLD,&(rid));
      MPI_Send(sp,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
      sp += len;
      if (sp > slast) sp = sbuffer;
    }
    MPI_Wait(&(rid),&status);
    MPI_Send(sp,len,MPI_BYTE,to,MSG_TAG(i),MPI_COMM_WORLD);
  }

  free(sbuffer );
  free(rbuffer );
  return(elapsed_time);
}


