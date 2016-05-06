/* Prototypes for the mpe extension routines */
#ifndef MPE_EXTENSION_INC
#define MPE_EXTENSION_INC
/*
 * In order to provide better compile time error checking for the 
 * implementation, we use a union to store the actual copy/delete functions
 * for the different languages
 */
#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

#include <stdio.h>
/*
int MPIR_PrintDatatypePack ANSI_ARGS(( FILE *, int, MPI_Datatype, 
					long, long ));
int MPIR_PrintDatatypeUnpack ANSI_ARGS(( FILE *, int, MPI_Datatype, 
					 long, long ));
					 */
int MPE_Print_datatype_unpack_action ANSI_ARGS(( FILE *, int, MPI_Datatype,
						 int, int ));
int MPE_Print_datatype_pack_action ANSI_ARGS(( FILE *, int, MPI_Datatype,
					       int, int ));

void MPE_Comm_global_rank ANSI_ARGS(( MPI_Comm, int, int * ));

/* dbxerr.c */
void MPE_Errors_call_debugger ANSI_ARGS(( char *, char *, char ** ));
void MPE_Errors_call_dbx_in_xterm ANSI_ARGS(( char *, char * ));
void MPE_Errors_call_gdb_in_xterm ANSI_ARGS(( char *, char * ));
void MPE_Signals_call_debugger ANSI_ARGS(( void ));

#endif
