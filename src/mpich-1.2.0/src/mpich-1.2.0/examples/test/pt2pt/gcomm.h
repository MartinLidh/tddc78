#ifndef MPITEST_GCOMMS
#define MPITEST_GCOMMS

#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

void MakeComms ANSI_ARGS(( MPI_Comm *, int, int *, int ));
void FreeComms ANSI_ARGS(( MPI_Comm *, int ));
#endif
