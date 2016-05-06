#include <stdio.h>
#include "mpi.h"
#include "test.h"

int Test_errorhandling (void);
/* This is complicated by the fact that not all systems correctly 
   implement stdargs (for the ...) declarations).  MPICH uses USE_STDARG
   as the choice here, instead of the cases 
 if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES) 
 */
#if defined(USE_STDARG)
void handler_a( MPI_Comm *, int *, ...);
void handler_b( MPI_Comm *, int *, ...);
void error_handler(MPI_Comm *, int *, ...);
#else
void handler_a ( MPI_Comm *, int * );
void handler_b ( MPI_Comm *, int * );
void error_handler ( MPI_Comm *, int * );
#endif

/* 
   Test the error handers (based on a Fortran test program)
 */
int main( int argc, char **argv )
{
    MPI_Init( &argc, &argv );

    Test_errorhandling();

    Test_Waitforall( );
    MPI_Finalize();
    return 0;
}

static int a_errors, b_errors;

int Test_errorhandling( void )
{
    char errstring[MPI_MAX_ERROR_STRING];
    MPI_Comm dup_comm_world, dummy;
    MPI_Comm tempcomm;
    MPI_Errhandler errhandler_a, errhandler_b, errhandler, old_handler;
    int  err, world_rank, class, resultlen;

#ifdef FOO
    logical test_default, test_abort
#endif

    MPI_Comm_rank( MPI_COMM_WORLD, &world_rank );
    MPI_Comm_dup( MPI_COMM_WORLD, &dup_comm_world );

    if (world_rank == 0) {
	printf( "*** Error Handling ***\n" );
    }

/*
     Exercise save/restore of user error handlers.
 */
    a_errors = 0;
    MPI_Errhandler_create(handler_a, &errhandler_a);

    MPI_Errhandler_set(dup_comm_world, errhandler_a);
    MPI_Errhandler_free(&errhandler_a);

    MPI_Comm_create(dup_comm_world, MPI_GROUP_NULL, &dummy);
    if (a_errors != 1) {
	printf( "    error handler A not invoked\n" );
    }
    b_errors = 0;
    MPI_Errhandler_create(handler_b, &errhandler_b);
    MPI_Errhandler_get(dup_comm_world, &old_handler);
    /* The following is needed to preserve an old handler */
    MPI_Comm_dup( MPI_COMM_SELF, &tempcomm );
    MPI_Errhandler_set( tempcomm, old_handler );
    MPI_Errhandler_set(dup_comm_world, errhandler_b);
    MPI_Errhandler_free(&errhandler_b);
    MPI_Comm_create(dup_comm_world, MPI_GROUP_NULL, &dummy);
    if (b_errors != 1) {
	printf( "    error handler B not invoked\n" );
    }

    MPI_Errhandler_set(dup_comm_world, old_handler);
    MPI_Comm_free( &tempcomm );
    /* MPI_Errhandler_free(&old_handler); */
    MPI_Comm_create(dup_comm_world, MPI_GROUP_NULL, &dummy);
    if (a_errors != 2) {
	printf( "    error handler A not re-invoked\n" );
    }
/*
      Exercise class & string interrogation.
 */
    MPI_Errhandler_set(dup_comm_world, MPI_ERRORS_ARE_FATAL);

    printf( "    Three error messages (from two errors) are expected\n\
which should both show an error class of %d\n", MPI_ERR_GROUP );

    MPI_Errhandler_set(dup_comm_world, MPI_ERRORS_RETURN);
    err = MPI_Comm_create(dup_comm_world, MPI_GROUP_NULL, &dummy);
    if (err != MPI_SUCCESS) {
	MPI_Error_class(err, &class);
	MPI_Error_string(err, errstring, &resultlen);
	printf( "(first) %d : %s\n", class, errstring );
    }
    else
	MPI_Comm_free( &dummy );

    MPI_Errhandler_create(error_handler, &errhandler);
    MPI_Errhandler_set(dup_comm_world, errhandler);
    MPI_Errhandler_free(&errhandler);
    err = MPI_Comm_create(dup_comm_world, MPI_GROUP_NULL, &dummy);
    if (err != MPI_SUCCESS) {
	MPI_Error_class(err, &class);
	MPI_Error_string(err, errstring, &resultlen);
	printf( "(second) %d : %s\n", class, errstring );
    }
    else
	MPI_Comm_free( &dummy );
    MPI_Errhandler_set(dup_comm_world, MPI_ERRORS_ARE_FATAL);

#ifdef FOO
    if (test_default) {
	printf("Forcing error for default handler...\n");
	MPI_Comm_create(dup_comm_world, MPI_GROUP_NULL, &dummy);
    }
    if (test_abort) {
	printf( "Calling MPI_Abort...\n" );
	MPI_Abort(MPI_COMM_WORLD, 123456768);
    }
#endif

    MPI_Comm_free( &dup_comm_world );
    return 0;
}


/*

  Trivial error handler.  Note that FORTRAN error handlers can't
  deal with the varargs stuff the C handlers can.
 
 */
#if defined(USE_STDARG)
void error_handler(MPI_Comm *comm, int *err, ...)
#else
void error_handler(comm, err)
MPI_Comm *comm;
int *err;
#endif
{
int class;
int resultlen;
char string[MPI_MAX_ERROR_STRING];

MPI_Error_class(*err, &class);
MPI_Error_string(*err, string, &resultlen);
printf( "(errhandler) %d : %s\n", class, string );
}
/* 
  Error handler A, used for save/restore testing.
 */

#if defined(USE_STDARG)
void handler_a( MPI_Comm *comm, int *err, ...)
#else
void handler_a(comm, err)
MPI_Comm *comm; 
int      *err;
#endif
{
    int class;

    MPI_Error_class(*err, &class);
    if (class != MPI_ERR_GROUP) {
	printf( "handler_a: incorrect error class %d\n", class );
    }
    *err = MPI_SUCCESS;
    a_errors++;
}

/* 
  Error handler B, used for save/restore testing.
 */

#if defined(USE_STDARG)
void handler_b(MPI_Comm *comm, int *err, ...)
#else
void handler_b(comm, err)
MPI_Comm *comm;
int      *err;
#endif
{
    int class;

    MPI_Error_class(*err, &class);
    if (class != MPI_ERR_GROUP) {
	printf( "handler_b: incorrect error class %d\n", class );
    }
    *err = MPI_SUCCESS;
    b_errors++;
}
