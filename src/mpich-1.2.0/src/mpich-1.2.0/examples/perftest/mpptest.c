/*D
   mpptest - Measure the communications performance of a message-passing system

   Details:
   The greatest challange in performing these experiments in making the results
   reproducible.  On many (most?) systems, there are various events that 
   perturb timings; these can occur on the scale of 10's of milliseconds. 
   To attempt to remove the effect of these events, we make multiple tests,
   taking the minimum of many tests, each of which gives an average time.  To
   reduce the effect of transient perturbations, the entire sequence of tests
   is run several times, taking the best (fastest) time on each test.  Finally,
   a post-processing step retests any anomolies, defined as single peaks peaks
   that are significantly greater than the surrounding times (using a locally
   linear-fit model).
D*/

/* 
  This code is a major re-write of an older version that was generated 
  automatically from an older Chameleon program.  Previous versions
  worked with a wide variety of message-passing systems.
*/

#include <stdio.h>
#include <math.h>
#ifndef HUGE_VAL
#define HUGE_VAL 10.0e38
#endif

#include "mpi.h"
#include "mpptest.h"
int __NUMNODES, __MYPROCID;

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifndef DEFAULT_REPS
#define DEFAULT_REPS 50
#endif

/*
    This is a simple program to test the communications performance of
    a parallel machine.  
 */
    
/* These statics (globals) are used to estimate the parameters in the
   basic (s + rn) complexity model
   Sum of lengths is stored as a double to give 53 bit integer on
   systems where sizeof(int) == 4.
 */
static double sumtime = 0.0, sumlentime = 0.0;
static double sumlen  = 0.0,  sumlen2 = 0.0;
static double sumtime2 = 0.0;
static int    ntest   = 0;

/* If doinfo is 0, don't write out the various text lines */
static int    doinfo = 1;

/* Scaling of time and rate */
static double TimeScale = 1.0;
static double RateScale = 1.0;

/* This is the number of times to run a test, taking as time the minimum
   achieve timing.  This uses an adaptive approach that also stops when
   minThreshTest values are within a few percent of the current minimum */
static int    minreps       = 30;
static int    minThreshTest = 3;
static double repsThresh    = 0.05;
static int    NatThresh     = 3;
char   protocol_name[256];

/* 
   We would also like to adaptively modify the number of repetitions to 
   meet a time estimate (later, we'd like to meet a statistical estimate).
   
   One relatively easy way to do this is to use a linear estimate (either
   extrapolation or interpolation) based on 2 other computations.
   That is, if the goal time is T and the measured tuples (time,reps,len)
   are, the formula for the local time is s + r n, where

   r = (time2/reps2 - time1/reps1) / (len2 - len1)
   s = time1/reps1 - r * len1

   Then the appropriate number of repititions to use is

   Tgoal / (s + r * len) = reps
 */
static double Tgoal = 1.0;
/* If less than Tgoalmin is spent, increase the number of repititions */
static double TgoalMin = 0.5;
static int    AutoReps = 0;

/* This structure allows a collection of arbitray sizes to be specified */
#define MAX_SIZE_LIST 256
static int sizelist[MAX_SIZE_LIST];
static int nsizes = 0;

/* We wish to control the TOTAL amount of time that the test takes.
   We could do this with gettimeofday or clock or something, but fortunately
   the MPI timer is an elapsed timer */
static double max_run_time = 15.0*60.0;
static double start_time = 0.0;

/* All test data is contained in an array of values.  Because we may 
   adaptively choose the message lengths, provision is made to maintain the
   list elements in an array, and for many processing tasks (output, smoothing)
   only the list version is used. */

/* These are used to contain results for a single test */
typedef struct _TwinResults {
/*    double len, t, mean_time, rate; */
    double t,               /* min of the observations (per loop) */
	   max_time,        /* max of the observations (per loop) */
           sum_time;        /* sum of all of the observations */
    int    len;             /* length of the message for this test */
    int    ntests;          /* number of observations */
    int    reps;
    struct _TwinResults *next, *prev;
    } TwinResults;

TwinResults *AllocResultsArray( int );
void SetResultsForStrided( int first, int last, int incr, TwinResults *twin );
void SetResultsForList( int sizelist[], int nsizes, TwinResults *twin );
void SetRepsForList( TwinResults *, int );
int RunTest( TwinResults *, double (*)(int,int,PairData *), PairData *,
	     double );
void RunTestList( TwinResults *, double (*)(int,int,PairData *), PairData * );
int SmoothList( TwinResults *, double (*)(int,int,PairData *), PairData * );
int RefineTestList( TwinResults *, double (*)(int,int,PairData *),PairData *,
		    int, double );
void OutputTestList( TwinResults *, void *, int, int, int );

/* Initialize the results array of a given list of data */

/* This structure is used to provice information for the automatic 
   message-length routines */
