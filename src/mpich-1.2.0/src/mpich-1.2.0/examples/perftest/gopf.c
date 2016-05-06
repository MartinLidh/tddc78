#include <stdio.h>

#include "mpi.h"
#include "mpptest.h"

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
/*****************************************************************************

  Each collection of test routines contains:

  Initialization function (returns pointer to context to pass back to test
  functions

  Routine to return test function (and set parameters) based on command-line
  arguments

  Routine to output "help" text

 *****************************************************************************/

/*****************************************************************************
 
 Here are the test functions for the collective (global) operations

 This is designed to allow testing of both "native" (vendor-supplied) and 
 Chameleon collective operations.  Note that Chameleon supports subsets of
 processes, which most vendor (non-MPI) systems do not.
 *****************************************************************************/

#define GETMEM(type,nitem,p) {\
                          p = (type *)malloc((unsigned)(nitem * sizeof(type) )) ;\
				  if (!p)return 0;;}
#define SETMEM(nitem,p) {int _i; for (_i=0; _i<nitem; _i++) p[_i] = 0; }

void *GOPInit( argc, argv )
int  *argc;
char **argv;
{
GOPctx *new;
char   psetname[50];
int    psetlen = 50;

new = (GOPctx  *)malloc(sizeof(GOPctx ));   if (!new)return 0;;
new->pset = 0;
new->src  = 0;

#ifdef CHAMELEON_COMM
if (SYArgGetString( &argc, argv, 1, "-pset", psetname, 50 )) {
    int n1, m1, i;
    new->pset = PSCreate( 1 );
    sscanf( psetname, "%d-%d", &n1, &m1 );
    for (i=n1; i<=m1; i++) {
	PSAddMember( new->pset, &i, 1 );
	}
    PSCompile( new->pset );
    }
#else
new->pset = 0;
#endif 
return new;
}

typedef enum { GDOUBLE, GFLOAT, GINT, GCHAR } GDatatype;
typedef enum { 
    GOPSUM, GOPMIN, GOPMAX, GOPSYNC, GOPBCAST, GOPCOL, GOPCOLX } GOperation;

double TestGDSum(), TestGISum(), TestGCol(), TestGColx(), TestGScat(), 
       TestGSync();
double TestGDSumGlob(), TestGISumGlob(), TestGColGlob(), TestGColxGlob(),
       TestGScatGlob(), TestGSyncGlob();

/* Determine the function from the arguments */
double ((*GetGOPFunction( argc, argv, test_name, units )) (int,int,GOPctx*))
int *argc;
char **argv;
char *test_name;
char *units;
{
GOperation op    = GOPSYNC;
GDatatype  dtype = GDOUBLE;
int        use_native = 0;
double     (*f)();
int        rrsize;
extern void gsetopL(), gsetopT(), gsetopTP();
extern void gscattersetR();

/* Default choice */
strcpy( test_name, "sync" );

/* Get information on the actual problem to run */

/* Choose the operations */
if (SYArgHasName( argc, argv, 1, "-dsum" )) {
    op    = GOPSUM;
    dtype = GDOUBLE;
    strcpy( test_name, "dsum" );
    strcpy( units, "(doubles)" );
    }
if (SYArgHasName( argc, argv, 1, "-isum" )) {
    op    = GOPSUM;
    dtype = GINT;
    strcpy( test_name, "isum" );
    strcpy( units, "(ints)" );
    }
if (SYArgHasName( argc, argv, 1, "-sync" )) {
    op    = GOPSYNC;
    strcpy( test_name, "sync" );
    }
if (SYArgHasName( argc, argv, 1, "-scatter" ) ||
    SYArgHasName( argc, argv, 1, "-bcast" )) {
    op    = GOPBCAST;
    dtype = GINT;
    strcpy( test_name, "scatter" );
    strcpy( units, "(ints)" );
    }
if (SYArgHasName( argc, argv, 1, "-col" )) {
    op    = GOPCOL;
    dtype = GINT;
    strcpy( test_name, "col" );
    strcpy( units, "(ints)" );
    }
if (SYArgHasName( argc, argv, 1, "-colx" )) {
    op    = GOPCOLX;
    dtype = GINT;
    strcpy( test_name, "colx" );
    strcpy( units, "(ints)" );
    }
if (SYArgHasName( argc, argv, 1, "-colxex" )) {
    extern void gcolxsetEX();
    op    = GOPCOLX;
    dtype = GINT;
    ;
    strcpy( test_name, "colxex" );
    strcpy( units, "(ints)" );
    }

/* Convert operation and dtype to routine */
f = 0;
switch (op) {
    case GOPSUM:
        switch (dtype) {
	    case GDOUBLE: 
	    f = TestGDSum;
#if defined(GDSUMGLOB)
	    if (use_native) f = TestGDSumGlob;
#else 
	    if (use_native) f = 0;
#endif
	    break;
	    case GINT:
	    f = TestGISum;
#if defined(GISUMGLOB)
	    if (use_native) f = TestGISumGlob;
#else 
	    if (use_native) f = 0;
#endif
	    break;
	    }
	break;
    case GOPMIN:
    case GOPMAX:
	f = 0;
	break;
    case GOPCOL:
	f = TestGCol;
#if defined(GCOLGLOB)
	if (use_native) f = TestGColGlob;
#else 
	if (use_native) f = 0;
#endif
	break;
    case GOPCOLX:
	f = TestGColx;
#if defined(GCOLGLOB)
	if (use_native) f = TestGColxGlob;
#else 
	if (use_native) f = 0;
#endif
	break;
    case GOPBCAST:
	f = TestGScat;
#if defined(GSCATTERGLOB)
	if (use_native) f = TestGScatGlob;
#else 
	if (use_native) f = 0;
#endif	
	break;
    case GOPSYNC:
	f = TestGSync;
#if defined(GSYNCGLOB)
	if (use_native) f = TestGSyncGlob;
#else 
	if (use_native) f = 0;
#endif
	break;
    }
return f;
}

