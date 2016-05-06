/* Procedures for recording and printing test results */

#include <stdio.h>
#include <string.h>
#include "test.h"

#if defined(USE_STDARG)
#include <stdarg.h>
#endif

static int tests_passed = 0;
static int tests_failed = 0;
static char failed_tests[255][81];
static char suite_name[255];
FILE *fileout = NULL;

void Test_Init(suite, rank)
char *suite;
int rank;
{
    char filename[512];

    sprintf(filename, "%s-%d.out", suite, rank);
    strncpy(suite_name, suite, 255);
    fileout = fopen(filename, "w");

    MPI_Errhandler_create( Test_Errors_warn, &TEST_ERRORS_WARN );
}

void Test_Message(mess)
char *mess;
{
    fprintf(fileout, "[%s]: %s\n", suite_name, mess);
    fflush(fileout);
}

void Test_Failed(test)
char *test;
{
    fprintf(fileout, "[%s]: *** Test '%s' Failed! ***\n", suite_name, test);
    strncpy(failed_tests[tests_failed], test, 81);
    fflush(fileout);
    tests_failed++;
}

void Test_Passed(test)
char *test;
{
    fprintf(fileout, "[%s]: Test '%s' Passed.\n", suite_name, test);
    fflush(fileout);
    tests_passed++;
}

int Summarize_Test_Results()
{
    fprintf(fileout, "For test suite '%s':\n", suite_name);
    fprintf(fileout, "Of %d attempted tests, %d passed, %d failed.\n", 
	    tests_passed + tests_failed, tests_passed, tests_failed);
    if (tests_failed > 0) {
	int i;

	fprintf(fileout, "*** Tests Failed:\n");
	for (i = 0; i < tests_failed; i++)
	    fprintf(fileout, "*** %s\n", failed_tests[i]);
    }
    return tests_failed;
}

void Test_Finalize()
{
    fflush(fileout);
    fclose(fileout);
}

#include "mpi.h"
/* Wait for every process to pass through this point.  This test is used
   to make sure that all processes complete, and that a test "passes" because
   it executed, not because it some process failed.  
 */
void Test_Waitforall( )
{
int m, one, myrank, n;

MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
MPI_Comm_size( MPI_COMM_WORLD, &n );
one = 1;
MPI_Allreduce( &one, &m, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD );

if (m != n) {
    printf( "[%d] Expected %d processes to wait at end, got %d\n", myrank, 
	    n, m );
    }
if (myrank == 0) 
    printf( "All processes completed test\n" );
}

/*
   Handler prints warning messsage and returns.  Internal.  Not
   a part of the standard.
 */
MPI_Errhandler TEST_ERRORS_WARN;

#ifdef USE_STDARG
void Test_Errors_warn(  MPI_Comm *comm, int *code, ... )
{  
  char buf[MPI_MAX_ERROR_STRING];
  int  myid, result_len; 
  char *string, *file;
  int  *line;
  static int in_handler = 0;
  va_list Argp;

#ifdef USE_OLDSTYLE_STDARG
  va_start( Argp );
#else
  va_start( Argp, code );
#endif
  string = va_arg(Argp,char *);
  file   = va_arg(Argp,char *);
  line   = va_arg(Argp,int *);
  va_end( Argp );
#else
void Test_Errors_warn(  comm, code, string, file, line )
MPI_Comm *comm;
int      *code, *line;
char     *string, *file;
{
  char buf[MPI_MAX_ERROR_STRING];
  int  myid, result_len; 
  static int in_handler = 0;
#endif

  if (in_handler) return;
  in_handler = 1;

  MPI_Comm_rank( MPI_COMM_WORLD, &myid );
  MPI_Error_string( *code, buf, &result_len );
#ifdef MPIR_DEBUG
  /* Generate this information ONLY when debugging MPIR */
  fprintf( stderr, "%d -  File: %s   Line: %d\n", myid, 
		   file, *line );
#endif
  fprintf( stderr, "%d - %s : %s\n", myid, 
          string ? string : "<NO ERROR MESSAGE>", buf );
  in_handler = 0;
}