typedef struct {
    double (*f)( int, int, PairData *);
    int    reps, proc1, proc2;
    void   *msgctx;
    /* Here is where we should put "recent" timing data used to estimate
       the values of reps */
    double t1, t2;
    int    len1, len2;
    } TwinTest;

/* 
   This function manages running a test and putting the data into the 
   accumulators.  The information is placed in result.  

   This function is intended for use by TSTAuto1d.  That routine controls
   the parameters passed to this routine (the value of x) and accumulates
   the results based on parameters (for accuracy, completeness, and 
   "smoothness") passed to the TST routine.
 */
#ifdef FOO
double GeneralF( x, result, ctx )
double      x;
TwinResults *result;
TwinTest    *ctx;
{
    double t, mean_time;
    int    len = (int)x, k;
    int    reps = ctx->reps;
    int    flag, iwork;
    double tmax, tmean;
    int    rank;

    if (AutoReps) {
	reps = GetRepititions( ctx->t1, ctx->t2, ctx->len1, ctx->len2, 
			       len, reps );
    }

    t = RunSingleTest( ctx->f, reps, len, ctx->msgctx, &tmax, &tmean );

    mean_time	   = t / reps;              /* take average over trials */
    result->t	   = t;
    result->len	   = x;
    result->reps	   = reps;
    result->mean_time  = mean_time;
    result->smean_time = tmean;
    result->max_time   = tmax;
    if (mean_time > 0.0) 
	result->rate      = ((double)len) / mean_time;
    else
	result->rate      = 0.0;

    /* Save the most recent timing data */
    ctx->t1     = ctx->t2;
    ctx->len1   = ctx->len2;
    ctx->t2     = mean_time;
    ctx->len2   = len;
 
    sumlen     += len;
    sumtime    += mean_time;
    sumlen2    += ((double)len) * ((double)len);
    sumlentime += mean_time * len;
    sumtime2   += mean_time * mean_time;
    ntest      ++;

    /* We need to insure that everyone gets the same result */
    MPI_Bcast(&result->rate, sizeof(double), MPI_BYTE, 0, MPI_COMM_WORLD );

    /* Check for max time exceeded */
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    if (rank == 0 && MPI_Wtime() - start_time > max_run_time) {
	fprintf( stderr, "Exceeded %f seconds, aborting\n", max_run_time );
	MPI_Abort( MPI_COMM_WORLD, 1 );
    }
    return result->rate;
}
#endif

int main( int argc, char *argv[])
{
    int    dist;
    double (* BasicCommTest)( int, int, PairData * );
    void *MsgCtx = 0; /* This is the context of the message-passing operation */
    void *outctx;
    void (*ChangeDist)() = 0;
    int  reps,proc1,proc2,len,error_flag,distance_flag,distance;
    double t;
    int  first,last,incr, svals[3];
    int      autosize = 0, autodx;
    double   autorel;
    char     units[32];         /* Name of units of length */

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &__NUMNODES );
    MPI_Comm_rank( MPI_COMM_WORLD, &__MYPROCID );

    strcpy( protocol_name, "blocking" );
    strcpy( units, "(bytes)" );

    if (SYArgHasName( &argc, argv, 1, "-help" )) {
	if (__MYPROCID == 0) PrintHelp( argv );
	MPI_Finalize();
	return 0;
    }

    if (__NUMNODES < 2) {
	fprintf( stderr, "Must run mpptest with at least 2 nodes\n" );
	return 1;
    }

/* Get the output context */
    outctx = SetupGraph( &argc, argv );
    if (SYArgHasName( &argc, argv, 1, "-noinfo" ))    doinfo    = 0;

    reps          = DEFAULT_REPS;
    proc1         = 0;
    proc2         = __NUMNODES-1;
    error_flag    = 0;
    distance_flag = 0;
    svals[0]      = 0;
    svals[1]      = 1024;
    svals[2]      = 32;

    if (SYArgHasName( &argc, argv, 1, "-distance" ))  distance_flag++;
    SYArgGetIntVec( &argc, argv, 1, "-size", 3, svals );
    nsizes = SYArgGetIntList( &argc, argv, 1, "-sizelist", MAX_SIZE_LIST, 
			      sizelist );

    SYArgGetInt(    &argc, argv, 1, "-reps", &reps );
    if (SYArgHasName( &argc, argv, 1, "-autoreps" ))  AutoReps  = 1;
    if (SYArgGetDouble( &argc, argv, 1, "-tgoal", &Tgoal )) {
	AutoReps = 1;
	if (TgoalMin > 0.1 * Tgoal) TgoalMin = 0.1 * Tgoal;
    }
    SYArgGetDouble( &argc, argv, 1, "-rthresh", &repsThresh );

    SYArgGetInt( &argc, argv, 1, "-sample_reps", &minreps );

    SYArgGetInt( &argc, argv, 1, "-max_run_time", &max_run_time );

    autosize = SYArgHasName( &argc, argv, 1, "-auto" );
    if (autosize) {
	autodx = 4;
	SYArgGetInt( &argc, argv, 1, "-autodx", &autodx );
	autorel = 0.02;
	SYArgGetDouble( &argc, argv, 1, "-autorel", &autorel );
    }