void PrintGOPHelp( void )
{
  fprintf( stderr, "\nCollective Tests:\n" );
  fprintf( stderr, "-dsum     : reduction (double precision)\n" );
  fprintf( stderr, "-isum     : reduction (integer)\n" );
  fprintf( stderr, "-sync     : synchronization\n" );
  fprintf( stderr, "-colx     : collect with known sizes\n" );
  fprintf( stderr, 
                "-colxex   : collect with known sizes with exchange alg.\n" );
  fprintf( stderr, "-scatter  : scatter\n" );
  fprintf( stderr, "-bcast    : another name for -scatter\n" );

  fprintf( stderr, "\nCollective test control:\n" );
  fprintf( stderr, 
	  "-pset n-m            : processor set consisting of nodes n to m" );

}

/*****************************************************************************
 Here are the actual routines
 *****************************************************************************/
/* First are the Chameleon versions */
double TestGDSum( reps, len, ctx )
int    reps, len;
GOPctx *ctx;
{
int    i;
MPI_Comm pset = ctx->pset;
double *lval, *work, time;
double t0, t1;

GETMEM(double,len,lval);
GETMEM(double,len,work);
SETMEM(len,lval);

MPI_Barrier(MPI_COMM_WORLD );
*(&t0)=MPI_Wtime();
for (i=0; i<reps; i++) {
    MPI_Allreduce(lval, work, len, MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD );
memcpy(lval,work,(len)*sizeof(double));;
    }
*(&t1)=MPI_Wtime();
MPI_Barrier(MPI_COMM_WORLD );
time = *(&t1 )-*(&t0);
MPI_Bcast(&time, sizeof(double), MPI_BYTE, 0, MPI_COMM_WORLD );
free(lval );
free(work );
return time;
}

double TestGISum( reps, len, ctx )
int    reps, len;
GOPctx *ctx;
{
int     i;
MPI_Comm pset = ctx->pset;
int     *lval, *work;
double  time;
double t0, t1;

GETMEM(int,len,lval);
GETMEM(int,len,work);
SETMEM(len,lval);

MPI_Barrier(MPI_COMM_WORLD );
*(&t0)=MPI_Wtime();
for (i=0; i<reps; i++) {
    MPI_Allreduce(lval, work, len, MPI_INT,MPI_SUM,MPI_COMM_WORLD );
memcpy(lval,work,(len)*sizeof(int));;
    }
*(&t1)=MPI_Wtime();
MPI_Barrier(MPI_COMM_WORLD );
time = *(&t1 )-*(&t0);
MPI_Bcast(&time, sizeof(double), MPI_BYTE, 0, MPI_COMM_WORLD );
free(lval );
free(work );
return time;
}

double TestGScat( reps, len, ctx )
int    reps, len;
GOPctx *ctx;
{
int     i;
MPI_Comm pset = ctx->pset;
int     *lval;
double  time;
double t0, t1;

GETMEM(int,len,lval);
SETMEM(len,lval);

MPI_Barrier(MPI_COMM_WORLD );
*(&t0)=MPI_Wtime();
for (i=0; i<reps; i++) {
    MPI_Bcast(lval, len, MPI_BYTE, 0, MPI_COMM_WORLD );
    }
*(&t1)=MPI_Wtime();
MPI_Barrier(MPI_COMM_WORLD );
time = *(&t1 )-*(&t0);
MPI_Bcast(&time, sizeof(double), MPI_BYTE, 0, MPI_COMM_WORLD );
free(lval );
return time;
}

