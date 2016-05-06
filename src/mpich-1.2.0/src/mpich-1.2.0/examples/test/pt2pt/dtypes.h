#ifndef MPITEST_DTYPES
#define MPITEST_DTYPES

#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

void GenerateData ANSI_ARGS(( MPI_Datatype *, void **, void **, int *, int *,
			      char **, int * ));
void AllocateForData ANSI_ARGS(( MPI_Datatype **, void ***, void ***, 
				 int **, int **, char ***, int * ));
int CheckData ANSI_ARGS(( void *, void *, int ));
int CheckDataAndPrint ANSI_ARGS(( void *, void *, int, char *, int ));
void FreeDatatypes ANSI_ARGS(( MPI_Datatype *, void **, void **, 
			       int *, int *, char **, int ));
#endif