/* Pick the general test based on the presence of an -gop, -overlap, -bisect
   or no arg */
    SetPattern( &argc, argv );
    if (SYArgHasName( &argc, argv, 1, "-gop")) {
	/* we need to fix this cast eventually */
	BasicCommTest = (double (*)(int,int,PairData*))
			 GetGOPFunction( &argc, argv, protocol_name, units );
	MsgCtx = GOPInit( &argc, argv );
    }
    else if (SYArgHasName( &argc, argv, 1, "-bisect" )) {
	BasicCommTest = GetPairFunction( &argc, argv, protocol_name );
	dist = 1;
	SYArgGetInt( &argc, argv, 1, "-bisectdist", &dist );
	MsgCtx     = BisectInit( dist );
	ChangeDist = BisectChange;
	strcat( protocol_name, "-bisect" );
	if (SYArgHasName( &argc, argv, 1, "-debug" ))
	    PrintPairInfo( MsgCtx );
	TimeScale = 0.5;
	RateScale = (double) __NUMNODES; /* * (2 * 0.5) */
    }
    else if (SYArgHasName( &argc, argv, 1, "-overlap" )) {
	int MsgSize;
	char cbuf[32];
	if (SYArgHasName( &argc, argv, 1, "-sync" )) {
	    BasicCommTest = round_trip_b_overlap;
	    strcpy( protocol_name, "blocking" );
	}
	else {  /* Assume -async */
	    BasicCommTest = round_trip_nb_overlap;
	    strcpy( protocol_name, "nonblocking" );
	}
	MsgSize = 0;
	SYArgGetInt( &argc, argv, 1, "-overlapmsgsize", &MsgSize );
	MsgCtx  = OverlapInit( proc1, proc2, MsgSize );
	/* Compute floating point lengths if requested */
	if (SYArgHasName( &argc, argv, 1, "-overlapauto")) {
	    OverlapSizes( MsgSize >= 0 ? MsgSize : 0, svals, MsgCtx );
	}
	strcat( protocol_name, "-overlap" );
	if (MsgSize >= 0) {
	    sprintf( cbuf, "-%d bytes", MsgSize );
	}
	else {
	    strcpy( cbuf, "-no msgs" );
	}
	strcat( protocol_name, cbuf );
	TimeScale = 0.5;
	RateScale = 2.0;
    }
    else if (SYArgHasName( &argc, argv, 1, "-memcpy" )) {
	BasicCommTest = memcpy_rate;
	MsgCtx     = 0;
	ChangeDist = 0;
	strcpy( protocol_name, "memcpy" );
	TimeScale = 1.0;
	RateScale = 1.0;
    }
    else {
	/* Pair by default */
	BasicCommTest = GetPairFunction( &argc, argv, protocol_name );
	MsgCtx = PairInit( proc1, proc2 );
	ChangeDist = PairChange;
	if (SYArgHasName( &argc, argv, 1, "-debug" ))
	    PrintPairInfo( MsgCtx );
	TimeScale = 0.5;
	RateScale = 2.0;
    }
    first = svals[0];
    last  = svals[1];
    incr  = svals[2];
    if (incr == 0) incr = 1;

/*
   Finally, we are ready to run the tests.  We want to report times as
   the times for a single link, and rates as the aggregate rate.
   To do this, we need to know how to scale both the times and the rates.

   Times: scaled by the number of one-way trips measured by the base testing
   code.  This is often 2 trips, or a scaling of 1/2.

   Rates: scaled by the number of simultaneous participants (as well as
   the scaling in times).  Compute the rates based on the updated time, 
   then multiply by the number of participants.  Note that, for a single
   sender, time and rate are inversely proportional (that is, if TimeScale 
   is 0.5, RateScale is 2.0).

 */

    start_time = MPI_Wtime();

/* If the distance flag is set, we look at a range of distances.  Otherwise,
   we just use the first and last processor */
    if (doinfo && __MYPROCID == 0) {
	HeaderGraph( outctx, protocol_name, (char *)0, units );
    }
    if(distance_flag) {
	for(distance=1;distance<GetMaxIndex();distance++) {
	    proc2 = GetNeighbor( 0, distance, 0 );
	    if (ChangeDist)
		(*ChangeDist)( distance, MsgCtx );
	    time_function(reps,first,last,incr,proc1,proc2,
			  BasicCommTest,outctx,
			  autosize,autodx,autorel,MsgCtx);
	    ClearTimes();
	}
    }
    else{
	time_function(reps,first,last,incr,proc1,proc2,BasicCommTest,outctx,
		      autosize,autodx,autorel,MsgCtx);
    }