#ifdef CHAMELEON_COMM
double TestGCol( reps, len, ctx )
int    reps, len;
GOPctx *ctx;
{
int     i, gsize, glen;
MPI_Comm pset = ctx->pset;
int     *lval, *gval;
double  time;
double t0, t1;

gsize = len * (MPI_Comm_size(pset,&_n),_n);
GETMEM(int,len,lval);
GETMEM(int,gsize,gval);
SETMEM(len,lval);

MPI_Barrier(MPI_COMM_WORLD );
*(&t0)=MPI_Wtime();
for (i=0; i<reps; i++) {
    PIgcol( lval, len, gval, gsize, &glen, pset, MPI_INT );
    }
*(&t1)=MPI_Wtime();
MPI_Barrier(MPI_COMM_WORLD );
time = *(&t1 )-*(&t0);
MPI_Bcast(&time, sizeof(double), MPI_BYTE, 0, MPI_COMM_WORLD );
free(lval );
free(gval );
return time;
}

/* Test with data of the same length */
double TestGColx( reps, len, ctx )
int    reps, len;
GOPctx *ctx;
{
int     i, gsize, glen;
MPI_Comm pset = ctx->pset;
int     *lval, *gval, *gsizes;
double  time;
int     np, offset;
double t0, t1;

np    = (MPI_Comm_size(pset,&_n),_n);
gsize = len * np;
GETMEM(int,len,lval);
GETMEM(int,gsize,gval);
GETMEM(int,np,gsizes);
for (i=0; i<np; i++) 
    gsizes[i] = len * sizeof(int);
/* Set the values so that we can do a correctness check */
offset = (MPI_Comm_rank(pset,&_n),_n)*len;
for (i=0; i<len; i++) 
    lval[i] = offset + i;
MPI_Barrier(MPI_COMM_WORLD );
*(&t0)=MPI_Wtime();
for (i=0; i<reps; i++) {
    PIgcolx( lval, gsizes, gval, pset, MPI_INT ); 
    }
*(&t1)=MPI_Wtime();
MPI_Barrier(MPI_COMM_WORLD );
time = *(&t1 )-*(&t0);
MPI_Bcast(&time, sizeof(double), MPI_BYTE, 0, MPI_COMM_WORLD );
/* Do an error check */
for (i=0; i<gsize; i++) {
    if (gval[i] != i) 
	fprintf( stderr, "[%d] error in gcolx; gval[%d] = %d\n", 
	        (MPI_Comm_rank(pset,&_n),_n), i, gval[i] );
    }
free(lval );
free(gval );
free(gsizes );
return time;
}
#else
double TestGColx( reps, len, ctx )
int    reps, len;
GOPctx *ctx;
{
MPI_Abort( MPI_COMM_WORLD, 1 );
return -1.0;
}
double TestGCol( reps, len, ctx )
int    reps, len;
GOPctx *ctx;
{
MPI_Abort( MPI_COMM_WORLD, 1 );
return -1.0;
}
#endif

double TestGSync( reps, len, ctx )
int    reps, len;
GOPctx *ctx;
{
int     i;
MPI_Comm pset = ctx->pset;
double  time;
double t0, t1;

MPI_Barrier(MPI_COMM_WORLD );
*(&t0)=MPI_Wtime();
for (i=0; i<reps; i++) {
    MPI_Barrier(MPI_COMM_WORLD );
    }
*(&t1)=MPI_Wtime();
MPI_Barrier(MPI_COMM_WORLD );
time = *(&t1 )-*(&t0);
MPI_Bcast(&time, sizeof(double), MPI_BYTE, 0, MPI_COMM_WORLD );
return time;
}

/* Next are the "vendor" versions (when available) */
#ifdef GDSUMGLOB
double TestGDSumGlob( reps, len, ctx )
int    reps, len;
GOPctx *ctx;
{
int    i;
MPI_Comm pset = ctx->pset;
double *lval, *work, time;
double t0, t1;

GETMEM(double,len,lval);
GETMEM(double,len,work);
SETMEM(len,lval);

MPI_Barrier(MPI_COMM_WORLD );
*(&t0)=MPI_Wtime();
for (i=0; i<reps; i++) {
    GDSUMGLOB( lval, len, work );
    }
*(&t1)=MPI_Wtime();
MPI_Barrier(MPI_COMM_WORLD );
time = *(&t1 )-*(&t0);
MPI_Bcast(&time, sizeof(double), MPI_BYTE, 0, MPI_COMM_WORLD );
free(lval );
free(work );
return time;
}
#endif

