#ifndef __PTRCVT
#define __PTRCVT

/*
 * Definitions for the index <-> pointer conversion code
 */

void MPIR_PointerPerm ANSI_ARGS(( int ));
void *MPIR_ToPointer ANSI_ARGS(( int ));
int MPIR_FromPointer ANSI_ARGS(( void * ));
void MPIR_RmPointer ANSI_ARGS(( int ));
int MPIR_UsePointer ANSI_ARGS(( FILE * ));
void MPIR_RegPointerIdx ANSI_ARGS(( int, void * ));
void MPIR_PointerOpts ANSI_ARGS(( int ));

/* 
 * Eventually, this should include a shortcut for ToPointer that takes the
 * low numbered indices directly to the pointer by direct lookup.
 */
#endif