/* 
   Generate the "end of page".  This allows multiple distance graphs on the
   same plot 
 */
    if (doinfo && __MYPROCID == 0) 
	EndPageGraph( outctx );

    MPI_Finalize();
    return 0;
}

/* 
   This is the basic routine for timing an operation.

   Input Parameters:
.  reps - Basic number of times to run basic test (see below)
.  first,last,incr - length of data is first, first+incr, ... last
         (if last != first + k * incr, then actual last value is the 
         value of first + k * incr that is <= last and such that 
         first + (k+1) * incr > last, just as you'd expect)
.  proc1,proc2 - processors to participate in communication.  Note that
         all processors must call because we use global operations to 
         manage some operations, and we want to avoid using process-subset
         operations (supported in Chameleon) to simplify porting this code
.  CommTest  -  Routine to call to run a basic test.  This routine returns 
         the time that the test took in seconds.
.  outctx -  Pointer to output context
.  autosize - If true, the actual sizes are picked automatically.  That is
         instead of using first, first + incr, ... , the routine choses values
         of len such that first <= len <= last and other properties, given 
         by autodx and autorel, are satisfied.
.  autodx - Parameter for TST1dauto, used to set minimum distance between 
         test sizes.  4 (for 4 bytes) is good for small values of last
.  autorel - Relative error tolerance used by TST1dauto in determining the 
         message sizes used.
.  msgctx - Context to pass through to operation routine
 */
void time_function(reps,first,last,incr,proc1,proc2,CommTest,outctx,
		   autosize,autodx,autorel,msgctx)
int    reps,first,last,incr,proc1,proc2,autosize,autodx;
double autorel;
double (* CommTest)();
void      *outctx;
PairData  *msgctx;
{
    int    len,distance,myproc;
    double mean_time;
    double s, r;
    double T1, T2;
    int    Len1, Len2;

    myproc   = __MYPROCID;
    distance = ((proc1)<(proc2)?(proc2)-(proc1):(proc1)-(proc2));

    /* Run test, using either the simple direct test or the automatic length
     test */
    ntest = 0;
    if (autosize) {
#ifdef FOO
	int    maxvals = 256, nvals, i;
	int    dxmax;
	TwinTest ctx;
	TwinResults *results;
      
	/* We should really set maxvals as 2+(last-first)/autodx */
	results	 = (TwinResults *)malloc((unsigned)
					 (maxvals * sizeof(TwinResults) ));
	if (!results) MPI_Abort( MPI_COMM_WORLD, 1 );
	ctx.reps	 = reps;
	ctx.f	 = CommTest;
	ctx.msgctx = msgctx;
	ctx.proc1	 = proc1;
	ctx.proc2	 = proc2;
	ctx.t1	 = 0.0;
	ctx.t2	 = 0.0;
	ctx.len1	 = 0;
	ctx.len2	 = 0;

	/* We need to pick a better minimum resolution */
	dxmax = (last - first) / 16;
	/* make dxmax a multiple of 4 */
	dxmax = (dxmax & ~0x3);
	if (dxmax < 4) dxmax = 4;
      
	nvals = TSTAuto1d( (double)first, (double)last, (double)autodx,
			   (double)dxmax, autorel, 1.0e-10, 
			   results, sizeof(TwinResults),
			   maxvals, GeneralF, &ctx );
	if (myproc == 0) {
	    TSTRSort( results, sizeof(TwinResults), nvals );
	    /* This would be a good place to rerun each of the 
	       nvals results */
	    
	    for (i = 0; i < nvals; i++) {
		DataoutGraph( outctx, proc1, proc2, distance, 
			      (int)results[i].len, results[i].t * TimeScale,
			      results[i].mean_time * TimeScale, 
			      results[i].rate * RateScale, 
			      results[i].smean_time * TimeScale, 
			      results[i].max_time * TimeScale );
	    }
	}
	free(results );
#else
	TwinResults *twin;
	int k;

	twin = AllocResultsArray( 1024 );
	SetResultsForStrided( first, last, (last-first)/8, twin );

	/* Run tests */
	SetRepsForList( twin, reps );
	for (k=0; k<minreps; k++) {
	    int kk;
	    for (kk=0; kk<5; kk++)
		RunTestList( twin, CommTest, msgctx );
	    RefineTestList( twin, CommTest, msgctx, autodx, autorel );
/*	    if (myproc == 0) 
		OutputTestList( twin, outctx, proc1, proc2, distance ); */
	}
	for (k=1; k<5; k++) {
	    if (!SmoothList( twin, CommTest, msgctx )) break;
	}
	/* Final output */
	if (myproc == 0) 
	    OutputTestList( twin, outctx, proc1, proc2, distance );
#endif
    }
#ifndef FOO
    else {
	TwinResults *twin;
	int k;
	if (nsizes) {
	    twin = AllocResultsArray( nsizes );
	    SetResultsForList( sizelist, nsizes, twin );
	}
	else {
	    nsizes = 1 + (last - first)/incr;
	    twin = AllocResultsArray( nsizes );
	    SetResultsForStrided( first, last, incr, twin );
	}

	/* Run tests */
	for (k=1; k<minreps; k++) {
	    SetRepsForList( twin, reps );
	    RunTestList( twin, CommTest, msgctx );
	}
	for (k=1; k<5; k++) {
	    if (!SmoothList( twin, CommTest, msgctx )) break;
	}
	/* Final output */
	if (myproc == 0) 
	    OutputTestList( twin, outctx, proc1, proc2, distance );
    }
#else
    else {
	T1 = 0;
	T2 = 0;
	if (nsizes) {
	    int i;
	    for (i=0; i<nsizes; i++) {
		len = sizelist[i];
		RunATest( len, &Len1, &Len2, &T1, &T2, &reps, CommTest, 
			  myproc, proc1, proc2, distance, outctx, msgctx );
	    }
	}
	else {
	    for(len=first;len<=last;len+=incr){
		RunATest( len, &Len1, &Len2, &T1, &T2, &reps, CommTest, 
			  myproc, proc1, proc2, distance, outctx, msgctx );
	      
	    }
	}
    }
#endif
/* Generate C.It output */
    if (doinfo && myproc == 0) {
	RateoutputGraph( outctx, 
			 sumlen, sumtime, sumlentime, sumlen2, sumtime2, 
			 ntest, &s, &r );
	DrawGraph( outctx, first, last, s, r );
    }

}