#ifdef GISUMGLOB
double TestGISumGlob( reps, len, ctx )
int    reps, len;
GOPctx *ctx;
{
int     i;
MPI_Comm pset = ctx->pset;
int     *lval, *work;
double  time;
double t0, t1;

GETMEM(int,len,lval);
GETMEM(int,len,work);
SETMEM(len,lval);

MPI_Barrier(MPI_COMM_WORLD );
*(&t0)=MPI_Wtime();
for (i=0; i<reps; i++) {
    GISUMGLOB( lval, len, work );
    }
*(&t1)=MPI_Wtime();
MPI_Barrier(MPI_COMM_WORLD );
time = *(&t1 )-*(&t0);
MPI_Bcast(&time, sizeof(double), MPI_BYTE, 0, MPI_COMM_WORLD );
free(lval );
free(work );
return time;
}
#endif

#ifdef GSCATTERGLOB
double TestGScatGlob( reps, len, ctx )
int    reps, len;
GOPctx *ctx;
{
int     i;
MPI_Comm pset = ctx->pset;
int     *lval;
double  time;
double t0, t1;

GETMEM(int,len,lval);
SETMEM(len,lval);

MPI_Barrier(MPI_COMM_WORLD );
*(&t0)=MPI_Wtime();
for (i=0; i<reps; i++) {
    GSCATTERGLOB( lval, len, PSISROOT(pset), MPI_INT );
    }
*(&t1)=MPI_Wtime();
MPI_Barrier(MPI_COMM_WORLD );
time = *(&t1 )-*(&t0);
MPI_Bcast(&time, sizeof(double), MPI_BYTE, 0, MPI_COMM_WORLD );
free(lval );
return time;
}
#endif

#ifdef GCOLGLOB
double TestGColGlob( reps, len, ctx )
int    reps, len;
GOPctx *ctx;
{
int     i, gsize, glen;
MPI_Comm pset = ctx->pset;
int     *lval, *gval;
double  time;
double t0, t1;

gsize = len * (MPI_Comm_size(pset,&_n),_n);
GETMEM(int,len,lval);
GETMEM(int,gsize,gval);
SETMEM(len,lval);

MPI_Barrier(MPI_COMM_WORLD );
*(&t0)=MPI_Wtime();
for (i=0; i<reps; i++) {
    GCOLGLOB( lval, len, gval, gsize, &glen, G_INT );
    }
*(&t1)=MPI_Wtime();
MPI_Barrier(MPI_COMM_WORLD );
time = *(&t1 )-*(&t0);
MPI_Bcast(&time, sizeof(double), MPI_BYTE, 0, MPI_COMM_WORLD );
free(lval );
free(gval );
return time;
}
#endif 

#ifdef GSYNCGLOB
double TestGSyncGlob( reps, len, ctx )
int    reps, len;
GOPctx *ctx;
{
int     i;
MPI_Comm pset = ctx->pset;
double  time;
double t0, t1;

MPI_Barrier(MPI_COMM_WORLD );
*(&t0)=MPI_Wtime();
for (i=0; i<reps; i++) {
    GSYNCGLOB();
    }
*(&t1)=MPI_Wtime();
MPI_Barrier(MPI_COMM_WORLD );
time = *(&t1 )-*(&t0);
MPI_Bcast(&time, sizeof(double), MPI_BYTE, 0, MPI_COMM_WORLD );
return time;
}
#endif 

#ifdef GCOLXGLOB
double TestGColxGlob( reps, len, ctx )
int    reps, len;
GOPctx *ctx;
{
int     i, gsize, glen;
MPI_Comm pset = ctx->pset;
int     *lval, *gval, *gsizes;
double  time;
int     np;
double t0, t1;

np    = (MPI_Comm_size(pset,&_n),_n);
gsize = len * np;
GETMEM(int,len,lval);
GETMEM(int,gsize,gval);
GETMEM(int,np,gsizes);
SETMEM(len,lval);
for (i=0; i<np; i++) 
    gsizes[i] = len * sizeof(int);
MPI_Barrier(MPI_COMM_WORLD );
*(&t0)=MPI_Wtime();
for (i=0; i<reps; i++) {
    GCOLXGLOB( lval, gsizes, gval, MPI_INT );
    }
*(&t1)=MPI_Wtime();
MPI_Barrier(MPI_COMM_WORLD );
time = *(&t1 )-*(&t0);
MPI_Bcast(&time, sizeof(double), MPI_BYTE, 0, MPI_COMM_WORLD );
free(lval );
free(gval );
free(gsizes );
return time;
}
#endif