/*****************************************************************************
   Utility routines
 *****************************************************************************/

#ifdef FOO
/* This runs a test for a given length */
void RunATest( len, Len1, Len2, T1, T2, reps, CommTest, 
	      myproc, proc1, proc2, distance, outctx, msgctx ) 
int len, *Len1, *Len2, *reps, myproc, proc1, proc2, distance;
double *T1, *T2;
double (*CommTest)( );
void   *outctx, *msgctx;
{
    double mean_time, t, rate;
    double tmax, tmean;

    if (AutoReps) {
	*reps = GetRepititions( *T1, *T2, *Len1, *Len2, len, *reps );
    }
    t = RunSingleTest( CommTest, *reps, len, msgctx, &tmax, &tmean );
    mean_time = t;
    mean_time = mean_time / *reps;  /* take average over trials */
    if (mean_time > 0.0) 
	rate      = ((double)len)/mean_time;
    else 
	rate      = 0.0;
    if(myproc == 0) {
	DataoutGraph( outctx, proc1, proc2, distance, len, 
		      t * TimeScale, mean_time * TimeScale, 
		      rate * RateScale, tmean * TimeScale, tmax * TimeScale );
    }

    *T1   = *T2;
    *Len1 = *Len2;
    *T2   = mean_time;
    *Len2 = len;
}
#endif

/*
   This routine computes a good number of repititions to use based on 
   previous computations
 */
int ComputeGoodReps( t1, len1, t2, len2, len )
double t1, t2;
int    len1, len2, len;
{
    double s, r;
    int    reps;

    r = (t2 - t1) / (len2 - len1);
    s = t1 - r * len1;

    if (s <= 0.0) s = 0.0;
    reps = Tgoal / (s + r * len );
 
    if (reps < 1) reps = 1;

/*
printf( "Reps = %d (%d,%d,%d)\n", reps, len1, len2, len ); fflush( stdout );
 */
    return reps;
}


#ifdef FOO
/*
  This runs the tests for a single size.  It adapts to the number of 
  tests necessary to get a reliable value for the minimum time.
  It also keeps track of the average and maximum times (which are unused
  for now).

  We can estimate the variance of the trials by using the following 
  formulas:

  variance = (1/N) sum (t(i) - (s+r n(i))**2
           = (1/N) sum (t(i)**2 - 2 t(i)(s + r n(i)) + (s+r n(i))**2)
	   = (1/N) (sum t(i)**2 - 2 s sum t(i) - 2 r sum t(i)n(i) + 
	      sum (s**2 + 2 r s n(i) + r**2 n(i)**2))
  Since we compute the parameters s and r, we need only maintain
              sum t(i)**2
              sum t(i)n(i)
              sum n(i)**2
  We already keep all of these in computing the (s,r) parameters; this is
  simply a different computation.

  In the case n == constant (that is, inside a single test), we can use
  a similar test to estimate the variance of the individual measurements.
  In this case, 

  variance = (1/N) sum (t(i) - s**2
           = (1/N) sum (t(i)**2 - 2 t(i)s + s**2)
	   = (1/N) (sum t(i)**2 - 2 s sum t(i) + sum s**2)
  Here, s = sum t(i)/N
  (For purists, the divison should be slightly different from (1/N) in the
  variance formula.  I'll deal with that later.)

  tmax = max time observed
  tmean = mean time observed
 */
double RunSingleTest( CommTest, reps, len, msgctx, tmaxtime, tmean )
double   (*CommTest)(int,int,PairData*);
int      reps;
PairData *msgctx;
double   *tmaxtime, *tmean;
{
    int    flag, k, iwork, natmin;
    double t, tmin, mean_time, tmax, tsum;
    int    rank;

    flag   = 0;
    tmin   = 1.0e+38;
    tmax   = tsum = 0.0;
    natmin = 0;

    for (k=0; k<minreps && flag == 0; k++) {
	t = (* CommTest) (reps,len,msgctx);
	if (__MYPROCID == 0) {
	    tsum += t;
	    if (t > tmax) tmax = t;
	    if (t < tmin) {
		tmin   = t;
		natmin = 0;
	    }
	    else if (minThreshTest < k && tmin * (1.0 + repsThresh) > t) {
		/* This time is close to the minimum; use this to decide
		   that we've gotten close enough */
		natmin++;
		if (natmin >= NatThresh) 
		    flag = 1;
	    }
	}
	MPI_Allreduce(&flag, &iwork, 1, MPI_INT,MPI_SUM,MPI_COMM_WORLD );
	flag = iwork;

	/* Check for max time exceeded */
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	if (rank == 0 && MPI_Wtime() - start_time > max_run_time) {
	    fprintf( stderr, "Exceeded %f seconds, aborting\n", max_run_time );
	    MPI_Abort( MPI_COMM_WORLD, 1 );
	}
    }

    mean_time  = tmin / reps;
    sumlen     += len;
    sumtime    += mean_time;
    sumlen2    += ((double)len)*((double)len);
    sumlentime += mean_time * len;
    sumtime2   += mean_time * mean_time;
    ntest      ++;

    if (tmaxtime) *tmaxtime = tmax / reps;
    if (tmean)    *tmean    = (tsum / reps ) / k;
    return tmin;
}

#endif 

int PrintHelp( argv ) 
char **argv;
{
  if (__MYPROCID != 0) return 0;
  fprintf( stderr, "%s - test individual communication speeds\n", argv[0] );
  fprintf( stderr, 
"Test a single communication link by various methods.  The tests are \n\
combinations of\n\
  Protocol: \n\
  -sync        Blocking sends/receives    (default)\n\
  -async       NonBlocking sends/receives\n\
  -force       Ready-receiver (with a null message)\n\
  -persistant  Persistant communication (only with MPI)\n\
  -vector      Data is separated by constant stride (only with MPI, using UBs)\n\
  -vectortype  Data is separated by constant stride (only with MPI, using \n\
               MPI_Type_vector)\n\
\n\
  Message data:\n\
  -cachesize n Perform test so that cached data is NOT reused\n\
\n\
  -vstride n   For -vector, set the stride between elements\n\
  Message pattern:\n\
  -roundtrip   Roundtrip messages         (default)\n\
  -head        Head-to-head messages\n\
    \n" );
  fprintf( stderr, 
"  Message test type:\n\
  (if not specified, only communication tests run)\n\
  -overlap     Overlap computation with communication (see -size)\n\
  -overlapmsgsize nn\n\
               Size of messages to overlap with is nn bytes.\n\
  -bisect      Bisection test (all processes participate)\n\
  -bisectdist n Distance between processes\n\
    \n" );
  fprintf( stderr, 
"  Message sizes:\n\
  -size start end stride                  (default 0 1024 32)\n\
               Messages of length (start + i*stride) for i=0,1,... until\n\
               the length is greater than end.\n\
  -sizelist n1,n2,...\n\
               Messages of length n1, n2, etc are used.  This overrides \n\
               -size\n\
  -auto        Compute message sizes automatically (to create a smooth\n\
               graph.  Use -size values for lower and upper range\n\
  -autodx n    Minimum number of bytes between samples when using -auto\n\
  -autorel d   Relative error tolerance when using -auto (0.02 by default)\n");

  fprintf( stderr, "\n\
  Number of tests\n\
  -reps n      Number of times message is sent (default %d)\n\
  -autoreps    Compute the number of times a message is sent automatically\n\
  -tgoal  d    Time that each test should take, in seconds.  Use with \n\
               -autoreps\n\
  -rthresh d   Fractional threshold used to determine when minimum time\n\
               has been found.  The default is 0.05.\n\
  -sample_reps n   Number of times a full test is run inorder to find the\n\
               minimum average time.  The default is 30\n\
  -max_run_time n  Maximum number of seconds for all tests.  The default\n\
               is %d\n\
\n", DEFAULT_REPS, (int)max_run_time );
fprintf( stderr, "  -gop [ options ]:\n" );
PrintGOPHelp();
PrintGraphHelp();
PrintPatternHelp(); 
return 0;
}

/* 
   Re-initialize the variables used to estimate the time that it
   takes to send data
 */
void ClearTimes()
{
    sumtime	   = 0.0;
    sumlentime = 0.0;
    sumlen	   = 0.0;
    sumlen2	   = 0.0;
    sumtime2   = 0.0;
    ntest	   = 0;
}

/****************************************************************************
 * New code that uses a list to manage all timing experiments

 ****************************************************************************/
/* Setup the results array */
static TwinResults *twin_avail = 0;

TwinResults *AllocResultsArray( int nsizes )
{
    TwinResults *new;
    int         i;

    new = (TwinResults *)calloc( nsizes+1, sizeof(TwinResults) );
    if (!new) MPI_Abort( MPI_COMM_WORLD, 1 );

    for (i=1; i<nsizes-1; i++) {
	new[i].next = &new[i+1];
	new[i].prev = &new[i-1];
    }
    new[0].next	       = &new[1];
    new[0].prev	       = 0;
    new[nsizes-1].next = 0;
    new[nsizes-1].prev = &new[nsizes-2];
    /* Note that the last member (new[nsizes]) has null prev and next */

    return new;
}

/* Initialize the results array for a strided set of data */
void SetResultsForStrided( int first, int last, int incr, TwinResults *twin )
{
    int i = 0, len;
    for (len=first; len<=last; len += incr) {
	twin[i].len = len;
	twin[i].t   = HUGE_VAL;
	i++;
    }
    /* Fixup list */
    twin[i-1].next = 0;
    twin[i].prev   = 0;

    /* Setup to the avail list */
    twin_avail = &twin[i];
}

/* Initialize the results array of a given list of data */
void SetResultsForList( int sizelist[], int nsizes, TwinResults *twin )
{
    int i = 0;
    for (i=0; i<nsizes; i++) {
	twin[i].len = sizelist[i];
	twin[i].t   = HUGE_VAL;
    }
    /* Fixup list */
    twin[i-1].next = 0;
    twin[i].prev   = 0;

    /* Setup to the avail list */
    twin_avail = &twin[i];
}

/* Run a test for a single entry in the list. Return 1 if the test
   was accepted, 0 otherwise */
int RunTest( TwinResults *twin_p, double (*CommTest)(int,int,PairData *),
		  PairData *msgctx, double wtick )
{
    double t;

    t = (*CommTest)( twin_p->reps, twin_p->len, msgctx );
    /* t is the time overall repititions */

    CheckTimeLimit();

    /* Ignore times that are much shorter than the clock resolution */
    if (t > 10*wtick) {
	twin_p->sum_time += t;
	twin_p->ntests   += twin_p->reps;
	/* Now, convert t to a per-loop time */
	t = t / twin_p->reps;
	if (t < twin_p->t) twin_p->t = t;
	if (t > twin_p->max_time) twin_p->max_time = t;
	return 1;
    }
    return 0;
}

/* For each message length in the list, run the experiement CommTest */
void RunTestList( TwinResults *twin, double (*CommTest)(int,int,PairData *),
		  PairData *msgctx )
{
    double wtick;
    TwinResults *twin_p = twin;
    
    wtick = MPI_Wtick();

    while (twin_p) {
	(void)RunTest( twin_p, CommTest, msgctx, wtick );
	twin_p = twin_p->next;
    }
}

/* This estimates the time at twin_p using a linear interpolation from the
   surrounding entries */
double LinearTimeEst( TwinResults *twin_p, double min_dx )
{
    double t_prev, t_next, t_est, dn_prev, dn_next;

    /* Look at adjacent times */
    if (twin_p->prev) {
	t_prev = twin_p->prev->t;
	dn_prev = twin_p->len - twin_p->prev->len;
    }
    else {
	t_prev = twin_p->t;
	dn_prev = min_dx;
    }
    if (twin_p->next) {
	t_next = twin_p->next->t;
	dn_next = twin_p->next->len - twin_p->len;
    }
    else {
	t_next = twin_p->t;
	dn_next = min_dx;
    }
    /* Compute linear estimate, adjusting for the possibly unequal
       interval sizes, at twin_p->len. */
    t_est = t_prev + (dn_next/(dn_next + dn_prev))*(t_next-t_prev);

    return t_est;
}

/* Add an entry to the list half way (in length) betwen prev and next */
TwinResults *InsertElm( TwinResults *prev, TwinResults *next )
{
    TwinResults *tnew;

    tnew = twin_avail;
    twin_avail = twin_avail->next;
    twin_avail->prev = 0;
    
    tnew->next = next;
    tnew->prev = prev;
    prev->next = tnew;
    next->prev = tnew;
    tnew->len  = (prev->len + next->len) / 2;
    tnew->reps = next->reps;
    tnew->t    = HUGE_VAL;

/*    printf( "%d running test with reps=%d, len=%d\n", 
	    __MYPROCID, tnew->reps, (int)tnew->len );fflush(stdout); */

    return tnew;
}

/* This is a breadth-first refinement approach.  Each call to this routine
   adds one level of refinement */
int RefineTestList( TwinResults *twin, double (*CommTest)(int,int,PairData *),
		    PairData *msgctx, int min_dx, double autorel )
{
    double t_offset, t_center, wtick;
    double abstol = 1.0e-10;
    int do_refine, n_refined = 0, i;
    TwinResults *twin_p = twin, *tnew;

    /* There is a dummy empty entry at the end of the list */
    if (!twin_avail->next) return 0;
    
    wtick = MPI_Wtick();
    
    if (min_dx < 1) min_dx = 1;

    while (twin_p && twin_avail) {
	/* Compute error estimate, adjusting for the possibly unequal
	 interval sizes.  t_center is the linear interpolation at tnew_p->len,
	 t_offset is the difference with the computed value */
	t_center = LinearTimeEst( twin_p, min_dx );
	t_offset = fabs(twin_p->t - t_center);
	do_refine = t_offset > autorel * t_center + abstol;
	MPI_Bcast( &do_refine, 1, MPI_INT, 0, MPI_COMM_WORLD );
	if (do_refine) {
	    /* update the list by refining both segments */
	    if (twin_p->prev && twin_avail &&
		min_dx < twin_p->len - twin_p->prev->len) {
		tnew = InsertElm( twin_p->prev, twin_p );
		n_refined += RunTest( tnew, CommTest, msgctx, wtick );
	    }
	    if (twin_p->next && twin_avail && 
		min_dx < twin_p->next->len - twin_p->len) {
		tnew = InsertElm( twin_p, twin_p->next );
		n_refined += RunTest( tnew, CommTest, msgctx, wtick );
		/* Skip this new element in this time through the loop */
		twin_p = twin_p->next;
	    }
	}
	twin_p = twin_p->next;
    }
    MPI_Bcast( &n_refined, 1, MPI_INT, 0, MPI_COMM_WORLD );
    return n_refined;
}

/* Initialize the number of repititions to use in the basic test */
void SetRepsForList( TwinResults *twin, int reps )
{
    TwinResults *twin_p = twin;
    
    while (twin_p) {
	twin_p->reps = reps;
	twin_p = twin_p->next;
    }
}

/* Smooth the entries in the list by looking for anomolous results and 
   rerunning those tests */
int SmoothList( TwinResults *twin, double (*CommTest)(int,int,PairData *),
		PairData *msgctx )
{
    double wtick, t_est;
    int do_test;
    TwinResults *twin_p = twin;
    int n_smoothed = 0;
    
    wtick = MPI_Wtick();

    while (twin_p) {
	/* Look at adjacent times */
	t_est = LinearTimeEst( twin_p, 4.0 );
	do_test = (twin_p->t > 1.1 * t_est);
	MPI_Bcast( &do_test, 1, MPI_INT, 0, MPI_COMM_WORLD );
	if (do_test) {
	    n_smoothed += RunTest( twin_p, CommTest, msgctx, wtick );
	}
	twin_p = twin_p->next;
    }
    MPI_Bcast( &n_smoothed, 1, MPI_INT, 0, MPI_COMM_WORLD );
    return n_smoothed;
}

/* Output the results using the chosen graphics output package */
void OutputTestList( TwinResults *twin, void *outctx, int proc1, int proc2, 
		     int distance )
{
    TwinResults *twin_p = twin;
    double rate;

    while (twin_p) {
	/* Compute final quantities */
	if (twin_p->t > 0) 
	    rate = ((double)twin_p->len) / twin_p->t;
	else
	    rate = 0.0;

	DataoutGraph( outctx, proc1, proc2, distance, 
		      (int)twin_p->len, twin_p->t * TimeScale,
		      twin_p->t * TimeScale, 
		      rate * RateScale, 
		      twin_p->sum_time / twin_p->ntests * TimeScale, 
		      twin_p->max_time * TimeScale );
	twin_p = twin_p->next;
    }
}

void CheckTimeLimit( void )
{
    /* Check for max time exceeded */
    if (__MYPROCID == 0 && MPI_Wtime() - start_time > max_run_time) {
	fprintf( stderr, "Exceeded %f seconds, aborting\n", max_run_time );
	MPI_Abort( MPI_COMM_WORLD, 1 );
    }
}
